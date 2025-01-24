extends Node

@onready var soar_program:SoarProgram=$SoarProgram
@onready var start_screen=$StartingScreen
@onready var play_screen = $PlayScreen
@onready var leaderboard_screen=$LeaderboardScreen

@export var start_game_button:Button
@export var score_label:Label
@export var submit_score_button:Button
@export var leaderboard_button:Button

@export var leaderboard_system:LeaderboardSystem

#THIS IS VERY UNSAFE!! 
#For players to submit scores, they need the game's authority (set in init_game) to sign as well
#So that players couldnt just sign the transaction without playing the game with setting any score
#For a test, we have exposed the authority so sign along the player

#For production, you need to store the keypair in some server, and send the serialized transaction
#for the server to sign with that keypair and return the signed transaction to send it out.
# Called when the node enters the scene tree for the first time.
@export var game_authority_path:String

var game_auth:Keypair

var player_score:int

#devnet Game account, created once using setup_game function
var game_address:String = "5XSs9TZRasg3k6dJLo1fKko3gtc749KNgUoj9mCqRF26"
#devned leaderboard No.1 created from the game. Players can submit their scores to it
var leaderboard_address:String = "9ztUqAyKPvPUmTAiopEZcc4QGghMvuM9oURixDQCgGcK"
#collection used as nft_meta for games and leaderboards. this is devnet Rubians
var nft_collection:String = "EE1XTVRxVX5UtTLKRg7Y5bFQEKm2wm2nao9SGF27fypH"

func _ready() -> void:
	start_game_button.pressed.connect(start_game)
	submit_score_button.pressed.connect(submit_score)
	leaderboard_button.pressed.connect(show_leaderboard)
	
	game_auth = Keypair.new_from_file(game_authority_path)

	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
	
func start_game() -> void:
	#check if this wallet is initialized as a player in SOAR program
	#if not, initialize and then start
	
	var player_data:Dictionary = await soar_program.fetch_player_data(SolanaService.wallet.get_pubkey())
	if player_data.size()==0:
		await initialize_player()
		
	play()
		
func play() -> void:
	start_screen.visible=false
	play_screen.visible=true
	
	player_score = randi_range(1,420)
	score_label.text = str(player_score)
	
func setup_game() -> void:
	var game_attributes:SoarUtils.GameAttributes = SoarUtils.GameAttributes.new()
	game_attributes.title="Dev Game"
	game_attributes.description="Best Game ever by Zen Republic"
	#game's collection nft. Rubians provided by default
	game_attributes.nft_meta = Pubkey.new_from_string(nft_collection)
	
	var tx_data:TransactionData = await soar_program.init_game(game_attributes)


func register_leaderboard() -> void:
	var leaderboard_data:SoarUtils.LeaderboardData = SoarUtils.LeaderboardData.new()
	leaderboard_data.description="Mega leaderboard win big in 24 hours!"
	leaderboard_data.nft_meta = Pubkey.new_from_string(nft_collection)
	leaderboard_data.set_decimals(0)
	leaderboard_data.set_scores_to_retain(10)
	leaderboard_data.is_ascending=false
	leaderboard_data.allow_multiple_scores=true
	
	var tx_data:TransactionData = await soar_program.add_leaderboard(game_address,leaderboard_data,game_auth)
	
func update_leaderboard() -> void:
	var leaderboard_data:SoarUtils.LeaderboardData = SoarUtils.LeaderboardData.new()
	leaderboard_data.description="Brand new leaderboard, win big in 24 hours!"
	leaderboard_data.nft_meta = Pubkey.new_from_string(nft_collection)
	leaderboard_data.is_ascending=false
	leaderboard_data.allow_multiple_scores=true
	
	var tx_data:TransactionData = await soar_program.update_leaderboard(game_address,leaderboard_address,leaderboard_data,game_auth)
	

func initialize_player() -> void:
	var username:String = "Pokemon Go"
	#devnet rubian nft example
	var user_nft:Pubkey = Pubkey.new_from_string("9aNFiE6mdcQSGaytpoqpWvJMeA2h6vDa4sJttsyyKFPA")
	
	var tx_data:TransactionData = await soar_program.initialize_player(username,user_nft)
	#soar_program.update_player(username,user_nft)
	
func submit_score() -> void:
	var game_account:Pubkey = Pubkey.new_from_string(game_address)
	var leaderboard_account:Pubkey = Pubkey.new_from_string(leaderboard_address)
	
	var auth_keypair = Keypair.new_from_file(game_authority_path)
	var tx_data:TransactionData = await soar_program.submit_score_to_leaderboard(game_account,leaderboard_account,auth_keypair,player_score)
	start_screen.visible=true
	play_screen.visible=false

	
func show_leaderboard() -> void:
	var leaderboard_scores:Dictionary = await soar_program.fetch_leaderboard_scores(Pubkey.new_from_string(leaderboard_address))
	var player_scores:Dictionary = await soar_program.fetch_player_scores(SolanaService.wallet.get_pubkey(),Pubkey.new_from_string(leaderboard_address))
	leaderboard_system.refresh(leaderboard_scores,player_scores)
	start_screen.visible=false
	leaderboard_screen.visible=true
