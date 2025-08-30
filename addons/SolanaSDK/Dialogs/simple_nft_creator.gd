@tool
extends Control

const MINT_SIZE := 234
var passed_test_mask : int = 0

const NAME = "Token"
const SYMBOL = "TK"
const URI = "https://raw.githubusercontent.com/solana-developers/opos-asset/main/assets/DeveloperPortal/metadata.json"

const CUSTOM_FIELD_NAME = "color"
const CUSTOM_FIELD_VALUE = "blue"

var API_ENDPOINT = "https://cryptoforts.com/arweave"

func set_status_message(msg):
	$VBoxContainer/LineEdit.text = msg
	$VBoxContainer/LineEdit.visible = true

func set_done(token_address: Pubkey):
	$VBoxContainer/LineEdit.text = "Token Address: " + token_address.to_string()
	$VBoxContainer/LineEdit.visible = true
	$VBoxContainer/CreateButton.text = "Done"
	$VBoxContainer/CreateButton.disabled = true
	

func PASS(unique_identifier: int):
	passed_test_mask += (1 << unique_identifier)
	print("[OK]: ", unique_identifier)


func minumum_balance_for_rent_extemtion(data_size):
	var client = SolanaClient.new()
	add_child(client)
	client.url_override = $Transaction.url_override
	client.get_minimum_balance_for_rent_extemption(data_size)
	var result = await client.http_response_received
	print(result)
	assert(result.has("result"))
	remove_child(client)
	return result['result']


func _ready() -> void:
	print("Hi im ready")

func can_create() -> bool:
	return not ($VBoxContainer/HBoxContainer2/PayerDialog.current_file.is_empty() or $VBoxContainer/HBoxContainer/FileDialog.current_file.is_empty())

func _on_button_pressed() -> void:
	$VBoxContainer/HBoxContainer/FileDialog.visible = true

func _on_file_dialog_file_selected(path: String) -> void:
	$VBoxContainer/HBoxContainer/LineEdit.text = path.get_file()
	$VBoxContainer/CreateButton.disabled = not can_create()
	$VBoxContainer/SimulateButton.disabled = not can_create()

func _on_button_pressed2() -> void:
	$VBoxContainer/HBoxContainer2/PayerDialog.visible = true

func _on_payer_dialog_file_selected(path: String) -> void:
	$VBoxContainer/HBoxContainer2/LineEdit.text = path.get_file()
	$Transaction.set_payer(load(path))
	print(path)
	print($Transaction.payer)
	$VBoxContainer/CreateButton.disabled = not can_create()
	$VBoxContainer/SimulateButton.disabled = not can_create()


func _on_option_button_item_selected(index: int) -> void:
	if index == 0:
		$Transaction.url_override = "https://api.devnet.solana.com:443"
	elif index == 1:
		$Transaction.url_override = "https://api.mainnet-beta.solana.com:443"
	elif index == 2:
		$Transaction.url_override =  "https://api.testnet.solana.com:443"
	elif index == 3:
		$Transaction.url_override =  "http://localhost:8899"

func display_error(error: String):
	set_status_message("Error: " + error)
	$VBoxContainer/CreateButton.text = "Failed"
	$VBoxContainer/CreateButton.disabled = true

