# Story 2-1 T10 — CR-32 backstop for the NoOp-bridge (non-Android) path.
# Story 2-3 T6 — extended to cover the disconnect flow's UNSUPPORTED_PLATFORM
# emission as well, per concern C6.
#
# A-13 retired the C++ host-mode GoogleTest tier that would have covered
# AC-3's UNSUPPORTED_PLATFORM assertion. CR-32 tracked the replacement; this
# GDScript-level test is the replacement — runs on desktop (Linux/macOS) via
# headless Godot, drives the MWA facade for BOTH connect and disconnect
# scenarios, and asserts the NoOp bridge path fires the correct mwa_error
# within a bounded number of frames in each case.
#
# C6 (Story 2-3): on desktop, MWA.disconnect() MUST emit
# mwa_error{code=UNSUPPORTED_PLATFORM, source_method="disconnect"} and MUST
# NOT emit disconnect_completed — there is no MwaSessionState on desktop
# (Kotlin-only class); the C++ MobileWalletAdapter node routes through the
# NoOpMwaAndroidBridge short-circuit at mobile_wallet_adapter.cpp:166-169.
#
# Harness: a root-Node script instantiated by `test_unsupported_platform.tscn`
# as the main scene. On overall success emits "ALL TESTS PASSED" to stdout so
# the workflow grep (run_module_test.yml pattern) succeeds. On any scenario
# failure emits "FAIL:" lines to stderr and exits non-zero.
extends Node

const TEST_TAG := "[CR-32]"
const TIMEOUT_FRAMES := 10  # 1 frame per story spec; allow 10 for CI slack.

# Per-scenario capture. Reset between scenarios via `_reset_capture()`.
var _error_received := false
var _error_payload: Dictionary = {}
# Story 2-3 T6: disconnect scenario additionally asserts disconnect_completed
# MUST NOT fire on desktop — track emission count independently.
var _disconnect_completed_count := 0


func _ready() -> void:
	# MWA autoload must be registered by the host project's [autoload] block
	# (example/WalletAdapterAndroid/project.godot pre-registers MWA per T7).
	# If the autoload is missing this is a test-harness configuration error,
	# not an AC-3/C6 regression — surface with a distinct message.
	var mwa: Node = get_node_or_null("/root/MWA")
	if mwa == null:
		printerr("%s FAIL: MWA autoload not registered at /root/MWA — check project.godot" % TEST_TAG)
		get_tree().quit(1)
		return

	# Subscribe BEFORE calling any MWA op — mwa_error fires via call_deferred so
	# it arrives on the next frame; a post-call connect could miss it.
	# Both scenarios share the error sink; `_reset_capture()` clears state
	# between phases.
	mwa.mwa_error.connect(_on_mwa_error)
	mwa.disconnect_completed.connect(_on_disconnect_completed)

	# Scenario 1 — connect on desktop emits UNSUPPORTED_PLATFORM (Story 2-1 T10).
	if not await _run_connect_scenario(mwa):
		get_tree().quit(1)
		return

	# Scenario 2 — disconnect on desktop emits UNSUPPORTED_PLATFORM (Story 2-3 T6 / C6).
	if not await _run_disconnect_scenario(mwa):
		get_tree().quit(1)
		return

	print("ALL TESTS PASSED")
	get_tree().quit(0)


func _reset_capture() -> void:
	_error_received = false
	_error_payload = {}
	_disconnect_completed_count = 0


func _on_mwa_error(payload: Dictionary) -> void:
	_error_received = true
	_error_payload = payload


func _on_disconnect_completed(_request_id: String, _result: Dictionary) -> void:
	_disconnect_completed_count += 1


# Asserts the captured payload matches the UNSUPPORTED_PLATFORM shape for a
# given source_method. Returns true on pass, false on fail (with printerr).
func _assert_unsupported_platform_payload(source_method: String) -> bool:
	var code: String = _error_payload.get("code", "")
	if code != "UNSUPPORTED_PLATFORM":
		printerr("%s FAIL [%s]: expected code=UNSUPPORTED_PLATFORM, got code=%s (full payload: %s)" %
			[TEST_TAG, source_method, code, _error_payload])
		return false

	var sm: String = _error_payload.get("source_method", "")
	if sm != source_method:
		printerr("%s FAIL [%s]: expected source_method=%s, got source_method=%s" %
			[TEST_TAG, source_method, source_method, sm])
		return false

	# AC-3 breadcrumb: arch §3.2 + A-14 want a 10-key envelope. Assert the
	# canonical keys are present so a regression that drops one is caught.
	var required_keys := [
		"request_id", "code", "message", "user_message", "developer_details",
		"recoverable", "retry_hint", "layer", "cause", "source_method",
	]
	for k in required_keys:
		if not _error_payload.has(k):
			printerr("%s FAIL [%s]: mwa_error payload missing required key '%s' (A-14 10-key shape)" %
				[TEST_TAG, source_method, k])
			return false

	# Extra breadcrumb checks per CR-32's test scope.
	if _error_payload.get("layer", "") != "cpp":
		printerr("%s FAIL [%s]: expected layer=cpp (NoOp bridge path), got layer=%s" %
			[TEST_TAG, source_method, _error_payload.get("layer", "")])
		return false
	if _error_payload.get("recoverable", true) != false:
		printerr("%s FAIL [%s]: expected recoverable=false on UNSUPPORTED_PLATFORM" %
			[TEST_TAG, source_method])
		return false

	return true


func _run_connect_scenario(mwa: Node) -> bool:
	_reset_capture()

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
		printerr("%s FAIL [connect]: mwa_error did not fire within %d frames on desktop NoOp bridge" %
			[TEST_TAG, TIMEOUT_FRAMES])
		return false

	if not _assert_unsupported_platform_payload("connect"):
		return false

	print("%s PASS [connect]: mwa_error{code=UNSUPPORTED_PLATFORM, layer=cpp, recoverable=false}" % TEST_TAG)
	return true


# Story 2-3 T6 / concern C6: on desktop, MWA.disconnect() routes through the
# C++ mwa_disconnect null-bridge branch and emits mwa_error — it MUST NOT emit
# disconnect_completed (which is Android-path-only, Kotlin-driven).
func _run_disconnect_scenario(mwa: Node) -> bool:
	_reset_capture()

	# @warning_ignore("shadowed_global_identifier") is applied in MWA.gd at the
	# `func disconnect() -> void:` declaration (D-10); caller side needs no
	# annotation because `disconnect()` on a Node is a regular method call.
	mwa.disconnect()

	# Wait up to TIMEOUT_FRAMES for the deferred mwa_error.
	for _i in range(TIMEOUT_FRAMES):
		await get_tree().process_frame
		if _error_received:
			break

	if not _error_received:
		printerr("%s FAIL [disconnect]: mwa_error did not fire within %d frames on desktop NoOp bridge" %
			[TEST_TAG, TIMEOUT_FRAMES])
		return false

	if not _assert_unsupported_platform_payload("disconnect"):
		return false

	# C6 critical invariant: disconnect_completed MUST NOT fire on desktop.
	# If it does, the plugin either leaked cross-platform (Android-only state
	# logic ran on desktop) or the dispatcher double-posted.
	if _disconnect_completed_count != 0:
		printerr("%s FAIL [disconnect]: disconnect_completed fired %d times on desktop — expected 0 (C6)" %
			[TEST_TAG, _disconnect_completed_count])
		return false

	print("%s PASS [disconnect]: mwa_error{code=UNSUPPORTED_PLATFORM, source_method=disconnect, layer=cpp}" % TEST_TAG)
	return true
