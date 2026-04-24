# Story 2-1 T10 — CR-32 backstop for the NoOp-bridge (non-Android) path.
#
# A-13 retired the C++ host-mode GoogleTest tier that would have covered
# AC-3's UNSUPPORTED_PLATFORM assertion. CR-32 tracked the replacement; this
# GDScript-level test is the replacement — runs on desktop (Linux/macOS) via
# headless Godot, drives the MWA facade, and asserts the NoOp bridge path
# still fires the correct mwa_error within 1 frame.
#
# Harness: a root-Node script instantiated by `test_unsupported_platform.tscn`
# as the main scene. On success emits "ALL TESTS PASSED" to stdout so the
# workflow grep (run_module_test.yml pattern) succeeds. On failure emits
# "FAIL:" lines to stderr and exits non-zero.
extends Node

const TEST_TAG := "[CR-32]"
const TIMEOUT_FRAMES := 10  # 1 frame per story spec; allow 10 for CI slack.

var _error_received := false
var _error_payload: Dictionary = {}


func _ready() -> void:
	# MWA autoload must be registered by the host project's [autoload] block
	# (example/WalletAdapterAndroid/project.godot pre-registers MWA per T7).
	# If the autoload is missing this is a test-harness configuration error,
	# not an AC-3 regression — surface with a distinct message.
	var mwa: Node = get_node_or_null("/root/MWA")
	if mwa == null:
		printerr("%s FAIL: MWA autoload not registered at /root/MWA — check project.godot" % TEST_TAG)
		get_tree().quit(1)
		return

	# Subscribe BEFORE calling connect — mwa_error fires via call_deferred so
	# it arrives on the next frame; a post-call connect could miss it.
	mwa.mwa_error.connect(_on_mwa_error)

	var identity := {
		"name": "T",
		"icon_uri": "",
		"identity_uri": "https://example.com/cr-32",
	}
	# @warning_ignore is applied in MWA.gd itself — this call is fine here.
	@warning_ignore("return_value_discarded")
	mwa.connect(identity, "devnet")

	# Wait up to TIMEOUT_FRAMES frames for the deferred emit to land.
	for _i in range(TIMEOUT_FRAMES):
		await get_tree().process_frame
		if _error_received:
			break

	if not _error_received:
		printerr("%s FAIL: mwa_error did not fire within %d frames on desktop NoOp bridge" %
			[TEST_TAG, TIMEOUT_FRAMES])
		get_tree().quit(1)
		return

	var code: String = _error_payload.get("code", "")
	if code != "UNSUPPORTED_PLATFORM":
		printerr("%s FAIL: expected code=UNSUPPORTED_PLATFORM, got code=%s (full payload: %s)" %
			[TEST_TAG, code, _error_payload])
		get_tree().quit(1)
		return

	# AC-3 breadcrumb: arch §3.2 + A-14 want a 10-key envelope. Assert the
	# canonical keys are present so a regression that drops one is caught.
	var required_keys := [
		"request_id", "code", "message", "user_message", "developer_details",
		"recoverable", "retry_hint", "layer", "cause", "source_method",
	]
	for k in required_keys:
		if not _error_payload.has(k):
			printerr("%s FAIL: mwa_error payload missing required key '%s' (A-14 10-key shape)" %
				[TEST_TAG, k])
			get_tree().quit(1)
			return

	# Extra breadcrumb checks per CR-32's test scope.
	if _error_payload.get("layer", "") != "cpp":
		printerr("%s FAIL: expected layer=cpp (NoOp bridge path), got layer=%s" %
			[TEST_TAG, _error_payload.get("layer", "")])
		get_tree().quit(1)
		return
	if _error_payload.get("recoverable", true) != false:
		printerr("%s FAIL: expected recoverable=false on UNSUPPORTED_PLATFORM" % TEST_TAG)
		get_tree().quit(1)
		return

	print("%s PASS: mwa_error{code=UNSUPPORTED_PLATFORM, layer=cpp, recoverable=false}" % TEST_TAG)
	print("ALL TESTS PASSED")
	get_tree().quit(0)


func _on_mwa_error(payload: Dictionary) -> void:
	_error_received = true
	_error_payload = payload