func get_arweave_transaction(payer_kp: Keypair, file_name: String, file_data: PackedByteArray):
	var arweave_error = "Simulating Arweave upload failed\n"

	var boundary = "----GodotBoundary123456"
	var body = []

	body.append_array(("--" + boundary + "\r\n").to_utf8_buffer())
	body.append_array(("Content-Disposition: form-data; name=\"file\"; filename=\"" + file_name + "\"\r\n").to_utf8_buffer())
	body.append_array("Content-Type: application/octet-stream\r\n\r\n".to_utf8_buffer())
	body.append_array(file_data)
	body.append_array("\r\n".to_utf8_buffer())

	body.append_array(("--" + boundary + "--\r\n").to_utf8_buffer())

	var headers = ["Content-Type: multipart/form-data; boundary=" + boundary, "Content-Length: " + str(body.size())]

	var get_price_path = API_ENDPOINT + "/upload_price_sol"

	var error = $HTTPRequest_real.request_raw(get_price_path, headers, HTTPClient.METHOD_POST, body)
	if error != OK:
		display_error(arweave_error + "Failed to send request")
		return null
	
	var upload_price_str = await $HTTPRequest_real.request_completed
	
	if upload_price_str[0] != HTTPRequest.RESULT_SUCCESS:
		display_error(arweave_error + "Unexpected request response: " + str(upload_price_str[3].get_string_from_ascii()))
		return null
	if upload_price_str[1] != HTTPClient.RESPONSE_OK:
		display_error(arweave_error + "Unexpected request response: " + str(upload_price_str[3].get_string_from_ascii()))
		return null

	var upload_price = int(str_to_var(upload_price_str[3].get_string_from_ascii()) * 1000000000 * 1.05)
	
	$SolanaClient2.get_latest_blockhash()
	var bh = await $SolanaClient2.http_response_received
	if not bh.has("result"):
		display_error(arweave_error + "Failed to fetch blockhash: " + str(bh))
		return null
	if not bh["result"].has("value"):
		display_error(arweave_error + "Failed to fetch blockhash: " + str(bh))
		return null
	if not bh["result"]["value"].has("blockhash"):
		display_error(arweave_error + "Failed to fetch blockhash: " + str(bh))
		return null
	
	var transaction_path = API_ENDPOINT + "/transaction?payer_pubkey=" + payer_kp.get_public_string() + "&blockhash=" + bh["result"]["value"]["blockhash"] + "&amount=" + str(upload_price)
	
	error = $HTTPRequest_real.request(transaction_path)
	if error != OK:
		display_error(arweave_error + "Failed to send request")
		return null

	var dat = await $HTTPRequest_real.request_completed
	if dat[0] != HTTPRequest.RESULT_SUCCESS:
		display_error(arweave_error + "Unexpected request response: " + str(dat[3].get_string_from_ascii()))
		return null
	if dat[1] != HTTPClient.RESPONSE_OK:
		display_error(arweave_error + "Unexpected request response: " + str(dat[3].get_string_from_ascii()))
		return null

	var encoded = dat[3].get_string_from_ascii()

	var tx = Transaction.new_from_bytes(SolanaUtils.bs64_decode(encoded))
	add_child(tx)
	tx.set_signers([payer_kp])
	tx.sign()
	
	return tx

