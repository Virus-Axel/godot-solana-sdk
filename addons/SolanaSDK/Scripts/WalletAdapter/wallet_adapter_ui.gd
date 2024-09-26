extends Node
class_name WalletAdapterUI

@export var provider_button_scn:PackedScene
@export var provider_names:Array[String]
@export var provider_images:Array[Texture]
@export var cancel_button:TextureButton

@export var selection_spawn:Container

@export var option_select_sound:AudioStreamMP3
@export var cancel_sound:AudioStreamMP3

@onready var audio_player:AudioStreamPlayer = $AudioStreamPlayer

signal on_provider_selected(provider_id:int)
signal on_adapter_cancel

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if cancel_button!=null:
		cancel_button.pressed.connect(cancel_login)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
	
func setup(available_wallets) -> void:
	for i in selection_spawn.get_children():
		i.queue_free()
	for i in range(available_wallets.size()):
		var provider_id = available_wallets[i]
		var button_instance:WalletAdapterButton = provider_button_scn.instantiate()
		button_instance.set_data(provider_id,provider_names[provider_id],provider_images[provider_id])
		selection_spawn.add_child(button_instance)
		
		button_instance.on_pressed.connect(on_button_pressed,CONNECT_ONE_SHOT)
		
func on_button_pressed(id_selected:int) -> void:
	if option_select_sound!=null:
		audio_player.stream = option_select_sound
		audio_player.play()
	on_provider_selected.emit(id_selected)

func cancel_login() -> void:
	if cancel_sound!=null:
		audio_player.stream = cancel_sound
		audio_player.play()
	on_adapter_cancel.emit()
		
		
		
