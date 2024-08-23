extends Node
enum TokenType{SOL, SPL}

@export var input_field_system:InputFieldSystem
@export var token_type:TokenType

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	input_field_system.connect("on_input_submit", transfer_tokens)
	
func transfer_tokens(input_data:Array[String]) -> void:
	match token_type:
		TokenType.SOL:
			var receiver = input_data[0]
			var amount = float(input_data[1])
			var tx_data:TransactionData = await SolanaService.transaction_manager.transfer_sol(receiver,amount)
			if tx_data.is_successful():
				input_field_system.clear_fields()
		TokenType.SPL:
			var token_address = input_data[0]
			var receiver = input_data[1]
			var amount = float(input_data[2])
			var tx_data:TransactionData = await SolanaService.transaction_manager.transfer_token(token_address,receiver,amount)
			if tx_data.is_successful():
				input_field_system.clear_fields()
		
