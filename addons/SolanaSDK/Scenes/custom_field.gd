@tool
extends HBoxContainer

signal delete

func _on_button_pressed() -> void:
	emit_signal("delete")

func remove_self():
	get_parent().remove_child(self)
	queue_free()

func get_field():
	return $LineEdit.text

func get_value():
	return $LineEdit2.text
