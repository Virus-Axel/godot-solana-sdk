extends Label
class_name NumberLabel

enum NumberFormatMode {COMMA, LETTER}
@export var format_mode:NumberFormatMode
@export var decimal_amount:int = 0

var number_value
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
	
func set_value(number) -> void:
	number_value = number
		
	match format_mode:
		NumberFormatMode.COMMA:
			text = comma_format(number)
		NumberFormatMode.LETTER:
			text = zeros_to_letter(number)
	
func comma_format(number) -> String:
	if decimal_amount != 0:
		var format_string = "%0." + str(clampi(decimal_amount,0,9)) + "f"
		number = format_string % number  # Formats number to 2 decimal places if needed

	var parts = str(number).split(".")   # Splits the integer and decimal parts
	var integer_part = parts[0]
	var decimal_part
	if parts.size() > 1:
		decimal_part = "." + parts[1]
	else:
		decimal_part = ""
		
	# Add commas to the integer part
	var with_commas = ""
	while integer_part.length() > 3:
		with_commas = "," + integer_part.substr(integer_part.length() - 3, 3) + with_commas
		integer_part = integer_part.substr(0, integer_part.length() - 3)
	with_commas = integer_part + with_commas
	
	return with_commas + decimal_part
	
func zeros_to_letter(number) -> String:
	var suffix = ""
	var formatted_number = number

	if number >= 1_000_000_000:
		formatted_number = number / 1_000_000_000.0
		suffix = "B"
	elif number >= 1_000_000:
		formatted_number = number / 1_000_000.0
		suffix = "M"
	elif number >= 1_000:
		formatted_number = number / 1_000.0
		suffix = "K"
		
	var floored_number:float = floor(formatted_number * pow(10, decimal_amount)) / pow(10, decimal_amount)
	var formatted_string:String
	# Determine if we should display decimals or not
	if floored_number == int(floored_number):
		# No decimals needed, display as integer
		formatted_string = str(int(floored_number))
	else:
		# Use formatting to display with the specified decimal places
		var format_string = "%0." + str(decimal_amount) + "f"
		formatted_string = format_string % floored_number
	
	return formatted_string + suffix

func get_number_value():
	return number_value
