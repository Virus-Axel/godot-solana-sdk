extends Control

const MINT_SIZE := 82
const MINT_ACCOUNT_SIZE := 165

var payer: Keypair = await Keypair.new_from_file("payer.json")

# Address of new token type.
var mint_keypair: Keypair = Keypair.new_random()

var mint_account_keypair = Keypair.new_random()


func PASS(unique_identifier: int):
	print("[OK]: ", unique_identifier)

func display_dict(data: Variant, parent: TreeItem):
	if(typeof(data) == TYPE_STRING):
		parent.set_text(1, data)
	elif(typeof(data) != TYPE_DICTIONARY):
		parent.set_text(1, str(data))
	else:
		var data_dict: Dictionary = data
		var keys = data_dict.keys()
		var values = data_dict.values()
		for i in range(keys.size()):
			var subchild = parent.create_child()
			subchild.set_editable(1, true)
			subchild.set_text(0, keys[i])
			display_dict(values[i], subchild)


func minumum_balance_for_rent_extemtion(data_size):
	var client = SolanaClient.new()
	add_child(client)
	client.get_minimum_balance_for_rent_extemption(data_size)
	var result = await client.http_response_received
	assert(result.has("result"))
	remove_child(client)
	return result['result']


func create_mint_demo():
	# Ask our RPC server how much we need to transfer to our new mint account.
	var lamports = await minumum_balance_for_rent_extemtion(MINT_SIZE)
	
	# Create a transaction
	var tx := Transaction.new()
	add_child(tx)
	
	# Append instructions.
	var ix: Instruction = SystemProgram.create_account(payer, mint_keypair, lamports, MINT_SIZE, TokenProgram.get_pid())
	tx.add_instruction(ix)
	# The payer keypair will be able to modify the mint and freeze it.
	ix = TokenProgram.initialize_mint(mint_keypair, payer, payer, 0)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()
	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(0)


func create_mint_account_demo():
	var lamports = await minumum_balance_for_rent_extemtion(MINT_ACCOUNT_SIZE)
	
	var tx := Transaction.new()
	add_child(tx)
	
	var ix: Instruction = SystemProgram.create_account(payer, mint_account_keypair, lamports, MINT_ACCOUNT_SIZE, TokenProgram.get_pid())
	tx.add_instruction(ix)
	ix = TokenProgram.initialize_account(mint_account_keypair, mint_keypair, payer)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()
	tx.sign_and_send()

	var result = await tx.transaction_response_received
	assert(result.has("result"))
	await tx.confirmed
	
	PASS(1)


func mint_to_demo():
	var tx := Transaction.new()
	add_child(tx)

	var ix = TokenProgram.mint_to(mint_keypair, mint_account_keypair, payer, payer, 10)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()
	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))
	
	await tx.confirmed

	PASS(2)


func transfer_checked_demo():
	var lamports = await minumum_balance_for_rent_extemtion(MINT_SIZE)
	var receiver_account_keypair = Keypair.new_random()
	var tx := Transaction.new()
	add_child(tx)

	# Create a receiver account.
	var ix: Instruction = SystemProgram.create_account(payer, receiver_account_keypair, lamports*2, MINT_ACCOUNT_SIZE, TokenProgram.get_pid())
	tx.add_instruction(ix)
	ix = TokenProgram.initialize_account(receiver_account_keypair, mint_keypair, payer)
	tx.add_instruction(ix)
	
	# Transfer to newly created receiver account.
	ix = TokenProgram.transfer_checked(mint_account_keypair, mint_keypair, receiver_account_keypair, payer, 1, 0)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()
	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))
	
	await tx.confirmed

	PASS(3)


func freeze_account_demo():
	var tx := Transaction.new()
	add_child(tx)

	var ix: Instruction = TokenProgram.freeze_account(mint_account_keypair, mint_keypair, payer)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()
	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))
	
	await tx.confirmed
	
	ix = TokenProgram.mint_to(mint_keypair, mint_account_keypair, payer, payer, 10)
	
	# Replace current instructions.
	tx.set_instructions([ix])
	
	tx.update_latest_blockhash()
	tx.sign_and_send()
	
	result = await tx.transaction_response_received
	
	# Expect an error because the account is frozen.
	assert(result.has("error"))

	PASS(4)

func _ready():
	# Use created mint in upcoming transactions, so await it.
	await create_mint_demo()
	await create_mint_account_demo()
	await mint_to_demo()
	await transfer_checked_demo()
	await freeze_account_demo()

	print("ALL TESTS PASSED")
	get_tree().quit(0)
