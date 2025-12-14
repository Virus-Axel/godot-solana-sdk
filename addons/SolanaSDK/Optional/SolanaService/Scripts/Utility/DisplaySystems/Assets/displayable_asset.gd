extends Node
class_name DisplayableAsset

@export var visual:TextureRect
@export var image_size = 512
#@export_file("*.png","*.jpg") var default_image_path:String

@export var name_label:Label
@export var max_name_length:int=-1
@export var symbol_label:Label

@export var select_button:BaseButton
@export var unavailable_overlay:Control

@export var inspect_button:LinkedButton

@export_category("Displayable Token Settings")
@export var balance_label:NumberLabel
@export var auto_load_balance:bool

var asset:WalletAsset
var default_tex:Texture2D
var default_name:String

signal on_data_loaded()
signal on_selected(displayable:DisplayableAsset)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if visual!=null:
		default_tex = visual.texture
	if name_label!=null:
		default_name = name_label.text
		
	if select_button!=null:
		select_button.pressed.connect(handle_select)
	set_interactive(true)

	
func set_data(asset:WalletAsset) -> void:	
	self.asset = asset
	if name_label!=null:
		name_label.text = truncate_name(asset.asset_name)
	if symbol_label!=null:
		symbol_label.text = asset.symbol
	
	if visual!=null:
		if asset.image!=null and image_size <= asset.image.get_size().x and image_size <= asset.image.get_size().y:
			visual.texture = asset.image
		else:
			asset.on_image_loaded.connect(handle_image_load_complete,CONNECT_ONE_SHOT)
			asset.try_load_image(image_size)
			
	if asset is Token:
		var token = asset as Token
		if balance_label!=null:
			balance_label.set_value(await token.get_balance())
		if auto_load_balance:
			SolanaService.transaction_manager.on_tx_finish.connect(update_balance)
		
	if inspect_button!=null:
		if asset is Token:
			inspect_button.link = SolanaService.account_inspector.get_inspect_link(asset.mint,AccountInspector.InspectSite.DEXSCREENER)
		elif asset is Nft or asset is CoreAsset:
			inspect_button.link = SolanaService.account_inspector.get_inspect_link(asset.mint,AccountInspector.InspectSite.TENSOR)
		
	on_data_loaded.emit()
		

func set_data_manual(texture:Texture2D, nft_name:String, balance:float=0.0) -> void:
	if visual!=null:
		visual.texture = texture
	if name_label!=null:
		name_label.text = nft_name
		
	if balance_label!=null and balance != 0.0:
		balance_label.set_value(balance)
	
func get_associated_asset() -> WalletAsset:
	return asset
	
func handle_select() -> void:
	if select_button.toggle_mode:
		if select_button.button_pressed:
			on_selected.emit(self)
		else:
			on_selected.emit(null)
	else:
		on_selected.emit(self)
		
func deselect() -> void:
	if select_button.toggle_mode and select_button.button_pressed:
		select_button.button_pressed = false
	
func handle_image_load_complete() -> void:
	if asset.image!=null:
		visual.texture = asset.image
	else:
		reset_to_default()
		push_warning("Couldn't load the image for mint: %s" % asset.mint.to_string())
	
func truncate_name(full_name:String,include_id:bool=true) -> String:
	if max_name_length < 0:
		return full_name
	if len(full_name) <= max_name_length:
		return full_name
		
	var truncated_name:String
	var parts = full_name.split("#")
#	if there's no # identifier, return regular truncated
	if parts.size() == 1:
		return "%s.." % full_name.substr(0,max_name_length)
		
#	adding + 2 because of space and hashtag
	var id_length:int = len(parts[1]) + 2
	var name_part_length = max_name_length-id_length
	return "%s... #%s" % [full_name.substr(0,name_part_length).strip_edges(),parts[1]]
	
func update_balance(tx_data:TransactionData=null) -> void:
	if tx_data!=null and !tx_data.is_successful():
		return
	
	if balance_label == null:
		return
		
	var token:Token = asset as Token
	balance_label.set_value(await token.get_balance(true))
	
func reset_to_default():
	if visual != null:
		visual.texture = default_tex
	if name_label!=null:
		name_label.text = default_name
	
func set_interactive(interactive:bool) -> void:
	if select_button!=null:
		select_button.disabled = !interactive
	if unavailable_overlay!=null:
		unavailable_overlay.visible = !interactive
