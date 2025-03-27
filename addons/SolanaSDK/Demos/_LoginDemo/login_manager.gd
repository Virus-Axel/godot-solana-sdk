extends Node

@export_file("*.tscn") var path_to_scene:String

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	SolanaService.wallet.on_login_success.connect(load_menu)


func _on_login_button_pressed() -> void:
	#if generated wallet option, it will just login, if not, it will trigger on_login_begin
	SolanaService.wallet.try_login()
		
func load_menu() -> void:
	$SceneLoader.load_scene(path_to_scene)
