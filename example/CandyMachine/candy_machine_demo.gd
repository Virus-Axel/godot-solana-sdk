extends Control

const MINT_SIZE := 82
const CANDY_MACHINE_SIZE := 951
const MINT_ACCOUNT_SIZE := 165

const TOTAL_CASES := 2
var passed_test_mask := 0
var payer: Keypair = await Keypair.new_from_file("payer.json")

# New addresses
var mint_keypair: Keypair = Keypair.new_random()
var candy_machine_keypair: Keypair = Keypair.new_random()
var mint_account_keypair: Keypair = Keypair.new_random()


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


func initialize_mint_collection_demo():
	# Ask our RPC server how much we need to transfer to our new mint account.
	var mint_lamports = await minumum_balance_for_rent_extemtion(MINT_SIZE)
	var mint_account_lamports = await minumum_balance_for_rent_extemtion(MINT_ACCOUNT_SIZE)
	
	var token_account_keypair = Keypair.new_random()

	
	# Create a transaction
	var tx := Transaction.new()
	add_child(tx)
	
	# Append instructions.
	var ix: Instruction = SystemProgram.create_account(payer, mint_keypair, mint_lamports, MINT_SIZE, TokenProgram.get_pid())
	tx.add_instruction(ix)
	ix = TokenProgram.initialize_mint(mint_keypair, payer, payer, 0)
	tx.add_instruction(ix)
	ix = SystemProgram.create_account(payer, mint_account_keypair, mint_account_lamports, MINT_ACCOUNT_SIZE, TokenProgram.get_pid())
	tx.add_instruction(ix)
	ix = TokenProgram.initialize_account(mint_account_keypair, mint_keypair, payer)
	tx.add_instruction(ix)
	ix = TokenProgram.mint_to(mint_keypair, mint_account_keypair, payer, payer, 1)
	tx.add_instruction(ix)
	ix = MplTokenMetadata.create_metadata_account(mint_keypair, payer, payer, load("res://meta_data.tres"), true)
	tx.add_instruction(ix)
	ix = MplTokenMetadata.update_metadata_account(MplTokenMetadata.new_associated_metadata_pubkey(mint_keypair), payer)
	tx.add_instruction(ix)
	ix = MplTokenMetadata.create_master_edition(mint_keypair, payer, payer, payer, 1)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(0)
	

func initialize_candy_machine_demo():
	var candy_machine_lamports = await minumum_balance_for_rent_extemtion(CANDY_MACHINE_SIZE)
	
		# Create a transaction
	var tx := Transaction.new()
	add_child(tx)
	
	var ix: Instruction = SystemProgram.create_account(payer, candy_machine_keypair, candy_machine_lamports, CANDY_MACHINE_SIZE, MplCandyMachine.get_pid())
	tx.add_instruction(ix)
	ix = MplCandyMachine.initialize(payer, candy_machine_keypair, mint_keypair, load("res://candy_machine_data.tres"))
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received

	assert(result.has("result"))

	await tx.confirmed

	PASS(1)


func _ready():
	# Use created mint in upcoming transactions, so await it.
	await initialize_mint_collection_demo()
	initialize_candy_machine_demo()


func _on_timeout_timeout():
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)
