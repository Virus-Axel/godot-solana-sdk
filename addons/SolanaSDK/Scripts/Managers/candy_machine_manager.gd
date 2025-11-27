extends Node
class_name CandyMachineManager

var MPL_CORE_PID:String = "CoREENxT6tW1HoK8ypY1SxRMZTcVPm7R94rH4PZNhX7d"
var CANDY_GUARD_PID:String = "Guard1JwRhJkVH6XZhzoYxeBVQe872VH6QggF4BWmS9g"
var CORE_CM_PID:String = "CMACYFENjoBMHzapRXyo1JZkVS6EtaDDzkjMrmQLvr4J"
var CORE_CANDY_GUARD_PID:String = "CMAGAKJ67e9hRZgfC5SFTbZH8MgEmtqazKXjmkaJjWTJ"
var SYSVAR_INSTRUCTIONS_KEY:String = "Sysvar1nstructions1111111111111111111111111"
var SYSVAR_SLOTHASHES_KEY:String = "SysvarS1otHashes111111111111111111111111111"

func spawn_mpl_candy_machine_client() -> MplCandyMachine:
	var client:MplCandyMachine = MplCandyMachine.new()
	add_child(client)
	return client
	
func fetch_candy_machine(cm_id:Pubkey) -> CandyMachineData:
	var instance:MplCandyMachine = spawn_mpl_candy_machine_client()
	instance.get_candy_machine_info(cm_id)
	var cm_data:CandyMachineData = await instance.account_fetched
	instance.queue_free()
	return cm_data
	
func mint_nft(cm_id:Pubkey, cm_data:CandyMachineData, payer:WalletService, receiver,tx_commitment:TransactionManager.Commitment=TransactionManager.Commitment.FINALIZED, custom_mint_account:Keypair=null) -> TransactionData:
	var mint_account:Keypair = custom_mint_account
	if mint_account==null:
		mint_account = SolanaService.generate_keypair()
	var instructions:Array[Instruction]
	
	var mint_ix:Instruction = MplCandyMachine.mint(
		payer.get_kp(),
		receiver,
		mint_account,
		cm_data.collection_mint,
		cm_data.authority,
		cm_id
		)
		
	instructions.append(mint_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,payer)
	transaction = await SolanaService.transaction_manager.add_signature(transaction,mint_account)
	await SolanaService.transaction_manager.add_signature(transaction,payer.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.send_transaction(transaction,tx_commitment)
	return tx_data
	
	
func mint_nft_with_guards(cm_id:Pubkey,guard_id:Pubkey,cm_data:CandyMachineData,payer:WalletService,receiver,guards:CandyGuardAccessList,group:String,tx_commitment:TransactionManager.Commitment=TransactionManager.Commitment.FINALIZED,custom_mint_account:Keypair=null) -> TransactionData:
	var mint_account:Keypair = custom_mint_account
	if mint_account==null:
		mint_account = SolanaService.generate_keypair()
	var instructions:Array[Instruction]

	var mint_ix:Instruction = MplCandyGuard.mint(
		cm_id,
		guard_id,
		payer.get_kp(),
		receiver,
		mint_account,
		payer.get_kp(),
		cm_data.collection_mint,
		cm_data.authority,
		guards,
		group,
		)

	instructions.append(mint_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,payer.get_kp())
	transaction = await SolanaService.transaction_manager.add_signature(transaction,mint_account)
	await SolanaService.transaction_manager.add_signature(transaction,payer.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.send_transaction(transaction,tx_commitment)
	return tx_data
	
func spawn_core_candy_machine_client() -> candy_machine_core:
	var client:candy_machine_core = candy_machine_core.new()
	add_child(client)
	return client
	
#func fetch_core_candy_machine(cm_id:Pubkey) -> Dictionary:
	#var instance:candy_machine_core = spawn_core_candy_machine_client()
	#instance.fetch_CandyMachine(cm_id)
	#print("FETCHING CM")
	#var cm_data = await instance.account_fetched
	#print(cm_data)
	#instance.queue_free()
	#return cm_data
	
func mint_core_nft_with_guards(cm_key:Pubkey,payer,receiver:Pubkey,cm_data:Dictionary,meta_accounts:Array,mint_group,guard_set:GuardSet, tx_commitment:TransactionManager.Commitment=TransactionManager.Commitment.FINALIZED,custom_asset_account:Keypair=null,custom_minter=null) -> TransactionData:
	var core_cm:candy_machine_core = candy_machine_core.new()
	
	var core_candy_guard:candy_guard = candy_guard.new()
	var cm_pda:Pubkey = Pubkey.new_pda_bytes(["candy_machine".to_utf8_buffer(), cm_key.to_bytes()], core_cm.get_pid())
	var asset_account:Keypair = custom_asset_account
	if asset_account==null:
		asset_account = SolanaService.generate_keypair()
		
	var instructions:Array[Instruction]
	
	var minter = payer
	if custom_minter!=null:
		minter = custom_minter
		

	var mint_ix:Instruction = core_candy_guard.mintV1(
		cm_data["mint_authority"],
		core_cm.get_pid(),
		cm_key,
		cm_pda,
		payer,
		minter,
		receiver,
		asset_account,
		cm_data["collection_mint"],
		Pubkey.new_from_string(MPL_CORE_PID),
		SystemProgram.get_pid(),
		Pubkey.new_from_string(SYSVAR_INSTRUCTIONS_KEY),
		Pubkey.new_from_string(SYSVAR_SLOTHASHES_KEY),
		meta_accounts,
		guard_set.serialize_core_mint_args(),
		AnchorProgram.option(mint_group)
	)
	
	instructions.append(mint_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,payer)
	transaction = await SolanaService.transaction_manager.add_signature(transaction,asset_account)
	await SolanaService.transaction_manager.add_signature(transaction,payer)
	#transaction.skip_preflight=true
	var tx_data:TransactionData = await SolanaService.transaction_manager.send_transaction(transaction,tx_commitment)
	#var tx_data:TransactionData = TransactionData.new({})
	return tx_data
	
func fetch_core_candy_machine(candy_machine_key: Pubkey) -> Dictionary:
	var client: SolanaClient = SolanaClient.new()
	add_child(client)
	client.get_account_info(candy_machine_key.to_string())
	var result := {}
	var data: Dictionary = await client.http_response_received

	assert(data.has("result"))
	assert(data["result"].has("value"))
	assert(data["result"]["value"].has("data"))
	assert(data["result"]["value"]["data"].size() >= 1)

	var decoded_data: PackedByteArray = SolanaUtils.bs64_decode(data["result"]["value"]["data"][0])

	result["authority"] = Pubkey.new_from_bytes(decoded_data.slice(8, 40))
	result["mint_authority"] = Pubkey.new_from_bytes(decoded_data.slice(40, 72))
	result["collection_mint"] = Pubkey.new_from_bytes(decoded_data.slice(72, 104))

	result["items_redeemed"] = decoded_data.decode_u64(104)
	result["items_available"] = decoded_data.decode_u64(112)
	result["total_supply"] = decoded_data.decode_u64(120)
	result["is_mutable"] = decoded_data[128] == 1

	remove_child(client)
	client.queue_free()
	return result
	
func derive_candy_guard_id(cm_id:Pubkey) -> Pubkey:
	var name_bytes = "candy_guard".to_utf8_buffer()
	var cm_id_bytes = cm_id.to_bytes()
	return Pubkey.new_pda_bytes([name_bytes,cm_id_bytes],Pubkey.new_from_string(CORE_CANDY_GUARD_PID))
