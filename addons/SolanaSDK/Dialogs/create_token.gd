@tool
extends Control

func update_todo():
	var todo_text = ""
	if not $MarginContainer/VBoxContainer/KeypairSelector.is_valid_keypair():
		todo_text += "- No valid token keypair selected\n"
	if not $MarginContainer/VBoxContainer/KeypairSelector2.is_valid_keypair():
		todo_text += "- No valid authority selected\n"
	$MarginContainer/VBoxContainer/Label2.text = todo_text
	
	$MarginContainer/VBoxContainer/HBoxContainer/SimulateButton.disabled = !todo_text.is_empty()
	$MarginContainer/VBoxContainer/HBoxContainer/CreateButton.disabled = !todo_text.is_empty()


func get_create_instructions():
	var payer = $MarginContainer/VBoxContainer/KeypairSelector2.selected_keypair
	var auth = $MarginContainer/VBoxContainer/KeypairSelector2.selected_keypair
	var token_keypair = $MarginContainer/VBoxContainer/KeypairSelector.selected_keypair
	print(auth.get_public_string())
	
	$SolanaClient.get_minimum_balance_for_rent_extemption(TokenProgram.TOKEN_MINT_SIZE);
	var response = await $SolanaClient.http_response_received

	if not response.has("result"):
		push_error("Invalid response from RPC server")
		return []
	if typeof(response["result"]) != TYPE_FLOAT:
		push_error("Invalid response from RPC server")
		return []
	
	var lamports = response["result"]
	
	if $MarginContainer/VBoxContainer/HBoxContainer2/OptionButton.selected == 0:
		return [
			SystemProgram.create_account(payer, token_keypair, lamports, TokenProgram.TOKEN_MINT_SIZE, TokenProgram2022.get_pid()),
			TokenProgram2022.initialize_mint(token_keypair, auth, auth, 0)
		]
	elif $MarginContainer/VBoxContainer/HBoxContainer2/OptionButton.selected == 1:
		return [
			SystemProgram.create_account(payer, token_keypair, lamports, TokenProgram.TOKEN_MINT_SIZE, TokenProgram.get_pid()),
			TokenProgram.initialize_mint(token_keypair, auth, auth, 0)
		]


func _ready() -> void:
	update_todo()


func setup_transaction():
	if $MarginContainer/VBoxContainer/HBoxContainer2/OptionButton.selected == 1:
		var token_program = TokenProgram2022
	
	var payer = $MarginContainer/VBoxContainer/KeypairSelector2.selected_keypair
	var token_keypair = $MarginContainer/VBoxContainer/KeypairSelector.selected_keypair
	print(payer.get_public_string())
	$Transaction.set_payer(payer)
	
	$Transaction.instructions.clear()
	
	$Transaction.url_override = $MarginContainer/VBoxContainer/RpcSelector.get_selected_address()
	$SolanaClient.url_override = $MarginContainer/VBoxContainer/RpcSelector.get_selected_address()

	$Transaction.instructions = await get_create_instructions()

func _on_create_button_pressed() -> void:
	setup_transaction()
	
	$Transaction.update_latest_blockhash()
	$Transaction.sign_and_send()



func _on_keypair_selector_invalid_keypair_selected() -> void:
	push_error("Invalid Keypair selected")
	update_todo()


func _on_keypair_selector_2_invalid_keypair_selected() -> void:
	push_error("Invalid Keypair selected")
	update_todo()


func _on_transaction_transaction_response_received(result: Dictionary) -> void:
	print(result)


func _on_simulate_button_pressed() -> void:
	setup_transaction()
	
	$Transaction.update_latest_blockhash()
	$MarginContainer/VBoxContainer/TransactionSimulator.visible = true
	await $Transaction.blockhash_updated
	$MarginContainer/VBoxContainer/TransactionSimulator.simulate($Transaction, $Transaction.get_payer().to_pubkey())
