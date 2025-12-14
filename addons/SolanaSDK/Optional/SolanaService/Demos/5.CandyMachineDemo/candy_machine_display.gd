extends Node
class_name CandyMachineDisplay

@export var minted_amount:Label
@export var progress_bar:ProgressBar
@export var collection_displayable:DisplayableAsset
# Called when the node enters the scene tree for the first time.
	
func load_cm_display(cm_data) -> void:
	if minted_amount!=null:
		minted_amount.text = "%s/%s Minted" % [cm_data["items_redeemed"],cm_data["items_available"]]
	if progress_bar!=null:
		progress_bar.value = float(cm_data["items_redeemed"])/float(cm_data["items_available"])
		
	if collection_displayable!=null:
		var collection_asset:WalletAsset = await SolanaService.asset_manager.get_asset_from_mint(cm_data["collection_mint"],true)
		await collection_displayable.set_data(collection_asset)
