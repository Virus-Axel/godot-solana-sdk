extends Node
class_name MobileWalletAdapter

const ANDROID_PLUGIN_NAME := "WalletAdapterAndroid"

## Godot-side wrapper providing React-Native MWA-style authorization lifecycle.

enum Chain {
	DEVNET = 0,
	MAINNET = 1,
	TESTNET = 2,
}

@export var wallet_adapter: WalletAdapter
var auth_cache: MWAAuthCache = MWAFileCache.new()
@export var connect_timeout_sec: float = 60.0

func _get_android_plugin() -> Variant:
	if OS.get_name() == "Android":
		if Engine.has_singleton(ANDROID_PLUGIN_NAME):
			return Engine.get_singleton(ANDROID_PLUGIN_NAME)
	return null

func _safe_android_call(method_name: String, arg0 = null, arg1 = null, arg2 = null, arg3 = null) -> Variant:
	var plugin = _get_android_plugin()
	if plugin == null:
		return null
		
	# Try the call directly without has_method.
	# Some Godot 4 JNISingleton versions don't report methods correctly via has_method.
	# We rely on Godot's internal JNI dispatch to handle it.
	print("MWA: JNI call: ", method_name)
	
	# Determine target name (optional snakification).
	var target_name = method_name
	
	# Try calling directly and returning the result.
	# NOTE: We use call() directly. If it fails, Godot will print an error.
	if arg3 != null:
		return plugin.call(target_name, arg0, arg1, arg2, arg3)
	if arg2 != null:
		return plugin.call(target_name, arg0, arg1, arg2)
	if arg1 != null:
		return plugin.call(target_name, arg0, arg1)
	if arg0 != null:
		return plugin.call(target_name, arg0)
	return plugin.call(target_name)

signal authorized(auth_token: String, account_address: String)
signal authorization_failed(error: String)
signal deauthorized()
signal deauthorization_failed(error: String)
signal disconnected()
signal connected(auth_token: String, account_address: String)
signal capabilities_ready(capabilities: Dictionary)
signal capabilities_failed(error: String)
signal transactions_signed(signed_transactions: Array)
signal messages_signed(signed_messages: Array, addresses: Array)
signal signing_failed(error: String)

func is_logged_in() -> bool:
	# For the cache lifecycle, the primary signal is "do we have a token?"
	return auth_cache != null and auth_cache.has_valid_token()

func _wait_for_authorization(start_call: Callable) -> Dictionary:
	if wallet_adapter == null:
		return {"error": "wallet_adapter is null"}

	var done := false
	var result: Dictionary = {}

	var established_cb = func() -> void:
		done = true
		var token : String = wallet_adapter.get_auth_token()
		var wallet_uri_base := ""
		if wallet_adapter.has_method("get_wallet_uri_base"):
			wallet_uri_base = String(wallet_adapter.get_wallet_uri_base())
		var key_variant := wallet_adapter.get_connected_key()
		var account := ""
		if key_variant != null:
			if key_variant is PackedByteArray:
				account = SolanaUtils.bs58_encode(key_variant)
			elif key_variant.has_method("to_string"):
				account = key_variant.to_string()
			else:
				account = str(key_variant)

		result = {
			"auth_token": token,
			"account_address": account,
			"wallet_uri_base": wallet_uri_base,
		}

	var failed_cb = func() -> void:
		done = true
		result = {"error": "MWA authorization failed"}

	wallet_adapter.connection_established.connect(established_cb, CONNECT_ONE_SHOT)
	wallet_adapter.connection_failed.connect(failed_cb, CONNECT_ONE_SHOT)

	start_call.call()
	
	var elapsed = 0.0
	while !done and elapsed < connect_timeout_sec:
		await get_tree().create_timer(0.1).timeout
		elapsed += 0.1
		var status = _safe_android_call("getConnectionStatus")
		var action = _safe_android_call("getLatestAction")
		print("MWA: Polling session... Status: ", status, " | Action: ", action, " | Elapsed: ", elapsed, "s")
		
		if status == 1:
			print("MWA: Status is SUCCESS (1). Pulling results directly from JNI.")
			var token = _safe_android_call("getAuthToken")
			var key = _safe_android_call("getConnectedKey")
			var wallet_uri_base = _safe_android_call("getWalletUriBase")
			
			var address = ""
			if key != null:
				if key is PackedByteArray:
					address = SolanaUtils.bs58_encode(key)
				else:
					address = str(key)
				
			result = {
				"auth_token": token,
				"account_address": address,
				"wallet_uri_base": wallet_uri_base,
			}
			# Disconnect one-shot listeners before manually triggering so they don't double-fire.
			if wallet_adapter.connection_established.is_connected(established_cb):
				wallet_adapter.connection_established.disconnect(established_cb)
			if wallet_adapter.connection_failed.is_connected(failed_cb):
				wallet_adapter.connection_failed.disconnect(failed_cb)
			done = true
			break
		elif status == 2:
			print("MWA: Status is FAILURE (2).")
			# Disconnect one-shot listeners before manually triggering.
			if wallet_adapter.connection_established.is_connected(established_cb):
				wallet_adapter.connection_established.disconnect(established_cb)
			if wallet_adapter.connection_failed.is_connected(failed_cb):
				wallet_adapter.connection_failed.disconnect(failed_cb)
			result = {"error": "MWA authorization failed"}
			done = true
			break
		elif status == 3:
			print("MWA: Status is CANCELLED (3). User dismissed the wallet.")
			if wallet_adapter.connection_established.is_connected(established_cb):
				wallet_adapter.connection_established.disconnect(established_cb)
			if wallet_adapter.connection_failed.is_connected(failed_cb):
				wallet_adapter.connection_failed.disconnect(failed_cb)
			_safe_android_call("clearState")
			result = {"error": "user_cancelled"}
			done = true
			break
			
	if !done:
		# Timeout — clean up any remaining listeners.
		if wallet_adapter.connection_established.is_connected(established_cb):
			wallet_adapter.connection_established.disconnect(established_cb)
		if wallet_adapter.connection_failed.is_connected(failed_cb):
			wallet_adapter.connection_failed.disconnect(failed_cb)
		_safe_android_call("clearState")
		return {"error": "authorize timeout"}

	return result

