extends Control

# Story 2-2 T6.1 — Release-gate reauthorize wiring.
#
# This is the WalletAdapterAndroid demo scene script.  It exercises the MWA
# autoload (registered via project.godot) and serves as the human-operated
# harness for the AC-5 10-cycle reauthorize release-gate.
#
# Diagnostic UI layout (added by T6.1):
#   DiagnosticLabel  — RichTextLabel; shows Public Key / Fingerprint / Last Cycle
#   ReauthorizeBtn   — Button; calls MWA.reauthorize({}) on pressed
#
# [CR-32-RELEASE-GATE] logcat marker is gated on debug builds (OS.is_debug_build())
# to prevent token-related data from leaking into production logcat streams.


const TOTAL_CASES := 3

# --- T6.1: diagnostic state ---------------------------------------------------
# Persisted across launches via ProjectSettings autoload-backed Dictionary stored
# in a simple file.  We only need one bit: whether the user completed a Connect
# in a prior launch, so that _ready() knows whether to auto-trigger reauthorize.
var _diagnostic_state: Dictionary = {"has_seen_connect": false}
const _STATE_PATH := "user://mwa_release_gate_state.json"


func _ready() -> void:
	_load_diagnostic_state()
	_wire_mwa_signals()

	# Two modes of operation, gated on `has_seen_connect`:
	#
	#   - **Release-gate mode** (has_seen_connect == true): only call
	#     MWA.reauthorize({}). The scaffold demo flow is SKIPPED — it would
	#     re-trigger $WalletAdapter.connect_wallet() and open the wallet
	#     chooser, which breaks AC-5's "0 wallet activity transitions"
	#     assertion. This is what the 10-cycle release-gate procedure runs.
	#
	#   - **Bootstrap mode** (has_seen_connect == false): run the original
	#     scaffold demo flow. The user is also expected to tap the
	#     "MWA Connect (bootstrap)" button during this launch — that calls
	#     MWA.connect(...), MWA.connect_completed fires, _on_connect_completed
	#     persists has_seen_connect=true, and from the next launch onward
	#     this branch is skipped.
	if _diagnostic_state.get("has_seen_connect", false):
		MWA.reauthorize({})
		return

	# Bootstrap mode — original demo-flow runs alongside the user-driven
	# MWA.connect() bootstrap. This branch only executes on first launch
	# (or after the persistent state file is wiped, e.g., uninstall).
	await wallet_adapter_sign(0)
	await wallet_adapter_sign(1)
	await sign_text_message(3)
	await wallet_adapter_in_deserialized_transaction()

	print("[ALL TESTS PASSED]")


# --- T6.1: signal wiring ------------------------------------------------------

func _wire_mwa_signals() -> void:
	# reauthorize_completed — primary release-gate signal
	MWA.reauthorize_completed.connect(_on_reauthorize_completed)
	# connect_completed — set has_seen_connect so future launches auto-reauth
	MWA.connect_completed.connect(_on_connect_completed)
	# mwa_error — surface errors in the diagnostic label
	MWA.mwa_error.connect(_on_mwa_error)

	# Wire the Reauthorize button if present in the scene tree
	if has_node("ReauthorizeBtn"):
		$ReauthorizeBtn.pressed.connect(_on_reauthorize_pressed)

	# Wire the MWA Connect (bootstrap) button — used once on first launch to
	# fire MWA.connect_completed → flip has_seen_connect → enable the
	# release-gate auto-reauth path on subsequent launches.
	if has_node("MWAConnectBtn"):
		$MWAConnectBtn.pressed.connect(_on_mwa_connect_pressed)


func _on_reauthorize_pressed() -> void:
	MWA.reauthorize({})


func _on_mwa_connect_pressed() -> void:
	# Release-gate bootstrap. Identity URI / icon are placeholders meaningful
	# only to the wallet's authorize prompt; cluster is "devnet" to match the
	# AC-5 procedure and the Story 2-2 unit-test seeds.
	var identity := {
		"name": "WalletAdapterAndroid Demo",
		"icon_uri": "https://example.com/wallet-adapter-android-icon.png",
		"identity_uri": "https://example.com/wallet-adapter-android",
	}
	MWA.connect(identity, "devnet", {})


func _on_reauthorize_completed(request_id: String, result: Dictionary) -> void:
	var pk: String = result.get("public_key", "")
	var fp: String = result.get("auth_token_fingerprint", "")
	var ts: String = Time.get_datetime_string_from_system(true)

	# Defensive: the procedure's regex `grep -oP 'public_key=\K[^ ]+'` would
	# capture an empty string if pk were "" — and `[ "$CURRENT_PK" != "$INITIAL_PK" ]`
	# would compare two empty strings as equal, masking a regression as a
	# false-positive AC-5 success. DD-2-2-4 guarantees the keys are present
	# and populated, but defense-in-depth is cheap.
	if pk.is_empty() or fp.is_empty():
		push_error(
			"[CR-32-RELEASE-GATE] reauthorize_completed missing pk/fp; pk='"
			+ pk + "' fp='" + fp + "'"
		)
		return

	# [CR-32-RELEASE-GATE] marker — debug builds only (token-leak guard).
	# The line format is stable: grep -oP 'public_key=\K[^ ]+' and
	# grep -oP 'fingerprint=\K[^ ]+' extract the fields used by the 10-cycle
	# procedure script.
	if OS.is_debug_build():
		print("[CR-32-RELEASE-GATE] cycle_marker public_key=" + pk + " fingerprint=" + fp)

	# Update the diagnostic UI label if present.
	_update_diagnostic_label(pk, fp, ts)


