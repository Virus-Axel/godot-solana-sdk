extends VBoxContainer


const TOTAL_CASES := 2
var passed_test_mask := 0
		

func PASS(unique_identifier: int):
	passed_test_mask += (1 << unique_identifier)
	print("[OK]: ", unique_identifier)


func display_dict(data: Variant, parent: TreeItem):
	if(typeof(data) == TYPE_STRING):
		parent.set_text(1, data)
	elif(typeof(data) == TYPE_ARRAY):
		for item in data:
			display_dict(item, parent.create_child())
	elif(typeof(data) != TYPE_DICTIONARY):
		parent.set_text(1, str(data))
	else:
		var data_dict: Dictionary = data
		var keys = data_dict.keys()
		var values = data_dict.values()
		for i in range(keys.size()):
			var subchild = parent.create_child()
			subchild.set_editable(1, true)
			subchild.set_text(0, keys[i])
			display_dict(values[i], subchild)


func fetch_idl_from_pid_demo():
	# This should not be done from gdscript.
	# Use the editor properties instead.
	# This function is only for testing purposes.
	
	$AnchorProgram.set_pid("CndyV3LdqHUfDLmE5naZjVN8rBZz4tqhdefbAnjHG3JR")
	$AnchorProgram.set_try_from_pid(true)
	await $AnchorProgram.idl_fetched
	assert(not $AnchorProgram.idl.is_empty())
	display_dict($AnchorProgram.idl, $GridContainer/IdlTree.create_item())
	
	PASS(0)
	
	
func fetch_idl_account_demo():
	var res = $AnchorProgram.fetch_account("CandyMachine", "2kJKEGhqGXJJtoWPfxnKq1Y2bN4eF9GQ39SAcGu8TDZn")
	assert(res == OK)
	var account_data = (await $AnchorProgram.account_fetched)
	assert(not account_data.is_empty())
	display_dict(account_data, $GridContainer/AccountDataTree.create_item())

	PASS(1)


func _ready():
	# Next test case is depending on IDL so await it.
	await fetch_idl_from_pid_demo()
	fetch_idl_account_demo()
	

func _on_timeout_timeout():
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)