func _wait_for_deauthorization(start_call: Callable) -> bool:
	if wallet_adapter == null:
		return false

	var done := false
	var ok := false

	# We no longer connect to C++ signals for deauth as they are inconsistent/missing.
	start_call.call()
	
	var elapsed = 0.0
	while !done and elapsed < connect_timeout_sec:
		await get_tree().create_timer(0.1).timeout
		elapsed += 0.1
		var status = _safe_android_call("getConnectionStatus")
		print("MWA: Polling deauth... Status: ", status, " | Elapsed: ", elapsed, "s")
		
		# For deauth, Success=1 means the disconnect operation finished successfully.
		if status == 1:
			print("MWA: Deauth SUCCESS (1).")
			ok = true
			done = true
			break
		elif status == 2:
			print("MWA: Deauth FAILURE (2).")
			done = true
			break
		elif status == 3:
			print("MWA: Deauth CANCELLED (3). User dismissed the wallet.")
			_safe_android_call("clearState")
			# Treat cancel as successful disconnect from the user's perspective.
			ok = true
			done = true
			break
			
	if !done:
		_safe_android_call("clearState")
		
	return ok

func _start_deauthorize(token: String) -> void:
	wallet_adapter.deauthorize(token)

## Connects / authorizes an MWA session.
##
## If `auth_token` is provided, the Android MWA client will attempt a silent re-authorization first.
func authorize(identity_name: String, identity_uri: String, chain: Chain, auth_token: String = "") -> Dictionary:
	if wallet_adapter == null:
		push_error("MobileWalletAdapter.wallet_adapter is not set")
		return {"error": "wallet_adapter is null"}

	# Reset any stale state from a previous (possibly cancelled) operation.
	_safe_android_call("clearState")

	# Configure wallet identity + chain.
	wallet_adapter.mobile_identity_name = identity_name
	wallet_adapter.mobile_identity_uri = identity_uri
	wallet_adapter.mobile_blockchain = chain

	# Configure silent re-auth token.
	_safe_android_call("setAuthToken", auth_token)
	if wallet_adapter.has_method("set_auth_token"):
		wallet_adapter.set_auth_token(auth_token)

	var res := await _wait_for_authorization(func():
		var plugin = _get_android_plugin()
		if plugin:
			_safe_android_call("connectWallet", chain, identity_uri, "favicon.ico", identity_name)
		else:
			wallet_adapter.connect_wallet()
	)
	if res.has("error"):
		var err_msg := String(res.get("error", "unknown"))
		if err_msg == "user_cancelled":
			print("MWA: Authorization cancelled by user.")
		authorization_failed.emit(err_msg)
		return res

	var token := String(res.get("auth_token", ""))
	var account := String(res.get("account_address", ""))
	var wallet_uri_base := String(res.get("wallet_uri_base", ""))
	if auth_cache != null and token != "":
		auth_cache.save_token(token, account)

	authorized.emit(token, account)
	connected.emit(token, account)
	return {
		"auth_token": token,
		"accounts": [account],
		"wallet_uri_base": wallet_uri_base,
		"account_address": account,
	}

