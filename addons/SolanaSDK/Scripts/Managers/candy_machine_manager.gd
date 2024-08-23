extends Node
class_name CandyMachineManager

	
func fetch_candy_machine(cm_id:Pubkey) -> CandyMachineData:
	var candy_machine = SolanaService.spawn_mpl_candy_machine_client()
	candy_machine.get_candy_machine_info(cm_id)
	var cm_data:CandyMachineData = await candy_machine.account_fetched
	candy_machine.queue_free()
	return cm_data
	
func mint_nft(cm_id:Pubkey, cm_data:CandyMachineData, payer:WalletService, receiver,tx_commitment:TransactionManager.Commitment=TransactionManager.Commitment.FINALIZED,priority_fee:float=0.0, custom_mint_account:Keypair=null) -> TransactionData:
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
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_transaction(instructions,tx_commitment,priority_fee,payer.get_kp())
	return tx_data
	
	
	
	
func mint_nft_with_guards(cm_id:Pubkey,guard_id:Pubkey,cm_data:CandyMachineData,payer:WalletService,receiver,guards:CandyGuardAccessList,group:String,tx_commitment:TransactionManager.Commitment=TransactionManager.Commitment.FINALIZED,priority_fee:float=0.0,custom_mint_account:Keypair=null) -> TransactionData:
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
		group
		)

	instructions.append(mint_ix)
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_transaction(instructions,tx_commitment,priority_fee,payer.get_kp())
	return tx_data
