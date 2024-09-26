extends Node

@export var time_to_close = 2.5

@onready var screen_switcher = $ScreenSwitcher

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	SolanaService.transaction_manager.on_tx_init.connect(enable_tx_screen)
	SolanaService.transaction_manager.on_tx_finish.connect(process_tx_finish)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
	
func enable_tx_screen() -> void:
	screen_switcher.switch_active_panel(0)
	
func process_tx_finish(tx_data:TransactionData) -> void:
	if !tx_data.is_successful():
		screen_switcher.switch_active_panel(1)
	else:
		screen_switcher.switch_active_panel(2)
	
	await get_tree().create_timer(time_to_close).timeout
	screen_switcher.close_active_panel()
