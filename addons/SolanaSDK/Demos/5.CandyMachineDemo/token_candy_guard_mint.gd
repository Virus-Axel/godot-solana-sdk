extends Node

@export var mint_manager:CoreMintManager
@export var displayable_token:DisplayableAsset
@export var display_token_address:String
@export var price_label:NumberLabel
@export var mint_button:ButtonLock
@export var group_name:String = "default"

var token:Token = null

func _ready() -> void:
	mint_button.pressed.connect(initiate_mint)
	mint_manager.on_asset_minted.connect(handle_asset_mint)
	
	if SolanaService.wallet.is_logged_in():
		setup_mint_option()
	else:
		SolanaService.wallet.on_login_success.connect(setup_mint_option)
		
	if display_token_address!="":
		token = await SolanaService.asset_manager.get_asset_from_mint(Pubkey.new_from_string(display_token_address),true)
		await displayable_token.set_data(token)
	
		
func setup_mint_option() -> void:
	var mint_group_id:int = mint_manager.get_group_id_from_name(group_name)
	var guard_set:GuardSet
	if mint_group_id>=0:
		guard_set = mint_manager.guard_settings.groups[mint_group_id].guards
	else:
		guard_set = mint_manager.guard_settings.default
		
	await refresh_balance_display()
	
	var price:float
	
	if guard_set.enable_solPayment:
		price = guard_set.solPayment.lamports / pow(10,9)
	if guard_set.enable_tokenBurn:
		price = guard_set.tokenBurn.amount / pow(10,token.decimals)
	if guard_set.enable_tokenPayment:
		price = guard_set.tokenPayment.amount / pow(10,token.decimals)
	
	mint_button.token_gate_list[display_token_address] = price
	
	if price_label!=null:
		price_label.set_value(price)
	
	mint_button.try_unlock()
	
func initiate_mint() -> void:
	mint_manager.try_mint(group_name)
	
func handle_asset_mint(_asset_key:Pubkey) -> void:
	await refresh_balance_display()
	
func refresh_balance_display() -> void:
	if token!=null:
		await token.refresh_balance()
	else:
		var sol_balance:float = await SolanaService.get_balance(SolanaService.wallet.get_pubkey().to_string())
		await displayable_token.balance_label.set_value(sol_balance)
		
