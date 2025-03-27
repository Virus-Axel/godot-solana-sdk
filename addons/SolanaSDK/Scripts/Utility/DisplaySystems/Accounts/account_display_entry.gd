extends Node
class_name AccountDisplayEntry

var account_id:String
var data:Dictionary

signal on_selected(account:AccountDisplayEntry)

func setup_account_entry(id:String,account_data:Dictionary,index:int) -> void:
	account_id = id
	data = account_data
	
func select() -> void:
	on_selected.emit(self)
	