## Revokes the stored auth token (disconnects the session).
func deauthorize(auth_token: String) -> bool:
	if wallet_adapter == null or auth_cache == null:
		disconnected.emit()
		return false

	var token := String(auth_token)
	if token == "":
		authorization_failed.emit("deauthorize called with empty auth_token")
		return false

	# Reset any stale state from a previous (possibly cancelled) operation.
	_safe_android_call("clearState")

	# Deauthorize the same token we cached.
	_safe_android_call("setAuthToken", token)
	if wallet_adapter.has_method("set_auth_token"):
		wallet_adapter.set_auth_token(token)

	var ok := await _wait_for_deauthorization(func():
		var plugin = _get_android_plugin()
		if plugin:
			_safe_android_call("deauthorizeWallet", token)
		else:
			# Fallback to C++ if it has the method; otherwise this action is unsupported on this binary.
			if wallet_adapter.has_method("deauthorize"):
				wallet_adapter.deauthorize(token)
	)
	if ok:
		auth_cache.clear_token()
		deauthorized.emit()
		disconnected.emit()
		return true

	deauthorization_failed.emit("deauthorization failed")
	return false

## Full disconnect using the cached auth token.
func disconnect_session() -> bool:
	if auth_cache == null:
		disconnected.emit()
		return false

	var loaded := auth_cache.load_token()
	var token := String(loaded.get("auth_token", ""))

	# If there's no cached token, treat it as already disconnected.
	if token == "":
		auth_cache.clear_token()
		disconnected.emit()
		return true

	return await deauthorize(token)

## Silently refresh an existing session using a stored auth token (fallbacks to full authorize).
func reauthorize(identity_name: String, identity_uri: String, chain: Chain, auth_token: String) -> Dictionary:
	var res := await authorize(identity_name, identity_uri, chain, auth_token)
	if res.has("error"):
		# Token may have expired; clear and retry full authorization.
		auth_cache.clear_token()
		res = await authorize(identity_name, identity_uri, chain, "")
	return res

## Attempts to reconnect using cached token first; falls back to interactive authorize.
func reconnect(identity_name: String, identity_uri: String, chain: Chain) -> Dictionary:
	if auth_cache == null:
		return await authorize(identity_name, identity_uri, chain, "")

	var loaded := auth_cache.load_token()
	var token := String(loaded.get("auth_token", ""))

	# Try silent re-authorization first.
	return await reauthorize(identity_name, identity_uri, chain, token)

func _wait_for_signing(start_call: Callable) -> Dictionary:
	if wallet_adapter == null:
		return {"error": "wallet_adapter is null"}

	var done := false
	var ok := false
	var signature: PackedByteArray = PackedByteArray()

	var signed_cb = func(sig: PackedByteArray) -> void:
		done = true
		ok = true
		signature = sig

	var failed_cb = func() -> void:
		done = true
		ok = false

	wallet_adapter.message_signed.connect(signed_cb, CONNECT_ONE_SHOT)
	wallet_adapter.signing_failed.connect(failed_cb, CONNECT_ONE_SHOT)

	start_call.call()
	var elapsed = 0.0
	while !done and elapsed < connect_timeout_sec:
		await get_tree().create_timer(0.1).timeout
		elapsed += 0.1
		var conn_status = _safe_android_call("getConnectionStatus")
		var status = _safe_android_call("getSigningStatus")
		print("MWA: Polling signing... ConnStatus: ", conn_status, " | SignStatus: ", status, " | Elapsed: ", elapsed, "s")
		
		if status == 1:
			print("MWA: Signing SUCCESS (1). Pulling signature via JNI.")
			var sig = _safe_android_call("getMessageSignature")
			signature = sig
			# Disconnect one-shot listeners before manually resolving.
			if wallet_adapter.message_signed.is_connected(signed_cb):
				wallet_adapter.message_signed.disconnect(signed_cb)
			if wallet_adapter.signing_failed.is_connected(failed_cb):
				wallet_adapter.signing_failed.disconnect(failed_cb)
			ok = true
			done = true
			break
		elif status == 2:
			print("MWA: Signing FAILURE (2).")
			if wallet_adapter.message_signed.is_connected(signed_cb):
				wallet_adapter.message_signed.disconnect(signed_cb)
			if wallet_adapter.signing_failed.is_connected(failed_cb):
				wallet_adapter.signing_failed.disconnect(failed_cb)
			ok = false
			done = true
			break
		elif conn_status == 3 or status == 3:
			print("MWA: Signing CANCELLED (3). User dismissed the wallet. (conn_status=", conn_status, ", status=", status, ")")
			if wallet_adapter.message_signed.is_connected(signed_cb):
				wallet_adapter.message_signed.disconnect(signed_cb)
			if wallet_adapter.signing_failed.is_connected(failed_cb):
				wallet_adapter.signing_failed.disconnect(failed_cb)
			_safe_android_call("clearState")
			return {"error": "user_cancelled"}

	if !done:
		if wallet_adapter.message_signed.is_connected(signed_cb):
			wallet_adapter.message_signed.disconnect(signed_cb)
		if wallet_adapter.signing_failed.is_connected(failed_cb):
			wallet_adapter.signing_failed.disconnect(failed_cb)
		_safe_android_call("clearState")
		return {"error": "signing timeout"}

	if !ok:
		return {"error": "signing failed"}

	return {"signature": signature}

