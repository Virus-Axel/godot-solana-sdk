@tool
extends TextEdit

const MAX_DOTS := 4

var dots = 0

func send_transaction(tx: Transaction):
	text = "Sending Transaction..."
	$RollTimer.start()
	tx.connect("transaction_response_received", response_received, CONNECT_ONE_SHOT)
	tx.send()
	
func response_received(response: Dictionary):
	$RollTimer.stop()
	if not response.has("result"):
		text = str(response)
		return
	if typeof(response["result"]) != TYPE_STRING:
		text = str(response)
		return
		
	text = response["result"]


func _on_roll_timer_timeout() -> void:
	text = "Sending Transaction"
	for i in range(dots):
		text += '.'
	
	dots = (dots+1) % MAX_DOTS
	pass # Replace with function body.
