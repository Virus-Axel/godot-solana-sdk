extends Node
class_name TransactionManager

enum Commitment{PROCESSED,CONFIRMED,FINALIZED}

signal on_tx_init
signal on_tx_signed
signal on_tx_finish(tx_data:TransactionData)

func sign_transaction(instructions:Array[Instruction],tx_commitment:Commitment=Commitment.CONFIRMED,priority_fee:float=0.0,custom_signer=null) -> TransactionData:
	on_tx_init.emit()
	var transaction:Transaction = Transaction.new()	
	add_child(transaction)
	#
	for idx in range(instructions.size()):
		if instructions[idx] == null:
			push_error("instruction %s is null, couldn't build a transaction!"%idx)
			on_tx_finish.emit("")
			return TransactionData.new({})
			
		transaction.add_instruction(instructions[idx])
	
	var wallet
	if custom_signer!=null:
		wallet = custom_signer
	else:
		wallet = SolanaService.wallet.get_kp()
	transaction.set_payer(wallet)
	
	transaction.set_unit_limit(priority_fee)
	transaction.set_unit_price(priority_fee)
	
	transaction.update_latest_blockhash()
	await transaction.blockhash_updated
	
	transaction.sign()
	#await transaction.fully_signed
	print("SIGNED")
	on_tx_signed.emit()
	#var serialized_tx:PackedByteArray = transaction.serialize()
	var tx_data:TransactionData = await send_transaction(transaction,tx_commitment)
	
	transaction.queue_free()
	on_tx_finish.emit(tx_data)
	return tx_data
	

func sign_serialized_transaction(wallet,transaction_bytes:PackedByteArray,tx_commitment:Commitment=Commitment.CONFIRMED) -> TransactionData:
	on_tx_init.emit()
	var transaction:Transaction = Transaction.new_from_bytes(transaction_bytes)

	add_child(transaction)
	transaction.set_signers([wallet])
	
	var tx_data:TransactionData = await send_transaction(transaction,tx_commitment)
	
	transaction.queue_free()
	on_tx_finish.emit(tx_data)
	
	return tx_data
	
func send_transaction(tx:Transaction,tx_commitment:Commitment=Commitment.CONFIRMED) -> TransactionData:
	tx.send()
	var response:Dictionary = await tx.transaction_response_received
	var tx_data:TransactionData = TransactionData.new(response)
	
	if !tx_data.is_successful():
		print(tx_data.get_error_message())
		return

	match tx_commitment:
		Commitment.PROCESSED:
			await tx.processed
		Commitment.CONFIRMED:
			await tx.confirmed
		Commitment.FINALIZED:
			await tx.finalized  
				
	print_rich("[url]%s[/url]" % tx_data.get_link())
	return tx_data
		
	
# a couple of default transactions (sol_transfer, spl_transfer)
	
func transfer_sol(receiver:String,amount:float,tx_commitment=Commitment.CONFIRMED,priority_fee:float=0.0, custom_sender:Keypair=null) -> TransactionData:
	var instructions:Array[Instruction]
	
	var sender_keypair = SolanaService.wallet.get_kp()
	var sender_account:Pubkey = SolanaService.wallet.get_pubkey()
	if custom_sender!=null:
		sender_keypair = custom_sender
		sender_account = Pubkey.new_from_string(custom_sender.get_public_string())
	
	var receiver_account:Pubkey = Pubkey.new_from_string(receiver)  
	
	var amount_in_lamports = int(amount*1000000000)
	
	var sol_transfer_ix:Instruction = SystemProgram.transfer(sender_account,receiver_account,amount_in_lamports)
	instructions.append(sol_transfer_ix)
	
	if custom_sender!=null:
		var tx_data:TransactionData = await sign_transaction(instructions,tx_commitment,priority_fee,custom_sender)
		return tx_data
	else:
		var tx_data:TransactionData = await sign_transaction(instructions)
		return tx_data

func transfer_token(token_address:String,receiver:String,amount:float,tx_commitment=Commitment.CONFIRMED,priority_fee:float=0.0,custom_sender:Keypair=null) -> TransactionData:
	var instructions:Array[Instruction]
	
	var sender_keypair = SolanaService.wallet.get_kp()
	var sender_account:Pubkey = SolanaService.wallet.get_pubkey()
	if custom_sender!=null:
		sender_keypair = custom_sender
		sender_account = Pubkey.new_from_string(custom_sender.get_public_string())
		
	var receiver_account:Pubkey = Pubkey.new_from_string(receiver) 
	var token_mint:Pubkey = Pubkey.new_from_string(token_address) 
	
	var sender_ata:Pubkey = await SolanaService.get_associated_token_account(sender_account.to_string(),token_address)
	
	#check if an ATA for this token exists in wallet. if not, add initalize as instruction
	var receiver_ata:Pubkey = await SolanaService.get_associated_token_account(receiver_account.to_string(),token_address)
	if receiver_ata == null:
		receiver_ata = Pubkey.new_associated_token_address(receiver_account,token_mint)
		var init_ata_ix:Instruction = AssociatedTokenAccountProgram.create_associated_token_account(
			sender_account,
			receiver_account,
			token_mint,
			SolanaService.associated_token_pid
			)
		instructions.append(init_ata_ix)
		
	#get the decimals of the token to multiply by the amount provided
	var token_decimals = await SolanaService.get_token_decimals(token_address)
	var transfer_ix:Instruction = TokenProgram.transfer_checked(sender_ata,token_mint,receiver_ata,sender_account,amount,token_decimals)
	instructions.append(transfer_ix)
	
	if custom_sender!=null:
		var tx_data:TransactionData = await sign_transaction(instructions,tx_commitment,priority_fee,custom_sender)
		return tx_data
	else:
		var tx_data:TransactionData = await sign_transaction(instructions)
		return tx_data
