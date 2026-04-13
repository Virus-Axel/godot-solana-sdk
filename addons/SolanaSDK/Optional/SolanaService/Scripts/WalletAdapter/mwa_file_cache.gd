extends MWAAuthCache
class_name MWAFileCache

const CACHE_PATH := "user://mwa_auth_cache.cfg"
const SECTION := "auth"

func save_token(auth_token: String, account_address: String) -> void:
	var cfg := ConfigFile.new()
	var err := cfg.load(CACHE_PATH)
	if err != OK:
		# Start fresh if the file doesn't exist yet.
		cfg = ConfigFile.new()

	cfg.set_value(SECTION, "auth_token", auth_token)
	cfg.set_value(SECTION, "account_address", account_address)
	cfg.save(CACHE_PATH)

func load_token() -> Dictionary:
	var cfg := ConfigFile.new()
	var err := cfg.load(CACHE_PATH)
	if err != OK:
		return {}

	var token := String(cfg.get_value(SECTION, "auth_token", ""))
	var address := String(cfg.get_value(SECTION, "account_address", ""))

	if token == "" or address == "":
		return {}

	return {
		"auth_token": token,
		"account_address": address,
	}

func clear_token() -> void:
	if FileAccess.file_exists(CACHE_PATH):
		DirAccess.remove_absolute(CACHE_PATH)

func has_valid_token() -> bool:
	var loaded := load_token()
	return loaded.has("auth_token") and loaded["auth_token"] != ""

