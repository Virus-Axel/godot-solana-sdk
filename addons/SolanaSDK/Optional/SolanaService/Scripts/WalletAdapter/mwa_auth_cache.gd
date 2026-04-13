extends RefCounted
class_name MWAAuthCache

## Base class for persisting and loading MWA authorization tokens.
##
## Implementations must store enough data to later associate the token with an account address.

func save_token(auth_token: String, account_address: String) -> void:
	push_warning("MWAAuthCache.save_token not implemented")

func load_token() -> Dictionary:
	return {}

func clear_token() -> void:
	push_warning("MWAAuthCache.clear_token not implemented")

func has_valid_token() -> bool:
	var loaded := load_token()
	return loaded.has("auth_token") and typeof(loaded["auth_token"]) == TYPE_STRING and loaded["auth_token"] != ""

