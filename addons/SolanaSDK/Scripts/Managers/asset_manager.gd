extends Node
class_name AssetManager

enum AssetType {NONE,NFT, TOKEN, ASSET, MISSING}

@export var load_on_login:bool
@export var load_asset_textures:bool
@export var load_token_balances:bool
@export var log_loaded_assets:bool=false

@export var missing_texture_visual:Texture2D

@export_category("DAS Specific Settings")
##collection ids which to load when loading DAS assets. Loads all if empty
@export var collection_id_whitelist:Array[String]

var owned_assets:Array[WalletAsset]

var is_loading:bool=false

var metadata_cache:Dictionary
var das_asset_data:Array


signal on_asset_load_started()
signal on_asset_loaded(asset:WalletAsset)
signal on_asset_load_finished(assets:Array[WalletAsset])

func setup() -> void:
	if load_on_login:
		SolanaService.wallet.on_login_success.connect(try_load_assets)

func try_load_assets() -> void:
	load_assets()
		
func load_assets(das_only:bool=false)->void:
#	if currently loading, dont trigger again
	if is_loading:
		return
		
	is_loading=true
	var wallet_to_load:Pubkey = SolanaService.wallet.get_pubkey()
	#var wallet_to_load:Pubkey = Pubkey.new_from_string("7cCiymR6zPr4t4ThHpF51F5Y1GfN6JciyS74fwWkiCuc")
	owned_assets.clear()
	
	on_asset_load_started.emit()
	#loading from OG and DAS are 2 different ways completely
	if SolanaService.get_das_rpc()!=null:
		if das_asset_data.size() == 0:
			das_asset_data = await SolanaService.get_wallet_assets_data(wallet_to_load,1000)
			on_asset_load_started.emit(das_asset_data.size())
		var assets_data:Array = das_asset_data
		await load_das_assets(assets_data)
		
#	all assets loaded previous by DAS will be skipped, so this will only load user's tokens
	if !das_only:
		#	all assets loaded previous by DAS will be skipped, so this will only load user's tokens
		var wallet_token_accounts:Array[Dictionary] = await SolanaService.get_token_accounts(wallet_to_load)
		await load_user_assets_og(wallet_token_accounts)
	else:
		print("Skipping load of assets OG way, because DAS-ONLY load is selected")
		
	on_asset_load_finished.emit(owned_assets)
	is_loading=false
	
func load_user_assets_og(token_accounts:Array[Dictionary]) -> void:
	for i in range(token_accounts.size()):	 
		var asset_mint:Pubkey = Pubkey.new_from_string(token_accounts[i]["mint"])
		if get_owned_asset(asset_mint) != null:
			continue
			
		var asset:WalletAsset = await get_asset_from_mint(asset_mint,load_asset_textures)
		if asset == null:
			continue

		if asset is Token && load_token_balances:
			var token = asset as Token
			token.decimals = await SolanaService.get_token_decimals(asset_mint.to_string())
			token.balance = token_accounts[i]["amount"] / pow(10,token.decimals)
			
		if log_loaded_assets:
			print("Loaded: ",asset.asset_name," ",asset.mint.to_string())
		owned_assets.append(asset)
		on_asset_loaded.emit(asset)
		
		
func load_das_assets(assets_data:Array) -> void:
	var assets_to_load:Array
	if collection_id_whitelist.size() > 0:
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
			for collection in collection_id_whitelist:
				if asset_collection_id == collection:
					assets_to_load.append(asset_data)
					break
	else:
		assets_to_load = assets_data

	for asset_data in assets_to_load:			
		var asset_mint:Pubkey = Pubkey.new_from_string(asset_data["id"])
			
		var asset:WalletAsset = await create_asset(asset_mint,asset_data,load_asset_textures)
		if asset == null:
			continue
		
		if log_loaded_assets:
			print("Loaded: ",asset.asset_name," ",asset.mint.to_string())
			
		if !metadata_cache.has(asset.mint.to_string()):
			metadata_cache[asset.mint.to_string()] = asset_data
		owned_assets.append(asset)
		on_asset_loaded.emit(asset)
		
#func load_assets_from_collection(nft_owner:Pubkey,collection_id:Pubkey) -> void:
	#match asset_fetch_mode:
		#AssetFetchMode.OG:	
			#push_error("This function only works in DAS fetch mode!")
			#return
		#AssetFetchMode.DAS:
			#var assets_data:Array = await SolanaService.get_collection_assets_data(nft_owner,collection_id)
			#print(assets_data)
			#await load_das_assets(assets_data)

func get_asset_from_mint(asset_mint:Pubkey, load_texture:bool=false) -> WalletAsset:		
	var asset_data = await fetch_asset_data(asset_mint)
	if asset_data == null or asset_data.size() == 0:
		return null
		
	var metadata:MetaData
	
	if asset_data is Dictionary:
		metadata = MetaData.new()
		metadata.copy_from_dict(asset_data)
		
	elif asset_data is MetaData:
		metadata = asset_data
	
	var asset:WalletAsset = await create_asset(asset_mint,asset_data,load_texture)
	return asset
	
