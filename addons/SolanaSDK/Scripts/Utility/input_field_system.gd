extends Node
class_name InputFieldSystem

@export var input_fields: Array[InputField]
@export var confirm_button:Button

signal on_input_submit
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	confirm_button.disabled=true
	confirm_button.connect("pressed",on_input_confirm)
	for field in input_fields:
		field.connect("on_field_updated",check_validity)


func on_input_confirm() -> void:
	emit_signal("on_input_submit",get_fields_data())
	

func check_validity():
	var all_valid=true
	for field in input_fields:
		if !field.is_valid():
			all_valid=false
			break
	confirm_button.disabled = !all_valid
		
func get_fields_data()-> Array[String]:
	var fields_data:Array[String]
	for input in input_fields:
		fields_data.append(input.text)
	
	return fields_data
		
func clear_fields() -> void:
	for field in input_fields:
		field.clear()
