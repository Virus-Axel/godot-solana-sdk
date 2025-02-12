extends Node
enum RpcCluster{MAINNET,DEVNET,SONIC}
@export var rpc_cluster:RpcCluster=RpcCluster.MAINNET
@export var mainnet_rpc:String
@export var devnet_rpc:String
@export var sonic_rpc:String

var default_devnet = "https://api.devnet.solana.com"
var default_mainnet = "https://api.mainnet-beta.solana.com"


var active_rpc:String

var token_pid:String = "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA"
var associated_token_pid:String = "ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL"

@onready var wallet:WalletService = $WalletService
@onready var transaction_manager:TransactionManager = $TransactionManager
@onready var file_loader:FileLoader = $FileLoader
@onready var account_inspector:AccountInspector = $AccountInspector
@onready var candy_machine_manager:CandyMachineManager = $CandyMachineManager
@onready var asset_manager:AssetManager = $AssetManager

var rpc:String

## Called when the node enters the scene tree for the first time.
func _ready() -> void:
	asset_manager.setup()
	
	if mainnet_rpc=="":
		mainnet_rpc=default_mainnet
	if devnet_rpc=="":
		devnet_rpc=default_devnet
		
	set_rpc_cluster(rpc_cluster)
	
	
func set_rpc_cluster(new_cluster:RpcCluster)->void:
	match new_cluster:
		RpcCluster.MAINNET:
			active_rpc = mainnet_rpc
		RpcCluster.DEVNET:
			active_rpc = devnet_rpc
		RpcCluster.SONIC:
			active_rpc = sonic_rpc
			
	ProjectSettings.set_setting("solana_sdk/client/default_url",active_rpc)
	rpc_cluster = new_cluster
	
func generate_keypair(derive_from_machine:bool=false) -> Keypair:
	var randomizer = RandomNumberGenerator.new()
	randomizer.randomize()
	if derive_from_machine:
		randomizer.set_seed(OS.get_unique_id().hash()) 

	var keypair = Keypair.new();
	keypair.set_unique(false)
	var seed := PackedByteArray()
	for i in range(32):
		var random_value := randomizer.randi() % 256  # randi() generates a random integer
		seed.append(random_value)
#	seed.resize(32)
	keypair.set_seed(seed);
	return keypair
	
func generate_keypair_from_pk(pk:String) -> Keypair:
	var seed = SolanaUtils.bs58_decode(pk)
	var keypair = Keypair.new_from_seed(seed)
	#var keypair = Keypair.new_from_file("C:\\Users\\thoma\\Desktop\\kp\\kp.json")
	return keypair
	
#	print(SolanaClient.get_latest_blockhash())
func spawn_client_instance()->SolanaClient:
	var sol_client:SolanaClient = SolanaClient.new()
	add_child(sol_client)
	return sol_client
	
func spawn_mpl_metadata_client() -> MplTokenMetadata:
	var mpl_metadata:MplTokenMetadata = MplTokenMetadata.new()
	add_child(mpl_metadata)
	return mpl_metadata
	
func spawn_mpl_candy_machine_client() -> MplCandyMachine:
	var candy_machine:MplCandyMachine = MplCandyMachine.new()
	add_child(candy_machine)
	return candy_machine
	
func get_account_info(account:Pubkey) -> Dictionary:
	var client:SolanaClient = spawn_client_instance()
	client.get_account_info(account.to_string())
	var response_dict:Dictionary = await client.http_response_received
	client.queue_free()
	return response_dict
	
func get_balance(address_to_check:String,token_address:String="") -> float:
	var client:SolanaClient = spawn_client_instance()
	if token_address == "":
		client.get_balance(address_to_check)
		var response_dict:Dictionary = await client.http_response_received
		client.queue_free()
		var balance = response_dict["result"]["value"] / 1000000000
		return balance
	else:
		var token_account:Pubkey = await get_associated_token_account(address_to_check,token_address)
		if token_account == null:
			return 0
		
		client.get_token_account_balance(token_account.to_string())
		var response_dict:Dictionary = await client.http_response_received
		client.queue_free()
		
		var lamport_balance = response_dict["result"]["value"]["amount"]
		var token_decimals = response_dict["result"]["value"]["decimals"]
		return float(lamport_balance)/(10**token_decimals)	
		
func get_token_decimals(token_address:String)->int:
	var client:SolanaClient = spawn_client_instance()
	client.get_token_supply(token_address)
	var response_dict:Dictionary = await client.http_response_received
	client.queue_free()
	
	return response_dict["result"]["value"]["decimals"]
	
func get_associated_token_account(address_to_check:String,token_address:String) -> Pubkey:
	var client:SolanaClient = spawn_client_instance()
	client.get_token_accounts_by_owner(address_to_check,token_address,associated_token_pid)
	var response_dict:Dictionary = await client.http_response_received
	client.queue_free()
	
	var ata:String
	
	if response_dict.has("error"):
		return null

	if response_dict["result"]["value"].size() == 0:
		return null
	
	return Pubkey.new_from_string(response_dict["result"]["value"][0]["pubkey"])
	
func get_wallet_assets(wallet_address:String) -> Array[Dictionary]:
	var client:SolanaClient = spawn_client_instance()
	client.get_token_accounts_by_owner(wallet_address,"",token_pid)
	var response_dict:Dictionary = await client.http_response_received
	client.queue_free()

	var wallet_tokens:Array[Dictionary]
	for token in response_dict["result"]["value"]:
		var token_byte_data = SolanaUtils.bs64_decode(token["account"]["data"][0])
		var token_data:Dictionary = parse_token_data(token_byte_data)
		print(token_data)
		#remove token accounts which no longer hold an NFT
		if token_data["amount"] == 0:
			continue
		wallet_tokens.append(token_data)
	
	return wallet_tokens
	
func parse_token_data(data: PackedByteArray) -> Dictionary:
	# Ensure that the data has a minimum length
	if data.size() < 64:
		print("Invalid token data")
		return {}
	
	# Extract the mint address (first 32 bytes)
	var mint_address = SolanaUtils.bs58_encode(data.slice(0, 32))
	var owner_address = SolanaUtils.bs58_encode(data.slice(32, 64))

	# Extract the amount (next 8 bytes) and convert it to a 64-bit integer
	var amount_bytes = data.slice(64, 72)
	var amount = amount_bytes.decode_u64(0)
	
	return {"mint":mint_address,"owner":owner_address,"amount":amount}
	
