extends Node

## Simple runtime test for `MWAAuthCache` implementations.
##
## Intended to be run as a Godot scene to verify:
## save -> load -> clear -> empty result.

func _ready() -> void:
	print("[MWA Auth Cache Test] Starting...")

	_test_memory_cache()
	_test_file_cache()

	print("[MWA Auth Cache Test] Passed.")

func _test_memory_cache() -> void:
	var cache := MWAMemoryCache.new()
	cache.clear_token()
	assert(cache.load_token().is_empty())
	assert(cache.has_valid_token() == false)

	cache.save_token("test_token_mem", "test_addr_mem")
	var loaded := cache.load_token()
	assert(loaded["auth_token"] == "test_token_mem")
	assert(loaded["account_address"] == "test_addr_mem")
	assert(cache.has_valid_token() == true)

	cache.clear_token()
	assert(cache.load_token().is_empty())
	assert(cache.has_valid_token() == false)

func _test_file_cache() -> void:
	var cache := MWAFileCache.new()
	cache.clear_token()
	assert(cache.load_token().is_empty())
	assert(cache.has_valid_token() == false)

	cache.save_token("test_token_file", "test_addr_file")
	var loaded := cache.load_token()
	assert(loaded["auth_token"] == "test_token_file")
	assert(loaded["account_address"] == "test_addr_file")
	assert(cache.has_valid_token() == true)

	cache.clear_token()
	assert(cache.load_token().is_empty())
	assert(cache.has_valid_token() == false)

