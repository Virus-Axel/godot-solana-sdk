extends Node
class_name WalletService

@export var use_generated = false
#for testing in editor with a custom wallet, you may create a txt file with private key as text in it
#and enter the path to the file here
@export var custom_wallet_path:String
@export var autologin = false

@export var wallet_adapter_scn:PackedScene

@onready var wallet_adapter:WalletAdapter = $WalletAdapter

var keypair:Keypair

signal on_login_begin
signal on_login_finish(success:bool)

signal on_login_success
signal on_login_fail

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if !use_generated:
		wallet_adapter.connection_established.connect(log_in_success)
		wallet_adapter.connection_failed.connect(log_in_fail)
	
	if autologin:
		try_login()


func try_login() -> void:
	if use_generated:
		login_game_wallet()
	else:
		pop_adapter()
		on_login_begin.emit()
	
func login_game_wallet() -> void:
	if custom_wallet_path.length()==0:
		keypair = SolanaService.generate_keypair(true)
		#uncomment this to print your derived private key
		#print(keypair.get_private_bytes())
	else:
		keypair = Keypair.new_from_file(custom_wallet_path)
		if keypair==null:
			print("Failed to fetch keypair from a local file")
			return
	log_in_success()
	
	
func pop_adapter() -> void:
	var wallet_adapter_ui:WalletAdapterUI = wallet_adapter_scn.instantiate()
	get_tree().root.add_child(wallet_adapter_ui)
	wallet_adapter_ui.setup_adapter(SolanaService.wallet.get_available_wallet_extensions())
	
	wallet_adapter_ui.on_provider_selected.connect(login_adapter)

func login_adapter(provider_id:int) -> void:
	print("PROVIDER: ",provider_id)
	wallet_adapter.wallet_type = provider_id	
	wallet_adapter.connect_wallet()

func log_in_success() -> void:
	on_login_success.emit()
	print("Logged in as: ",get_pubkey().to_string())
	
func log_in_fail() -> void:
	print("Signing Cancelled")
	on_login_fail.emit()
	
func get_wallet_provider_id() -> int:
	if use_generated:
		return -1
	return wallet_adapter.wallet_type

func get_pubkey() -> Pubkey:
	if use_generated:
		if keypair == null:
			return null
		return Pubkey.new_from_string(keypair.get_public_string())
	else:
		return wallet_adapter.get_connected_key()
	
func get_kp():
	if use_generated:
		return keypair
	else:
		return wallet_adapter
		
func get_shorthand_address(key:Pubkey=null):
	var address:String 
	if key == null:
		address = get_pubkey().to_string()
	else:
		address = key.to_string()
		
	var part1:String = address.substr(0,4)
	var part2:String = address.substr(address.length()-5,4)
	return "%s...%s" % [part1,part2]
	
func get_available_wallet_extensions() -> Array:
	return wallet_adapter.get_available_wallets()
		
func is_logged_in() -> bool:
	if use_generated:
		return keypair!=null
		
	return wallet_adapter.get_connected_key()!=null and wallet_adapter.get_connected_key().to_string()!=""
