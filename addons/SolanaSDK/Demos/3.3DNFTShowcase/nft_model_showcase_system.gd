extends Node

@export var model_nft_scn:PackedScene
@export var model_spawn:Node3D

@export var prev_arrow:TextureButton
@export var next_arrow:TextureButton

@export var active_nft_name:Label

var models:Array[ModelNFT]

var curr_model:ModelNFT

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	prev_arrow.disabled=true
	next_arrow.disabled=true
	prev_arrow.pressed.connect(show_previous)
	next_arrow.pressed.connect(show_next)
	
	if !SolanaService.asset_manager.assets_loaded:
		SolanaService.asset_manager.on_asset_loaded.connect(add_to_list)
		SolanaService.asset_manager.on_asset_load_finished.connect(asset_load_finished)
		SolanaService.asset_manager.load_assets()
	else:
		setup(SolanaService.asset_manager.get_owned_nfts())
	pass # Replace with function body.


func setup(nfts:Array[Nft]) -> void:
	if models.size()!=null:
		clear_models()
	
	if nfts.size()==0:
		return
	
	for nft in nfts:
		add_to_list(nft)
	show_at_index(0)
		
func add_to_list(asset:WalletAsset) -> void:
	if !asset is Nft:
		return
		
	if !asset.is_3D_model():
		return
	var model_nft:ModelNFT = model_nft_scn.instantiate()
	model_spawn.add_child(model_nft)
	model_nft.visible=false
		
	model_nft.setup(asset)
	models.append(model_nft)
	if models.size()==1:
		show_at_index(0)
	if models.size()>=2:
		prev_arrow.disabled=false
		next_arrow.disabled=false
	
func show_at_index(index:int) -> void:
	if curr_model!=null:
		curr_model.visible=false
	
	curr_model = models[index]
	curr_model.visible=true
	if curr_model.model==null:
		curr_model.try_load_model()
	active_nft_name.text = curr_model.nft.nft_name
	
func show_next() -> void:
	var curr_index = models.find(curr_model)
	if curr_index+1 < models.size():
		curr_index += 1
	else:
		curr_index = 0
		
	show_at_index(curr_index)

func show_previous() -> void:
	var curr_index = models.find(curr_model)
	if curr_index-1 >= 0:
		curr_index -= 1
	else:
		curr_index = models.size()-1
	
	show_at_index(curr_index)

	
func clear_models() -> void:
	for model in models:
		model.queue_free()
	
	models.clear()
	
func asset_load_finished(owned_assets:Array[WalletAsset]) -> void:
	if models.size()==0:
		active_nft_name.text = "no 3D NFTs found..."
