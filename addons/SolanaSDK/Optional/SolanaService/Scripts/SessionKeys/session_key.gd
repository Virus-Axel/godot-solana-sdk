extends Node
class_name SessionKey

var signer:Keypair
var token:Pubkey
var authority:Pubkey
var expiry_timestamp:int

func _init(session_keypair:Keypair,session_token:Pubkey, auth:Pubkey,valid_until:int) -> void:
	signer = session_keypair
	token = session_token
	authority = auth
	expiry_timestamp = valid_until
	
func get_kp() -> Keypair:
	return signer
	
func get_token() -> Pubkey:
	return token
	
func get_expiry_timestamp() -> int:
	return expiry_timestamp
	
func is_valid() -> bool:
	var curr_timestamp:int = Time.get_unix_time_from_system()
	return curr_timestamp < expiry_timestamp
	
