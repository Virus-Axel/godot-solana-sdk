extends Node
class_name SceneLoader

@onready var loading_canvas:Control = $LoadingCanvas

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	loading_canvas.visible=false


func load_scene(scene_path:String) -> void:
	loading_canvas.visible=true
	await get_tree().create_timer(0.01).timeout
	
	get_tree().change_scene_to_file(scene_path)
	
