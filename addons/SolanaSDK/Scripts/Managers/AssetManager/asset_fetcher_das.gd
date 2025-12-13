extends Node
class_name AssetFetcherDAS
@export var max_asset_count:int=1000

@export var collections_filter:Array[String]

var das_data_cache:Dictionary

func fetch_assets(wallet_to_check:Pubkey, force_data_refresh:bool=false) -> Array:
	var das_assets:Array
	
	if das_data_cache.has(wallet_to_check.to_string()) and !force_data_refresh:
		das_assets = das_data_cache[wallet_to_check.to_string()]
	else:
		das_assets = await SolanaService.get_wallet_assets_data(wallet_to_check,max_asset_count)
		das_data_cache[wallet_to_check.to_string()] = das_assets
		
	if collections_filter.size() > 0:
		das_assets = filter_by_collections(das_assets)
		
	for asset in das_assets:
		if asset["burnt"]:
			das_assets.erase(asset)
	
	return das_assets
	
func filter_by_collections(assets_data:Array) -> Array:
	var filtered_assets:Array
	for asset_data in assets_data:
		if asset_data["grouping"].size()==0:
			continue
#			check if has group of "collection
		var collection_group_index:int = -1
		var collection_found:bool=false
		for group_type in asset_data["grouping"]:
			collection_group_index+=1
			if group_type["group_key"] == "collection":
				collection_found=true
				break
		if !collection_found or collection_group_index == -1:
			continue
			
		var asset_collection_id:String = asset_data["grouping"][collection_group_index]["group_value"]
		for collection in collections_filter:
			if asset_collection_id == collection:
				filtered_assets.append(asset_data)
				break	
		
	return filtered_assets
	
func add_collection_filter(collection_key:Pubkey) -> void:
	var collection_address:String = collection_key.to_string()
	if !collections_filter.has(collection_address):
		collections_filter.append(collection_address)
