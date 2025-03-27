extends ButtonLock

@export var token_gate_list:Dictionary
@export var update_balances:bool

var balance_cache:Dictionary

func try_unlock() -> void:
	super()
	if !lock_active:
		return
		
	var account_address = SolanaService.wallet.get_pubkey()
	if account_address == null:
		set_interactable(false)
		return
	
	var all_amounts_met:bool=true
	for key in token_gate_list.keys():
		var token_balance:float
		var token_address:String = key
		var unlock_amount = token_gate_list[key]
		if balance_cache.has(key) and !update_balances:
			token_balance = balance_cache[key]
		else:
			if token_address.length()==0:
				token_balance = await SolanaService.get_balance(account_address.to_string())
				balance_cache[key] = token_balance
			else:
				token_balance = await SolanaService.get_balance(account_address.to_string(),token_address)
				balance_cache[key] = token_balance
			
		if token_balance<unlock_amount:
			all_amounts_met=false
			break
	
	set_interactable(all_amounts_met)


func add_token_gate(token:Pubkey,amount_needed:float) -> void:
	var address:String = ""
	if token!=null:
		address = token.to_string()
	
	token_gate_list[address] = amount_needed

func clear_gating() -> void:
	token_gate_list.clear()
