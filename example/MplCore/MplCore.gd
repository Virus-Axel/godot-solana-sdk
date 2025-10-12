extends Control

const MINT_SIZE := 82
const MINT_ACCOUNT_SIZE := 165

# TODO(VirAx): Make sense of this and bind constant to godot.
const CANDY_MACHINE_HIDDEN_SECTION = 8 + 32 + 32 + 32 + 8 + 8 + 8 + 1 + 1 + 4 + 32 + 4 + 4 + 200 + 4 + 1 +  1 + 4 + 32 + 4 + 200 + 32 +10000

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

var core_collection_guard_group: Keypair = Keypair.new_random()
var core_asset_guard_group: Keypair = Keypair.new_random()
var candy_machine_with_guard_group: Keypair = Keypair.new_random()
var guard_group_kp: Keypair = Keypair.new_random()
var guard_group_base: Keypair = Keypair.new_random()


func PASS(unique_identifier: int):
	print("[OK]: ", unique_identifier)


func minumum_balance_for_rent_extemtion(data_size):
	var client = SolanaClient.new()
	add_child(client)
	client.get_minimum_balance_for_rent_extemption(data_size)
	var result = await client.http_response_received
	assert(result.has("result"))
	remove_child(client)
	return result['result']


func send_instructions(instructions: Array):
	var tx: Transaction = Transaction.new()
	tx.set_instructions(instructions)
	add_child(tx)
	
	#tx.skip_preflight = true
	tx.set_payer(payer)
	tx.update_latest_blockhash()
	tx.sign_and_send()
	var result = await tx.transaction_response_received

	assert(result.has("result"))
	await tx.finalized

	tx.queue_free()
	remove_child(tx)
	return result


func initialize_core_collection():
	var ix: Instruction = $mpl_core_program.CreateCollectionV2(core_collection, payer, payer, SystemProgram.get_pid(), load("res://resources/new_create_collection_v_2_args.tres"))
	await send_instructions([ix])

	PASS(0)


