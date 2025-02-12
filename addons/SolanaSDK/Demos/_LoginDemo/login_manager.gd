extends Node

@export_file("*.tscn") var path_to_scene:String
@export var wallet_adapter_ui:WalletAdapterUI
@onready var login_overlay = $LoginOverlay

var adapter_instance:WalletAdapterUI

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	login_overlay.visible=false
	SolanaService.wallet.on_login_begin.connect(pop_adapter)	
	SolanaService.wallet.on_login_finish.connect(confirm_login)	
	
	wallet_adapter_ui.on_provider_selected.connect(process_adapter_result)
	wallet_adapter_ui.on_adapter_cancel.connect(cancel_login)


func _on_login_button_pressed() -> void:
	#if generated wallet option, it will just login, if not, it will trigger on_login_begin
	SolanaService.wallet.try_login()
	
func pop_adapter()-> void:
	login_overlay.visible=true
	wallet_adapter_ui.setup(SolanaService.wallet.wallet_adapter.get_available_wallets())
	
func process_adapter_result(provider_id:int) -> void:
	SolanaService.wallet.login_adapter(provider_id)

func cancel_login()-> void:
	login_overlay.visible=false
	
func confirm_login(login_success:bool) -> void:
	login_overlay.visible=false
	if login_success:
		$SceneLoader.load_scene(path_to_scene)
