@tool
extends HBoxContainer

var selected_keypair: Keypair = null

signal valid_keypair_selected
signal invalid_keypair_selected
signal keypair_updated

func display_keypair():
	if not is_valid_keypair():
		$LineEdit.text = ""
	else:
		$LineEdit.text = selected_keypair.get_public_string()

func is_valid_keypair():
	if selected_keypair == null:
		return false
	if selected_keypair.get_public_bytes().size() != Pubkey.PUBKEY_BYTES:
		return false
		
	return true

func _on_file_dialog_file_selected(path: String) -> void:
	selected_keypair = Keypair.new_from_file(path)
	emit_signal("keypair_updated")
	display_keypair()
	if is_valid_keypair():
		emit_signal("valid_keypair_selected")
	else:
		emit_signal("invalid_keypair_selected")


func _on_button_pressed() -> void:
	$FileDialog.show()


func _on_check_box_toggled(toggled_on: bool) -> void:
	$Button.disabled = toggled_on
	if toggled_on:
		selected_keypair = Keypair.new_random()
		emit_signal("keypair_updated")
		display_keypair()
	else:
		selected_keypair = null
		emit_signal("keypair_updated")
		display_keypair()
