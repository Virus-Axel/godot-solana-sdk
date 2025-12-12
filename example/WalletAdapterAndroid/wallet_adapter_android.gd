extends Control


const TOTAL_CASES := 3		

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
	# So we just make sure the error is not due to transaciton signature failure.
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


func _ready():
	await wallet_adapter_sign(0)
	await wallet_adapter_sign(1)
	
	# TODO(VirAx): Enable once implemented.
	#await sign_text_message(3)
	await wallet_adapter_in_deserialized_transaction()
	
	print("[ALL TESTS PASSED]")