func upload_file_to_arweave(payer_kp: Keypair, file_name: String, file_data):
	var arweave_error = "Uploading file to arweave failed\n"

	var boundary = "----GodotBoundary123456"
	var body = []

	body.append_array(("--" + boundary + "\r\n").to_utf8_buffer())
	body.append_array(("Content-Disposition: form-data; name=\"file\"; filename=\"" + file_name + "\"\r\n").to_utf8_buffer())
	body.append_array("Content-Type: application/octet-stream\r\n\r\n".to_utf8_buffer())
	body.append_array(file_data)
	body.append_array("\r\n".to_utf8_buffer())

	body.append_array(("--" + boundary + "--\r\n").to_utf8_buffer())

	var headers = ["Content-Type: multipart/form-data; boundary=" + boundary, "Content-Length: " + str(body.size())]

	var get_price_path = API_ENDPOINT + "/upload_price_sol"
	var error = $HTTPRequest_real.request_raw(get_price_path, headers, HTTPClient.METHOD_POST, body)
	if error != OK:
		display_error(arweave_error + "Failed to send request")
		return
	
	var upload_price_str = await $HTTPRequest_real.request_completed
	
	if upload_price_str[0] != HTTPRequest.RESULT_SUCCESS:
		display_error(arweave_error + "Unexpected request response: " + str(upload_price_str[3].get_string_from_ascii()))
		return
	if upload_price_str[1] != HTTPClient.RESPONSE_OK:
		display_error(arweave_error + "Unexpected request response: " + str(upload_price_str[3].get_string_from_ascii()))
		return

	var upload_price = int(str_to_var(upload_price_str[3].get_string_from_ascii()) * 1000000000 * 1.05)
	
	$SolanaClient2.get_latest_blockhash()
	var bh = await $SolanaClient2.http_response_received
	if not bh.has("result"):
		display_error(arweave_error + "Failed to fetch blockhash: " + str(bh))
		return
	if not bh["result"].has("value"):
		display_error(arweave_error + "Failed to fetch blockhash: " + str(bh))
		return
	if not bh["result"]["value"].has("blockhash"):
		display_error(arweave_error + "Failed to fetch blockhash: " + str(bh))
		return
	
	var transaction_path = API_ENDPOINT + "/transaction?payer_pubkey=" + payer_kp.get_public_string() + "&blockhash=" + bh["result"]["value"]["blockhash"] + "&amount=" + str(upload_price)
	
	error = $HTTPRequest_real.request(transaction_path)
	if error != OK:
		display_error(arweave_error + "Failed to send request")
		return

	var dat = await $HTTPRequest_real.request_completed
	if dat[0] != HTTPRequest.RESULT_SUCCESS:
		display_error(arweave_error + "Unexpected request response: " + str(dat[3].get_string_from_ascii()))
		return
	if dat[1] != HTTPClient.RESPONSE_OK:
		display_error(arweave_error + "Unexpected request response: " + str(dat[3].get_string_from_ascii()))
		return

	var encoded = dat[3].get_string_from_ascii()

	var tx = Transaction.new_from_bytes(SolanaUtils.bs64_decode(encoded))
	add_child(tx)
	tx.set_signers([payer_kp])
	tx.sign()
	
	var signature = SolanaUtils.bs58_encode(tx.serialize().slice(1, 65))
	var send_path = API_ENDPOINT + "/upload?payer_pubkey=" + payer_kp.get_public_string() + "&blockhash=" + bh["result"]["value"]["blockhash"] + "&transfer_amount=" + str(upload_price) + "&signature=" + signature

	error = $HTTPRequest_real.request_raw(send_path, headers, HTTPClient.METHOD_POST, body)
	
	if error != OK:
		display_error(arweave_error + "Failed to send request")
		return
	
	dat = await $HTTPRequest_real.request_completed
	if dat[0] != HTTPRequest.RESULT_SUCCESS:
		display_error(arweave_error + "Unexpected request response: " + str(dat[3].get_string_from_ascii()))
		return
	
	if dat[1] != HTTPClient.RESPONSE_OK:
		display_error(arweave_error + "Unexpected request response: " + str(dat[3].get_string_from_ascii()))
		return
	
	# Assumptions made of the server output here.
	var cutoff = 2 + file_name.length()
	var strings = dat[3].get_string_from_ascii().split('\n')
	var id = strings[0].substr(cutoff)
	
	return "https://arweave.net/" + id


