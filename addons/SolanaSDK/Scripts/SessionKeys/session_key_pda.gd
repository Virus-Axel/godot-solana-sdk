extends Node
class_name SessionKeyPDA

static var PROGRAM_ID:Pubkey = Pubkey.new_from_string("KeyspM2ssCJbqUhQ4k7sveSiY4WjnYsrXkC8oDbwde5")
	
static func get_session_token_pda(target_program:Pubkey,session_account:Pubkey,owner:Pubkey) -> Pubkey:
	var name_bytes = "session_token".to_utf8_buffer()
	return Pubkey.new_pda_bytes([
		name_bytes,
		target_program.to_bytes(),
		session_account.to_bytes(),
		owner.to_bytes()],
		PROGRAM_ID
	)
