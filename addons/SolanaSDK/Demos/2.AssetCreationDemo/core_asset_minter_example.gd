extends Node

@export var mint_input_system:DataInputSystem
@export var mint_button:BaseButton
@export var attribute_pairs:Array[AttributePairExample]

signal on_asset_minted(key:Pubkey)

func _ready() -> void:
	mint_button.pressed.connect(mint_asset)

func mint_asset() -> void:
	if !mint_input_system.get_inputs_valid():
		print("Missing fields, skipping asset mint")
		return
		
	var asset_data:Dictionary = mint_input_system.get_input_data()
	var asset_name:String = asset_data["name"]
	
	var attributes_plugin:PluginAuthorityPair = PluginAuthorityPair.new()
	attributes_plugin.plugin = GenericAnchorResource.PluginAttributes
	var attributes:Attributes = Attributes.new()
	attributes.attributeList = []
	for pair in attribute_pairs:
		var attribute:Attribute = pair.get_attribute()
		if attribute != null:
			attributes.attributeList.append(attribute)
	attributes_plugin.Attributes_value_0 = attributes
	
	var asset_plugins:Array[PluginAuthorityPair] = []
	if attributes.attributeList.size()>0:
		asset_plugins.append(attributes_plugin)
	
	var tx_data:TransactionData = await SolanaService.core_manager.mint_asset(asset_data["name"],asset_data["uri"],null,null,asset_plugins)
	if tx_data.is_successful():
		on_asset_minted.emit(tx_data.custom_data["mint"])
		mint_input_system.reset_all_fields()
		for pair in attribute_pairs:
			pair.reset()
		print("Asset successfully minted!")
