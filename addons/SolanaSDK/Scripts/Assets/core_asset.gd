extends WalletAsset
class_name CoreAsset

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
		
#func get_collection_mint() -> Pubkey:
	#var nft_collection:MetaDataCollection = metadata.get_collection()[0]
	#return nft_collection.key
	
func get_plugin_data() -> Dictionary:
	if das_metadata.size()==0:
		return {}
	return das_metadata["interface"]["plugins"]
	
func get_external_plugin_data() -> Dictionary:
	if das_metadata.size()==0:
		return {}
	return das_metadata["interface"]["external_plugins"]
	
func is_3D_model() -> bool:
	return get_category() == "vr"
	
func try_load_model() -> void:
	model_state = await SolanaService.file_loader.load_3d_model(offchain_metadata["animation_url"])
	
