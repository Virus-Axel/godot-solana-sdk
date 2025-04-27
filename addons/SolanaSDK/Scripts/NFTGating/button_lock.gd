extends Button
class_name ButtonLock

@export var lock_active:bool=true
@export var hide_if_locked:bool
@export var locked_text:String
@export var no_auth_text:String

var is_hidden:bool

var original_text:String

func _ready() -> void:
	original_text = text
	self.visibility_changed.connect(on_visibility_changed)
	
	if lock_active:
		set_interactable(false)
		if SolanaService.wallet.is_logged_in():
			try_unlock()
		else:
			SolanaService.wallet.on_login_success.connect(handle_auth)
			if no_auth_text != "":
				text = no_auth_text

func on_visibility_changed() -> void:
	if !lock_active:
		return
		
	if hide_if_locked and is_hidden:
		return
	
	if self.visible and SolanaService.wallet.is_logged_in():
		set_interactable(false)
		try_unlock()
		
func handle_auth() -> void:
	try_unlock()

func set_interactable(state:bool) -> void:
	disabled = !state
	if hide_if_locked:
		visible = state
		is_hidden = !state
	
	if SolanaService.wallet.is_logged_in() and locked_text != "":
		if disabled:
			text = locked_text
		else:
			text = original_text
		
func try_unlock() -> void:
	set_interactable(SolanaService.wallet.is_logged_in())
	
func get_hashlist_from_file(file_path: String) -> Array[String]:
	var file = FileAccess.open(file_path, FileAccess.READ)
	
	if file!=null:
		var json_content = file.get_as_text()
		var parsed_json = parse_json(json_content)
		file.close()
		return parsed_json
	else:
		print("Error opening the file:", file_path)
		return []
		
func parse_json(json_string: String) -> Array[String]:
	# Parse the JSON string
	var json:JSON = JSON.new()
	var error = json.parse(json_string)
	# Check if parsing was successful
	if error != OK:
		print("Error parsing JSON")
		return []
	
	var address_list:Array[String]
	for address in json.data:
		address_list.append(address)
		
	return address_list
