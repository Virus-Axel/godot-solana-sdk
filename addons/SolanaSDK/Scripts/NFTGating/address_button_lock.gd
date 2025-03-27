extends ButtonLock
class_name AddressButtonLock

@export_file("*.json") var wallet_snapshot_path:String

var address_list:Array[Pubkey]=[]

func _ready() -> void:
	super._ready()
	if wallet_snapshot_path!="":
		var wallet_addresses:Array[String] = get_hashlist_from_file(wallet_snapshot_path)
		for address in wallet_addresses:
			address_list.append(Pubkey.new_from_string(address))

func try_unlock() -> void:
	if !SolanaService.wallet.is_logged_in():
		return
		
	var user_in_list:bool=false
	for address in address_list:
		if address.to_string() == SolanaService.wallet.get_pubkey().to_string():
			user_in_list=true
			break
	set_interactable(user_in_list)
	
func set_address_list(list:Array[Pubkey]) -> void:
	address_list = list
	try_unlock()
	
