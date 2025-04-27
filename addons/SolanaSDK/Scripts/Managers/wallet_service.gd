extends Node
class_name WalletService

@export var use_generated = false
#for testing in editor with a custom wallet, you may create a txt file with private key as text in it
#and enter the path to the file here
@export var custom_wallet_path:String
@export var autologin = false

@export var wallet_adapter_scn:PackedScene

@onready var wallet_adapter:WalletAdapter = $WalletAdapter
@onready var session_manager:SessionKeyManager = $SessionKeyManager

var keypair:Keypair
var session_keypair:Keypair
var active_session_keys:Dictionary

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
	wallet_adapter_ui.setup_adapter(wallet_adapter.get_available_wallets())
	
	wallet_adapter_ui.on_provider_selected.connect(login_adapter)
	
func get_wallet_provider_id() -> int:
	if use_generated:
		return -1
	return wallet_adapter.wallet_type

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
	
func is_logged_in() -> bool:
	if use_generated:
		return keypair!=null
		
	return wallet_adapter.get_connected_key()!=null and wallet_adapter.get_connected_key().to_string()!=""

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
	
#	SESSION KEYS

##if null, set it as a random keypair that will live for the duration of current game instance
func init_session_keypair(keypair:Keypair=null, override_existing:bool=false) -> void:
	if session_keypair!=null && !override_existing:
		push_error("Session Keypair already initialized, skipping...")
		return
		
	if keypair == null:
		keypair = Keypair.new_random()
		
	session_keypair = keypair
	
func get_session_kp() -> Keypair:
	return session_keypair
	
func start_session(target_program:Pubkey, initial_top_up_amount:float=0.0,duration_in_minutes:int=120) -> SessionKey:
	if session_keypair == null:
		init_session_keypair()
	var session_token:Pubkey = SessionKeyPDA.get_session_token_pda(target_program,session_keypair.to_pubkey(),get_pubkey())
	
	var do_top_up = null
	var lamport_top_up_value = null
	if initial_top_up_amount > 0:
		do_top_up = true
		lamport_top_up_value = int(initial_top_up_amount*pow(10,9))
	
	var session_params:Dictionary = {
		"top_up": do_top_up,
		"valid_until":get_end_timestamp(duration_in_minutes),
		"lamports": lamport_top_up_value
	}
	var tx_data:TransactionData = await session_manager.create_session(target_program,session_keypair,session_token,session_params)
	if tx_data.is_successful():
		var session_key:SessionKey = SessionKey.new(session_keypair,session_token,get_pubkey(),session_params["valid_until"])
		active_session_keys[target_program.to_string()] = session_key
		return session_key
	else:
		return null
		
func end_session(session_key:SessionKey) -> void:
	var tx_data:TransactionData = await session_manager.revoke_session(session_key.get_token())
	if tx_data.is_successful():
		for key in active_session_keys.keys():
			var session:SessionKey = active_session_keys[key]
			if session.get_token().to_string() == session_key.get_token().to_string():
				active_session_keys.erase(key)
		
func get_session_key(program:Pubkey) -> SessionKey:
	if active_session_keys.has(program.to_string()):
		return active_session_keys[program.to_string()]
	else:
		return null
		
func fund_session_wallet(amount_to_fund:float,fund_token_mint:Pubkey=null) -> TransactionData:
	if session_keypair == null:
		push_error("Need to initialize a session keypair before you can fund it")
		return
	var tx_data:TransactionData = null
	
	if fund_token_mint == null:
		tx_data = await SolanaService.transaction_manager.transfer_sol(session_keypair.to_pubkey(),amount_to_fund)
	else:
		tx_data = await SolanaService.transaction_manager.transfer_token(fund_token_mint,session_keypair.to_pubkey(),amount_to_fund)
	
	return tx_data
	
func withdraw_session_balance(token_mint:Pubkey=null) -> TransactionData:
	var tx_data:TransactionData = null
	if token_mint == null:
		var amount_to_withdraw:float = await SolanaService.get_balance(session_keypair.to_pubkey().to_string())
#		transaction price is 0.000005 SOL
#		minimum balance is 0.00089088 SOL
#		may only withdraw all sol that is left after taking off this amount
		amount_to_withdraw -= (0.000005 + 0.0009)
		if amount_to_withdraw > 0:
			tx_data = await SolanaService.transaction_manager.transfer_sol(get_pubkey(),amount_to_withdraw,TransactionManager.Commitment.CONFIRMED,session_keypair)
	else:
		var amount_to_withdraw:float = await SolanaService.get_balance(session_keypair.to_pubkey().to_string(),token_mint.to_string())
		if amount_to_withdraw > 0:
			tx_data = await SolanaService.transaction_manager.transfer_token(token_mint,get_pubkey(),amount_to_withdraw,TransactionManager.Commitment.CONFIRMED,session_keypair)
	return tx_data
	
func get_end_timestamp(duration_in_minutes:int) -> int:
	var utc_timestamp:float = Time.get_unix_time_from_system()
	var duration_in_seconds:int = duration_in_minutes * 60
	var end_time:int = floori(utc_timestamp + duration_in_seconds)
	return end_time
