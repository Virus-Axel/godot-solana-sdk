extends LineEdit
class_name InputField

enum InputType{Alphanumeric,Integer,FractionNumber}

@export var input_type = InputType.Alphanumeric
@export var min_length:int = 0
@export var allow_negative = false
@export var is_optional = false


var fraction_regex = "^[0-9.]*$"
var integer_regex = "^[0-9]+$"
var alphanumeric_regex = "^[a-zA-Z0-9]+$"

@onready var input_constraint = RegEx.new()

signal on_field_updated

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	match input_type:
		InputType.Alphanumeric:
			input_constraint.compile(alphanumeric_regex)
		InputType.Integer:
			input_constraint.compile(integer_regex)
		InputType.FractionNumber:
			input_constraint.compile(fraction_regex)
			
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
	
var old_text = ""
func _on_text_changed(new_text: String) -> void:
	if input_constraint.search(new_text):
			if !allow_negative:
				if input_type == InputType.Integer && int(new_text) < 0:
					new_text = "0"
				if input_type == InputType.FractionNumber && float(new_text) < 0.0:
					new_text = "0.0"
			old_text = new_text
	else:
		text = old_text
#		set_cursor_position(text.length())
	
	emit_signal("on_field_updated")
	
		
func is_valid() -> bool:
	if text.length() < min_length:
		return false
	if !is_optional && text.length()==0:
		return false
		
	return true
