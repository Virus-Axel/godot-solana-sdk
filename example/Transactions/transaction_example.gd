extends VBoxContainer

# Be careful with your keypairs.
@onready var payer: Keypair = Keypair.new_from_file("res://payer.json")
const LAMPORTS_PER_SOL = 1000000000

const TOTAL_CASES := 16
var passed_test_mask := 0

signal transaction_tests_passed

func PASS(unique_identifier: int):
	passed_test_mask += (1 << unique_identifier)
	print("[OK]: ", unique_identifier)


func transaction_example_airdrop_to():
	# Requesting airdrop is done through the SolanaClient
	# so building a transaction is not necessary
	

	$SolanaClient.request_airdrop(payer.get_public_string(), 3 * LAMPORTS_PER_SOL)
	var response = await $SolanaClient.http_response_received

	# Error check the RPC result
	assert(response.has("result"))
	var result = response["result"]
	assert(typeof(result) == TYPE_STRING)
	
	$AirdropSolanaLabel.text += result
	PASS(0)


func transaction_example_transfer():
	# Build a transaction with a transfer instruction.
	
	var receiver: Pubkey = Pubkey.new_from_string("78GVwUb8ojcJVrEVkwCU5tfUKTfJuiazRrysGwgjqsif")
	var tx = Transaction.new()

	add_child(tx)
	
	# A transaction can be sent after three steps:
	# Set the payer.
	# Add instruction(s).
	# Set latest blockhash.
	
	tx.set_payer(payer)
	
	var ix: Instruction = SystemProgram.transfer(payer, receiver, LAMPORTS_PER_SOL / 10)
	tx.add_instruction(ix)
	
	tx.update_latest_blockhash()

	tx.sign_and_send()
	
	# On success transaction response signal will contain results.
	# Connect it to avoid errors in you application.
	var response = await tx.transaction_response_received
	
	assert(response.has("result"))
	var signature = response["result"]
	assert(typeof(signature) == TYPE_STRING)
	
	$TransferSolanaLabel.text += signature
	
	PASS(1)


func create_account_example():
	# Build a transaction with a create account instruction.
	
	var account_key: Keypair = Keypair.new_random()
	var tx = Transaction.new()
	
	add_child(tx)
	tx.set_payer(payer)
	
	# Example account with 0.1 solana and 400 bytes of data.
	# Use SolanaClient to figure out minimum balance for rent extemption.
	var ix: Instruction = SystemProgram.create_account(payer, account_key, LAMPORTS_PER_SOL / 10, 400, SystemProgram.get_pid())
	tx.add_instruction(ix)
	
	tx.update_latest_blockhash()
	tx.sign_and_send()
	
	# On success transaction response signal will contain results.
	# Connect it to avoid errors in you application.
	var response = await tx.transaction_response_received
	
	assert(response.has("result"))
	var signature = response["result"]
	assert(typeof(signature) == TYPE_STRING)
	
	$CreateAccountLabel.text += signature
	tx.sign_and_send()
	PASS(2)


func transaction_with_confirmation_1():
	# Transfer instruction with connections before getting signature.
	
	var receiver: Pubkey = Pubkey.new_random()
	var tx = Transaction.new()

	add_child(tx)
	
	tx.set_payer(payer)
	var ix: Instruction = SystemProgram.transfer(payer, receiver, LAMPORTS_PER_SOL / 10)
	tx.add_instruction(ix)
	
	tx.update_latest_blockhash()

	tx.connect("processed", Callable(self, "transaction_processed"))
	tx.connect("confirmed", Callable(self, "transaction_confirmed"))
	tx.connect("finalized", Callable(self, "transaction_finalized"))

	tx.sign_and_send()
	var response = await tx.transaction_response_received
	
	assert(response.has("result"))
	var signature = response["result"]
	assert(typeof(signature) == TYPE_STRING)
	
	PASS(3)


func transaction_processed():
	PASS(4)

func transaction_confirmed():
	PASS(5)
	
func transaction_finalized():
	PASS(6)


func transaction_with_confirmation_2():
	# Transfer instruction with connections before after signature.
	
	var receiver: Pubkey = Pubkey.new_random()
	var tx = Transaction.new()

	add_child(tx)
	
	tx.set_payer(payer)
	
	var ix: Instruction = SystemProgram.transfer(payer, receiver, LAMPORTS_PER_SOL / 10)
	tx.add_instruction(ix)
	
	tx.update_latest_blockhash()

	tx.sign_and_send()
	
	var response = await tx.transaction_response_received
	
	assert(response.has("result"))
	var signature = response["result"]
	assert(typeof(signature) == TYPE_STRING)
	
	await tx.processed
	PASS(7)
	await tx.confirmed
	PASS(8)
	await tx.finalized
	PASS(9)


func blockhash_before_instruction():
	# Test that blockhash can be updated before adding instructions.
	
	var account_key: Keypair = Keypair.new_random()
	var tx = Transaction.new()
	
	add_child(tx)
	tx.set_payer(payer)

	tx.update_latest_blockhash()

	var ix: Instruction = SystemProgram.create_account(payer, account_key, LAMPORTS_PER_SOL / 10, 400, SystemProgram.get_pid())
	tx.add_instruction(ix)

	tx.sign_and_send()
	var response = await tx.transaction_response_received
	assert(response.has("result"))

	PASS(10)


