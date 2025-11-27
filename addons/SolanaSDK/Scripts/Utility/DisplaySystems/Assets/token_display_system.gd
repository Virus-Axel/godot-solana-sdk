extends AssetDisplaySystem
class_name TokenDisplaySystem

@export var show_zero_balance:bool=false

func filter_loaded_asset(asset:WalletAsset) -> void:
	if asset is not Token:
		return
	
	var token = asset as Token
	if !show_zero_balance:
		var balance = await token.get_balance()
		if balance <= 0:
			return

	add_to_list(asset)
	
func setup_owned_assets() -> void:
	var assets:Array[WalletAsset]
	assets = SolanaService.asset_manager.get_owned_tokens()
	
	for asset in assets:
		add_to_list(asset)

	
