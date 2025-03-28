extends Node
class_name MintedAssetDisplay

@export var displayable_asset:DisplayableAsset
@export var inspect_button:LinkedButton

@export var loading_screen:Control

func display_asset(asset_key: Pubkey) -> void:
	if loading_screen!=null:
		loading_screen.visible=true
		
	if inspect_button!=null:
		inspect_button.link = SolanaService.account_inspector.get_account_link(asset_key)
		
#	wait just a little bit to make sure asset is loaded up in chain
	await get_tree().create_timer(2).timeout
	
	var asset:WalletAsset = await SolanaService.asset_manager.get_asset_from_mint(asset_key,true)
	if asset!=null:
		await displayable_asset.set_data(asset)
	
	if loading_screen!=null:
		loading_screen.visible=false
	
	
func close() -> void:
	queue_free()
