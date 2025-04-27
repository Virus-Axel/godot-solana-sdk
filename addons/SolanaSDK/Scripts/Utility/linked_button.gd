extends Node
class_name LinkedButton

@export var button:BaseButton
@export var link:String
@export var open_as_new_tab:bool=true
@export var audio_player:AudioStreamPlayer
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	button.pressed.connect(open_link)
	pass # Replace with function body.

func open_link() -> void:
	if audio_player!=null and audio_player.stream!=null:
		audio_player.play()
		
	if OS.has_feature("editor"):
		OS.shell_open(link)
		
	elif OS.has_feature("web") and JavaScriptBridge:
		if open_as_new_tab:
			OS.shell_open(link)
		else:
			JavaScriptBridge.eval("window.location.replace('%s');" % link)
