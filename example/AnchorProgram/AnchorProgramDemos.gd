extends VBoxContainer


const TOTAL_CASES := 1
var passed_test_mask := 0
		

func PASS(unique_identifier: int):
	passed_test_mask += (1 << unique_identifier)
	print("[OK]: ", unique_identifier)

func display_dict(data: Variant, parent: TreeItem):
	if(typeof(data) == TYPE_STRING):
		parent.set_text(1, data)
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
	
	$AnchorProgram.set_pid("cndy3Z4yapfJBmL3ShUp5exZKqR3z33thTzeNMm2gRZ")
	$AnchorProgram.set_try_from_pid(true)
	await $AnchorProgram.idl_fetched
	assert(not $AnchorProgram.idl.is_empty())
	
	PASS(0)


func fetch_account_demo():
	$AnchorProgram.fetch_account("CandyMachine")

func _ready():
	await fetch_idl_from_pid_demo()
	

func _on_timeout_timeout():
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)
