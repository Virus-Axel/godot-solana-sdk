extends Control

const MINT_SIZE := 82
const MINT_ACCOUNT_SIZE := 165

const TOTAL_CASES := 8
# TODO(VirAx): Make sense of this and bind constant to godot.
const CANDY_MACHINE_HIDDEN_SECTION = 8 + 32 + 32 + 32 + 8 + 8 + 8 + 1 + 1 + 4 + 32 + 4 + 4 + 200 + 4 + 1 +  1 + 4 + 32 + 4 + 200 + 32 +10000

var passed_test_mask := 0
var payer: Keypair = await Keypair.new_from_file("res://payer.json")

# New addresses
var core_collection: Keypair = Keypair.new_random()
var core_asset: Keypair = Keypair.new_random()
var mint_keypair: Keypair = Keypair.new_random()
var candy_machine_keypair: Keypair = Keypair.new_random()
var mint_account_keypair: Keypair = Keypair.new_random()

var core_collection_guard: Keypair = Keypair.new_random()
var core_asset_guard: Keypair = Keypair.new_random()
var candy_machine_with_guard: Keypair = Keypair.new_random()
var guard_kp: Keypair = Keypair.new_random()
var guard_base: Keypair = Keypair.new_random()


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


func initialize_core_collection():
	var tx := Transaction.new()
	add_child(tx)
	
	# Append instructions.
	var ix: Instruction = $mpl_core_program.CreateCollectionV2(core_collection, payer, payer, SystemProgram.get_pid(), load("res://resources/new_create_collection_v_2_args.tres"))
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(0)


func initialize_candy_machine_demo():
	var candy_machine_size = CANDY_MACHINE_HIDDEN_SECTION
	var candy_machine_lamports = await minumum_balance_for_rent_extemtion(candy_machine_size)
	
	# Create a transaction
	var tx := Transaction.new()
	add_child(tx)
	var candy_pid = $candy_machine_core.get_pid()
	
	var ix: Instruction = SystemProgram.create_account(payer, candy_machine_keypair, candy_machine_lamports, candy_machine_size, candy_pid)
	tx.add_instruction(ix)

	var core_pid = $mpl_core_program.get_pid()
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_keypair.get_public_bytes()], candy_machine_core.new().get_pid())
	ix = candy_machine_core.new().initialize(candy_machine_keypair, cm_pda, payer, payer, core_collection, payer, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", load("res://new_candy_machine_data_2.tres"))
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(1)


func initialize_core_collection_guard():
	var tx := Transaction.new()
	add_child(tx)
	
	# Append instructions.
	var ix: Instruction = $mpl_core_program.CreateCollectionV2(core_collection_guard, payer, payer, SystemProgram.get_pid(), load("res://resources/new_create_collection_v_2_args.tres"))
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(4)


func add_config_line():
	var tx := Transaction.new()
	add_child(tx)
	
	# Append instructions.
	var core_pid = $mpl_core_program.get_pid()
	var ix: Instruction = candy_machine_core.new().addConfigLines(candy_machine_keypair, payer, 0, [load("res://resources/new_config_line_2.tres"), load("res://resources/new_config_line_3.tres")])
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(2)


func mint_core_asset():
	var tx := Transaction.new()
	add_child(tx)
	
	# Append instructions.
	var core_pid = $mpl_core_program.get_pid()
	var data = PackedByteArray([0])
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_keypair.get_public_bytes()], candy_machine_core.new().get_pid())
	var ix: Instruction = candy_machine_core.new().mintAsset(candy_machine_keypair, cm_pda, payer, payer, payer, core_asset, core_collection, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", "SysvarS1otHashes111111111111111111111111111", load("res://resources/new_mint_asset_args.tres"))
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(3)


func create_candy_machine_with_guard():
	var candy_machine_size = CANDY_MACHINE_HIDDEN_SECTION
	var candy_machine_lamports = await minumum_balance_for_rent_extemtion(candy_machine_size)
	
	# Create a transaction
	var tx := Transaction.new()
	add_child(tx)
	var candy_pid = $candy_machine_core.get_pid()
	
	var ix: Instruction = SystemProgram.create_account(payer, candy_machine_with_guard, candy_machine_lamports, candy_machine_size, candy_pid)
	tx.add_instruction(ix)
	
	var core_pid = $mpl_core_program.get_pid()
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_with_guard.get_public_bytes()], candy_machine_core.new().get_pid())
	ix = candy_machine_core.new().initialize(candy_machine_with_guard, cm_pda, payer, payer, core_collection_guard, payer, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", load("res://new_candy_machine_data_2.tres"))
	tx.add_instruction(ix)
	
	var guard_pda = Pubkey.new_pda_bytes(["candy_guard".to_ascii_buffer(), guard_base.get_public_bytes()], candy_guard.new().get_pid())
	
	ix = candy_guard.new().initialize(guard_pda, guard_base, payer, payer, SystemProgram.get_pid(), load("res://resources/new_candy_guard_data.tres").serialize_core_mint_args())
	
	tx.add_instruction(ix)
	ix = candy_guard.new().wrap(guard_pda, payer, candy_machine_with_guard, candy_machine_core.new().get_pid(), payer)
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(5)


func add_config_line_guards():
	var tx := Transaction.new()
	add_child(tx)
	
	# Append instructions.
	var core_pid = $mpl_core_program.get_pid()
	var ix: Instruction = candy_machine_core.new().addConfigLines(candy_machine_with_guard, payer, 0, [load("res://resources/new_config_line_2.tres"), load("res://resources/new_config_line_3.tres")])
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(6)


func mint_with_guard():
	var candy_machine_size = CANDY_MACHINE_HIDDEN_SECTION
	var candy_machine_lamports = await minumum_balance_for_rent_extemtion(candy_machine_size)
	
	# Create a transaction
	var tx := Transaction.new()
	add_child(tx)
	var candy_pid = $candy_machine_core.get_pid()
	var core_pid = $mpl_core_program.get_pid()
	var guard_pda = Pubkey.new_pda_bytes(["candy_guard".to_ascii_buffer(), guard_base.get_public_bytes()], candy_guard.new().get_pid())
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_with_guard.get_public_bytes()], candy_machine_core.new().get_pid())

	var ix = candy_guard.new().mintV1(guard_pda, candy_pid, candy_machine_with_guard, cm_pda, payer, payer, payer, core_asset_guard, core_collection_guard, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", "SysvarS1otHashes111111111111111111111111111", load("res://resources/new_candy_guard_data.tres").default.get_extra_account_metas(), load("res://resources/new_mint_asset_args.tres").serialize_core_mint_args(), AnchorProgram.option(null))
	tx.add_instruction(ix)
	
	tx.set_payer(payer)
	tx.update_latest_blockhash()

	tx.sign_and_send()
	var result = await tx.transaction_response_received
	assert(result.has("result"))

	await tx.confirmed

	PASS(7)


func _ready():
	await initialize_core_collection()
	await initialize_candy_machine_demo()
	await add_config_line()
	await mint_core_asset()
	
	await initialize_core_collection_guard()
	await create_candy_machine_with_guard()
	await add_config_line_guards()
	await mint_with_guard()


func _on_timer_timeout() -> void:
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)