#Returns either DAS data dictionary or MetaData class. Later you can convert DAS data into metadata class
func fetch_asset_data(asset_mint:Pubkey):
	var asset_data
	if metadata_cache.has(asset_mint.to_string()):
		#print("found metadata of %s in cache" % asset_mint.to_string())
		return metadata_cache[asset_mint.to_string()]
	
	if SolanaService.get_das_rpc()!=null:
		asset_data = await SolanaService.get_asset_data(asset_mint)
	else:
		var mpl_metadata:MplTokenMetadata = SolanaService.spawn_mpl_metadata_client()
		mpl_metadata.get_mint_metadata(asset_mint)
		asset_data = await mpl_metadata.metadata_fetched
		mpl_metadata.queue_free()
			
	metadata_cache[asset_mint.to_string()] = asset_data
	return asset_data
	
func create_asset(asset_mint:Pubkey, asset_data,load_texture:bool) -> WalletAsset:	
	var metadata:MetaData
	
	if asset_data is Dictionary:
		if asset_data["burnt"] == true:
#			burnt assets dont load pls
			return null
		metadata = MetaData.new()
		metadata.copy_from_dict(asset_data)
		
	elif asset_data is MetaData:
		metadata = asset_data
		asset_data = {}
		
	var existing_asset:WalletAsset = get_owned_asset(metadata.get_mint())
	if existing_asset!= null:
		print("Asset already loaded, skipping creation")
		return existing_asset
		
	var asset_type:AssetType = get_asset_type(metadata)
	if asset_type == AssetType.NONE and (asset_data["interface"] == "MplCoreAsset" or asset_data["interface"] == "MplCoreCollection"):
		asset_type = AssetType.ASSET
		
	match asset_type:
		AssetType.MISSING:
			var asset:WalletAsset = WalletAsset.new()
			await asset.set_data(asset_mint,metadata,asset_data,asset_type,load_texture)
			return asset
		AssetType.NFT:
			var nft:Nft = Nft.new()
			await nft.set_data(asset_mint,metadata,asset_data,asset_type,load_texture)
			return nft
		AssetType.TOKEN:
			var token:Token = Token.new()
			await token.set_data(asset_mint,metadata,asset_data,asset_type,load_texture)
			return token
		AssetType.ASSET:
			var asset:CoreAsset = CoreAsset.new()
			await asset.set_data(asset_mint,metadata,asset_data,asset_type,load_texture)
			return asset
			
	return null
	
func add_collection_to_whitelist(collection_key:Pubkey) -> void:
	var collection_id:String = collection_key.to_string()
	if !collection_id_whitelist.has(collection_id):
		collection_id_whitelist.append(collection_id)
	
# 0 - Non fungible token (Simple NFT)
# 1 - Semi fungible token (SFT)
# 2 - Fungible token (SPL)
# 3 - Non fungible edition (NFT edition)
# 4 - Programmable NFT
# 5 - Programmable NFT Edition		
func get_asset_type(metadata:MetaData) -> AssetType:
	if metadata.get_token_standard() == null:
		return AssetType.NONE
		
	var token_standard:int = metadata.get_token_standard()
	match token_standard:
		1,2:
			return AssetType.TOKEN
		0,3,4,5:
			return AssetType.NFT
			
	return AssetType.MISSING

func get_owned_asset(asset_mint:Pubkey) -> WalletAsset:
	for asset in owned_assets:
		if asset.mint.to_string() == asset_mint.to_string():
			return asset
	return null

func get_owned_assets() -> Array[WalletAsset]:
	var owned_assets:Array[WalletAsset]
	for asset in owned_assets:
		if asset is Nft:
			owned_assets.append(asset)
	return owned_assets
	
func get_owned_tokens() -> Array[WalletAsset]:
	var owned_tokens:Array[WalletAsset]
	for asset in owned_assets:
		if asset is Token:
			owned_tokens.append(asset)
	return owned_tokens
	
func get_owned_assets_from_collection_key(collection_id:Pubkey) -> Array[WalletAsset]:
	var collection_assets:Array[WalletAsset]
	var owned_assets:Array[WalletAsset] = get_owned_assets()
	for asset in owned_assets:
		if asset.metadata.get_collection() == null:
			continue
		if asset.metadata.get_collection().key.to_string() == collection_id.to_string():
			collection_assets.append(asset)
			
	return collection_assets
	
func get_owned_assets_from_collection(collection:NFTCollection) -> Array[WalletAsset]:
	var collection_assets:Array[WalletAsset]
	var owned_assets:Array[WalletAsset] = get_owned_assets()
	for asset in owned_assets:
		if collection.belongs_to_collection(asset):
			collection_assets.append(asset)
			
	return collection_assets
	
