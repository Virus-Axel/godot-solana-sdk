extends Node
class_name LeaderboardSystem

@export var soar_program:SoarProgram
@export var leaderboard_entry:PackedScene

@export var scorebox:VBoxContainer
@export var back_button:Button

@export var best_score:Label

@export var start_screen:Control

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	back_button.pressed.connect(back_to_menu)
	pass # Replace with function body.


func refresh(scores_data:Dictionary, player_scores:Dictionary) -> void:
	clear_list()
	var index=1
	for entry in scores_data["topScores"]:
		#if reached -1, means end of filled scores, following ones will be empty
		if entry["entry"]["score"] == 0:
			break
			
		var player_data:Dictionary = await soar_program.fetch_player_data_from_pda(entry["player"])
		var entry_instance:LeaderboardEntry = leaderboard_entry.instantiate() as LeaderboardEntry
		scorebox.add_child(entry_instance)
		entry_instance.set_data(index,player_data["username"],entry["entry"]["score"])
		index+=1
	
	if player_scores.size()==0:
		best_score.text="-"
	else:
		var top_score=0
		for score in player_scores["scores"]:
			if score["score"] > top_score:
				top_score = score["score"]
		best_score.text=str(top_score)
	
func clear_list() -> void:
	for entry in scorebox.get_children():
		entry.queue_free()
		
func back_to_menu() -> void:
	self.visible=false
	start_screen.visible=true
