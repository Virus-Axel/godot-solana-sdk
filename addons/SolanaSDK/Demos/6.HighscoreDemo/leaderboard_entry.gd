extends Node
class_name LeaderboardEntry

@export var id:Label
@export var nickname:Label
@export var score:Label
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


func set_data(leaderboard_id:int,player_name:String,player_score:int) -> void:
	id.text = "%s." % str(leaderboard_id)
	nickname.text = player_name
	score.text = str(player_score)
