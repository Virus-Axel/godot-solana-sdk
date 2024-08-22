extends Node
class_name WalletService

@export var use_generated = false
#for testing in editor with a custom wallet, you may create a txt file with private key as text in it
#and enter the path to the file here
@export var custom_wallet_path:String
@export var autologin = false

@onready var wallet_adapter:WalletAdapter = $WalletAdapter

var keypair:Keypair

signal on_login_begin
signal on_login_finish(success:bool)

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

func login_adapter(provider_id:int) -> void:
	print("LOGIN ADAPTEr")
	wallet_adapter.wallet_type = provider_id	
	wallet_adapter.connect_wallet()

func log_in_success() -> void:
	on_login_finish.emit(true)
	print(get_pubkey().to_string())
	
func log_in_fail() -> void:
	on_login_finish.emit(false)

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
		
func is_logged_in() -> bool:
	if use_generated:
		return keypair!=null
	return wallet_adapter.get_connected_key().to_string()!=""
