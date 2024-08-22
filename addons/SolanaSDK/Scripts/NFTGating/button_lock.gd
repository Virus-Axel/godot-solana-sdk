extends Button
class_name ButtonLock

@export var lock_active:bool=true
@export var hide_if_locked:bool


func _ready() -> void:
	set_interactable(false)
	try_unlock()
	self.visibility_changed.connect(on_visibility_changed)

func on_visibility_changed() -> void:
	if self.visible and lock_active:
		set_interactable(false)
		try_unlock()

func set_interactable(state:bool) -> void:
	disabled = !state
	if state==false && hide_if_locked:
		visible=false
		
func try_unlock() -> void:
	pass
	
func get_hashlist_from_file(file_path: String) -> Array[String]:
	var file = FileAccess.open(file_path, FileAccess.READ)
	
	# Open the file
	if file!=null:
		# Read the entire file content
		var json_content = file.get_as_text()
		# Parse the JSON content
		#print(json_content)
		var parsed_json = parse_json(json_content)
		# Close the file
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
