extends VBoxContainer

const EXAMPLE_ACCOUNT := "4sGjMW1sUnHzSxGspuhpqLDx6wiyjNtZAMdL4VZHirAn"

const TOTAL_CASES := 9
var passed_test_mask := 0
		

func PASS(unique_identifier: int):
	passed_test_mask += (1 << unique_identifier)
	print("[OK]: ", unique_identifier)


func display_dict(data: Variant, parent: TreeItem):
	if(typeof(data) == TYPE_STRING):
		parent.set_text(1, data)
	elif(typeof(data) != TYPE_DICTIONARY):
		parent.set_text(1, str(data))
	else:
		var data_dict: Dictionary = data
		var keys = data_dict.keys()
		var values = data_dict.values()
		for i in range(keys.size()):
			var subchild = parent.create_child()
			subchild.set_editable(1, true)
			subchild.set_text(0, keys[i])
			display_dict(values[i], subchild)


func add_solana_client() -> SolanaClient:
	var res = SolanaClient.new()
	
	# RPC HTTP URL is set in project settings.
	# You can override it by setting url_override property.

	# Solana Client needs to be in scene tree for async to work.
	add_child(res)
	
	return res

func delete_solana_client(client: SolanaClient):
	remove_child(client)

func get_account_info_demo():
	var client: SolanaClient = add_solana_client()
	client.get_account_info(EXAMPLE_ACCOUNT)
	var response: Dictionary = await client.http_response_received
	assert(response.has("result"))
	display_dict(response["result"], $ResultTree1.create_item())
	delete_solana_client(client)
	PASS(0)


func get_latest_blockhash_demo():
	var client: SolanaClient = add_solana_client()
	client.get_latest_blockhash()
	var response: Dictionary = await client.http_response_received
	assert(response.has("result"))
	display_dict(response["result"], $ResultTree2.create_item())
	delete_solana_client(client)
	PASS(1)
	
func get_minimum_balance_for_rent_extemption_demo():
	const EXAMPLE_DATA_SIZE := 100
	var client: SolanaClient = add_solana_client()
	client.get_minimum_balance_for_rent_extemption(EXAMPLE_DATA_SIZE)
	var response: Dictionary = await client.http_response_received
	assert(response.has("result"))
	display_dict(response["result"], $ResultTree3.create_item())
	delete_solana_client(client)
	PASS(2)


func subscribe_account_demo():
	var client: SolanaClient = add_solana_client()

	client.set_commitment("finalized")
	var account_callback := Callable(self, "_account_subscribe_callback")
	client.account_subscribe(EXAMPLE_ACCOUNT, account_callback)

	# Make lamports of the account change to trigger the callback.
	client.request_airdrop(EXAMPLE_ACCOUNT, 1000000)
	var airdrop_response = await client.http_response_received
	assert(airdrop_response.has("result"))
	var airdrop_signature: String = airdrop_response["result"]
	
	var signature_callback := Callable(self, "_signature_subscribe_callback")
	client.signature_subscribe(airdrop_signature, signature_callback, "finalized")
	
	# Keep the client node in the tree to keep it processing
	# You should call unsubscribe_all() when you are done.
	
	PASS(3)


func test_project_settings():
	const CORRECT_URL = "http://localhost"
	const INCORRECT_URL = "nonsense url"
	const CORRECT_HTTP_PORT = "8899"
	const INCORRECT_HTTP_PORT = "8898"
	const CORRECT_WS_PORT = "8900"
	const INCORRECT_WS_PORT = "8901"
	
	var client = add_solana_client()
	var ws_client = add_solana_client()
	
	client.get_account_info(EXAMPLE_ACCOUNT)
	var response = await client.http_response_received
	assert(response.has("result"))
	
	ProjectSettings.set_setting("solana_sdk/client/default_url", INCORRECT_URL)
	client.get_account_info(EXAMPLE_ACCOUNT)
	response = await client.http_response_received
	assert(response.is_empty())
	
	ProjectSettings.set_setting("solana_sdk/client/default_url", CORRECT_URL)
	ProjectSettings.set_setting("solana_sdk/client/default_http_port", CORRECT_HTTP_PORT)
	client.get_account_info(EXAMPLE_ACCOUNT)
	response = await client.http_response_received
	assert(response.has("result"))
	
	ProjectSettings.set_setting("solana_sdk/client/default_http_port", INCORRECT_HTTP_PORT)
	client.get_account_info(EXAMPLE_ACCOUNT)
	response = await client.http_response_received
	assert(response.is_empty())
	
	ProjectSettings.set_setting("solana_sdk/client/default_ws_port", INCORRECT_WS_PORT)
	ws_client.account_subscribe(EXAMPLE_ACCOUNT, Callable(self, "_dummy_callback"))
	await ws_client.socket_response_received
	
	ProjectSettings.set_setting("solana_sdk/client/default_ws_port", CORRECT_WS_PORT)
	ws_client.account_subscribe(EXAMPLE_ACCOUNT, Callable(self, "_dummy_callback"))
	await ws_client.socket_response_received
	
	# Port in URL overrides port setting and triggers a warning.
	ProjectSettings.set_setting("solana_sdk/client/default_url", CORRECT_URL + ":" + CORRECT_HTTP_PORT)
	ProjectSettings.set_setting("solana_sdk/client/default_http_port", INCORRECT_HTTP_PORT)
	client.get_account_info(EXAMPLE_ACCOUNT)
	response = await client.http_response_received
	assert(response.has("result"))
	
	# Restore correct settings
	ProjectSettings.set_setting("solana_sdk/client/default_url", CORRECT_URL)
	ProjectSettings.set_setting("solana_sdk/client/default_http_port", CORRECT_HTTP_PORT)
	ProjectSettings.set_setting("solana_sdk/client/default_ws_port", CORRECT_WS_PORT)
	
	PASS(6)