func setup_tx3(token_name, token_symbol, image_url):
	# Ask our RPC server how much we need to transfer to our new mint account.
	var account_size = TokenProgram2022.get_mint_account_size_from_data(token_name, token_symbol, image_url, {}) + 4
	var mint_lamports = await minumum_balance_for_rent_extemtion(account_size)
	
	$Transaction.instructions.clear()
	var mint_keypair: Keypair = Keypair.new_random()

	# Append instructions.
	var ix: Instruction = SystemProgram.create_account($Transaction.payer, mint_keypair, mint_lamports, MINT_SIZE, TokenProgram2022.get_pid())
	$Transaction.add_instruction(ix)
	ix = TokenProgram2022.initialize_metadata_pointer($Transaction.payer, mint_keypair)
	$Transaction.add_instruction(ix)
	ix = TokenProgram2022.initialize_mint(mint_keypair, $Transaction.payer, $Transaction.payer, 0)
	$Transaction.add_instruction(ix)
	ix = TokenProgram2022.initialize_metadata($Transaction.payer, mint_keypair, token_name, token_symbol, image_url)
	$Transaction.add_instruction(ix)
	
	print($Transaction.payer.get_data())
	
	var mint_account_keypair = Pubkey.new_associated_token_address($Transaction.payer, mint_keypair, TokenProgram2022.get_pid())
	var recv = Pubkey.new_from_string("8FcV1zXqopri4KfRsTqH4RxTcVdxMwnVskHsby7KRYtm")
	
	const MINT_ACCOUNT_SIZE := 165
	var mint_account_lamports = await minumum_balance_for_rent_extemtion(MINT_ACCOUNT_SIZE)
	
	#ix = SystemProgram.create_account($Transaction.payer, mint_account_keypair, mint_account_lamports, MINT_ACCOUNT_SIZE, TokenProgram2022.get_pid())
	#$Transaction.add_instruction(ix)
	ix = AssociatedTokenAccountProgram.create_associated_token_account($Transaction.payer, $Transaction.payer, mint_keypair, TokenProgram2022.get_pid())
	$Transaction.add_instruction(ix)
	ix = TokenProgram2022.mint_to(mint_keypair, mint_account_keypair, $Transaction.payer, $Transaction.payer, 1)
	$Transaction.add_instruction(ix)
	
	ix = TokenProgram2022.set_authority(mint_keypair, $Transaction.payer, null, 0)
	$Transaction.add_instruction(ix)
	
	$Transaction.update_latest_blockhash()
	print("awaiting blockhash")
	await $Transaction.blockhash_updated
	
	return mint_keypair


func _on_create_button_pressed() -> void:
	$VBoxContainer/CreateButton.disabled = true
	$VBoxContainer/CreateButton.text = "Creating"
	
	var token_name = $VBoxContainer/TokenSettings/HBoxContainer/TextEdit.text
	var token_symbol = $VBoxContainer/TokenSettings/HBoxContainer2/TextEdit.text
	var token_description = $VBoxContainer/TokenSettings/HBoxContainer4/TextEdit.text

	var payer_kp = Keypair.new_from_file($VBoxContainer/HBoxContainer2/PayerDialog.current_file)
	if typeof(payer_kp) != TYPE_OBJECT:
		display_error("Unexpected payer type")
		return
	if payer_kp.get_public_bytes().size() != 32:
		display_error("Unexpected payer type")
		return
	
	var file_path = $VBoxContainer/HBoxContainer/FileDialog.current_file
	
	var file = FileAccess.open(file_path, FileAccess.READ)
	if not file:
		display_error("Could not open file: " + file_path)
		return

	var file_data = file.get_buffer(file.get_length())
	file.close()
	
	set_status_message("Uploading image to Arweave...")
	var image_url = await upload_file_to_arweave(payer_kp, file_path.get_file(), file_data)
	print(image_url)
	
	var lamport_offset = $VBoxContainer/ScrollContainer/VBoxContainer/HBoxContainer3/TransactionSimulator.get_lamport_offset()
	
	var offchain_metadata = {}
	offchain_metadata["name"] = token_name
	offchain_metadata["symbol"] = token_symbol
	offchain_metadata["description"] = token_description
	offchain_metadata["image"] = image_url
	if $VBoxContainer/CustomFields.get_child_count() > 0:
		var custom_fields = {}
		for child in $VBoxContainer/CustomFields.get_children():
			custom_fields[child.get_field()] = child.get_value()
		offchain_metadata["attributes"] = custom_fields
	
	set_status_message("Uploading metadata to Arweave...")
	var metadata_url = await upload_file_to_arweave(payer_kp, "metadata.json", str(offchain_metadata).to_ascii_buffer())
	print(metadata_url)

	print("getting last tx")
	
	set_status_message("Sending token creation transaction...")
	var mint_kp = await setup_tx3(token_name, token_symbol, metadata_url)
	print("sending")
	$Transaction.sign_and_send()
	await $Transaction.finalized
	set_done(mint_kp.to_pubkey())


