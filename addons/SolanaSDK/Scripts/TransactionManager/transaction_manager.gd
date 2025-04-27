extends Node
class_name TransactionManager

@export var confirmation_check_rate:float=1.0
@export var confirmation_timeout:float = 35.0
## wallet adapters automatically add unit limit of 800000 and unit price of 8000
@export var use_custom_priority_fee:bool
## used to fetch estimated unit price. if you don't configure it with your own rpc key, fallback value will be used
@export var helius_api:HeliusAPI
@export var fallback_compute_unit_limit = 800000
@export var fallback_compute_unit_price = 8000

enum Commitment{PROCESSED,CONFIRMED,FINALIZED}

var tx_blockhash:String
var tx_timeout_timer:Timer

var failed_consecutive_tx_count:int=0

signal on_tx_create_start
signal on_tx_sign_start
signal on_tx_signed(confirmed:bool)
signal on_tx_finish(tx_data:TransactionData)
signal on_tx_cancelled

signal on_tx_confirmed(success:bool)

func setup() -> void:
#	temporary solution. Don't use custom fees if using phantom
	#var selected_provider_id:int = SolanaService.wallet.get_wallet_provider_id()
	#if selected_provider_id == 0:
		#use_custom_priority_fee=false
	pass

func create_transaction(instructions:Array[Instruction],payer) -> Transaction:
	on_tx_create_start.emit()
	var transaction:Transaction = Transaction.new()	
	add_child(transaction)
	transaction.set_payer(payer)

	for idx in range(instructions.size()):
		if instructions[idx] == null:
			push_error("instruction %s is null, couldn't build a transaction!"%idx)
			return null
		print(idx)
		transaction.add_instruction(instructions[idx])
		print("HANDLED")
		
	transaction = await update_blockhash(transaction)
	
	if use_custom_priority_fee and SolanaService.rpc_cluster == SolanaService.RpcCluster.MAINNET:
		var consumed_units:int = await get_compute_units_used(transaction,20)
		transaction.set_unit_limit(consumed_units)
		var estimated_fee = await get_needed_unit_price(transaction)
		transaction.set_unit_price(estimated_fee)
		print("Using custom priority fee with following values:")
		print("Compute Unit Limit: ",consumed_units)
		print("Microlamports fee per Compute Unit: ",estimated_fee)

	return transaction
	

#for simple transaction with one signer
func sign_and_send(transaction:Transaction,tx_commitment:Commitment=Commitment.CONFIRMED,custom_signer=null) -> TransactionData:
	if transaction == null:
		on_tx_finish.emit(TransactionData.new({}))
		return TransactionData.new({})
		
	var signer = SolanaService.wallet.get_kp()
	if custom_signer!= null:
		signer = custom_signer
	
	transaction = await add_signature(transaction,signer)
	if transaction == null:
		return TransactionData.new({})
		
	var tx_data:TransactionData = await send_transaction(transaction,tx_commitment)
	
	return tx_data
	
func send_transaction(tx:Transaction,tx_commitment:Commitment=Commitment.CONFIRMED) -> TransactionData:
	print(tx.serialize())
	#trying to force a staked connection if network is considered congested	
	if helius_api!=null and SolanaService.rpc_cluster == SolanaService.RpcCluster.MAINNET:
		if helius_api.is_network_congested(tx.get_unit_price()):
			var staked_url = helius_api.get_rpc_url(true)
			if staked_url != "":
				print("CONGESTION IDENTIFIED, USING STAKED CONNECTION RPC!")
				print(staked_url)
				ProjectSettings.set_setting("solana_sdk/client/default_url",staked_url)
				#tx.set_url_override(staked_url)
		
	tx.send()
	var response:Dictionary = await tx.transaction_response_received
	var tx_data:TransactionData = TransactionData.new(response)
	
	if !tx_data.is_successful():
		print(tx_data.get_error_message())
		on_tx_finish.emit(tx_data)
		tx.queue_free()
		return tx_data
		
	var tx_signature:String = tx_data.data["result"]
	print("Transaction %s is sent! \nAwaiting confirmation..." % tx_signature)
	handle_transaction_confirmation(tx,tx_signature,tx_blockhash,tx_commitment)		
	var transaction_success:bool = await on_tx_confirmed
		
	if transaction_success:
		print_rich("[url]%s[/url]" % tx_data.get_link())
	else:
		tx_data = TransactionData.new({})
		
	tx.queue_free()
	on_tx_finish.emit(tx_data)
	return tx_data
	
	
