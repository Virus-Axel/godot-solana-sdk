extends Node
class_name AssetManager

enum AssetType {NONE,NFT, TOKEN}

@export var load_on_login:bool
@export var load_asset_textures:bool
@export var load_token_balances:bool

var owned_assets:Array[WalletAsset]

@export var missing_texture_visual:Texture2D

var is_loading:bool=false
var assets_loaded=false

signal on_asset_load_started(asset_keys:Array[Pubkey])
signal on_asset_loaded(asset:WalletAsset)
signal on_asset_load_finished(assets:Array[WalletAsset])

# Called when the node enters the scene tree for the first time.
func setup() -> void:
	if load_on_login:
		SolanaService.wallet.on_login_finish.connect(try_load_assets)
		
func try_load_assets(logged_in:bool) -> void:
	if logged_in:
		load_assets()
		
func load_assets()->void:
#	if currently loading, dont trigger again
	if is_loading:
		return
		
	is_loading=true
	assets_loaded=false
	
	var connected_wallet:Pubkey = SolanaService.wallet.get_pubkey()
	var wallet_assets:Array[Dictionary] = await SolanaService.get_wallet_assets(connected_wallet.to_string())
	on_asset_load_started.emit(wallet_assets.size())
	
	for i in range(wallet_assets.size()):	
		var asset_mint:Pubkey = Pubkey.new_from_string(wallet_assets[i]["mint"])
		var asset:WalletAsset = await get_asset_from_mint(asset_mint,load_asset_textures)
		if asset == null:
			continue
		
		if asset is Token && load_token_balances:
			var token = asset as Token
			token.lamport_balance = wallet_assets[i]["amount"]
			token.decimals = await SolanaService.get_token_decimals(asset_mint.to_string())
			
		owned_assets.append(asset)
		on_asset_loaded.emit(asset)
		
	on_asset_load_finished.emit(owned_assets)
	assets_loaded=true
	is_loading=false

func get_asset_from_mint(asset_mint:Pubkey, load_texture:bool=false) -> WalletAsset:
	var mpl_metadata:MplTokenMetadata = SolanaService.spawn_mpl_metadata_client()
	mpl_metadata.get_mint_metadata(asset_mint)
	var metadata:MetaData = await mpl_metadata.metadata_fetched
	mpl_metadata.queue_free()

	if metadata==null:
		return null
	var asset_type:AssetType = get_asset_type(metadata)
	match asset_type:
		AssetType.NONE:
			return null
		AssetType.NFT:
			var nft:Nft = Nft.new()
			await nft.set_data(asset_mint,metadata,asset_type,load_texture)
			return nft
			pass
		AssetType.TOKEN:
			var token:Token = Token.new()
			await token.set_data(asset_mint,metadata,asset_type,load_texture)
			return token
			pass
			
	return null
	
# 0 - Non fungible token (Simple NFT)
# 1 - Semi fungible token (SFT)
# 2 - Fungible token (SPL)
# 3 - Non fungible edition (NFT edition)
# 4 - Programmable NFT
# 5 - Programmable NFT Edition		
func get_asset_type(metadata:MetaData) -> AssetType:
	var token_standard:int = metadata.get_token_standard()
	match token_standard:
		1,2:
			return AssetType.TOKEN
		0,3,4,5:
			return AssetType.NFT
			
	return AssetType.NONE

func get_owned_nfts() -> Array[Nft]:
	var owned_nfts:Array[Nft]
	for asset in owned_assets:
		if asset is Nft:
			owned_nfts.append(asset)
	return owned_nfts
	
func get_owned_tokens() -> Array[Token]:
	var owned_tokens:Array[Token]
	for asset in owned_assets:
		if asset is Token:
			owned_tokens.append(asset)
	return owned_tokens
	
func get_nfts_from_collection(collection:NFTCollection) -> Array[Nft]:
	var collection_nfts:Array[Nft]
	var owned_nfts:Array[Nft] = get_owned_nfts()
	for nft in owned_nfts:
		if collection.belongs_to_collection(nft):
			collection_nfts.append(nft)
			
	return collection_nfts
	
