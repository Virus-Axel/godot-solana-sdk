extends ButtonLock

@export var token_address:String
@export var unlock_amount:float


func try_unlock() -> void:
	super()
	var account_address = SolanaService.wallet.get_pubkey()
	var token_balance:float
	if token_address.length()==0:
			token_balance = await SolanaService.get_balance(account_address.to_string())
	else:
		token_balance = await SolanaService.get_balance(account_address.to_string(),token_address)
	set_interactable(token_balance>=unlock_amount)