func initialize_candy_machine_demo():
	var instructions = []
	var candy_machine_size = CANDY_MACHINE_HIDDEN_SECTION
	var candy_machine_lamports = await minumum_balance_for_rent_extemtion(candy_machine_size)
	
	var candy_pid = $candy_machine_core.get_pid()
	
	var ix: Instruction = SystemProgram.create_account(payer, candy_machine_keypair, candy_machine_lamports, candy_machine_size, candy_pid)
	instructions.append(ix)

	var core_pid = $mpl_core_program.get_pid()
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_keypair.get_public_bytes()], candy_machine_core.new().get_pid())
	ix = candy_machine_core.new().initialize(candy_machine_keypair, cm_pda, payer, payer, core_collection, payer, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", load("res://new_candy_machine_data_2.tres"))
	instructions.append(ix)
	
	await send_instructions(instructions)

	PASS(1)


func initialize_core_collection_guard():
	var ix: Instruction = $mpl_core_program.CreateCollectionV2(core_collection_guard, payer, payer, SystemProgram.get_pid(), load("res://resources/new_create_collection_v_2_args.tres"))
	await send_instructions([ix])

	PASS(4)


func add_config_line():
	var ix: Instruction = candy_machine_core.new().addConfigLines(candy_machine_keypair, payer, 0, [load("res://resources/new_config_line_2.tres"), load("res://resources/new_config_line_3.tres")])
	await send_instructions([ix])

	PASS(2)


func mint_core_asset():
	var core_pid = $mpl_core_program.get_pid()
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_keypair.get_public_bytes()], candy_machine_core.new().get_pid())
	var ix: Instruction = candy_machine_core.new().mintAsset(candy_machine_keypair, cm_pda, payer, payer, payer, core_asset, core_collection, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", "SysvarS1otHashes111111111111111111111111111", load("res://resources/new_mint_asset_args.tres"))

	await send_instructions([ix])

	PASS(3)


func create_candy_machine_with_guard():
	var instructions = []
	var candy_machine_size = CANDY_MACHINE_HIDDEN_SECTION
	var candy_machine_lamports = await minumum_balance_for_rent_extemtion(candy_machine_size)
	
	var candy_pid = $candy_machine_core.get_pid()
	
	var ix: Instruction = SystemProgram.create_account(payer, candy_machine_with_guard, candy_machine_lamports, candy_machine_size, candy_pid)
	instructions.append(ix)
	
	var core_pid = $mpl_core_program.get_pid()
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_with_guard.get_public_bytes()], candy_machine_core.new().get_pid())
	ix = candy_machine_core.new().initialize(candy_machine_with_guard, cm_pda, payer, payer, core_collection_guard, payer, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", load("res://new_candy_machine_data_2.tres"))
	instructions.append(ix)
	
	var guard_pda = Pubkey.new_pda_bytes(["candy_guard".to_ascii_buffer(), guard_base.get_public_bytes()], candy_guard.new().get_pid())
	
	ix = candy_guard.new().initialize(guard_pda, guard_base, payer, payer, SystemProgram.get_pid(), load("res://resources/new_candy_guard_data.tres").serialize_core_mint_args())
	instructions.append(ix)
	
	ix = candy_guard.new().wrap(guard_pda, payer, candy_machine_with_guard, candy_machine_core.new().get_pid(), payer)
	instructions.append(ix)
	
	await send_instructions(instructions)

	PASS(5)


func add_config_line_guards():
	var ix: Instruction = candy_machine_core.new().addConfigLines(candy_machine_with_guard, payer, 0, [load("res://resources/new_config_line_2.tres"), load("res://resources/new_config_line_3.tres")])
	await send_instructions([ix])

	PASS(6)


func mint_with_guard():
	var candy_pid = $candy_machine_core.get_pid()
	var core_pid = $mpl_core_program.get_pid()
	var guard_pda = Pubkey.new_pda_bytes(["candy_guard".to_ascii_buffer(), guard_base.get_public_bytes()], candy_guard.new().get_pid())
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_with_guard.get_public_bytes()], candy_machine_core.new().get_pid())

	var ix = candy_guard.new().mintV1(guard_pda, candy_pid, candy_machine_with_guard, cm_pda, payer, payer, payer, core_asset_guard, core_collection_guard, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", "SysvarS1otHashes111111111111111111111111111", load("res://resources/new_candy_guard_data.tres").default.get_extra_account_metas(null), load("res://resources/new_candy_guard_data.tres").serialize_core_mint_args(), AnchorProgram.option(null))
	await send_instructions([ix])

	PASS(7)


func initialize_core_collection_guard_group():
	var ix: Instruction = $mpl_core_program.CreateCollectionV2(core_collection_guard_group, payer, payer, SystemProgram.get_pid(), load("res://resources/guard_groups/new_create_collection_v_2_args.tres"))
	await send_instructions([ix])

	PASS(8)


func create_candy_machine_with_guard_group():
	var instructions = []
	var candy_machine_size = 100000
	var candy_machine_lamports = await minumum_balance_for_rent_extemtion(candy_machine_size)
	
	var candy_pid = $candy_machine_core.get_pid()
	
	var ix: Instruction = SystemProgram.create_account(payer, candy_machine_with_guard_group, candy_machine_lamports, candy_machine_size, candy_pid)
	instructions.append(ix)
	
	var core_pid = $mpl_core_program.get_pid()
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_with_guard_group.get_public_bytes()], candy_machine_core.new().get_pid())
	ix = candy_machine_core.new().initialize(candy_machine_with_guard_group, cm_pda, payer, payer, core_collection_guard_group, payer, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", load("res://resources/guard_groups/new_candy_machine_data_2.tres"))
	instructions.append(ix)
	
	var guard_group_pda = Pubkey.new_pda_bytes(["candy_guard".to_ascii_buffer(), guard_group_base.get_public_bytes()], candy_guard.new().get_pid())
	
	ix = candy_guard.new().initialize(guard_group_pda, guard_group_base, payer, payer, SystemProgram.get_pid(), load("res://resources/guard_groups/new_candy_guard_data.tres").serialize_core_mint_args())
	instructions.append(ix)
	
	ix = candy_guard.new().wrap(guard_group_pda, payer, candy_machine_with_guard_group, candy_machine_core.new().get_pid(), payer)
	instructions.append(ix)
	
	await send_instructions(instructions)

	PASS(9)


func add_config_line_guard_group():
	const CONFIG_LINES_PER_TX = 50
	
	for rounds in range(1000 / CONFIG_LINES_PER_TX):
		var config_lines = []
		for i in range(CONFIG_LINES_PER_TX):
			
			var config_line = ConfigLine2.new()
			config_line.name = str(i)
			config_line.uri = str(i)
			
			config_lines.append(config_line)
		
		var ix: Instruction = candy_machine_core.new().addConfigLines(candy_machine_with_guard_group, payer, rounds * CONFIG_LINES_PER_TX, config_lines)
		await send_instructions([ix])

	PASS(10)


func mint_with_guard_group():
	var candy_pid = $candy_machine_core.get_pid()
	var core_pid = $mpl_core_program.get_pid()
	var guard_group_pda = Pubkey.new_pda_bytes(["candy_guard".to_ascii_buffer(), guard_group_base.get_public_bytes()], candy_guard.new().get_pid())
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_with_guard_group.get_public_bytes()], candy_machine_core.new().get_pid())

	var ix = candy_guard.new().mintV1(guard_group_pda, candy_pid, candy_machine_with_guard_group, cm_pda, payer, payer, payer, core_asset_guard_group, core_collection_guard_group, core_pid, SystemProgram.get_pid(), "Sysvar1nstructions1111111111111111111111111", "SysvarS1otHashes111111111111111111111111111", load("res://resources/new_candy_guard_data.tres").default.get_extra_account_metas(null), load("res://resources/new_candy_guard_data.tres").serialize_core_mint_args(), AnchorProgram.option("base"))
	await send_instructions([ix])

	PASS(11)


