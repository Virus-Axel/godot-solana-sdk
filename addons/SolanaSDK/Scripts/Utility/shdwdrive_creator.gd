@tool
extends Node
class_name ShdwDriveCreator

enum StorageSizeUnit {KB,MB,GB}
@export_category("General Settings")
@export var path_to_keypair:String:
	set(value):
		if in_progress:
			return
		else:
			path_to_keypair = value
			fetch_user_storages()
		
var selected_storage: String = ""

@export var refresh_storages: bool:
	set(value):
		if !in_progress:
			fetch_user_storages()

@export var abort: bool:
	set(value):
		if in_progress:
			abort_action()

@export_category("Manage ShdwDrive Storage")
@export_subgroup("Create Storage")
@export var storage_name:String
@export var storage_size:int
@export var size_unit:StorageSizeUnit

## Please make sure you have some SOL and $SHDW tokens in your wallet or else you can't create storage!
@export var create: bool:
	set(value):
		if !in_progress:
			create_shdw_storage()
			
@export_subgroup("Select Storage")
@export_subgroup("Upload File To Storage")
@export_file() var file_path:String

## Please make sure you have some SOL and $SHDW tokens in your wallet or else you can't upload a file!
@export var upload_file: bool:
	set(value):
		if !in_progress:
			upload_to_storage()

		
var in_progress:bool=false
var user_storages:Array

func _ready() -> void:
	if Engine.is_editor_hint():
		if path_to_keypair.length() > 0:
			fetch_user_storages()

func get_shdw_drive_instance() -> ShdwDrive:
	var shdw_drive:ShdwDrive = ShdwDrive.new()
	add_child(shdw_drive)
	return shdw_drive
	
func fetch_user_storages() -> void:
	in_progress = true
	print("Fetching user Shdwdrive storages, please wait...")
	
	var owner_keypair:Keypair = Keypair.new_from_file(path_to_keypair)
	if owner_keypair == null:
		print("Failed to parse provided keypair..")
		user_storages = []
		notify_property_list_changed()
		in_progress = false
		return
		
	var shdw_drive:ShdwDrive = get_shdw_drive_instance()
	shdw_drive.get_all_storage_accounts(owner_keypair)
	await shdw_drive.all_storage_accounts_fetched
	user_storages = shdw_drive.get_cached_storage_accounts()
	#print(user_storages)
	notify_property_list_changed()
	in_progress = false
	
func _get_property_list() -> Array:
	var properties = []
	
	 # Manually create a comma-separated string from PackedStringArray
	var storage_entries:PackedStringArray
	for i in range(user_storages.size()):
		storage_entries.append(user_storages[i]["identifier"])
	var hint_string:String = String(",").join(storage_entries)
	
	# Define the dynamic dropdown for `selected_storage`
	var dropdown = {
		"name": "selected_storage",
		"type": TYPE_STRING,
		"hint": PROPERTY_HINT_ENUM,
		"hint_string": hint_string,  # Join keys as dropdown options
		"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_EDITOR
	}
	dropdown["category"] = "General Settings"
	
	properties.append(dropdown)
	return properties

func create_shdw_storage() -> void:
	in_progress = true
	print("Creating storage, please wait...")
	
	var owner_keypair:Keypair = Keypair.new_from_file(path_to_keypair)
	if owner_keypair == null:
		print("Failed to parse provided keypair..")
		in_progress = false
		return
		
	var size:String = str(storage_size)+StorageSizeUnit.keys()[size_unit]
	
	var shdw_drive:ShdwDrive = get_shdw_drive_instance()
	shdw_drive.create_storage_account(owner_keypair,storage_name,size)
	var result:Dictionary = await shdw_drive.storage_account_response
	
	if result.size()==0 or result.has("error"):
		print("Failed to create storage, please try again...")
		print("Error: %s" % result["error"])
	else:
		print(result) 
   	
	remove_child(shdw_drive)
	in_progress = false
	
func upload_to_storage() -> void:
	in_progress = true
	print("Uploading file to storage, please wait...")
	
	var owner_keypair:Keypair = Keypair.new_from_file(path_to_keypair)
	if owner_keypair == null:
		print("Failed to parse provided keypair..")
		in_progress = false
		return
		
	if selected_storage == "":
		print("No storage selected...")
		in_progress=false
		return
	
	var shdw_drive:ShdwDrive = get_shdw_drive_instance()
	shdw_drive.fetch_storage_key_by_name(owner_keypair,selected_storage)
	var storage_account:Pubkey = await shdw_drive.storage_account_fetched
	if storage_account == null:
		print("Failed to fetch a storage by name of: '%s'" % selected_storage)
		in_progress = false
		return
	
	shdw_drive.upload_file_to_storage(file_path,owner_keypair,storage_account)
	var result:Dictionary = await shdw_drive.upload_response
	
	if result.size()==0 or result.has("error"):
		print("Failed to upload a file to storage, please try again...")
		print("Error: %s" % result["error"])
	else:
		print("File Uploaded to ShdwDrive Storage successfully!")
		print_rich("Link to file: [url]%s[/url]" % result["finalized_locations"][0])
		
	remove_child(shdw_drive)
	in_progress = false
	
	
func abort_action() -> void:
	for child in get_children():
		if child is ShdwDrive:
			remove_child(child)
	
	in_progress=false
	print("Action Aborted!")
	
	

	
	
