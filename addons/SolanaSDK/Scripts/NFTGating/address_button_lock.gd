extends ButtonLock

@export_file("*.json") var wallet_snapshot_path:String

func try_unlock() -> void:
	var wallet_addresses:Array[String] = get_hashlist_from_file(wallet_snapshot_path)
	super()

	var user_address:String = SolanaService.wallet.get_pubkey().to_string()
	set_interactable(wallet_addresses.has(user_address))
	

