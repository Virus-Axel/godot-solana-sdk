extends Node

@export var account_address:Label
@export var sol_balance:Label

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var wallet_address = SolanaService.wallet.get_pubkey().to_string()
	account_address.text = wallet_address
	
	sol_balance.text = str(await SolanaService.get_balance(wallet_address))