func _create_custom_field() -> void:
	var new_field = load("res://addons/SolanaSDK/Scenes/CustomField.tscn").instantiate()
	$VBoxContainer/CustomFields.add_child(new_field)
	new_field.connect("delete", new_field.remove_self)


func _on_simulate_button_pressed() -> void:
	var token_name = $VBoxContainer/TokenSettings/HBoxContainer/TextEdit.text
	var token_symbol = $VBoxContainer/TokenSettings/HBoxContainer2/TextEdit.text
	var token_description = $VBoxContainer/TokenSettings/HBoxContainer4/TextEdit.text

	var payer_kp = Keypair.new_from_file($VBoxContainer/HBoxContainer2/PayerDialog.current_file)
	if typeof(payer_kp) != TYPE_OBJECT:
		display_error("Unexpected payer type")
		return
	if payer_kp.get_public_bytes().size() != 32:
		display_error("Unexpected payer type")
		return
	
	var file_path = $VBoxContainer/HBoxContainer/FileDialog.current_file
	
	var file = FileAccess.open(file_path, FileAccess.READ)
	if not file:
		display_error("Could not open file: " + file_path)
		return

	var file_data = file.get_buffer(file.get_length())
	file.close()
	
	$VBoxContainer/ScrollContainer.visible = true
	var tx1 = await get_arweave_transaction(payer_kp, file_path.get_file(), file_data)
	
	$VBoxContainer/ScrollContainer/VBoxContainer/HBoxContainer3/TransactionSimulator.simulate(tx1, payer_kp.to_pubkey())
	await $VBoxContainer/ScrollContainer/VBoxContainer/HBoxContainer3/TransactionSimulator.simulation_done
	
	var lamport_offset = $VBoxContainer/ScrollContainer/VBoxContainer/HBoxContainer3/TransactionSimulator.get_lamport_offset()
	
	var placeholder_url = "https://arxrevyomdv434gyjpmw5bqhf2oat2t3jspvyyu2rsvty33xpyfa.arweave.net/BG8SVw5g683w2EvZboYHLpwJ6ntMn1ximoyrPG93fgo"
	var offchain_metadata = {}
	offchain_metadata["name"] = token_name
	offchain_metadata["symbol"] = token_symbol
	offchain_metadata["description"] = token_description
	offchain_metadata["image"] = placeholder_url
	if $VBoxContainer/CustomFields.get_child_count() > 0:
		var custom_fields = {}
		for child in $VBoxContainer/CustomFields.get_children():
			custom_fields[child.get_field()] = child.get_value()
		offchain_metadata["attributes"] = custom_fields
	
	var tx2 = await get_arweave_transaction(payer_kp, "metadata.json", str(offchain_metadata).to_ascii_buffer())
	$VBoxContainer/ScrollContainer/VBoxContainer/HBoxContainer4/TransactionSimulator.simulate(tx2, payer_kp.to_pubkey(), lamport_offset)
	await $VBoxContainer/ScrollContainer/VBoxContainer/HBoxContainer4/TransactionSimulator.simulation_done
	lamport_offset += $VBoxContainer/ScrollContainer/VBoxContainer/HBoxContainer4/TransactionSimulator.get_lamport_offset()

	await setup_tx3(token_name, token_symbol, placeholder_url)
	$VBoxContainer/ScrollContainer/VBoxContainer/HBoxContainer5/TransactionSimulator.simulate($Transaction, payer_kp.to_pubkey(), lamport_offset)
