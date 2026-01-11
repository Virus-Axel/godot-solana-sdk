extends Control

const MINT_SIZE := 82
const MINT_ACCOUNT_SIZE := 165

var payer: Keypair = await Keypair.new_from_file("payer.json")

# New addresses
var mint_keypair: Keypair = Keypair.new_random()
var candy_machine_keypair: Keypair = Keypair.new_random()
var mint_account_keypair: Keypair = Keypair.new_random()

var guard_kp: Keypair = Keypair.new_random()


func PASS(unique_identifier: int):
	print("[OK]: ", unique_identifier)


func minumum_balance_for_rent_extemtion(data_size):
	var client = SolanaClient.new()
	add_child(client)
	client.get_minimum_balance_for_rent_extemption(data_size)
	var result = (await client.http_request_completed)[1]
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
	ix = MplTokenMetadata.create_metadata_account(mint_keypair, payer, payer, load("res://create_meta_data_args.tres"), true)
	tx.add_instruction(ix)
	ix = MplTokenMetadata.update_metadata_account(MplTokenMetadata.new_associated_metadata_pubkey(mint_keypair), payer)
	tx.add_instruction(ix)
	ix = MplTokenMetadata.create_master_edition(mint_keypair, payer, payer, payer, 0)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(0)
	

func initialize_candy_machine_demo():
	var candy_machine_size = load("res://candy_machine_data.tres").get_space_for_candy()
	var candy_machine_lamports = await minumum_balance_for_rent_extemtion(candy_machine_size)
	
	# Create a transaction
	var tx := Transaction.new()
	add_child(tx)
	
	var ix: Instruction = SystemProgram.create_account(payer, candy_machine_keypair, candy_machine_lamports, candy_machine_size, MplCandyMachine.get_pid())
	tx.add_instruction(ix)
	ix = MplCandyMachine.initialize(payer, candy_machine_keypair, mint_keypair, load("res://candy_machine_data.tres"), true)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(1)


func add_config_line_demo():
	# Create a transaction
	var tx := Transaction.new()
	add_child(tx)
	
	var config_lines = [
		load("res://config_lines/1.tres"),
		load("res://config_lines/2.tres"),
	]
	
	var ix: Instruction = MplCandyMachine.add_config_lines(candy_machine_keypair, payer, config_lines, 0)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(2)


func mint_demo():
	var nft_keypair: Keypair = Keypair.new_random()
	var random_receiver: Pubkey = Pubkey.new_associated_token_address(payer, nft_keypair, TokenProgram.get_pid())

	# Create a transaction
	var tx := Transaction.new()
	add_child(tx)

	var ix: Instruction = MplCandyMachine.mint(payer, payer, nft_keypair, mint_keypair, payer, candy_machine_keypair)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(3)


func fetch_candy_machine_account_demo():
	$MplCandyMachine.get_candy_machine_info(candy_machine_keypair);
	var candy_machine_data: CandyMachineData = await $MplCandyMachine.account_fetched
	assert(candy_machine_data.token_standard == 4) # Make sure token standard is pnft.
	assert(candy_machine_data.symbol == "GSS")
	assert(candy_machine_data.seller_fee_basis_points == 800)
	PASS(4)


func initialize_mint_with_mint_limit():
	var tx := Transaction.new()
	add_child(tx)

	var ix: Instruction = MplCandyGuard.initialize(guard_kp, payer, payer, load("res://new_candy_guard_access_list.tres"))
	tx.add_instruction(ix)
	var guard_id = MplCandyGuard.new_associated_candy_guard_key(guard_kp)
	ix = MplCandyGuard.wrap(guard_id, payer, candy_machine_keypair, payer)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed
	PASS(5)

func mint_with_guards():

	var tx := Transaction.new()
	add_child(tx)

	var guard_id = MplCandyGuard.new_associated_candy_guard_key(guard_kp)
	var mint_account = Keypair.new_random()

	var ix: Instruction = MplCandyGuard.mint(candy_machine_keypair, guard_id, payer, payer, mint_account, payer, mint_keypair, payer, load("res://new_candy_guard_access_list.tres"), "default")
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed
	PASS(6)

func _ready():
	# Use created mint in upcoming transactions, so await it.
	await initialize_mint_collection_demo()
	await initialize_candy_machine_demo()
	await add_config_line_demo()
	await mint_demo()
	await fetch_candy_machine_account_demo()
	
	# Candy Machine V3 tests
	await initialize_mint_with_mint_limit()
	await mint_with_guards()
	
	print("ALL TESTS PASSED")
	get_tree().quit(0)
