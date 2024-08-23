extends Button

@export_file("*.tscn") var path_to_scene:String
@export var scene_loader:SceneLoader

func _on_pressed() -> void:
	scene_loader.load_scene(path_to_scene)
