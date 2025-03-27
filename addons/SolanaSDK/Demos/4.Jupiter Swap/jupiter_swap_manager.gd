extends Node

@export var jupiter_api:JupiterAPI
@export var token_selector:AssetSelector
@export var data_input_system:DataInputSystem
@export var sol_swap_value_label:Label
@export var swap_button:Button
@export var swap_slippage_percentage:float = 0.01

@export var refresh_quote:bool = true
@export var quote_refresh_seconds:float = 15

@export var verification_icon:TextureRect
@export var verified_icon:Texture2D
@export var unknown_icon:Texture2D

var curr_token_to_swap:Token
var swap_quote

var time_to_refresh_quote:float = 0

func _ready() -> void:
	data_input_system.on_fields_updated.connect(handle_fields_update)
	
	swap_button.disabled=true
	swap_button.pressed.connect(swap_tokens)
	sol_swap_value_label.text = "..."
	
func _process(delta: float) -> void:
	if !refresh_quote:
		return
	
	if !swap_button.disabled:
		time_to_refresh_quote -= delta
		swap_button.text = "Swap (%s)" % str(ceil(time_to_refresh_quote))
		if time_to_refresh_quote <= 0:
			await refresh_swap_quote()
			return

func handle_token_select(selected_asset:Token) -> void:
	verification_icon.visible=false
	if curr_token_to_swap!=null and selected_asset.mint.to_string() == curr_token_to_swap.mint.to_string():
		return
		
	curr_token_to_swap = selected_asset
	data_input_system.get_input_field("swapAmount").clear()
	data_input_system.get_input_field("swapAmount").max_value = await curr_token_to_swap.get_balance()
	data_input_system.force_input_update()
	
	var token_data = await jupiter_api.get_token_data(curr_token_to_swap.mint)
	data_input_system.get_input_field("swapAmount").editable=token_data.size()>0
	
	verification_icon.texture = await set_token_verification_mark(selected_asset.mint)
	verification_icon.visible=true
	
func set_token_verification_mark(token_mint:Pubkey) -> Texture2D:
	var token_status:JupiterAPI.TokenStatus = await jupiter_api.get_token_status(token_mint)
	match token_status:
		JupiterAPI.TokenStatus.VERIFIED:
			return verified_icon
		JupiterAPI.TokenStatus.UNKNOWN:
			return unknown_icon
	
	return null
	
func handle_fields_update() -> void:
	await handle_token_select(token_selector.selected_asset)
	
	if !data_input_system.get_inputs_valid():
		sol_swap_value_label.text = "..."
		return
		
	await refresh_swap_quote()
	
func refresh_swap_quote() -> void:
	freeze_selection(true)
	swap_button.text = "Loading..."
	
	var token_to_send:Pubkey = curr_token_to_swap.mint
	var token_to_receive:Pubkey = Pubkey.new_from_string(SolanaService.WRAPPED_SOL_CA)
	var amount_to_swap:float = data_input_system.get_input_data()["swapAmount"]
	
	swap_quote = await jupiter_api.get_swap_quote(token_to_send,token_to_receive,amount_to_swap,swap_slippage_percentage)
		
	sol_swap_value_label.text = str(float(swap_quote["outAmount"]) / pow(10,9))
	time_to_refresh_quote = quote_refresh_seconds
	freeze_selection(false)
	
	
func freeze_selection(freeze:bool) -> void:
	swap_button.disabled=freeze
	data_input_system.get_input_field("swapAmount").editable=!freeze
	token_selector.displayable_asset.select_button.disabled=freeze
		
func swap_tokens() -> void:
	if swap_quote == null:
		return
		
	var tx_data:TransactionData = await jupiter_api.swap_token(SolanaService.wallet.get_pubkey(),swap_quote)
	var selected_token:Token = token_selector.selected_asset as Token
	await selected_token.refresh_balance()
