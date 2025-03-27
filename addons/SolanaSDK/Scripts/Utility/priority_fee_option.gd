extends Node
class_name PriorityFeeOption

@export var priority_fee_option:OptionButton
@export var default_fee_id:int = 3

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if priority_fee_option!=null:
		set_priority_fee_options()


func set_priority_fee_options() -> void:
	var options:Array = HeliusAPI.PriorityFeeLevel.keys()
	for i in range(options.size()):
		priority_fee_option.add_item(options[i],i)
	
	priority_fee_option.select(default_fee_id)
	change_fee_level(default_fee_id)
	priority_fee_option.item_selected.connect(change_fee_level)
	

func change_fee_level(index_selected:int) -> void:
	var selected_level:HeliusAPI.PriorityFeeLevel = HeliusAPI.PriorityFeeLevel.values()[index_selected]
	SolanaService.transaction_manager.helius_api.priority_fee_level = selected_level
