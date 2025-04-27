extends Node
class_name WalletAsset

var mint:Pubkey

var asset_name:String
var symbol:String
var image:Texture2D
var uri:String

var asset_type:AssetManager.AssetType

var metadata:MetaData
var offchain_metadata:Dictionary

var das_metadata:Dictionary

signal on_data_loaded
signal on_image_loaded

func set_data(mint_address:Pubkey,metadata:MetaData,asset_data:Dictionary,asset_type:AssetManager.AssetType,autoload_image:bool=false,image_size:int=256) -> void:
	mint = mint_address
	self.metadata = metadata
	asset_name = metadata.get_token_name()
	symbol = metadata.get_symbol()
	self.asset_type = asset_type
	
	if asset_data.size()>0:
		das_metadata = asset_data
		uri = asset_data["content"]["json_uri"]
		offchain_metadata = asset_data["content"]["metadata"]

		if asset_data["content"]["links"].has("image"):
			offchain_metadata["image"] = asset_data["content"]["links"]["image"]
	else:
		uri = metadata.get_uri()
		if uri != null and uri.length() > 0:
			offchain_metadata = await SolanaService.file_loader.load_token_metadata(uri)
			if offchain_metadata.size() == 0:
				print("Offchain metadata of %s failed to load" % mint_address.to_string())
				
	if autoload_image:
		try_load_image(image_size)
		
	on_data_loaded.emit()
		
func try_load_image(size:int=256) -> void:
	if offchain_metadata.size()==0:
		image = SolanaService.asset_manager.missing_texture_visual
		return
	
	var image_link:String = ""
	
#	first go through files and check if theres any png/jpg image
#   because default could be gif/animated webp and mess everything up
	if offchain_metadata.has("properties"):
		for file in offchain_metadata["properties"]["files"]:
			if file.has("type") and (file["type"] == "image/png" or file["type"] == "image/jpg"):
				if file.has("uri"):
					image_link = file["uri"]
				elif file.has("file"):
					image_link = file["file"]	
			
	if image_link == "" and offchain_metadata.has("image"):
		image_link = offchain_metadata["image"]
		
	if image_link == "":
		print("image does not exist on mint: %s" % mint.to_string())
		image = SolanaService.asset_manager.missing_texture_visual
		return
		
	image = await SolanaService.file_loader.load_token_image(image_link,size)
	if image == null:
		push_warning("Couldn't fetch image for mint: %s" % mint.to_string())
		image = SolanaService.asset_manager.missing_texture_visual
	on_image_loaded.emit()
	
func get_collection_mint() -> Pubkey:
	return null
	
	
