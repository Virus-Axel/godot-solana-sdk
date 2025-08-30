@tool

extends Label

var MAX_DOTS = 2
var dots = 0
var info_text = "Simulating Transaction." 

func _on_timer_timeout() -> void:
	dots = (dots + 1) % (MAX_DOTS + 1)
	text = info_text
	for i in range(dots):
		text += "."
