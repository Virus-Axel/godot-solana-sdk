extends Control

func check_node_method(node: Node, method: String):
	var arg_count: int = node.get_method_argument_count(method)
	var arg_array = Array()
	arg_array.resize(arg_count)
	
	arg_array.fill(null)
	print_debug("Calling ", method, " in class ", node.get_class())
	node.callv(method, arg_array)
	
	arg_array.fill(Keypair.new_random())
	for i in range(arg_count):
		arg_array[i] = null
		print_debug("Calling ", method, " in class ", node.get_class())
		node.callv(method, arg_array)
		arg_array[i] = Keypair.new_random()


func check_all_node_methods(node: Node):
	var methods = node.get_method_list()
	for method in methods:
		if method["name"] != "free":
			check_node_method(node, method["name"])


func _ready() -> void:
	for child in get_children():
		check_all_node_methods(child)
	print("PASS")
