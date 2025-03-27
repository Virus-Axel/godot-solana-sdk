extends Node
class_name WalletAdapterUI

@export var provider_names:Array[String]
@export var provider_images:Array[Texture]

@export var selection_spawn:Container
@export var provider_button_scn:PackedScene
@export var cancel_button:TextureButton
@export var signing_overlay:Control

@export var option_select_sound:AudioStreamMP3
@export var cancel_sound:AudioStreamMP3

@onready var audio_player:AudioStreamPlayer = $AudioStreamPlayer

signal on_provider_selected(provider_id:int)
signal on_adapter_cancel

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if cancel_button!=null:
		cancel_button.pressed.connect(close_adapter)
		
	SolanaService.wallet.on_login_success.connect(handle_success)
	SolanaService.wallet.on_login_fail.connect(handle_fail)

		
func setup_adapter(available_wallets) -> void:
	for i in selection_spawn.get_children():
		i.queue_free()
	
#	20 is phantom, 25 solflare, 36 backpack
	var supported_wallets:Array = [20,25,36]
	for i in range(supported_wallets.size()):
		var wallet_array_index:int
		if supported_wallets[i] == 20:
			wallet_array_index = 0
		elif supported_wallets[i] == 25:
			wallet_array_index = 1
		elif supported_wallets[i] == 36:
			wallet_array_index = 2
			
		var button_instance:WalletAdapterButton = provider_button_scn.instantiate()
		button_instance.set_data(supported_wallets[i],provider_names[wallet_array_index],provider_images[wallet_array_index])
		selection_spawn.add_child(button_instance)
		
		button_instance.on_pressed.connect(on_button_pressed,CONNECT_ONE_SHOT)
		
func on_button_pressed(id_selected:int) -> void:
	if option_select_sound!=null:
		audio_player.stream = option_select_sound
		audio_player.play()
	on_provider_selected.emit(id_selected)
	
	if signing_overlay!=null:
		signing_overlay.visible=true
		
	
func handle_success() -> void:
	queue_free()
	
func handle_fail() -> void:
	if signing_overlay!=null:
		signing_overlay.visible=false
		

func close_adapter() -> void:
	on_adapter_cancel.emit()
	
	if cancel_sound!=null:
		audio_player.stream = cancel_sound
		audio_player.play()
		
	self.visible=false
	await get_tree().create_timer(0.3).timeout
	queue_free()
		
		
		
