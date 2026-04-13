extends MWAAuthCache
class_name MWAMemoryCache

var _auth_token: String = ""
var _account_address: String = ""

func save_token(auth_token: String, account_address: String) -> void:
	_auth_token = auth_token
	_account_address = account_address

func load_token() -> Dictionary:
	if _auth_token == "" or _account_address == "":
		return {}
	return {
		"auth_token": _auth_token,
		"account_address": _account_address,
	}

func clear_token() -> void:
	_auth_token = ""
	_account_address = ""

func has_valid_token() -> bool:
	return _auth_token != "" and _account_address != ""

