extends ButtonLock

enum NFTLockType {HELD_AMOUNT,MINT_MATCH}

@export var lock_type:NFTLockType

@export var collection_gate:NFTCollection
@export var unlock_amount:int
@export_file("*.json") var mint_list_path:String

func _ready() -> void:
	super()
	if lock_active:
		SolanaService.asset_manager.on_asset_loaded.connect(handle_asset_loaded)
		
func handle_asset_loaded(asset:WalletAsset) -> void:
	try_unlock()
	
func try_unlock() -> void:
	super()
	var held_nft_list:Array[Nft] = SolanaService.asset_manager.get_nfts_from_collection(collection_gate)
	
	match lock_type:
		NFTLockType.HELD_AMOUNT:
			set_interactable(held_nft_list.size()>=unlock_amount)
		NFTLockType.MINT_MATCH:
			var mint_list = get_hashlist_from_file(mint_list_path)
			var in_list=false
			for nft in held_nft_list:
				if mint_list.has(nft.mint.to_string()):
					in_list=true
					break
			set_interactable(in_list)
