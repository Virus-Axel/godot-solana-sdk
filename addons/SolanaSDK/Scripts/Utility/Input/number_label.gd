extends Label
class_name NumberLabel

enum NumberFormatMode {COMMA, LETTER}
@export var format_mode:NumberFormatMode
@export var decimal_amount:int = 0

@export var audio_player:AudioStreamPlayer
@export var pitch_increment:float = 0.02

var original_pitch:float

var number_value = 0
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if audio_player!=null:
		original_pitch = audio_player.pitch_scale
	pass # Replace with function body.

	
func lerp_to_value(end_number,increment_value,increment_duration:float=0.06) -> void:
	var increment_sign = 1 if end_number > number_value else -1
	while true:
		set_value(number_value+(increment_value*increment_sign))
		if (increment_sign< 0 and number_value < end_number) or (increment_sign > 0 and number_value > end_number):
			set_value(end_number)
			break
			
		if audio_player!=null:
			audio_player.pitch_scale += pitch_increment * increment_sign
			audio_player.pitch_scale = clampf(audio_player.pitch_scale,original_pitch*0.5,original_pitch*1.5)
			audio_player.play()
			
		await get_tree().create_timer(increment_duration).timeout
	
	if audio_player!=null:
		audio_player.pitch_scale = original_pitch
	
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
	
