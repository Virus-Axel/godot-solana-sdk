extends Node
class_name TokenManager

func create_token(mint_amount:float, decimals:int=9) -> TransactionData:
	var instructions:Array[Instruction]
	var extra_signers:Array[Keypair]
	
	var token_mint:Keypair = Keypair.new_random()
	extra_signers.append(token_mint)
	print("Creating token with address: ",token_mint.to_string())
	
	var init_token_ix:Instruction = TokenProgram.initialize_mint(
		token_mint.to_pubkey(),
		SolanaService.wallet.get_pubkey(),
		null,
		decimals
	)
	instructions.append(init_token_ix)
	
	var token_account:Keypair = Keypair.new_random()
	extra_signers.append(token_account)
	
	var init_account_ix:Instruction = TokenProgram.initialize_account(
		token_account.to_pubkey(),
		token_mint.to_pubkey(),
		SolanaService.wallet.get_pubkey()
	)
	instructions.append(init_account_ix)
	
	var mint_amount_in_lamports:int = int(mint_amount * pow(10,decimals))
	
	var mint_ix:Instruction = TokenProgram.mint_to(
		token_mint.to_pubkey(),
		token_account.to_pubkey(),
		SolanaService.wallet.get_pubkey(),
		SolanaService.wallet.get_pubkey(),
		mint_amount_in_lamports
	)
	instructions.append(mint_ix)
	
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	for extra_signer in extra_signers:
		transaction = await SolanaService.transaction_manager.add_signature(transaction,extra_signer)
	await SolanaService.transaction_manager.add_signature(transaction,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.send_transaction(transaction)
	
	return tx_data