func transaction_from_bytes():
	var receiver: Pubkey = Pubkey.new_from_string("78GVwUb8ojcJVrEVkwCU5tfUKTfJuiazRrysGwgjqsif")
	var tx = Transaction.new()

	add_child(tx)
	
	tx.set_payer(payer)
	
	var ix: Instruction = SystemProgram.transfer(payer, receiver, LAMPORTS_PER_SOL / 10)
	tx.add_instruction(ix)

	tx.update_latest_blockhash()

	tx.sign()
	
	var new_tx = Transaction.new_from_bytes(tx.serialize())
	
	var original = tx.serialize()
	var reconstructed = new_tx.serialize()
	
	assert(original == reconstructed)
	
	add_child(new_tx)
	
	# Signers are not stored in the bytes so need to set them.
	new_tx.set_signers([payer])
	new_tx.update_latest_blockhash()
	new_tx.sign()
	new_tx.send()
	# On success transaction response signal will contain results.
	# Connect it to avoid errors in you application.
	var response = await new_tx.transaction_response_received

	assert(response.has("result"))
	var signature = response["result"]
	assert(typeof(signature) == TYPE_STRING)
	
	PASS(11)


func string_as_account_input():
	# Make sure compilation works with strings as account types.
	var receiver: String = Pubkey.new_random().to_string()
	var account_key = Pubkey.new_random().to_string()
	var tx = Transaction.new()

	add_child(tx)
	
	tx.set_payer(payer)
	
	var ix: Instruction = SystemProgram.transfer(payer, receiver, LAMPORTS_PER_SOL / 10)
	tx.add_instruction(ix)
	ix = SystemProgram.transfer(payer, account_key, LAMPORTS_PER_SOL / 10)
	tx.add_instruction(ix)
	ix = SystemProgram.create_account(payer, account_key, LAMPORTS_PER_SOL / 10, 400, SystemProgram.get_pid())
	tx.add_instruction(ix)

	tx.update_latest_blockhash()

	tx.sign() # Will issue a warning about signer type
	
	PASS(12)


func handle_set_payer_on_deserialized_transaction():
	var receiver: Pubkey = Pubkey.new_from_string("78GVwUb8ojcJVrEVkwCU5tfUKTfJuiazRrysGwgjqsif")
	var tx = Transaction.new()

	add_child(tx)
	
	tx.set_payer(payer)
	
	var ix: Instruction = SystemProgram.transfer(payer, receiver, LAMPORTS_PER_SOL / 10)
	tx.add_instruction(ix)

	tx.update_latest_blockhash()

	tx.sign()
	
	var new_tx = Transaction.new_from_bytes(tx.serialize())
	
	var random_payer = Keypair.new_random()
	new_tx.set_payer(random_payer)
	
	var original = tx.serialize()
	var reconstructed = new_tx.serialize()

	assert(original == reconstructed)

	PASS(13)


func send_transaction_from_scene_tree():
	var account_key = Keypair.new_random()
	var ix: Instruction = SystemProgram.create_account(payer, account_key, 1000000, 10, SystemProgram.get_pid())
	$Transaction.set_instructions([ix])
	
	$Transaction.update_latest_blockhash()
	$Transaction.sign_and_send()
	
	var response = await $Transaction.transaction_response_received
	assert(response.has("result"))
	
	PASS(14)

func transaction_simulation_error_signal():
	var account_key = Keypair.new_random()
	var invalid_payer = Keypair.new_random()
	var ix: Instruction = SystemProgram.create_account(invalid_payer, account_key, 1000000, 10, SystemProgram.get_pid())
	var tx = Transaction.new()
	add_child(tx)
	tx.set_payer(invalid_payer)
	tx.set_instructions([ix])

	tx.update_latest_blockhash()
	tx.sign_and_send()

	var message_and_data = await tx.transaction_simulation_failed

	assert(message_and_data[0] == "Attempt to debit an account but found no record of a prior credit.")
	assert(message_and_data[1].has_all(["accounts", "err", "innerInstructions", "logs", "replacementBlockhash", "returnData", "unitsConsumed"]))

	remove_child(tx)

	PASS(15)


func _ready():
	# Use a local cluster for unlimited Solana airdrops.
	# SolanaClient defaults to devnet cluster URL if not specified.
	
	transaction_example_airdrop_to()
	
	# Await the airdrop, SolanaClient.get_signature_statuses() can be used.
	await get_tree().create_timer(5).timeout
	
	# Transaction tests are ran asynchronously. Might change in the future.
	transaction_example_transfer()
	create_account_example()
	transaction_with_confirmation_1()
	transaction_with_confirmation_2()
	blockhash_before_instruction()
	transaction_from_bytes()
	string_as_account_input()
	handle_set_payer_on_deserialized_transaction()
	send_transaction_from_scene_tree()
	transaction_simulation_error_signal()

	await transaction_tests_passed
	
	print("ALL TESTS PASSED")
	get_tree().quit(0)

func _process(delta: float) -> void:
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			return
	
	emit_signal("transaction_tests_passed")
