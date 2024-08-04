extends VBoxContainer

const EXAMPLE_ACCOUNT := "4sGjMW1sUnHzSxGspuhpqLDx6wiyjNtZAMdL4VZHirAn"

const TOTAL_CASES := 10
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

func synchronous_client_call():
	# Same call to get account info but synchronous.
	var client = SolanaClient.new()
	var response = client.get_account_info(EXAMPLE_ACCOUNT)
	assert(response.has("result"))
	display_dict(response["result"], $ResultTree4.create_item())
	PASS(6)


func test_project_settings():
	const CORRECT_URL = "http://localhost"
	const INCORRECT_URL = "nonsense url"
	const CORRECT_HTTP_PORT = "8899"
	const INCORRECT_HTTP_PORT = "8898"
	const CORRECT_WS_PORT = "8900"
	const INCORRECT_WS_PORT = "8901"
	
	var client = SolanaClient.new()
	var ws_client = add_solana_client()
	
	var response = client.get_account_info(EXAMPLE_ACCOUNT)
	assert(response.has("result"))
	ProjectSettings.set_setting("solana_sdk/client/default_url", INCORRECT_URL)
	response = client.get_account_info(EXAMPLE_ACCOUNT)
	assert(!response.has("result"))
	ProjectSettings.set_setting("solana_sdk/client/default_url", CORRECT_URL)
	ProjectSettings.set_setting("solana_sdk/client/default_http_port", CORRECT_HTTP_PORT)
	response = client.get_account_info(EXAMPLE_ACCOUNT)
	assert(response.has("result"))
	ProjectSettings.set_setting("solana_sdk/client/default_http_port", INCORRECT_HTTP_PORT)
	response = client.get_account_info(EXAMPLE_ACCOUNT)
	assert(!response.has("result"))
	
	ProjectSettings.set_setting("solana_sdk/client/default_ws_port", INCORRECT_WS_PORT)
	ws_client.account_subscribe(EXAMPLE_ACCOUNT, Callable(self, "_pass_if_failed"))
	
	ProjectSettings.set_setting("solana_sdk/client/default_ws_port", CORRECT_WS_PORT)
	ws_client.account_subscribe(EXAMPLE_ACCOUNT, Callable(self, "_pass_if_succeded"))
	
	# Port in URL overrides port setting and triggers a warning.
	ProjectSettings.set_setting("solana_sdk/client/default_url", CORRECT_URL + ":" + CORRECT_HTTP_PORT)
	ProjectSettings.set_setting("solana_sdk/client/default_http_port", INCORRECT_HTTP_PORT)
	response = client.get_account_info(EXAMPLE_ACCOUNT)
	assert(response.has("result"))
	
	# Restore correct settings
	ProjectSettings.set_setting("solana_sdk/client/default_url", CORRECT_URL)
	ProjectSettings.set_setting("solana_sdk/client/default_http_port", CORRECT_HTTP_PORT)
	ProjectSettings.set_setting("solana_sdk/client/default_ws_port", CORRECT_WS_PORT)
	
	PASS(7)


func test_account_encoding():
	var client = add_solana_client()
	client.set_encoding("base64")

	var account_callback := Callable(self, "_acconunt_encoding_test_callback")
	client.account_subscribe("2kJKEGhqGXJJtoWPfxnKq1Y2bN4eF9GQ39SAcGu8TDZn", account_callback)
	
	client.request_airdrop("2kJKEGhqGXJJtoWPfxnKq1Y2bN4eF9GQ39SAcGu8TDZn", 1000000)
	var airdrop_response = await client.http_response_received
	assert(airdrop_response.has("result"))


func _ready():
	# Disbled since RPC client does not respond with base64 encoding.
	# test_account_encoding()
	get_account_info_demo()
	get_latest_blockhash_demo()
	get_minimum_balance_for_rent_extemption_demo()
	subscribe_account_demo()
	synchronous_client_call()
	test_project_settings()


func _account_subscribe_callback(_params):
	PASS(4)

func _signature_subscribe_callback(_params):
	PASS(5)

func _acconunt_encoding_test_callback(_params):
	PASS(8)

func _pass_if_failed(params):
	assert(not params.has("result"))
	PASS(8)

func _pass_if_succeded(params):
	assert(params.has("result"))
	PASS(9)


func _on_timeout_timeout():
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)
