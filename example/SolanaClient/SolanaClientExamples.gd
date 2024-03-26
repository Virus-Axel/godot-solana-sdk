extends VBoxContainer

const TOTAL_CASES := 3
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
	
	# Async is true by default
	res.async = true
	
	# RPC HTTP URL goes here.
	res.url = "http://127.0.0.1:8899"

	# Solana Client needs to be in scene tree for async to work.
	add_child(res)
	
	return res

func delete_solana_client(client: SolanaClient):
	remove_child(client)

func get_account_info_demo():
	var client: SolanaClient = add_solana_client()
	client.get_account_info("4sGjMW1sUnHzSxGspuhpqLDx6wiyjNtZAMdL4VZHirAn")
	var response: Dictionary = await client.http_response
	assert(response.has("result"))
	display_dict(response["result"], $ResultTree1.create_item())
	delete_solana_client(client)
	PASS(0)

func get_latest_blockhash_demo():
	var client: SolanaClient = add_solana_client()
	client.get_latest_blockhash()
	var response: Dictionary = await client.http_response
	assert(response.has("result"))
	display_dict(response["result"], $ResultTree2.create_item())
	delete_solana_client(client)
	PASS(1)
	
func get_minimum_balance_for_rent_extemption_demo():
	const EXAMPLE_DATA_SIZE := 100
	var client: SolanaClient = add_solana_client()
	client.get_minimum_balance_for_rent_extemption(EXAMPLE_DATA_SIZE)
	var response: Dictionary = await client.http_response
	assert(response.has("result"))
	display_dict(response["result"], $ResultTree3.create_item())
	delete_solana_client(client)
	PASS(2)

# Called when the node enters the scene tree for the first time.
func _ready():
	get_account_info_demo()
	get_latest_blockhash_demo()
	get_minimum_balance_for_rent_extemption_demo()
	pass


func _on_timeout_timeout():
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)

	get_tree().quit(0)
