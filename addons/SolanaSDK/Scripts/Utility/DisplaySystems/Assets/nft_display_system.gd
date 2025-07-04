extends AssetDisplaySystem
class_name NFTDisplaySystem

@export var collection_filter:Array[NFTCollection]

func filter_loaded_asset(asset:WalletAsset) -> void:
	if asset is not Nft and asset is not CoreAsset:
		return
	
	if belongs_to_collection_filter(asset):
		add_to_list(asset)

func setup_owned_assets() -> void:
	var assets:Array[WalletAsset]
	if collection_filter.size()==0:
		assets = SolanaService.asset_manager.get_owned_assets()
	else:
		for collection in collection_filter:
			var collection_assets:Array[WalletAsset] = SolanaService.asset_manager.get_owned_assets_from_collection(collection)
			assets.append_array(collection_assets)
			
	for asset in assets:
		add_to_list(asset)
		
		
func belongs_to_collection_filter(asset:WalletAsset) -> bool:
	if collection_filter.size() == 0:
		return true
		
	var pass_collection_filter= (collection_filter.size()==0)
	for collection in collection_filter:
		if collection.belongs_to_collection(asset):
			return true
			
	return false
