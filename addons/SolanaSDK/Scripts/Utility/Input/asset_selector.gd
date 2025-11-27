extends Node
class_name AssetSelector

@export var displayable_asset:DisplayableAsset
@export var display_system_scn:PackedScene
@export var custom_entry_scn:PackedScene
@export var automatic_asset_load:bool=false
#set if you want for asset to be invisible before something is clicked
@export var asset_content:Control
@export var select_label:Label
@export var is_optional:bool=false

var selected_asset:WalletAsset
signal on_display_opened(display:AssetDisplaySystem)
signal on_selected(selected_asset:WalletAsset)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	displayable_asset.on_selected.connect(show_display_system)
	
	if asset_content!=null:
		asset_content.visible=false
	if select_label != null:
		select_label.visible=true
	pass # Replace with function body.

func show_display_system(_selected_asset:DisplayableAsset) -> void:
	var display_system:AssetDisplaySystem = display_system_scn.instantiate()
	get_tree().root.add_child(display_system)
		
	display_system.on_asset_selected.connect(select_asset)
	
	if custom_entry_scn!=null:
		display_system.display_entry_scn = custom_entry_scn
		
	on_display_opened.emit(display_system)
	
	display_system.display_owned_assets()

func select_asset(display_selection:WalletAsset) -> void:
	if select_label != null:
		select_label.visible= (display_selection==null)
	if asset_content != null:
		asset_content.visible= (display_selection!=null)
	
	selected_asset = display_selection
	
	if selected_asset!=null:
		await displayable_asset.set_data(selected_asset)
	else:
		displayable_asset.reset_to_default()
		
	on_selected.emit(selected_asset)
	
func is_valid() -> bool:
	if selected_asset == null:
		return is_optional
	
	return selected_asset != null
	
func clear_selection() -> void:
	if selected_asset == null:
		return
	select_asset(null)
		
	
		
