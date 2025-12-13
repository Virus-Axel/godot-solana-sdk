extends Node
class_name AssetDisplaySystem

@export var display_entry_scn:PackedScene
@export var container:Container
@export var none_select_entry:DisplayableAsset
@export var entry_minimum_size:Vector2 = Vector2.ZERO

@export var load_on_start:bool=false
@export var das_assets_only:bool=false
@export var close_on_select:bool

@export var no_asset_overlay:Control
@export var search_bar:InputField

var entries:Array[DisplayableAsset]

var exception_address_list:Array[String]

var curr_filter:String

var curr_selection:DisplayableAsset = null

signal on_display_updated
signal on_asset_selected(asset:WalletAsset)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if none_select_entry!=null:
		none_select_entry.custom_minimum_size = entry_minimum_size
		none_select_entry.on_selected.connect(handle_entry_select)
		
	on_display_updated.connect(handle_display_update)
	on_asset_selected.connect(handle_asset_selection)
	
	if load_on_start:
		if SolanaService.wallet.is_logged_in():
			display_owned_assets()
		else:
			SolanaService.wallet.on_login_success.connect(display_owned_assets)
	
	if search_bar!=null:
		search_bar.on_field_updated.connect(filter_entries)
		
		
func display_owned_assets() -> void:
#	load all nfts that have already been loaded and then add new ones as they load
	clear_display()
	
	SolanaService.asset_manager.on_asset_loaded.connect(filter_loaded_asset)
	SolanaService.asset_manager.on_asset_load_finished.connect(asset_load_finished,CONNECT_ONE_SHOT)
	
	if SolanaService.asset_manager.is_loading:
		setup_owned_assets()
	else:
		SolanaService.asset_manager.load_assets(das_assets_only)
		
		
func filter_loaded_asset(asset:WalletAsset) -> void:
	add_to_list(asset)

func setup_owned_assets() -> void:
	var assets:Array[WalletAsset] = SolanaService.asset_manager.owned_assets
	for asset in assets:
		add_to_list(asset)
			
		
func add_to_list(asset:WalletAsset) -> void:
	var entry_instance:DisplayableAsset = display_entry_scn.instantiate() as DisplayableAsset
	entry_instance.custom_minimum_size = entry_minimum_size
	entry_instance.visible = matches_filter(asset)
	container.add_child(entry_instance)
	
	if exception_address_list.has(asset.mint.to_string()):
		entry_instance.set_interactive(false)
		
	await entry_instance.set_data(asset)
	entry_instance.on_selected.connect(handle_entry_select)
	entries.append(entry_instance)
	
	#print("ADDED TO LIST: ",asset.mint.to_string()
	on_display_updated.emit()
	
func asset_load_finished(owned_assets:Array[WalletAsset])->void:
	on_display_updated.emit()
	pass
		
func clear_display() -> void:
	for entry in entries:
		entry.queue_free()
	
	entries.clear()
	on_display_updated.emit()
	
func handle_display_update() -> void:
	if no_asset_overlay!=null:
		no_asset_overlay.visible = (entries.size()==0)
		
func filter_entries(substring:String) -> void:
	curr_filter = substring
	
	if substring.length() == 0:
		for entry in entries:
			entry.visible=true
		return
		
	for entry in entries:
		entry.visible = matches_filter(entry.asset)
		
func matches_filter(asset:WalletAsset) -> bool:
	if curr_filter.length() == 0:
		return true
	return asset.asset_name.to_lower().contains(curr_filter.to_lower())
	
func handle_entry_select(selected_entry:DisplayableAsset) -> void:
	if curr_selection!=null and curr_selection!= selected_entry:
		curr_selection.deselect()
		
	curr_selection = selected_entry
	
	if curr_selection == null:
		on_asset_selected.emit(null)
	else:
		on_asset_selected.emit(curr_selection.asset)
	
		
func handle_asset_selection(_selected_asset:WalletAsset) -> void:
	if close_on_select:
		queue_free()