func unsubscribe_account_test():
	var client: SolanaClient = add_solana_client()
	var account_callback := Callable(self, "_should_not_be_called")
	
	var random_account = Pubkey.new_random()
	client.account_subscribe(random_account, account_callback)
	await client.socket_response_received
	client.unsubscribe_all(account_callback)
	
	# Make lamports of the account change to trigger the callback.
	client.request_airdrop(random_account.to_string(), 1000000)
	var airdrop_response = await client.http_response_received
	assert(airdrop_response.has("result"))
	var airdrop_signature: String = airdrop_response["result"]
	
	# Keep the client node in the tree to keep it processing
	# You should call unsubscribe_all() when you are done.
	
	PASS(7)


func test_account_encoding():
	var client = add_solana_client()
	client.set_encoding("base64")

	var account_callback := Callable(self, "_acconunt_encoding_test_callback")
	client.account_subscribe("2kJKEGhqGXJJtoWPfxnKq1Y2bN4eF9GQ39SAcGu8TDZn", account_callback)
	
	client.request_airdrop("2kJKEGhqGXJJtoWPfxnKq1Y2bN4eF9GQ39SAcGu8TDZn", 1000000)
	var airdrop_response = await client.http_response_received
	assert(airdrop_response.has("result"))


func test_das_methods():
	# Testing against official devnet and mainnet addresses.
	# Test is dependent on their normal operation.
	
	const MAINNET_URL = "https://api.mainnet-beta.solana.com:443"
	const DEVNET_URL = "https://api.devnet.solana.com:443"

	var devnet_client = add_solana_client()
	devnet_client.url_override = DEVNET_URL
	var mainnet_client = add_solana_client()
	mainnet_client.url_override = MAINNET_URL
	
	# Examples from https://developers.metaplex.com/das-api/methods/
	const ASSET_ADDRESS = "8vw7tdLGE3FBjaetsJrZAarwsbc8UESsegiLyvWXxs5A"
	const COMPRESSED_ASSET_ADDRESS = "Ez6ezCMkRaUkWS5v6WVcP7uuCWiKadr3W2dHFkoZmteW"
	const OWNER_ADDRESS = "N4f6zftYsuu4yT7icsjLwh4i6pB1zvvKbseHj2NmSQw"
	const AUTHORITY_ADDRESS = "mRdta4rc2RtsxEUDYuvKLamMZAdW6qHcwuq866Skxxv"
	const CREATOR_ADDRESS = "D3XrkNZz6wx6cofot7Zohsf2KSsu2ArngNk8VqU9cTY3"
	const GROUP_ADDRESS = "J2ZfLdQsaZ3GCmbucJef3cPnPwGcgjDW1SSYtMdq3L9p"
	
	
	devnet_client.get_asset(ASSET_ADDRESS)
	var result = await devnet_client.http_response_received
	
	var md = MetaData.new()
	md.copy_from_dict(result["result"])
	
	assert(result.has("result"))
	result = {}
	
	mainnet_client.get_asset_proof(COMPRESSED_ASSET_ADDRESS)
	result = await mainnet_client.http_response_received
	assert(result.has("result"))
	result = {}

	mainnet_client.get_assets_by_authority(AUTHORITY_ADDRESS)
	result = await mainnet_client.http_response_received
	assert(result.has("result"))
	result = {}

	mainnet_client.get_assets_by_creator_address(CREATOR_ADDRESS)
	result = await mainnet_client.http_response_received
	assert(result.has("result"))
	result = {}

	mainnet_client.get_assets_by_owner(Pubkey.new_from_string(OWNER_ADDRESS))
	result = await mainnet_client.http_response_received
	for nft in result["result"]["items"]:
		md = MetaData.new()
		md.copy_from_dict(nft)
	
	assert(result.has("result"))

	mainnet_client.get_assets_by_group("collection", GROUP_ADDRESS)
	result = await mainnet_client.http_response_received
	assert(result.has("result"))
	result = {}
	
	delete_solana_client(devnet_client)
	delete_solana_client(mainnet_client)
	
	PASS(8)

func _ready():
	# Disbled since RPC client does not respond with base64 encoding.
	# test_account_encoding()
	test_das_methods()
	return
	
	get_account_info_demo()
	get_latest_blockhash_demo()
	get_minimum_balance_for_rent_extemption_demo()
	
	subscribe_account_demo()
	await unsubscribe_account_test()
	await test_project_settings()
	test_das_methods()


func _account_subscribe_callback(_params):
	PASS(4)

func _signature_subscribe_callback(_params):
	PASS(5)

func _acconunt_encoding_test_callback(_params):
	PASS(7)

func _should_not_be_called(params):
	assert(false)

func _dummy_callback(_params):
	pass


func _on_timeout_timeout():
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)
