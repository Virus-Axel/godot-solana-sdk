extends Node
class_name WalletAsset

var mint:Pubkey

var asset_name:String
var symbol:String
var image:Texture2D
var balance:int

var type:AssetManager.AssetType

var metadata:MetaData
var offchain_metadata:Dictionary

signal on_data_loaded

func set_data(mint_address:Pubkey,token_metadata:MetaData,asset_type:AssetManager.AssetType,autoload_image:bool=false,image_size:int=256) -> void:
	mint = mint_address
	metadata = token_metadata
	asset_name = metadata.get_token_name()
	symbol = metadata.get_symbol()
	type = asset_type
	
	offchain_metadata = await SolanaService.file_loader.load_token_metadata(metadata.get_uri())
	if autoload_image:
		await try_load_image(image_size)
		
func try_load_image(size:int=256) -> void:
	if !offchain_metadata.has("image"):
		push_warning("image does not exist on mint: %s" % mint.to_string())
		image = SolanaService.asset_manager.missing_texture_visual
		return
		
	#print(offchain_metadata["image"])
	image = await SolanaService.file_loader.load_token_image(offchain_metadata["image"],size)
	if image == null:
		push_warning("Couldn't fetch image for mint: %s" % mint.to_string())
		image = SolanaService.asset_manager.missing_texture_visual
		
	