var confirmation_in_progress:bool=false
func handle_transaction_confirmation(tx:Transaction,tx_signature,blockhash:String,tx_commitment:Commitment) -> void:
	confirmation_in_progress=true
	#match tx_commitment:
		#Commitment.PROCESSED:
			#tx.processed.connect(handle_transaction_pass,CONNECT_ONE_SHOT)
		#Commitment.CONFIRMED:
			#tx.confirmed.connect(handle_transaction_pass,CONNECT_ONE_SHOT)
		#Commitment.FINALIZED:
			#tx.finalized.connect(handle_transaction_pass,CONNECT_ONE_SHOT)
	
	var wait_time:float=0
#	this can freeze and not return anything if after transaction polling for 30 seconds
#	the transaction doesnt land. so we need to add a timer and check for confirmation
	while confirmation_in_progress:
		print("Transaction in progress, please wait...")
		tx_timeout_timer = create_timeout_timer()
		tx_timeout_timer.start()
		await tx_timeout_timer.timeout
		tx_timeout_timer.queue_free()
		wait_time += confirmation_check_rate
		
#		if transaction is confirmed, then trigger signal and break out
		var tx_confirmed:bool = await SolanaService.is_transaction_confirmed([tx_signature])
		if tx_confirmed:
			handle_transaction_pass()
		elif wait_time >= confirmation_timeout:
			handle_transaction_timeout()
	
func handle_transaction_pass() -> void:
	if tx_timeout_timer!=null:
		tx_timeout_timer.queue_free()
	confirmation_in_progress=false
	failed_consecutive_tx_count=0
	on_tx_confirmed.emit(true)
	
func handle_transaction_timeout() -> void:
	if tx_timeout_timer!=null:
		tx_timeout_timer.queue_free()
	confirmation_in_progress=false
	failed_consecutive_tx_count+=1
	on_tx_confirmed.emit(false)

func create_timeout_timer() -> Timer:
	var timer = Timer.new()
	timer.wait_time = confirmation_check_rate
	timer.one_shot = true
	add_child(timer)
	return timer
	

func sign_transaction_serialized(tx_bytes:PackedByteArray, signer, all_needed_signers:Array) -> Transaction:
	var transaction:Transaction = Transaction.new_from_bytes(tx_bytes)
	add_child(transaction)
	transaction.set_signers(all_needed_signers)
	
	transaction = await add_signature(transaction,signer)
	return transaction
	
func add_signature(transaction:Transaction,signer) -> Transaction:
	on_tx_sign_start.emit()
	
	transaction.partially_sign([signer])
	if signer is WalletAdapter:
		transaction.signer_state_changed.connect(handle_transaction_approve,CONNECT_ONE_SHOT)
		transaction.signing_failed.connect(handle_transaction_cancel,CONNECT_ONE_SHOT)
		var confirmed:bool = await on_tx_signed
		if !confirmed:
			on_tx_cancelled.emit()
			return null
	return transaction
	
func handle_transaction_approve() -> void:
	print("Partial Signature Added!")
	on_tx_signed.emit(true)
	
func handle_transaction_cancel(signer_id:int) -> void:
	print("Transaction Cancelled")
	on_tx_signed.emit(false)
	
func update_blockhash(transaction:Transaction) -> Transaction:
	transaction.update_latest_blockhash()
	var response:Dictionary = await transaction.blockhash_updated
	if response.has("result"):
		tx_blockhash = response["result"]["value"]["blockhash"]
	else:
		print("Failed To Fetch valid blockhash for transaction")
		tx_blockhash = ""
	
	return transaction
	
