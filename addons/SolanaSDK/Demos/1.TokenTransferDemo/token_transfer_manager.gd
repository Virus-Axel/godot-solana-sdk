extends Node
enum TokenType{SOL, SPL}

@export var data_input_system:DataInputSystem
@export var token_type:TokenType
@export var send_button:Button

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	send_button.pressed.connect(transfer_tokens)
	
func transfer_tokens() -> void:
	if !data_input_system.get_inputs_valid():
		return
		
	var input_data:Dictionary= data_input_system.get_input_data()
	
	match token_type:
		TokenType.SOL:
			var receiver = input_data["receiverAddress"]
			var amount = input_data["transferAmount"]
			var tx_data:TransactionData = await SolanaService.transaction_manager.transfer_sol(receiver,amount)
			if tx_data.is_successful():
				data_input_system.reset_all_fields()
		TokenType.SPL:
			var token_address = input_data["tokenAddress"]
			var receiver = input_data["receiverAddress"]
			var amount = input_data["transferAmount"]
			var tx_data:TransactionData = await SolanaService.transaction_manager.transfer_token(token_address,receiver,amount)
			if tx_data.is_successful():
				data_input_system.reset_all_fields()
		
