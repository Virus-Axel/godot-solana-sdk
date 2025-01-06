extends Node
class_name DisplayableNFT

@export var visual:TextureRect
@export var image_size = 512

@export var name_label:Label
@export var button:BaseButton

var nft:Nft

signal on_selected

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if button!=null:
		button.pressed.connect(handle_select)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
	
func set_data(nft:Nft) -> void:
	self.nft = nft
	if name_label!=null:
		name_label.text = nft.metadata.get_token_name()
		if name_label.text=="":
			name_label.text="name missing"
	
	if nft.image!=null:
		visual.texture = nft.image
	else:
		await nft.try_load_image(image_size)
		if nft.image!=null:
			visual.texture = nft.image
		else:
			print("Couldn't load the image for mint: %s" % nft.mint.to_string())
		

func set_data_manual(texture:Texture2D, nft_name:String) -> void:
	if visual!=null:
		visual.texture = texture
	if name_label!=null:
		name_label.text = nft_name
	
func get_associated_nft() -> Nft:
	return nft
	
func handle_select() -> void:
	on_selected.emit()
