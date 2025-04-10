extends Node
class_name DataInputSystem

#@export var input_fields: Array[InputField]
@export var input_fields: Dictionary
@export var asset_selectors:Dictionary
@export var date_selectors:Dictionary
@export var option_selectors:Dictionary

signal on_input_submit
signal on_fields_updated
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	for key in input_fields.keys():
		var field = get_node(input_fields[key]) as InputField
		field.on_field_updated.connect(notify_change)
		
	for key in asset_selectors.keys():
		var selector = get_node(asset_selectors[key]) as AssetSelector
		selector.on_selected.connect(handle_asset_select)
		
	for key in date_selectors.keys():
		var selector = get_node(date_selectors[key]) as DateTimeSelector
		selector.on_updated.connect(handle_date_select)
		
	for key in option_selectors.keys():
		var selector = get_node(option_selectors[key]) as OptionButton
		selector.item_selected.connect(handle_option_select)
	
func force_input_update() -> void: notify_change()
func handle_asset_select(selected_asset:WalletAsset) -> void: notify_change()
func handle_date_select(selected_timestamp:int) -> void: notify_change()
func handle_option_select(selected_option_index:int) -> void: notify_change()

func notify_change(_new_field_value:String="") -> void:
	on_fields_updated.emit()

func handle_fields_updated() -> void:
	pass
	
func confirm_input() -> void:
	on_input_submit.emit(get_input_data())
	
func get_inputs_valid() -> bool:
	var all_valid=true
	for key in input_fields.keys():
		var field = get_node(input_fields[key]) as InputField
		if !field.is_valid():
			all_valid=false
			
	for key in asset_selectors.keys():
		var selector = get_node(asset_selectors[key]) as AssetSelector
		if !selector.is_valid():
			all_valid=false
			
	for key in date_selectors.keys():
		var selector = get_node(date_selectors[key]) as DateTimeSelector
		if !selector.is_valid():
			all_valid=false
			
	for key in option_selectors.keys():
		var selector = get_node(option_selectors[key]) as OptionButton
		if selector.get_selected_id() == -1:
			all_valid=false
	
	return all_valid
	
func get_input_data()-> Dictionary:
	var fields_data:Dictionary
	for key in input_fields.keys():
		var field = get_node(input_fields[key]) as InputField
		fields_data[key] = field.get_field_value()
		
	for key in asset_selectors.keys():
		var selector = get_node(asset_selectors[key]) as AssetSelector
		fields_data[key] = selector.selected_asset.mint
		
	for key in date_selectors.keys():
		var selector = get_node(date_selectors[key]) as DateTimeSelector
		fields_data[key] = selector.get_selected_timestamp()
		
	for key in option_selectors.keys():
		var selector = get_node(option_selectors[key]) as OptionButton
		fields_data[key] = {"id":selector.get_selected_id(),"value":selector.get_item_text(selector.get_selected_id())}
		
	return fields_data
	
func reset_all_fields() -> void:
	for key in input_fields.keys():
		var field = get_node(input_fields[key]) as InputField
		field.clear()
		
	for key in asset_selectors.keys():
		var selector = get_node(asset_selectors[key]) as AssetSelector
		selector.clear_selection()
	
	
#Input Fields specific functions:
func set_input_fields_data(premade_data:Dictionary) -> void:
	for key in premade_data.keys():
		if input_fields.has(key):
			var field:InputField = get_input_field(key)
			field.text = premade_data[key]
		
func get_input_field(name:String) -> InputField:
	return get_node(input_fields[name]) as InputField
	
	
