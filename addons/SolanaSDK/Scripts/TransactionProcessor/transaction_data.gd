extends Node
class_name TransactionData

var data:Dictionary

func _init(raw_data:Dictionary) -> void:
	data = raw_data
	
func is_successful():
	return data.size() != 0 && data.has("result")
	
func get_link(inspect_type:AccountInspector.InspectSite = AccountInspector.InspectSite.EXPLORER) -> String:
	if !is_successful():
		push_error("Trying to fetch link of a failed transaction!")
		return ""

	return SolanaService.account_inspector.get_tx_link(data["result"],inspect_type)
	
func get_error_message(full:bool=true) -> String:
	if data.size() == 0:
		return "Failed to send the transaction, please try again..."
		
	var error_message:String = "Error %s: %s" % [data["error"]["code"],data["error"]["message"]]
	if full:
		error_message += "\nTx Logs: %s" % data["error"]["logs"]
	return error_message
