extends Node

@export var asset_key_field:InputField
@export var asset_input_system:DataInputSystem
@export var edit_asset_button:BaseButton
@export var burn_asset_button:BaseButton
@export var edit_plugin_button:BaseButton
@export var attribute_pairs:Array[AttributePairExample]

@onready var asset_mint_manager = $"../AssetMintPanel"

var current_asset:Pubkey = null
var current_asset_data:Dictionary = {}

func _ready() -> void:
	edit_asset_button.pressed.connect(edit_asset)
	burn_asset_button.pressed.connect(burn_asset)
	edit_plugin_button.pressed.connect(edit_asset_plugin)
	edit_asset_button.disabled=true
	burn_asset_button.disabled=true
	edit_plugin_button.disabled=true
	
	asset_key_field.on_field_updated.connect(set_asset_data)
	asset_mint_manager.on_asset_minted.connect(set_asset)
	
func set_asset(key:Pubkey) -> void:
	asset_key_field.set_value(key.to_string())
	set_asset_data(key.to_string())
	
func set_asset_data(value:String) -> void:
	edit_asset_button.disabled=(value=="")
	burn_asset_button.disabled=(value=="")
	edit_plugin_button.disabled=(value=="")
	
	if value == "":
		asset_input_system.reset_all_fields()
		for pair in attribute_pairs:
			pair.reset()
	else:
		current_asset = asset_key_field.get_field_value()
		current_asset_data = await SolanaService.core_manager.get_asset_core_data(current_asset)
		asset_input_system.get_input_field("name").set_value(current_asset_data["name"])
		asset_input_system.get_input_field("uri").set_value(current_asset_data["uri"])
		if current_asset_data["plugins"].has("attributes"):
			var attribute_list:Array = current_asset_data["plugins"]["attributes"]["data"]["attribute_list"]
			for i in range(attribute_list.size()):
				attribute_pairs[i].set_attribute(attribute_list[i]["key"],attribute_list[i]["value"])

func edit_asset() -> void:
	if !asset_input_system.get_inputs_valid():
		print("Missing fields, skipping asset mint")
		return
		
	var asset_data:Dictionary = asset_input_system.get_input_data()
	var tx_data:TransactionData = await SolanaService.core_manager.edit_asset(current_asset,asset_data["name"],asset_data["uri"])
	print(tx_data)
	pass
	
func burn_asset() -> void:
	var tx_data:TransactionData = await SolanaService.core_manager.burn_asset(current_asset)
	if tx_data.is_successful():
		set_asset_data("")
		print("Asset burned successfully")
	
func edit_asset_plugin() -> void:
	var attributes_plugin:PluginAuthorityPair = PluginAuthorityPair.new()
	attributes_plugin.plugin = 6 
	var attributes:Attributes = Attributes.new()
	attributes.attributeList = []
	for pair in attribute_pairs:
		var attribute:Attribute = pair.get_attribute()
		if attribute != null:
			attributes.attributeList.append(attribute)
	attributes_plugin.Attributes_value_0 = attributes
	
	var plugin_manage_state:CoreManager.CoreManageState
	if current_asset_data["plugins"].has("attributes"):
		plugin_manage_state = CoreManager.CoreManageState.Edit
	else:
		plugin_manage_state = CoreManager.CoreManageState.Add
	
	var tx_data:TransactionData = await SolanaService.core_manager.manage_asset_plugin(current_asset,null,attributes_plugin,plugin_manage_state)
	if tx_data.is_successful():
		print("Asset successfully minted!")
