extends Node

@onready var start_screen = $StartingScreen
@onready var game_screen = $GameScreen
@onready var anchor_program:AnchorProgram = $AnchorProgram

@export var tiny_adventure_pid:String
@export var start_button:ButtonLock

@export var player:TextureRect
@export var chest:TextureRect
@export var chest_prize:float
@export var prize_label:Label
@export var step_blocks:Array[CenterContainer]
@export var left_button:TextureButton
@export var right_button:TextureButton

@export var in_game_balance:TokenVisualizer

var level_pda
var vault_pda

var curr_pos:int

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	level_pda = Pubkey.new_pda(["Level1"],Pubkey.new_from_string(tiny_adventure_pid))
	vault_pda = Pubkey.new_pda(["Vault1"],Pubkey.new_from_string(tiny_adventure_pid))
	
	start_button.text = "START GAME (%s SOL)" % chest_prize
	start_button.pressed.connect(init_game)
	left_button.pressed.connect(move_left)
	right_button.pressed.connect(move_right)
	
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
	
func init_game() -> void:
	var prize_in_lamports:int = int(chest_prize*pow(10,9))
	var instructions:Array[Instruction]
	var init_ix:Instruction = anchor_program.build_instruction("restartLevel",[
		level_pda, #gamedata
		vault_pda, #gamevault
		SolanaService.wallet.get_kp(), #signer
		SystemProgram.get_pid() #system program
	],
	AnchorProgram.u64(prize_in_lamports))
	
	instructions.append(init_ix)
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_transaction(instructions,TransactionManager.Commitment.FINALIZED)
	
	if !tx_data.is_successful():
		push_error("Failed to start game")
		return
		
	update_prize()
	set_player_pos()
	in_game_balance.load_token()
	
	start_screen.visible=false
	game_screen.visible=true
	
	
func move_left() -> void:
	move("moveLeft")
func move_right() -> void:
	move("moveRight")
	
func move(move_dir:String) -> void:
	var instructions:Array[Instruction]
	var move_ix:Instruction = anchor_program.build_instruction(move_dir,[
		level_pda, #gamedata
		vault_pda, #gamevault
		SolanaService.wallet.get_kp(), #signer
		SystemProgram.get_pid() #system program
	],null)
	
	instructions.append(move_ix)
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_transaction(instructions,TransactionManager.Commitment.FINALIZED)
	
	if !tx_data.is_successful():
		push_error("Failed to move")
		return
		
	set_player_pos()

	
func update_prize() -> void:
	var instance:AnchorProgram = spawn_anchor_program_instance()
	instance.fetch_account("GameVault",vault_pda)
	var data:Dictionary = await instance.account_fetched
	
	var prize_in_sol:float = float(data["chestPrize"])/pow(10,9)
	prize_label.text = "%s SOL" % str(prize_in_sol)
	
func set_player_pos() -> void:
	var instance:AnchorProgram = spawn_anchor_program_instance()
	instance.fetch_account("GameData",level_pda)
	var data:Dictionary = await instance.account_fetched
	
	var new_pos = data["characterPos"]
	player.get_parent().remove_child(player)
	step_blocks[new_pos].add_child(player)
	
	if new_pos == step_blocks.size()-1:
		in_game_balance.load_token()
		chest.visible=false
		

func spawn_anchor_program_instance()->AnchorProgram:
	var instance:AnchorProgram = AnchorProgram.new()
	add_child(instance)
	instance.set_pid(anchor_program.get_pid())
	instance.set_json_file(anchor_program.get_json_file())
	instance.set_idl(anchor_program.get_idl())
	return instance
