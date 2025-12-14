extends Node
class_name SessionKeyManager

@onready var program:AnchorProgram = $AnchorProgram

func get_program() -> AnchorProgram:
	return program
	
func get_pid() -> Pubkey:
	return Pubkey.new_from_string(program.get_pid())
	
func send_transaction(instructions:Array[Instruction],extra_signers:Array[Keypair]=[]) -> TransactionData:
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	for signer in extra_signers:
			transaction = await SolanaService.transaction_manager.add_signature(transaction,signer)
	await SolanaService.transaction_manager.add_signature(transaction,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.send_transaction(transaction)
	return tx_data
	
func create_session(target_program:Pubkey,session_keypair:Keypair,session_token:Pubkey,session_data:Dictionary) -> TransactionData:
	var ix:Instruction = get_create_session_ix(target_program,session_keypair,session_token,session_data)
	var tx_data:TransactionData = await send_transaction([ix],[session_keypair])
	return tx_data
	
func get_create_session_ix(target_program:Pubkey,session_keypair:Keypair,session_token:Pubkey,session_params:Dictionary) -> Instruction:
	var ix:Instruction = program.build_instruction("create_session",[
		session_token,
		session_keypair,
		SolanaService.wallet.get_kp(),
		target_program,
		SystemProgram.get_pid()
	],{
		"top_up":AnchorProgram.option(session_params["top_up"]),
		"valid_until": AnchorProgram.option(session_params["valid_until"]),
		"lamports":AnchorProgram.option(AnchorProgram.u64(session_params["lamports"]))
	})
	
	return ix
	
func revoke_session(session_token:Pubkey) -> TransactionData:
	var ix:Instruction = get_revoke_session_ix(session_token)
	var tx_data:TransactionData = await send_transaction([ix])
	return tx_data
	
func get_revoke_session_ix(session_token:Pubkey) -> Instruction:
	var ix:Instruction = program.build_instruction("revoke_session",[
		session_token,
		SolanaService.wallet.get_kp(),
		SystemProgram.get_pid()
	],null)
	
	return ix
	
