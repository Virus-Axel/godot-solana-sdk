extends Node
class_name WalletAdapterButton

@export var provider_name:Label
@export var visual:TextureRect

var provider_id:int

signal on_pressed

func set_data(id:int,name:String,tex:Texture2D) -> void:
	provider_id=id
	provider_name.text = name
	visual.texture = tex
	
func _on_pressed() -> void:
	emit_signal("on_pressed",provider_id)