## Signs one or more serialized transactions (Android-only; signed payload bytes are returned).
func signTransactions(serialized_transactions: Array) -> Array:
	var out: Array = []
	if wallet_adapter == null:
		signing_failed.emit("wallet_adapter is null")
		return out

	for serialized_tx in serialized_transactions:
		var res := await _wait_for_signing(func():
			var plugin = _get_android_plugin()
			if plugin:
				# signTransaction in the plugin handles both legacy and versioned txs for signing only.
				_safe_android_call("signTransaction", serialized_tx)
			else:
				wallet_adapter.sign_message(serialized_tx, 0)
		)
		if res.has("error"):
			signing_failed.emit(String(res["error"]))
			return []

		out.append(res["signature"])

	transactions_signed.emit(out)
	return out

## Signs one or more detached byte-message payloads (Android-only).
func signMessages(message_payloads: Array) -> Dictionary:
	var signed_messages: Array = []
	var addresses: Array = []

	if wallet_adapter == null:
		signing_failed.emit("wallet_adapter is null")
		return {"error": "wallet_adapter is null"}

	var jni_key = _safe_android_call("getConnectedKey")
	
	var key_variant := wallet_adapter.get_connected_key()
	var address := ""
	
	# Fix: Use JNI key if GDExtension key is empty.
	var final_key = key_variant
	if final_key == null or (final_key is Array and final_key.is_empty()) or (final_key is PackedByteArray and final_key.is_empty()):
		final_key = jni_key
		
	if final_key != null:
		if final_key is PackedByteArray:
			address = SolanaUtils.bs58_encode(final_key)
		else:
			address = str(final_key)

	for payload in message_payloads:
		var res := await _wait_for_signing(func():
			var plugin = _get_android_plugin()
			if plugin:
				_safe_android_call("signMessageBytes", payload)
			else:
				if wallet_adapter.has_method("sign_message_bytes"):
					wallet_adapter.sign_message_bytes(payload)
				else:
					push_error("sign_message_bytes not supported on this binary")
		)
		if res.has("error"):
			signing_failed.emit(String(res["error"]))
			return {"error": String(res["error"])}

		signed_messages.append(res["signature"])
		addresses.append(address)

	messages_signed.emit(signed_messages, addresses)
	return {"signed_messages": signed_messages, "addresses": addresses}

## Not implemented yet: requires additional Android JNI bridge wiring.
func signAndSendTransactions(serialized_transactions: Array) -> Dictionary:
	var sigs: Array = []
	if wallet_adapter == null:
		return {"error": "wallet_adapter is null"}

	print("MWA: signAndSendTransactions called with ", serialized_transactions.size(), " transactions")
	for tx in serialized_transactions:
		print("MWA: Calling JNI signTransaction (fallback) with tx size ", tx.size())
		var res := await _wait_for_signing(func():
			var plugin = _get_android_plugin()
			if plugin:
				# Use signTransaction instead of signAndSendTransaction to bypass Phantom devnet hang
				_safe_android_call("signTransaction", tx)
			else:
				if wallet_adapter.has_method("sign_and_send_transaction"):
					wallet_adapter.sign_and_send_transaction(tx)
				else:
					push_error("sign_and_send_transaction not supported on this binary")
		)
		
		if res.has("error"):
			signing_failed.emit(String(res["error"]))
			return {"error": String(res["error"])}
		
		var signed_tx: PackedByteArray = res["signature"]
		var signed_tx_obj = Transaction.new_from_bytes(signed_tx)
		print("MWA: Broadcasting transaction natively via Godot TransactionManager...")
		var tx_data: TransactionData = await SolanaService.transaction_manager.send_transaction(signed_tx_obj)
		
		if tx_data.is_successful():
			var sig_string = tx_data.data["result"]
			sigs.append(sig_string)
			print("MWA: Broadcasted signature: ", sig_string)
		else:
			sigs.append("unknown_or_failed")
			print("MWA: Failed to broadcast: ", tx_data.get_error_message())

	return {"transaction_signatures": sigs}

