extends Node
class_name AssetManager

enum AssetType {NONE,NFT, TOKEN, CORE, MISSING}

@onready var asset_fetcher_das:AssetFetcherDAS = $AssetFetcherDAS

@export var load_on_login:bool
@export var load_asset_textures:bool
@export var load_token_balances:bool
@export var log_loaded_assets:bool=false

@export var missing_texture_visual:Texture2D

var owned_assets:Array[WalletAsset]

var is_loading:bool=false

var metadata_cache:Dictionary

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
	#var wallet_to_load:Pubkey = Pubkey.new_from_string("84pL2GAuv8XGVPyZre2xcgUNSGz9csYHBt5AW4PUcEe9")
	owned_assets.clear()
	
	#loading from OG and DAS are 2 different ways completely
	if SolanaService.get_das_rpc()!=null:
		var das_assets:Array = await asset_fetcher_das.fetch_assets(wallet_to_load)
		await load_das_assets(das_assets)
		
	if !das_only:
		#	all assets loaded previous by DAS will be skipped, so this will only load user's tokens
		var wallet_token_accounts:Array[Dictionary] = await SolanaService.get_token_accounts(wallet_to_load)
		await load_user_assets_og(wallet_token_accounts)
	else:
		print("Skipping load of assets OG way, because DAS-ONLY load is selected")
		
	if load_token_balances:
		for token in get_owned_tokens():
			token.refresh_balance()
		
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
		add_to_owned_assets(asset)
		
		
func load_das_assets(das_assets:Array) -> void:
	for asset_data in das_assets:			
		var asset_mint:Pubkey = Pubkey.new_from_string(asset_data["id"])
		var asset:WalletAsset = await create_asset(asset_mint,asset_data,load_asset_textures)
		if asset == null:
			continue
			
		if !metadata_cache.has(asset.mint.to_string()):
			metadata_cache[asset.mint.to_string()] = asset_data
		add_to_owned_assets(asset)
		
func add_to_owned_assets(asset:WalletAsset):
	if log_loaded_assets:
		print("Loaded: ",asset.asset_name," ",asset.mint.to_string())
	owned_assets.append(asset)
	on_asset_loaded.emit(asset)
		
func get_asset_from_mint(asset_mint:Pubkey, load_texture:bool=false) -> WalletAsset:		
	var asset_data = await fetch_asset_data(asset_mint)
	if asset_data == null or asset_data.size() == 0:
		return null
	
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
		if asset_data.has("burnt") and asset_data["burnt"]:
			asset_data=null
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
		metadata = MetaData.new()
		metadata.copy_from_dict(asset_data)
		
	elif asset_data is MetaData:
		metadata = asset_data
		asset_data = {}
		
	var existing_asset:WalletAsset = get_owned_asset(metadata.get_mint())
	if existing_asset!= null:
		return existing_asset
		
	var asset_type:AssetType = get_asset_type(asset_data)
	
	if asset_type == AssetType.NONE and (asset_data["interface"] == "MplCoreAsset" or asset_data["interface"] == "MplCoreCollection"):
		asset_type = AssetType.CORE
		
	match asset_type:
		AssetType.MISSING:
			var asset:WalletAsset = WalletAsset.new()
			await asset.set_data(asset_mint,metadata,asset_data,asset_type,load_texture)
			owned_assets.append(asset)
			return asset
		AssetType.NFT:
			var nft:Nft = Nft.new()
			await nft.set_data(asset_mint,metadata,asset_data,asset_type,load_texture)
			owned_assets.append(nft)
			return nft
		AssetType.TOKEN:
			var token:Token = Token.new()
			await token.set_data(asset_mint,metadata,asset_data,asset_type,load_texture)
			owned_assets.append(token)
			return token
		AssetType.CORE:
			var asset:CoreAsset = CoreAsset.new()
			await asset.set_data(asset_mint,metadata,asset_data,asset_type,load_texture)
			owned_assets.append(asset)
			return asset
			
	return null
	
func add_collection_to_whitelist(collection_key:Pubkey) -> void:
	asset_fetcher_das.add_collection_filter(collection_key)
	
# 0 - Non fungible token (Simple NFT)
# 1 - Semi fungible token (SFT)
# 2 - Fungible token (SPL)
# 3 - Non fungible edition (NFT edition)
# 4 - Programmable NFT
# 5 - Programmable NFT Edition		
func get_asset_type(asset_data) -> AssetType:
	if asset_data is MetaData:
		if asset_data.get_token_standard() == null:
			return AssetType.NONE
			
		var token_standard:int = asset_data.get_token_standard()
		match token_standard:
			1,2:
				return AssetType.TOKEN
			0,3,4,5:
				return AssetType.NFT
				
		return AssetType.MISSING
	else:
		if asset_data["interface"] == "FungibleToken" or asset_data["interface"] == "FungibleAsset":
			return AssetType.TOKEN
		elif asset_data["interface"] == "V1_NFT":
			return AssetType.NFT
		elif asset_data["interface"] == "MplCoreAsset" or asset_data["interface"] == "MplCoreCollection":
			return AssetType.CORE
		else:
			print("UNKNOWN ASSET INTERFACE FOUND: ", asset_data["interface"])
			return AssetType.MISSING
			
		

func get_owned_asset(asset_mint:Pubkey) -> WalletAsset:
	for asset in owned_assets:
		if asset.mint.to_string() == asset_mint.to_string():
			if asset is Token:
#				for tokens create duplicates because might be a different token account each time with different balances
				var duplicate:Token = asset.duplicate(true)
				duplicate.token_account = null
				duplicate.balance = 0
				duplicate.decimals = 0
				return duplicate
			return asset
	return null

func get_owned_assets() -> Array[WalletAsset]:
	var assets:Array[WalletAsset]
	for asset in owned_assets:
		if asset is Nft or asset is CoreAsset:
			assets.append(asset)
	return assets
	
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
		if asset.get_collection_mint() == null:
			continue
		if asset.get_collection_mint().to_string() == collection_id.to_string():
			collection_assets.append(asset)
			
	return collection_assets
	
func get_owned_assets_from_collection(collection:NFTCollection) -> Array[WalletAsset]:
	var collection_assets:Array[WalletAsset]
	var owned_assets:Array[WalletAsset] = get_owned_assets()
	for asset in owned_assets:
		if collection.belongs_to_collection(asset):
			collection_assets.append(asset)
			
	return collection_assets
	
func is_assets_loaded() -> bool:
	return owned_assets.size()>0
