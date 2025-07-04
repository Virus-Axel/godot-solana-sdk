extends Node
class_name AccountDisplaySystem

@export var container:Container
@export var entry_scn:PackedScene
@export var no_entries_overlay:Control
@export var bring_new_to_back:bool=true
@export var page_loader:PageLoader
@export var refresh_button:BaseButton

var list_data:Dictionary
var filter_data:Dictionary
var sort_data:Dictionary

var raw_accounts:Dictionary
var paged_accounts:Array

var curr_page:int=0
var curr_page_container:Container
var page_loading:bool=false

signal on_account_added(account_entry:AccountDisplayEntry)
signal on_account_selected(account_entry:AccountDisplayEntry)

signal on_page_load_started
signal on_page_load_finished

func _ready() -> void:
	if no_entries_overlay!=null:
		no_entries_overlay.visible=false
	
	if refresh_button!=null:
		refresh_button.pressed.connect(refresh_account_list)
		
	if page_loader!=null:
		page_loader.on_page_changed.connect(load_page)
		
func clear_display() -> void:
	if curr_page_container!=null:
		curr_page_container.queue_free()
		
	list_data = {}
	sort_data = {}
	raw_accounts = {}
	paged_accounts.clear()
	curr_page = 0
	page_loading=false
	
		
func set_list(program:AnchorProgram,account_keyword:String,filter:Array=[],page_limit:int=5, spawn_accounts:bool=true) -> void:
	list_data = {
		"program":program,
		"acc_key": account_keyword,
		#"id_key":identifier_keyword,
		"filter":filter,
		"page_limit":page_limit
	}
#	resetting list resets page to 0 as well
	curr_page = 0
	
	if spawn_accounts:
		refresh_account_list()
		
func set_sort_data(sort_keyword:String,ascending:bool=true,max_entries:int=-1) -> void:
	sort_data = {
		"sort_key":sort_keyword,
		"ascending": ascending,
		"max_entries":max_entries,
	}
	
func add_filter_parameter(filter_keyword:String,value_to_check_against,check_type:String) -> void:
	filter_data[filter_keyword] = {
		"checkType":check_type,
		"value":value_to_check_against,
	}
	
func remove_filter_parameter(filter_keyword:String) -> void:
	if filter_data.has(filter_keyword):
		filter_data.erase(filter_keyword)
		
func clear_all_filters() -> void:
	filter_data.clear()
	
	
func refresh_account_list(fetch_new:bool=true) -> void:
	if list_data.size() == 0:
		return
		
	if curr_page_container!=null and page_loading:
		curr_page_container.visible=false
	
	if refresh_button!=null:
		refresh_button.disabled=true
	
	if fetch_new or raw_accounts.size()==0:
		raw_accounts = await SolanaService.fetch_all_program_accounts_of_type(list_data["program"],list_data["acc_key"],list_data["filter"])
	
	if raw_accounts.size() == 0:
		if curr_page_container!=null and !page_loading:
			curr_page_container.queue_free()
		if no_entries_overlay!=null:
			no_entries_overlay.visible=true
		if refresh_button!=null:
			refresh_button.disabled=false
		return
		
	var filtered_accounts:Dictionary = raw_accounts
	if filter_data.size()>0:
		filtered_accounts = filter_accounts(raw_accounts)
		
	var sorted_accounts:Dictionary = filtered_accounts
	if sort_data.size()>0:
		sorted_accounts = sort_accounts(filtered_accounts)
		
	paged_accounts = page_accounts(sorted_accounts,list_data["page_limit"])
	if curr_page>paged_accounts.size():
		curr_page = paged_accounts.size()-1
	if page_loader!=null:
		page_loader.setup(curr_page,paged_accounts.size())
			
	await load_page(curr_page)
	
	if refresh_button!=null:
		refresh_button.disabled=false
	
		
func load_page(page_id:int) -> void:
	if page_loading:
		curr_page_container.visible=false
	elif curr_page_container != null:
		curr_page_container.queue_free()

	if paged_accounts.size()==0:
		return
		
	var page_container = container.duplicate()
	container.get_parent().add_child(page_container)
	curr_page_container = page_container
	
	if refresh_button!=null:
		refresh_button.disabled=true
	if page_loader!=null:
		page_loader.freeze_input(true)
		
	page_loading=true
	on_page_load_started.emit()
		
	var account_idx:int=page_id*list_data["page_limit"]
	for key in paged_accounts[page_id].duplicate():	
		if curr_page_container != page_container:
#			stop loading if new page started loading while this was in the middle
			break
					
		var data:Dictionary = paged_accounts[page_id][key]
		#var identifier = data[list_data["id_key"]]
		##identifier needs to be a string to give it to label. if packed byte array, then convert to string		
		##check against array variants: https://docs.godotengine.org/en/stable/classes/class_@globalscope.html#enum-globalscope-variant-type			
		#if identifier is Pubkey:
			#identifier = identifier.to_string()
		#if typeof(identifier) in [28,29,30,31,32,33]:
			#var identifier_bytes:PackedByteArray = identifier as PackedByteArray
			#identifier = identifier_bytes.get_string_from_utf8()
		#elif typeof(identifier) != TYPE_STRING:
			#identifier = str(identifier)
			
		var account:AccountDisplayEntry = await spawn_account(key,data,account_idx,page_container)
		if curr_page_container != page_container:
#			stop loading if new page started loading while this was in the middle
			break
		on_account_added.emit(account)
		account_idx+=1	
		
	if curr_page_container != page_container:
		page_container.queue_free()
		return
	
	if page_loader!=null:
		page_loader.freeze_input(false)
	if refresh_button!=null:
		refresh_button.disabled=false
		
	curr_page = page_id
	page_loading=false
	on_page_load_finished.emit()
		
func filter_accounts(accounts:Dictionary) -> Dictionary:
	var filtered_accounts:Dictionary = accounts.duplicate()
	#var filtered_accounts:Dictionary
	for filter_key in filter_data.keys():
		var check_type:String = filter_data[filter_key]["checkType"]
		var filter_compare_value = filter_data[filter_key]["value"]
		
		for key in filtered_accounts.keys().duplicate():
			var account_filter_value = get_dict_value_from_path(accounts[key],filter_key)
			var pass_filter:bool
			match check_type:
				"equals":
					pass_filter = (account_filter_value == filter_compare_value)
				"notEquals":
					pass_filter = (account_filter_value != filter_compare_value)
				"higher":
					pass_filter = (account_filter_value > filter_compare_value)
				"lower":
					pass_filter = (account_filter_value < filter_compare_value)
			
			# Remove account if it fails a required filter or any filter (depending on logic)
			if not pass_filter:
				filtered_accounts.erase(key)
				
	return filtered_accounts

func sort_accounts(accounts:Dictionary) -> Dictionary:
	var sorted_keys:Array = accounts.keys()
	if sort_data["ascending"]:
		sorted_keys.sort_custom(func(a, b): 
			#return accounts[a][sort_data["sort_key"]] > accounts[b][sort_data["sort_key"]]
			return get_dict_value_from_path(accounts[a],sort_data["sort_key"]) > get_dict_value_from_path(accounts[b],sort_data["sort_key"])
		)
	else:
		sorted_keys.sort_custom(func(a, b): 
			#return accounts[a][sort_data["sort_key"]] < accounts[b][sort_data["sort_key"]]
			return get_dict_value_from_path(accounts[a],sort_data["sort_key"]) < get_dict_value_from_path(accounts[b],sort_data["sort_key"])
		)
		
	if sort_data["max_entries"] > 0:
		sorted_keys = sorted_keys.slice(0, min(sort_data["max_entries"], sorted_keys.size()))
	
	var sorted_accounts:Dictionary
	for key in sorted_keys:
		sorted_accounts[key] = accounts[key]
		
	return sorted_accounts
	
func page_accounts(accounts_list:Dictionary, page_limit:int) -> Array:
	var paged_account_array:Array[Dictionary]
	
	var accounts_in_page:Dictionary
	for key in accounts_list.keys():
		accounts_in_page[key] = accounts_list[key]
		if accounts_in_page.size() == page_limit:
			#print(accounts_in_page)
			paged_account_array.append(accounts_in_page.duplicate())
			accounts_in_page.clear()
#	if there are still a couple of entries left that didnt make a full page, add them in to one last page
	if accounts_in_page.size()>0:
		paged_account_array.append(accounts_in_page.duplicate())
		accounts_in_page.clear()

	return paged_account_array
		

func spawn_account(account_name:String,account_data:Dictionary,id:int,spawn_place:Container) -> AccountDisplayEntry:
	if no_entries_overlay!=null:
		no_entries_overlay.visible=false
		
	var entry_instance:AccountDisplayEntry = entry_scn.instantiate() as AccountDisplayEntry
	
	spawn_place.add_child(entry_instance)
	if bring_new_to_back:
		spawn_place.move_child(entry_instance,0)
	
	await entry_instance.setup_account_entry(account_name,account_data,id)
	entry_instance.on_selected.connect(handle_press)
	return entry_instance
	
func handle_press(selected_account:AccountDisplayEntry) -> void:
	on_account_selected.emit(selected_account)
	
func get_dict_value_from_path(dictionary, sort_key_path:String):
	var keys = sort_key_path.split(".")
	var value = dictionary
	for key in keys:
		if value.has(key):
			value = value[key]
		else:
			print("Failed to find a sort parameter in the program account")
			return null  # Return null if the path doesn't exist
	return value
