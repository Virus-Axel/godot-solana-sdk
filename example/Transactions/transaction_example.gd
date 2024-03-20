extends VBoxContainer

# Be careful with your keypairs.
@onready var payer: Keypair = Keypair.new_from_file("res://payer.json")
const LAMPORTS_PER_SOL = 1000000000

func CUSTOM_ASSERT(condition: bool):
	assert(condition)
	if !condition:
		get_tree().quit(1)

func transaction_example_airdrop_to():
	# Requesting airdrop is done through the SolanaClient
	# so building a transaction is not necessary
	

	$SolanaClient.request_airdrop(payer.get_public_value(), LAMPORTS_PER_SOL)
	var response = await $SolanaClient.http_response

	# Error check the RPC result
	CUSTOM_ASSERT(response.has("result"))
	var result = response["result"]
	CUSTOM_ASSERT(typeof(result) == TYPE_STRING)
	
	$AirdropSolanaLabel.text += result


func transaction_example_transfer():
	# Build a transaction with a transfer instruction.
	
	var receiver: Pubkey = Pubkey.new_from_string("78GVwUb8ojcJVrEVkwCU5tfUKTfJuiazRrysGwgjqsif")
	var tx = Transaction.new()

	tx.set_url("http://127.0.0.1:8899")
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
	var response = await tx.transaction_response
	
	CUSTOM_ASSERT(response.has("result"))
	var signature = response["result"]
	CUSTOM_ASSERT(typeof(signature) == TYPE_STRING)
	
	$TransferSolanaLabel.text += signature


func create_account_example():
	# Build a transaction with a create account instruction.
	
	var account_key: Keypair = Keypair.new_random()
	var tx = Transaction.new()
	tx.set_url("http://127.0.0.1:8899")
	
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
	var response = await tx.transaction_response
	
	CUSTOM_ASSERT(response.has("result"))
	var signature = response["result"]
	CUSTOM_ASSERT(typeof(signature) == TYPE_STRING)
	
	$CreateAccountLabel.text += signature
	tx.sign_and_send()


func _ready():
	# Use a local cluster for unlimited Solana airdrops.
	# SolanaClient defaults to devnet cluster URL if not specified.
	
	transaction_example_airdrop_to()
	
	# Await the airdrop, SolanaClient.get_signature_statuses() can be used.
	#await get_tree().create_timer(5).timeout
	
	transaction_example_transfer()
	create_account_example()
