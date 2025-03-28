extends WalletAsset
class_name Token

var token_account:Pubkey
var decimals:int
var balance:float

func refresh_balance() -> void:
	if decimals == 0:
		await SolanaService.get_token_decimals(mint.to_string())
	
	if token_account == null:
		token_account = Pubkey.new_associated_token_address(SolanaService.wallet.get_pubkey(),mint)
	balance = await SolanaService.get_ata_balance(token_account.to_string())

func get_balance(fetch_new:bool=false) -> float:
	if balance == 0 or fetch_new:
		await refresh_balance()
	return balance
	
func get_lamport_balance(fetch_new:bool=false) -> float:
	if balance == 0 or fetch_new:
		await refresh_balance()
	return balance*(10**decimals)
	