func _on_connect_completed(request_id: String, result: Dictionary) -> void:
	# Record that the user has successfully connected so that future launches
	# auto-trigger reauthorize instead of waiting for UI interaction.
	_diagnostic_state["has_seen_connect"] = true
	_save_diagnostic_state()

	var pk: String = result.get("public_key", "")
	var fp: String = result.get("auth_token_fingerprint", "")
	var ts: String = Time.get_datetime_string_from_system(true)
	_update_diagnostic_label(pk, fp, ts)


func _on_mwa_error(payload: Dictionary) -> void:
	var code: String = payload.get("code", "UNKNOWN")
	var msg: String = payload.get("message", "")
	if has_node("DiagnosticLabel"):
		$DiagnosticLabel.text = "MWA Error: " + code + "\n" + msg


# --- T6.1: diagnostic UI helpers ----------------------------------------------

func _update_diagnostic_label(pk: String, fp: String, ts: String) -> void:
	if not has_node("DiagnosticLabel"):
		return
	$DiagnosticLabel.text = (
		"Public Key: " + pk + "\n"
		+ "Fingerprint: " + fp + "\n"
		+ "Last Cycle: " + ts
	)


# --- T6.1: persistent state helpers ------------------------------------------

func _load_diagnostic_state() -> void:
	if not FileAccess.file_exists(_STATE_PATH):
		return
	var f := FileAccess.open(_STATE_PATH, FileAccess.READ)
	if f == null:
		return
	var json := JSON.new()
	var err := json.parse(f.get_as_text())
	f.close()
	if err == OK and json.data is Dictionary:
		_diagnostic_state = json.data


func _save_diagnostic_state() -> void:
	var f := FileAccess.open(_STATE_PATH, FileAccess.WRITE)
	if f == null:
		return
	f.store_string(JSON.stringify(_diagnostic_state))
	f.close()


# --- Original demo helpers (unchanged) ----------------------------------------

func PASS(unique_identifier: int):
	print("[OK]: ", unique_identifier)


func wallet_adapter_sign(test_id):
	$WalletAdapter.connect_wallet()
	await $WalletAdapter.connection_established

	$Transaction.set_payer($WalletAdapter)

	var ix = SystemProgram.transfer($WalletAdapter, Pubkey.new_random(), 1000000)
	$Transaction.add_instruction(ix)
	$Transaction.update_latest_blockhash()

	$Transaction.sign()
	await $Transaction.fully_signed
	var signature : PackedByteArray = $Transaction.serialize().slice(1, 65)
	var empty_signature : PackedByteArray
	empty_signature.resize(64)
	assert(signature != empty_signature)

	$Transaction.send()
	var response = await $Transaction.transaction_response_received

	# Wallets may add instructions for programs we don't have locally.
	# So we just make sure the error is not due to transaction signature failure.
	if response.has("error"):
		var error_message = response["error"]["message"]
		assert(error_message.find("signature verification") == -1)

	PASS(test_id)


func wallet_adapter_in_deserialized_transaction():
	$WalletAdapter.connect_wallet()
	await $WalletAdapter.connection_established

	var payer: Keypair = Keypair.new_random()
	var receiver: Pubkey = Pubkey.new_random()
	var tx: Transaction = Transaction.new()
	add_child(tx)

	tx.set_payer(payer)

	var ix: Instruction = SystemProgram.transfer($WalletAdapter, receiver, 1000)
	tx.add_instruction(ix)

	tx.update_latest_blockhash()
	await tx.blockhash_updated

	var serialized_transaction = tx.serialize()

	var reconstructed_transaction = Transaction.new_from_bytes(serialized_transaction)
	add_child(reconstructed_transaction)
	reconstructed_transaction.set_signers([payer, $WalletAdapter])

	reconstructed_transaction.sign()
	await reconstructed_transaction.fully_signed

	var empty_signature : PackedByteArray
	empty_signature.resize(64)
	var signature = reconstructed_transaction.serialize().slice(1, 65)
	assert(signature != empty_signature)
	assert(signature.size() == 64)

	PASS(2)


func sign_text_message(test_id):
	$WalletAdapter.connect_wallet()
	await $WalletAdapter.connection_established

	var text_message := "Hello Godot"

	$WalletAdapter.sign_text_message(text_message)
	var signature = await $WalletAdapter.message_signed

	var empty_signature : PackedByteArray
	empty_signature.resize(64)
	assert(signature != empty_signature)
	assert(signature.size() == 64)

	PASS(test_id)
