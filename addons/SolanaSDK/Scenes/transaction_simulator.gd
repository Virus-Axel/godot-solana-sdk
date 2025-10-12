@tool

extends Control

var before_received = false
var after_received = false

var from_lamports = 0
var result_lamports = 0
var result_data = PackedByteArray()
var simulation_valid = false
var lamport_offset = 0

signal simulation_done

func is_simulation_valid():
	return simulation_valid

func display_error(error: String):
	emit_signal("simulation_done")
	$TextEdit.text = "Simulation Failed\n" + error
	$TextEdit.visible = true

func check_done():
	if before_received and after_received:
		simulation_valid = true
		emit_signal("simulation_done")
		$MarginContainer.visible = true
		$LoadingLabel.visible = false

func simulate(tx: Transaction, account: Pubkey, from_lamports = null) -> void:
	var encoded_transaction = SolanaUtils.bs64_encode(tx.serialize())
	if typeof(from_lamports) == TYPE_INT:
		lamport_offset = from_lamports
	$SolanaClient2.get_account_info(account)
	$SolanaClient.simulate_transaction(encoded_transaction, false, false, [account], "base64")

func get_lamport_offset():
	if is_simulation_valid():
		return from_lamports - result_lamports
	return 0

func _on_solana_client_http_response_received(response: Dictionary) -> void:
	if not response.has("result"):
		display_error("Unexpected response: " + str(response))
		return

	if not response["result"].has("value"):
		display_error("Unexpected response: " + str(response))
		return
		
	if not response["result"]["value"].has("accounts"):
		display_error("Unexpected response: " + str(response))
		return

	if len(response["result"]["value"]["accounts"]) < 1:
		display_error("Unexpected response: " + str(response))
		return

	if typeof(response["result"]["value"]["accounts"][0]) != TYPE_DICTIONARY:
		display_error("Unexpected response: " + str(response))
		return

	if not response["result"]["value"]["accounts"][0].has_all(["lamports", "data"]):
		display_error("Unexpected response: " + str(response))
		return

	var lamports = response["result"]["value"]["accounts"][0]["lamports"]
	var encoded_data = response["result"]["value"]["accounts"][0]["data"][0]
	var data = SolanaUtils.bs64_decode(encoded_data)
	result_lamports = int(lamports) - lamport_offset
	$MarginContainer/VBoxContainer/HBoxContainer/TextEdit2.text = "Lamports: " + str(int(lamports) - lamport_offset)
	$MarginContainer/VBoxContainer/HBoxContainer2/TextEdit2.text = "Data:\n" + str(data)
	before_received = true
	check_done()

func _on_solana_client_2_http_response_received(response: Dictionary) -> void:
	if not response.has("result"):
		display_error("Unexpected response: " + str(response))
		return

	if not response["result"].has("value"):
		display_error("Unexpected response: " + str(response))
		return

	if typeof(response["result"]["value"]) != TYPE_DICTIONARY:
		display_error("Unexpected response: " + str(response))
		return

	if not response["result"]["value"].has_all(["lamports", "data"]):
		display_error("Unexpected response: " + str(response))
		return

	var lamports = response["result"]["value"]["lamports"]
	var encoded_data = response["result"]["value"]["data"][0]
	var data = SolanaUtils.bs64_decode(encoded_data)
	from_lamports = int(lamports) - lamport_offset
	$MarginContainer/VBoxContainer/HBoxContainer/TextEdit.text = "Lamports: " + str(int(lamports) - lamport_offset)
	$MarginContainer/VBoxContainer/HBoxContainer2/TextEdit.text = "Data:\n" + str(data)
	after_received = true
	check_done()
