extends Node
class_name TransactionData

var data:Dictionary

func _init(raw_data:Dictionary, custom_data:Dictionary={}) -> void:
	data = raw_data
	if custom_data.size()>0:
		data["custom_data"] = custom_data
		
func set_custom_data(custom_data:Dictionary) -> void:
	data["custom_data"] = custom_data
	
func is_successful():
	return data.size() != 0 && data.has("result")
	
func get_link(inspect_type:AccountInspector.InspectSite = AccountInspector.InspectSite.EXPLORER) -> String:
	if !is_successful():
		push_error("Trying to fetch link of a failed transaction!")
		return ""

	return SolanaService.account_inspector.get_tx_link(data["result"],inspect_type)
	
func get_error_message(include_logs:bool=true) -> String:
	if data.size() == 0:
		return "Failed to send the transaction, please try again..."
	var error_message:String = "Error %s: %s" % [data["error"]["code"],data["error"]["message"]]
	if include_logs and data["error"].has("data"):
		error_message += "\nTx Logs: %s" % str(data["error"]["data"]["logs"])
	return error_message
