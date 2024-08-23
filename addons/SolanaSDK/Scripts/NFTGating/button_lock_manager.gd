extends Node
class_name ButtonLockManager

@export var pass_locks:bool
@export var button_locks:Array[ButtonLock]

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if pass_locks:
		for button in button_locks:
			button.lock_active=false

