extends Node
class_name AssetDisplaySystem

@export var display_type:Array[AssetManager.AssetType]
@export var container:Container
@export var display_entry_scn:PackedScene

@export var load_on_ready:bool
@export var das_assets_only:bool=false
@export var close_on_select:bool
@export var destroy_on_close:bool

@export var no_asset_overlay:Control
@export var search_bar:InputField

@export_category("Token Display Settings")

@export_category("NFT Display Settings")
@export var collection_filter:Array[NFTCollection]

var entries:Array[DisplayableAsset]
var curr_filter:String

signal on_display_updated
signal on_asset_selected(asset:WalletAsset)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	on_display_updated.connect(handle_display_update)
	if load_on_ready:
		if SolanaService.wallet.is_logged_in():
			load_all_owned_assets()
		else:
			SolanaService.wallet.on_login_success.connect(load_all_owned_assets)
	
	if search_bar!=null:
		search_bar.on_field_updated.connect(filter_entries)
		
		
func load_all_owned_assets() -> void:
#	load all nfts that have already been loaded and then add new ones as they load
	clear_display()
	SolanaService.asset_manager.on_asset_loaded.connect(add_to_list)
	SolanaService.asset_manager.on_asset_load_finished.connect(asset_load_finished,CONNECT_ONE_SHOT)
	
	SolanaService.asset_manager.load_assets(das_assets_only)
	

func setup_owned_assets() -> void:
	match display_type:
		AssetManager.AssetType.NFT or AssetManager.AssetType.ASSET:
			if collection_filter.size()==0:
				setup(SolanaService.asset_manager.get_owned_assets())
			else:
				for collection in collection_filter:
					setup(SolanaService.asset_manager.get_owned_assets_from_collection(collection))
		AssetManager.AssetType.TOKEN:
			setup(SolanaService.asset_manager.get_owned_tokens())
			

func setup(assets:Array[WalletAsset],clear_previous:bool=true) -> void:
	if clear_previous && container.get_children().size() > 0:
		clear_display()
	
	if assets.size()==0:
		return
		
	for asset in assets:
		add_to_list(asset)
		
func add_to_list(asset:WalletAsset) -> void:
	if !display_type.has(asset.asset_type):
		return
		
	if (asset is Nft or asset is CoreAsset) and !belongs_to_collection_filter(asset):
		return
	
	var entry_instance:DisplayableAsset = display_entry_scn.instantiate() as DisplayableAsset
	entry_instance.visible = matches_filter(asset)
	container.add_child(entry_instance)
	
	await entry_instance.set_data(asset)
	entry_instance.on_selected.connect(handle_displayable_selection)
	
	entries.append(entry_instance)
	
	on_display_updated.emit()
	
func asset_load_finished(owned_assets:Array[WalletAsset])->void:
	SolanaService.asset_manager.on_asset_loaded.disconnect(add_to_list)
	on_display_updated.emit()
	pass
	
func handle_displayable_selection(selected_entry:DisplayableAsset) -> void:
	on_asset_selected.emit(selected_entry.asset)
	if close_on_select:
		close()
		
func select_none() -> void:
	on_asset_selected.emit(null)
	if close_on_select:
		close()
		
func clear_display() -> void:
	for entry in entries:
		entry.queue_free()
	
	entries.clear()
	on_display_updated.emit()
	
func handle_display_update() -> void:
	if no_asset_overlay!=null:
		no_asset_overlay.visible = (container.get_children().size()==0)
		container.visible = !no_asset_overlay.visible
		
func matches_filter(asset:WalletAsset) -> bool:
	if curr_filter.length() == 0:
		return true
	return asset.asset_name.to_lower().contains(curr_filter.to_lower())
		
func filter_entries(substring:String) -> void:
	curr_filter = substring
	
	if substring.length() == 0:
		for entry in entries:
			entry.visible=true
		return
		
	var displayables:Array = container.get_children()
	for entry in entries:
		entry.visible = matches_filter(entry.asset)
	
func close() -> void:
	if search_bar!=null:
		search_bar.text = ""
		filter_entries("")
		
	self.visible = false
	if destroy_on_close:
		queue_free()
		
func belongs_to_collection_filter(asset:WalletAsset) -> bool:
	if collection_filter.size() == 0:
		return true
		
	var pass_collection_filter= (collection_filter.size()==0)
	for collection in collection_filter:
		if collection.belongs_to_collection(asset):
			return true
			
	return false
