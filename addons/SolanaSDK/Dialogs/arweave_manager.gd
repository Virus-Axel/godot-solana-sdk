@tool

extends Control

var API_ENDPOINT = "https://cryptoforts.com/arweave"

func clear_error():
	$VBoxContainer/TextEdit.visible = false
	$VBoxContainer/TextEdit.text = ""

func display_error(error: String):
	$VBoxContainer/TextEdit.visible = true
	$VBoxContainer/TransactionSimulator.visible = false
	$VBoxContainer/TextEdit.text = error
	$VBoxContainer/CreateButton.text = "Upload (Mainnet)"

func start_tx_simulation():
	$VBoxContainer/TransactionSimulator.visible = true
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

	var boundary = "----GodotBoundary123456"
	var body = []

	body.append_array(("--" + boundary + "\r\n").to_utf8_buffer())
	body.append_array(("Content-Disposition: form-data; name=\"file\"; filename=\"" + file_path.get_file() + "\"\r\n").to_utf8_buffer())
	body.append_array("Content-Type: application/octet-stream\r\n\r\n".to_utf8_buffer())
	body.append_array(file_data)
	body.append_array("\r\n".to_utf8_buffer())

	body.append_array(("--" + boundary + "--\r\n").to_utf8_buffer())

	var headers = ["Content-Type: multipart/form-data; boundary=" + boundary, "Content-Length: " + str(body.size())]

	var get_price_path = API_ENDPOINT + "/upload_price_sol"
	var error = $HTTPRequest_sim.request_raw(get_price_path, headers, HTTPClient.METHOD_POST, body)
	if error != OK:
		display_error("Failed to send request")
		return
	
	var upload_price_str = await $HTTPRequest_sim.request_completed
	
	if upload_price_str[0] != HTTPRequest.RESULT_SUCCESS:
		display_error("Unexpected request response: " + str(upload_price_str[3].get_string_from_ascii()))
		return
	if upload_price_str[1] != HTTPClient.RESPONSE_OK:
		display_error("Unexpected request response: " + str(upload_price_str[3].get_string_from_ascii()))
		return
	
	var upload_price = int(str_to_var(upload_price_str[3].get_string_from_ascii()) * 1000000000 * 1.05)
	
	$SolanaClient.get_latest_blockhash()
	var bh = (await $SolanaClient.http_request_completed)[1]
	if not bh.has("result"):
		display_error("Failed to fetch blockhash: " + str(bh))
		return
	if not bh["result"].has("value"):
		display_error("Failed to fetch blockhash: " + str(bh))
		return
	if not bh["result"]["value"].has("blockhash"):
		display_error("Failed to fetch blockhash: " + str(bh))
		return

	var amount = 0
	
	var transaction_path = API_ENDPOINT + "/transaction?payer_pubkey=" + payer_kp.get_public_string() + "&blockhash=" + bh["result"]["value"]["blockhash"] + "&amount=" + str(upload_price)
	
	$HTTPRequest_sim.request(transaction_path)
	var dat = await $HTTPRequest_sim.request_completed
	var encoded = dat[3].get_string_from_ascii()

	var tx = Transaction.new_from_bytes(SolanaUtils.bs64_decode(encoded))
	add_child(tx)
	tx.set_signers([payer_kp])
	tx.sign()

	$VBoxContainer/TransactionSimulator.simulate(tx, payer_kp.to_pubkey())
	remove_child(tx)


func can_upload() -> bool:
	return not ($VBoxContainer/HBoxContainer2/PayerDialog.current_file.is_empty() or $VBoxContainer/HBoxContainer/FileDialog.current_file.is_empty())

func _on_button_pressed() -> void:
	$VBoxContainer/HBoxContainer/FileDialog.visible = true

func _on_file_dialog_file_selected(path: String) -> void:
	clear_error()
	$VBoxContainer/HBoxContainer/LineEdit.text = path.get_file()
	$VBoxContainer/CreateButton.disabled = not can_upload()
	if not $VBoxContainer/CreateButton.disabled:
		start_tx_simulation()

func _on_button_pressed2() -> void:
	$VBoxContainer/HBoxContainer2/PayerDialog.visible = true

