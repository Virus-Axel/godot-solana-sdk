extends Node

@export var balance_token_address:String
@export var balance_visualizer:TokenVisualizer

@onready var account_address:Label = $AccountAddress
@onready var balance_label:Label = $Balance

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var wallet_address = SolanaService.wallet.get_pubkey().to_string()
	account_address.text = wallet_address
	
	if balance_visualizer!=null:
		#SolanaClient.account_subscribe(SolanaService.wallet.get_pubkey(),Callable(self,"update_balance"))
		var balance:float
		if balance_token_address.length()==0:
			balance = await SolanaService.get_balance(wallet_address)
			balance_visualizer.set_token_data(balance)
		else:
			balance = await SolanaService.get_balance(wallet_address,balance_token_address)
			balance_visualizer.set_token_data(balance,Pubkey.new_from_string(balance_token_address))

#func update_balance(account_data:Dictionary) -> void:
	#print(account_data)