func mint_with_token_payment():
	var candy_machine_with_token_payment = Keypair.new_random()
	
	var token_payment_resource = load("res://resources/guard_groups/token_payment.tres")
	var payment_mint = Keypair.new_random()
	var payment_ata = Pubkey.new_associated_token_address(payer, payment_mint, TokenProgram.get_pid())
	token_payment_resource.default.tokenPayment.destinationAta = payment_ata
	token_payment_resource.default.tokenPayment.mint = payment_mint
	
	var candy_machine_size = CANDY_MACHINE_HIDDEN_SECTION
	var candy_machine_lamports = await minumum_balance_for_rent_extemtion(candy_machine_size)
	var lamports = await minumum_balance_for_rent_extemtion(TokenProgram.TOKEN_ACCOUNT_SIZE)

	var instructions = []	

	var candy_pid = $candy_machine_core.get_pid()
	
	var ix: Instruction = SystemProgram.create_account(payer, candy_machine_with_token_payment, candy_machine_lamports, candy_machine_size, candy_pid)
	instructions.append(ix)
	
	var core_pid = $mpl_core_program.get_pid()
	var cm_pda = Pubkey.new_pda_bytes(["candy_machine".to_ascii_buffer(), candy_machine_with_token_payment.get_public_bytes()], candy_machine_core.new().get_pid())
	var core_collection_token_payment = Keypair.new_random()
	ix = $mpl_core_program.CreateCollectionV2(core_collection_token_payment, payer, payer, SystemProgram.get_pid(), load("res://resources/new_create_collection_v_2_args.tres"))
	instructions.append(ix)
	ix = candy_machine_core.new().initialize(candy_machine_with_token_payment, cm_pda, payer, payer, core_collection_token_payment, payer, core_pid, SystemProgram.get_pid(), SystemProgram.get_sysvar_instructions_pubkey(), load("res://new_candy_machine_data_2.tres"))
	instructions.append(ix)
	
	ix = SystemProgram.create_account(payer, payment_mint, lamports, TokenProgram.TOKEN_MINT_SIZE, TokenProgram.get_pid())
	instructions.append(ix)
	ix = TokenProgram.initialize_mint(payment_mint, payer, payer, 0)
	instructions.append(ix)
	ix = AssociatedTokenAccountProgram.create_associated_token_account(payer, payer, payment_mint, TokenProgram.get_pid())
	instructions.append(ix)
	ix = TokenProgram.mint_to(payment_mint, payment_ata, payer, payer, 10)
	instructions.append(ix)
	ix = candy_machine_core.new().addConfigLines(candy_machine_with_token_payment, payer, 0, [load("res://resources/new_config_line_2.tres"), load("res://resources/new_config_line_3.tres")])
	instructions.append(ix)

	await send_instructions(instructions)
	
	instructions = []

	var guard_base_with_payment: Keypair = Keypair.new_random()
	var guard_pda = Pubkey.new_pda_bytes(["candy_guard".to_ascii_buffer(), guard_base_with_payment.get_public_bytes()], candy_guard.new().get_pid())

	ix = candy_guard.new().initialize(guard_pda, guard_base_with_payment, payer, payer, SystemProgram.get_pid(), token_payment_resource.serialize_core_mint_args())
	instructions.append(ix)
	
	ix = candy_guard.new().wrap(guard_pda, payer, candy_machine_with_token_payment, candy_machine_core.new().get_pid(), payer)
	instructions.append(ix)

	var core_asset_guard_with_payment = Keypair.new_random()
	ix = candy_guard.new().mintV1(guard_pda, candy_pid, candy_machine_with_token_payment, cm_pda, payer, payer, payer, core_asset_guard_with_payment, core_collection_token_payment, core_pid, SystemProgram.get_pid(), SystemProgram.get_sysvar_instructions_pubkey(), SystemProgram.get_sysvar_slot_hashes_pubkey(), token_payment_resource.default.get_extra_account_metas(payer), token_payment_resource.serialize_core_mint_args(), AnchorProgram.option(null))
	instructions.append(ix)

	await send_instructions(instructions)

	PASS(12)


func _ready():
	await initialize_core_collection()
	await initialize_candy_machine_demo()
	await add_config_line()
	await mint_core_asset()
	
	await initialize_core_collection_guard()
	await create_candy_machine_with_guard()
	await add_config_line_guards()
	await mint_with_guard()
	
	await initialize_core_collection_guard_group()
	await create_candy_machine_with_guard_group()
	await add_config_line_guard_group()
	await mint_with_guard_group()

	await mint_with_token_payment()
	
	print("ALL TESTS PASSED")
	get_tree().quit(0)
