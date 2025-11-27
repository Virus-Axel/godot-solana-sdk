extends Node

@onready var start_screen = $StartingScreen
@onready var game_screen = $GameScreen
@onready var anchor_program:AnchorProgram = $AnchorProgram

@export var start_button:ButtonLock

@export var player:TextureRect
@export var chest:TextureRect
@export var step_blocks:Array[CenterContainer]
@export var move_button:BaseButton
@export var password:String="gib"

@export var menu_balance:Label
@export var in_game_balance:Label

var game_account
var chest_vault

var curr_pos:int

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	game_account = Pubkey.new_pda(["level1"],Pubkey.new_from_string(anchor_program.get_pid()))
	chest_vault = Pubkey.new_pda(["chestVault"],Pubkey.new_from_string(anchor_program.get_pid()))
	
	start_button.pressed.connect(setup_game)
	move_button.pressed.connect(move)
	
	menu_balance.text = str(await SolanaService.get_balance(SolanaService.wallet.get_pubkey().to_string()))
	pass # Replace with function body.
	
func setup_game() -> void:
	var instructions:Array[Instruction]
	
	var init_ix:Instruction = anchor_program.build_instruction("initializeLevelOne",[
		game_account, #gamedata
		chest_vault, #gamevault
		SolanaService.wallet.get_kp(), #signer
		SystemProgram.get_pid() #system program
	],null)
	instructions.append(init_ix)
	
	var setup_ix:Instruction = anchor_program.build_instruction("resetLevelAndSpawnChest",[
		SolanaService.wallet.get_kp(), #signer
		chest_vault, #gamevault
		game_account, #gamedata
		SystemProgram.get_pid() #system program
	],null)
	instructions.append(setup_ix)
	
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_and_send(transaction)
	
	if !tx_data.is_successful():
		push_error("Failed to start game")
		return
		
	set_player_pos()
	in_game_balance.text = str(await SolanaService.get_balance(SolanaService.wallet.get_pubkey().to_string()))
	
	start_screen.visible=false
	game_screen.visible=true
	
func move() -> void:
	var instructions:Array[Instruction]
	var move_ix:Instruction = anchor_program.build_instruction("moveRight",[
		chest_vault, #gamevault
		game_account, #gamedata
		SolanaService.wallet.get_kp(), #signer
		SystemProgram.get_pid() #system program
	],{
		"password":password,
	})
	
	instructions.append(move_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_and_send(transaction)
	
	if !tx_data.is_successful():
		push_error("Failed to move")
		return
		
	set_player_pos()

	
func set_player_pos() -> void:
	anchor_program.fetch_account("GameDataAccount",game_account)
	var data:Dictionary = await anchor_program.account_fetched
	
	var new_pos = data["playerPosition"]
	player.get_parent().remove_child(player)
	step_blocks[new_pos].add_child(player)
	
	if new_pos == step_blocks.size()-1:
		in_game_balance.text = str(await SolanaService.get_balance(SolanaService.wallet.get_pubkey().to_string()))
		chest.visible=false
