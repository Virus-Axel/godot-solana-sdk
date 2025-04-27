extends LineEdit
class_name InputField

enum InputType{ALPHANUMERIC,INTEGER,DECIMAL, URL}

@export var input_type = InputType.ALPHANUMERIC
@export var min_length:int = 0
@export var is_optional:bool = false
@export var null_if_empty:bool = true
@export var is_pubkey:bool=false
#@export_category("Alphanumeric Settings")
#@export var bytes_required:int = 0

@export_category("Number Field Settings")
@export var min_value:float = 0.0
@export var max_value:float = 999.0
@export var allow_zero:bool=true


var alphanumeric_regex = "^[a-zA-Z0-9 _\\-@.,]+$"
var integer_regex = "^[-+]?\\d+$"
var fraction_regex = "^[-+]?[0-9]+(\\.[0-9]+)?$"
var url_regex = "^(https?:\\/\\/)?([a-zA-Z0-9.-]+)\\.([a-zA-Z]{2,6})(:[0-9]{1,5})?(\\/\\S*)?$"

@onready var input_constraint = RegEx.new()
var old_text

signal on_field_updated(value:String)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	old_text = text
	text_changed.connect(handle_text_change)
	text_submitted.connect(handle_text_submit)
	focus_exited.connect(handle_focus_lost)
	focus_entered.connect(select_input)
	match input_type:
		InputType.ALPHANUMERIC:
			input_constraint.compile(alphanumeric_regex)
		InputType.INTEGER:
			input_constraint.compile(integer_regex)
		InputType.DECIMAL:
			input_constraint.compile(fraction_regex)
		InputType.URL:
			input_constraint.compile(url_regex)
			
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

func handle_text_change(new_text: String) -> void:
	#if input_constraint.search(new_text) == null:
		#text = old_text
		#caret_column = text.length()
		#return
	
	#old_text = text
	#text = new_text
	#caret_column = text.length()
	pass

func select_input() -> void:
	# Defer select_all to the next frame to allow mouse events to finish
	await get_tree().process_frame

	if has_focus():
		# Only select all if the caret is still at the beginning (likely from tabbing)
		if caret_column == 0:
			select_all()
			caret_column = text.length()
	
func handle_text_submit(new_text:String) -> void:
#	when submit text, just release focus and all the logic goes there
	release_focus()
	
func handle_focus_lost() -> void:
	text = validate_text(text)
	on_field_updated.emit(text)
	
func validate_text(new_text:String) -> String:
	var adjusted_text:String = new_text
	
	if new_text.length() >0 and input_constraint.search(new_text) == null:
		if input_type == InputType.INTEGER or input_type == InputType.DECIMAL:
			if min_value <= 0 and !allow_zero:
				return ""
			else:
				return str(min_value)
		else:
			return ""

	match input_type:
		InputType.ALPHANUMERIC:
			adjusted_text = new_text.strip_edges()
			if is_pubkey:
				if SolanaUtils.bs58_decode(adjusted_text).size() != 32:
					return ""
		InputType.INTEGER:
			var value:int = int(new_text)
			value = clamp(value,get_min_value(),get_max_value())
			if value == 0 and not allow_zero:
				return ""
			adjusted_text = str(value)
		InputType.DECIMAL:
			var value:float = float(new_text)
			value = clamp(value,get_min_value(),get_max_value())
			if value == 0 and not allow_zero:
				return ""
			adjusted_text = str(value)
		InputType.URL:
			adjusted_text = new_text.strip_edges()
	
	return adjusted_text
		
	
func get_min_value():
	if min_value == -1:
		return -INF
	else:
		return min_value
		
func get_max_value():
	if max_value == -1:
		return INF
	else:
		return max_value
		
func set_value(value:String) -> void:
	text = validate_text(value)
	
		
func is_valid() -> bool:
	var input_valid:bool = true
	
	if !is_optional or (is_optional and text.length() != 0):
		if text.length() < min_length:
			input_valid = false
		if input_constraint.search(text) == null:
			input_valid = false
	return input_valid
	
func get_field_value():
	match input_type:
		InputType.ALPHANUMERIC:
			if text.length() == 0:
				if null_if_empty:
					return null
				else:
					return ""
			else:
				if is_pubkey:
					return Pubkey.new_from_string(text)
				else:
					return text
		InputType.INTEGER:
			if text.length() == 0:
				return null
			return int(text)
		InputType.DECIMAL:
			if text.length() == 0:
				return null
			return float(text)
		InputType.URL:
			if text.length() == 0:
				if null_if_empty:
					return null
				else:
					return ""
			else:
				return text
			
	