func get_compute_units_used(transaction:Transaction, inflate_percentage:int=0) -> int:
	var simulated_tx_data:Dictionary = await SolanaService.simulate_transaction(transaction)

	if simulated_tx_data.size() == 0:
		return fallback_compute_unit_limit
		
	var consumed_units = int(simulated_tx_data["result"]["value"]["unitsConsumed"])
	if consumed_units == 0:
		return fallback_compute_unit_limit
		
	if inflate_percentage > 0:
		var inflate_amount:float = inflate_percentage/float(100)
		consumed_units += consumed_units*inflate_amount
		
	return consumed_units
	
func get_needed_unit_price(transaction:Transaction) -> int:
	var estimated_unit_price:int = 0
	if helius_api!=null:
		estimated_unit_price = await helius_api.get_estimated_priority_fee(transaction)
		
	if estimated_unit_price == 0:
		print("Failed to fetch estimated priority fee, using default value...")
		estimated_unit_price = fallback_compute_unit_price
	
#	if solflare, make sure minimum price is 100000
	var wallet_provider_id = SolanaService.wallet.get_wallet_provider_id()
	if wallet_provider_id == 1:
		estimated_unit_price = max(estimated_unit_price,100000)
		
	estimated_unit_price += estimated_unit_price*failed_consecutive_tx_count
	return estimated_unit_price
	
	
func transfer_sol(receiver:Pubkey,amount:float,tx_commitment=Commitment.CONFIRMED, custom_sender:Keypair=null) -> TransactionData:
	var instructions:Array[Instruction]
	
	var sender_keypair = SolanaService.wallet.get_kp()
	var sender_account:Pubkey = SolanaService.wallet.get_pubkey()
	if custom_sender!=null:
		sender_keypair = custom_sender
		sender_account = custom_sender.to_pubkey()
	
	var amount_in_lamports = int(amount*pow(10,9))
	
	var sol_transfer_ix:Instruction = SystemProgram.transfer(sender_account,receiver,amount_in_lamports)
	instructions.append(sol_transfer_ix)
	
	var transaction:Transaction = await create_transaction(instructions,sender_keypair)
	
	if custom_sender!=null:
		var tx_data:TransactionData = await sign_and_send(transaction,tx_commitment,custom_sender)
		return tx_data
	else:
		var tx_data:TransactionData = await sign_and_send(transaction,tx_commitment)
		return tx_data

func transfer_token(token_mint:Pubkey,receiver:Pubkey,amount:float,tx_commitment=Commitment.CONFIRMED,custom_sender:Keypair=null) -> TransactionData:
	var instructions:Array[Instruction]
	
	var sender_keypair = SolanaService.wallet.get_kp()
	var sender_account:Pubkey = SolanaService.wallet.get_pubkey()
	if custom_sender!=null:
		sender_keypair = custom_sender
		sender_account = custom_sender.to_pubkey()
	
	var sender_ata:Pubkey = await SolanaService.get_associated_token_account(sender_account.to_string(),token_mint.to_string())
	
	#check if an ATA for this token exists in wallet. if not, add initalize as instruction
	var receiver_ata:Pubkey = await SolanaService.get_associated_token_account(receiver.to_string(),token_mint.to_string())
	if receiver_ata == null:
		receiver_ata = Pubkey.new_associated_token_address(receiver,token_mint)
		var init_ata_ix:Instruction = AssociatedTokenAccountProgram.create_associated_token_account(
			sender_keypair,
			receiver,
			token_mint,
			SolanaService.TOKEN_PID
			)
		instructions.append(init_ata_ix)
		
	#get the decimals of the token to multiply by the amount provided
	var token_decimals = await SolanaService.get_token_decimals(token_mint.to_string())
	var decimal_amount = amount*pow(10,token_decimals)
	var transfer_ix:Instruction = TokenProgram.transfer_checked(sender_ata,token_mint,receiver_ata,sender_keypair,decimal_amount,token_decimals)
	instructions.append(transfer_ix)
	
	var transaction:Transaction = await create_transaction(instructions,sender_keypair)
	
	if custom_sender!=null:
		var tx_data:TransactionData = await sign_and_send(transaction,tx_commitment,custom_sender)
		return tx_data
	else:
		var tx_data:TransactionData = await sign_and_send(transaction)
		return tx_data
