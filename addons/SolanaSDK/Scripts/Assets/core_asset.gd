extends WalletAsset
class_name CoreAsset

#this is a state of a model, it can be used to instantiate a 3D node whenever using this:
#	var model:Node = gltf_document.generate_scene(state)
#	add_child(model)
@export var model_state:GLTFState
	
func get_category() -> String:
	if offchain_metadata.size() == 0:
		return ""
		
	if offchain_metadata["properties"].has("category"):
		return offchain_metadata["properties"]["category"]
	else:
		return ""
		
func get_authority() -> Pubkey:
	if das_metadata.size()==0:
		return null
		
	if das_metadata["authorities"].size() == 0:
		return null 
		
	for i in range(das_metadata["authorities"].size()):
		if "full" in das_metadata["authorities"][i]["scopes"]:
			return Pubkey.new_from_string(das_metadata["authorities"][i]["address"])
			
	return null
		
func get_collection_mint() -> Pubkey:
	if das_metadata.size()==0:
		return null
		
	if das_metadata["grouping"].size() == 0:
		return null 
	
	for i in range(das_metadata["grouping"].size()):
		if das_metadata["grouping"][i]["group_key"] == "collection":
			return Pubkey.new_from_string(das_metadata["grouping"][i]["group_value"])
	
	return null
	
func get_asset_owner() -> Pubkey:
	if das_metadata.size() == 0:
		return null

	return Pubkey.new_from_string(das_metadata["ownership"]["owner"])
	
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
	
