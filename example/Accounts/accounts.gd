extends Control

const MAXIMUM_ACCOUNTS := 100

func PASS(unique_identifier: int):
	print("[OK]: ", unique_identifier)

func test_n_account_fetch(number_of_accounts: int):
	var start_id = SolanaClient.get_next_request_identifier()
	var acc
	for i in range(0, number_of_accounts):
		acc = Account.new()
		acc.key = Pubkey.new_random()
		add_child(acc)
	acc.key = SystemProgram.get_pid()
	await acc.synced_to_chain

	var stop_id = SolanaClient.get_next_request_identifier()
	
	assert(stop_id - 1 == start_id)

func _ready():
	await test_n_account_fetch(MAXIMUM_ACCOUNTS)

	print("ALL TESTS PASSED")
	get_tree().quit(0)


func _on_account_synced_to_chain() -> void:
	print("AAAAAAAAA")# Replace with function body.
