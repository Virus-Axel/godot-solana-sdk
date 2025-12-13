extends WalletAsset
class_name Token

@export var token_account:Pubkey
@export var decimals:int
@export var balance:float

func set_data(mint_address:Pubkey,metadata:MetaData,asset_data:Dictionary,asset_type:AssetManager.AssetType,autoload_image:bool=false,image_size:int=256) -> void:
	await super.set_data(mint_address,metadata,asset_data,asset_type,autoload_image,image_size)
	
	if das_metadata.size() > 0:
		decimals = das_metadata["token_info"]["decimals"]

func refresh_balance() -> void:
	if decimals == 0:
		await SolanaService.get_token_decimals(mint.to_string())
	
	if token_account == null:
		#token_account = Pubkey.new_associated_token_address(Pubkey.new_from_string("9DLXWVfBA6pwJHxJ6MJ7tVQR19xbRpFTStEEJqtypxyH"),mint,Pubkey.new_from_string(SolanaService.TOKEN_PID))
		token_account = Pubkey.new_associated_token_address(SolanaService.wallet.get_pubkey(),mint,Pubkey.new_from_string(SolanaService.TOKEN_PID))
		
	balance = await SolanaService.get_ata_balance(token_account.to_string())

func get_balance(fetch_new:bool=false) -> float:
	if balance == 0 or fetch_new:
		await refresh_balance()
	return balance
	
func get_lamport_balance(fetch_new:bool=false) -> float:
	if balance == 0 or fetch_new:
		await refresh_balance()
	return balance*(10**decimals)
	
func get_asset_owner() -> Pubkey:
	if das_metadata.size() > 0:
		return Pubkey.new_from_string(das_metadata["ownership"]["owner"])
		
	var token_account:Pubkey = await SolanaService.get_largest_account(mint)
	var account_info:Dictionary = await SolanaService.get_account_info(token_account)
	var owner_address:Pubkey = account_info["result"]["value"]["owner"]
	return owner_address
	
	
