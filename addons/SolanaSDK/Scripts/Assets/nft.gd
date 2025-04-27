extends WalletAsset
class_name Nft

#this is a state of a model, it can be used to instantiate a 3D node whenever using this:
#	var model:Node = gltf_document.generate_scene(state)
#	add_child(model)
var model_state:GLTFState
	
func get_category() -> String:
	if offchain_metadata.size() == 0:
		return ""
		
	if offchain_metadata["properties"].has("category"):
		return offchain_metadata["properties"]["category"]
	else:
		return ""
		
func get_collection_mint() -> Pubkey:
	if metadata.get_collection() == null:
		return null
		
	var nft_collection:MetaDataCollection = metadata.get_collection()
	return nft_collection.key
	
func is_3D_model() -> bool:
	return get_category() == "vr"
	
func try_load_model() -> void:
	model_state = await SolanaService.file_loader.load_3d_model(offchain_metadata["animation_url"])
	
	
	