func _on_payer_dialog_file_selected(path: String) -> void:
	clear_error()
	$VBoxContainer/HBoxContainer2/LineEdit.text = path.get_file()
	$VBoxContainer/CreateButton.disabled = not can_upload()
	if not $VBoxContainer/CreateButton.disabled:
		start_tx_simulation()


func _on_create_button_pressed() -> void:
	clear_error()
	$VBoxContainer/CreateButton.disabled = true
	$VBoxContainer/CreateButton.text = "Uploading..."
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

	var boundary = "----GodotBoundary123456"
	var body = []

	body.append_array(("--" + boundary + "\r\n").to_utf8_buffer())
	body.append_array(("Content-Disposition: form-data; name=\"file\"; filename=\"" + file_path.get_file() + "\"\r\n").to_utf8_buffer())
	body.append_array("Content-Type: application/octet-stream\r\n\r\n".to_utf8_buffer())
	body.append_array(file_data)
	body.append_array("\r\n".to_utf8_buffer())

	body.append_array(("--" + boundary + "--\r\n").to_utf8_buffer())

	var headers = ["Content-Type: multipart/form-data; boundary=" + boundary, "Content-Length: " + str(body.size())]

	var get_price_path = API_ENDPOINT + "/upload_price_sol"
	var error = $HTTPRequest_real.request_raw(get_price_path, headers, HTTPClient.METHOD_POST, body)
	if error != OK:
		display_error("Failed to send request")
		return
	
	var upload_price_str = await $HTTPRequest_real.request_completed
	
	if upload_price_str[0] != HTTPRequest.RESULT_SUCCESS:
		display_error("Unexpected request response: " + str(upload_price_str[3].get_string_from_ascii()))
		return
	if upload_price_str[1] != HTTPClient.RESPONSE_OK:
		display_error("Unexpected request response: " + str(upload_price_str[3].get_string_from_ascii()))
		return

	var upload_price = int(str_to_var(upload_price_str[3].get_string_from_ascii()) * 1000000000 * 1.05)
	
	$SolanaClient2.get_latest_blockhash()
	var bh = (await $SolanaClient2.http_request_completed)[1]
	if not bh.has("result"):
		display_error("Failed to fetch blockhash: " + str(bh))
		return
	if not bh["result"].has("value"):
		display_error("Failed to fetch blockhash: " + str(bh))
		return
	if not bh["result"]["value"].has("blockhash"):
		display_error("Failed to fetch blockhash: " + str(bh))
		return

	var amount = 0
	
	var transaction_path = API_ENDPOINT + "/transaction?payer_pubkey=" + payer_kp.get_public_string() + "&blockhash=" + bh["result"]["value"]["blockhash"] + "&amount=" + str(upload_price)
	
	error = $HTTPRequest_real.request(transaction_path)
	if error != OK:
		display_error("Failed to send request")
		return

	var dat = await $HTTPRequest_real.request_completed
	if dat[0] != HTTPRequest.RESULT_SUCCESS:
		display_error("Unexpected request response: " + str(dat[3].get_string_from_ascii()))
		return
	if dat[1] != HTTPClient.RESPONSE_OK:
		display_error("Unexpected request response: " + str(dat[3].get_string_from_ascii()))
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
		display_error("Failed to send request")
		return
	
	dat = await $HTTPRequest_real.request_completed
	if dat[0] != HTTPRequest.RESULT_SUCCESS:
		display_error("Unexpected request response: " + str(dat[3].get_string_from_ascii()))
		return
	
	if dat[1] != HTTPClient.RESPONSE_OK:
		display_error("Unexpected request response: " + str(dat[3].get_string_from_ascii()))
		return
	
	# Assumptions made of the server output here.
	var cutoff = 2 + file_path.get_file().length()
	var strings = dat[3].get_string_from_ascii().split('\n')
	var id = strings[0].substr(cutoff)
	$VBoxContainer/HBoxContainer3.visible = true
	$VBoxContainer/HBoxContainer3/Label.text = "https://arweave.net/" + id
	$VBoxContainer/CreateButton.text = "Done!"


func clear_window() -> void:
	$VBoxContainer/TextEdit.visible = false
	$VBoxContainer/TransactionSimulator.visible = false
	$VBoxContainer/HBoxContainer3.visible = false


func _on_ready() -> void:
	clear_window()
