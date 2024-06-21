extends Control

const TOTAL_CASES := 2
const MINT_SIZE := 234
var passed_test_mask : int = 0

const NAME = "Token"
const SYMBOL = "TK"
const URI = "myUri"

const CUSTOM_FIELD_NAME = "color"
const CUSTOM_FIELD_VALUE = "blue"

var payer: Keypair = Keypair.new_from_file("payer.json")
var mint_keypair: Keypair = Keypair.new_random()

func PASS(unique_identifier: int):
	passed_test_mask += (1 << unique_identifier)
	print("[OK]: ", unique_identifier)


func minumum_balance_for_rent_extemtion(data_size):
	var client = SolanaClient.new()
	add_child(client)
	client.get_minimum_balance_for_rent_extemption(data_size)
	var result = await client.http_response_received
	assert(result.has("result"))
	remove_child(client)
	return result['result']


func create_token_metadata_extension():
	# Ask our RPC server how much we need to transfer to our new mint account.
	var account_size = TokenProgram2022.get_mint_account_size_from_data(NAME, SYMBOL, URI, {CUSTOM_FIELD_NAME: CUSTOM_FIELD_VALUE})
	var mint_lamports = await minumum_balance_for_rent_extemtion(account_size)

	var tx := Transaction.new()
	add_child(tx)
	
	# Append instructions.
	var ix: Instruction = SystemProgram.create_account(payer, mint_keypair, mint_lamports, MINT_SIZE, TokenProgram2022.get_pid())
	tx.add_instruction(ix)
	ix = TokenProgram2022.initialize_metadata_pointer(payer, mint_keypair)
	tx.add_instruction(ix)
	ix = TokenProgram2022.initialize_mint(mint_keypair, payer, payer, 9)
	tx.add_instruction(ix)
	ix = TokenProgram2022.initialize_metadata(payer, mint_keypair, NAME, SYMBOL, URI)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed
	PASS(0)


func update_metadata_field():
	var tx := Transaction.new()
	add_child(tx)
	
	# Append instructions.
	var ix: Instruction = TokenProgram2022.update_metadata(payer, mint_keypair, CUSTOM_FIELD_NAME, CUSTOM_FIELD_VALUE)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed
	PASS(1)


func _ready():
	await create_token_metadata_extension()
	update_metadata_field()


func _on_timeout_timeout():
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)