## Not implemented yet: requires capability query wiring.
func getCapabilities() -> Dictionary:
	if wallet_adapter == null:
		return {"error": "wallet_adapter is null"}

	_safe_android_call("setAuthToken", auth_cache.load_token().get("auth_token", ""))
	if wallet_adapter.has_method("set_auth_token"):
		wallet_adapter.set_auth_token(auth_cache.load_token().get("auth_token", ""))

	var caps := await _wait_for_capabilities(func():
		var plugin = _get_android_plugin()
		if plugin:
			_safe_android_call("getCapabilitiesWallet")
		elif wallet_adapter.has_method("get_capabilities"):
			wallet_adapter.get_capabilities()
	)
	if caps.has("error"):
		capabilities_failed.emit(String(caps["error"]))
		return caps

	capabilities_ready.emit(caps)
	return caps

func _wait_for_capabilities(start_call: Callable) -> Dictionary:
	if wallet_adapter == null:
		return {"error": "wallet_adapter is null"}

	var done := false
	var failed := false
	var json_text := ""

	var received_cb = func(result_json: String) -> void:
		done = true
		failed = false
		json_text = result_json

	var failed_cb = func() -> void:
		done = true
		failed = true

	wallet_adapter.capabilities_received.connect(received_cb, CONNECT_ONE_SHOT)
	wallet_adapter.capabilities_failed.connect(failed_cb, CONNECT_ONE_SHOT)

	start_call.call()
	var elapsed = 0.0
	while !done and elapsed < connect_timeout_sec:
		await get_tree().create_timer(1.0).timeout
		elapsed += 1.0
		var status = _safe_android_call("getConnectionStatus")
		print("MWA: Polling capabilities... Status: ", status, " | Elapsed: ", elapsed, "s")
		
		if status == 1:
			print("MWA: Capabilities SUCCESS (1). Pulling JSON via JNI.")
			var json = _safe_android_call("getCapabilitiesResultJson")
			json_text = json
			# Disconnect one-shot listeners before manually resolving.
			if wallet_adapter.capabilities_received.is_connected(received_cb):
				wallet_adapter.capabilities_received.disconnect(received_cb)
			if wallet_adapter.capabilities_failed.is_connected(failed_cb):
				wallet_adapter.capabilities_failed.disconnect(failed_cb)
			done = true
			break
		elif status == 2:
			print("MWA: Capabilities FAILURE (2).")
			if wallet_adapter.capabilities_received.is_connected(received_cb):
				wallet_adapter.capabilities_received.disconnect(received_cb)
			if wallet_adapter.capabilities_failed.is_connected(failed_cb):
				wallet_adapter.capabilities_failed.disconnect(failed_cb)
			failed = true
			done = true
			break
		elif status == 3:
			print("MWA: Capabilities CANCELLED (3). User dismissed the wallet.")
			if wallet_adapter.capabilities_received.is_connected(received_cb):
				wallet_adapter.capabilities_received.disconnect(received_cb)
			if wallet_adapter.capabilities_failed.is_connected(failed_cb):
				wallet_adapter.capabilities_failed.disconnect(failed_cb)
			_safe_android_call("clearState")
			return {"error": "user_cancelled"}

	if !done:
		if wallet_adapter.capabilities_received.is_connected(received_cb):
			wallet_adapter.capabilities_received.disconnect(received_cb)
		if wallet_adapter.capabilities_failed.is_connected(failed_cb):
			wallet_adapter.capabilities_failed.disconnect(failed_cb)
		_safe_android_call("clearState")
		return {"error": "capabilities timeout"}

	if failed:
		return {"error": "capabilities failed"}

	if json_text == "":
		return {"error": "empty capabilities payload"}

	var parsed = JSON.parse_string(json_text)
	if typeof(parsed) != TYPE_DICTIONARY:
		return {"error": "invalid capabilities JSON"}

	return parsed
