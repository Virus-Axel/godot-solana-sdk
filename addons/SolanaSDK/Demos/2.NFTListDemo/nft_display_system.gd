extends Node
class_name NftDisplaySystem

@export var displayable_nft_scn:PackedScene
@export var collection_filter:Array[NFTCollection]
@export var container:Container

@export var no_nft_overlay:Control

var displayables:Array[DisplayableNFT]

signal on_display_updated

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if !SolanaService.asset_manager.assets_loaded:
		SolanaService.asset_manager.on_asset_loaded.connect(add_to_list)
		SolanaService.asset_manager.on_asset_load_finished.connect(asset_load_finished)
		SolanaService.asset_manager.load_assets()
	else:
		if collection_filter.size()==0:
			setup(SolanaService.asset_manager.get_owned_nfts())
		else:
			for collection in collection_filter:
				setup(SolanaService.asset_manager.get_nfts_from_collection(collection))
	pass # Replace with function body.


func setup(nfts:Array[Nft],clear_previous:bool=false) -> void:
	if clear_previous && displayables.size()!=null:
		clear_display()
	
	if no_nft_overlay!=null:
		no_nft_overlay.visible = (nfts.size()==0)
	if nfts.size()==0:
		return
		
	for nft in nfts:
		add_to_list(nft)
		
func add_to_list(asset:WalletAsset) -> void:
	if !asset is Nft:
		return
		
	var pass_collection_filter= (collection_filter.size()==0)
	for collection in collection_filter:
		if collection.belongs_to_collection(asset):
			pass_collection_filter=true
			break
	if !pass_collection_filter:
		return
		
	var displayable_nft:DisplayableNFT = displayable_nft_scn.instantiate()
	container.add_child(displayable_nft)
		
	displayable_nft.set_data(asset)
	displayables.append(displayable_nft)
	
	on_display_updated.emit()
	
func asset_load_finished(owned_assets:Array[WalletAsset])->void:
	pass
	
		
func clear_display() -> void:
	for nft in displayables:
		nft.queue_free()
	
	displayables.clear()
	on_display_updated.emit()
