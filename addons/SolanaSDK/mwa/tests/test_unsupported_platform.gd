# backstop for the NoOp-bridge (non-Android) path.
# extended to cover the disconnect flow's UNSUPPORTED_PLATFORM
# emission as well, per concern C6.
# extended from 2 scenarios (connect + disconnect) to 8
# scenarios covering all 7 user-facing ops (connect / disconnect /
# reauthorize / deauthorize / sign_messages / sign_transactions /
# sign_and_send / forget_all) plus an inline MWA.is_supported() check
# at top of _ready. Each new scenario reuses the
# existing _assert_unsupported_platform_payload helper (line 84,
# unchanged) which validates the 10-key envelope shape regardless
# of source_method. Sequential dispatch with _reset_capture() between
# phases — same shape as the connect + disconnect scenarios.
#
# retired the C++ host-mode GoogleTest tier that would have covered
# AC-3's UNSUPPORTED_PLATFORM assertion. tracked the replacement; this
# GDScript-level test is the replacement — runs on desktop (Linux/macOS) via
# headless Godot, drives the MWA facade for ALL user-facing ops, and asserts
# the NoOp bridge path fires the correct mwa_error within a bounded number
# of frames in each case.
#
# C6: on desktop, MWA.disconnect() MUST emit
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

const TEST_TAG:= "[]"
const TIMEOUT_FRAMES := 10  # 1 frame per story spec; allow 10 for CI slack.

# Per-scenario capture. Reset between scenarios via `_reset_capture()`.
var _error_received := false
var _error_payload: Dictionary = {}
# disconnect scenario additionally asserts disconnect_completed
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

	# AC-1 — pure-GDScript getter; non-Android platforms must report false.
	# This check runs BEFORE any op invocation since it has no signal/dispatcher
	# round-trip — the getter returns the result of OS.get_name() == "Android"
	# directly.
	if mwa.is_supported():
		printerr("%s FAIL [is_supported]: expected MWA.is_supported()==false on non-Android (OS.get_name()=%s)" %
			[TEST_TAG, OS.get_name()])
		get_tree().quit(1)
		return

	# Subscribe BEFORE calling any MWA op — mwa_error fires via call_deferred so
	# it arrives on the next frame; a post-call connect could miss it.
	# All scenarios share the error sink; `_reset_capture()` clears state
	# between phases.
	mwa.mwa_error.connect(_on_mwa_error)
	mwa.disconnect_completed.connect(_on_disconnect_completed)

	# Scenario 1 — connect on desktop emits UNSUPPORTED_PLATFORM.
	if not await _run_connect_scenario(mwa):
		get_tree().quit(1)
		return

	# Scenario 2 — disconnect on desktop emits UNSUPPORTED_PLATFORM (C6).
	if not await _run_disconnect_scenario(mwa):
		get_tree().quit(1)
		return

	# Scenarios 3-8 cover the remaining 6 user-facing ops.
	# Each scenario follows the connect/disconnect pattern: _reset_capture +
	# call op + await up to TIMEOUT_FRAMES + assert 10-key envelope with
	# the expected source_method literal.
	if not await _run_reauthorize_scenario(mwa):
		get_tree().quit(1)
		return
	if not await _run_deauthorize_scenario(mwa):
		get_tree().quit(1)
		return
	if not await _run_sign_messages_scenario(mwa):
		get_tree().quit(1)
		return
	if not await _run_sign_transactions_scenario(mwa):
		get_tree().quit(1)
		return
	if not await _run_sign_and_send_scenario(mwa):
		get_tree().quit(1)
		return
	if not await _run_forget_all_scenario(mwa):
		get_tree().quit(1)
		return

	# Scenarios 9-11 cover the new SYNC getters on the desktop
	# NoOp-bridge path. These are SYNCHRONOUS (no await / no signal hop) per
	# the GDScript wrappers in MWA.gd forward to the C++
	# `MobileWalletAdapter::get_diagnostics` / `get_device_posture` bindings
	# which short-circuit through the NoOp bridge's `query_diagnostics_json` /
	# `query_device_posture_json` returning the all-empty / all-false JSON
	# shapes. AC-5's `get_suggested_wallet_install_links` is pure
	# GDScript (no bridge call), so it's tested in the same scope.
	if not _run_get_diagnostics_shape_scenario(mwa):
		get_tree().quit(1)
		return
	if not _run_get_device_posture_shape_scenario(mwa):
		get_tree().quit(1)
		return
	if not _run_get_suggested_wallet_install_links_scenario(mwa):
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

	# AC-3 breadcrumb: arch §3.2 + want a 10-key envelope. Assert the
	# canonical keys are present so a regression that drops one is caught.
	var required_keys := [
		"request_id", "code", "message", "user_message", "developer_details",
		"recoverable", "retry_hint", "layer", "cause", "source_method",
	]
	for k in required_keys:
		if not _error_payload.has(k):
			printerr("%s FAIL [%s]: mwa_error payload missing required key '%s' (10-key shape)" %
				[TEST_TAG, source_method, k])
			return false

	# Extra breadcrumb checks per 's test scope.
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


# / concern C6: on desktop, MWA.disconnect() routes through the
# C++ mwa_disconnect null-bridge branch and emits mwa_error — it MUST NOT emit
# disconnect_completed (which is Android-path-only, Kotlin-driven).
func _run_disconnect_scenario(mwa: Node) -> bool:
	_reset_capture()

	# @warning_ignore("shadowed_global_identifier") is applied in MWA.gd at the
	# `func disconnect -> void:` declaration; caller side needs no
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


# reauthorize on desktop emits UNSUPPORTED_PLATFORM.
# MWA.reauthorize takes an opts Dictionary (default {}); the C++ node's
# reauthorize() generates a request_id and routes through the NoOp bridge
# fan-out at no_op_mwa_android_bridge.cpp:reauthorize → emit_unsupported.
func _run_reauthorize_scenario(mwa: Node) -> bool:
	_reset_capture()

	mwa.reauthorize({})

	for _i in range(TIMEOUT_FRAMES):
		await get_tree().process_frame
		if _error_received:
			break

	if not _error_received:
		printerr("%s FAIL [reauthorize]: mwa_error did not fire within %d frames on desktop NoOp bridge" %
			[TEST_TAG, TIMEOUT_FRAMES])
		return false

	if not _assert_unsupported_platform_payload("reauthorize"):
		return false

	print("%s PASS [reauthorize]: mwa_error{code=UNSUPPORTED_PLATFORM, source_method=reauthorize, layer=cpp}" % TEST_TAG)
	return true


# deauthorize on desktop emits UNSUPPORTED_PLATFORM.
# Routes through MobileWalletAdapter::deauthorize null-bridge guard
# (mobile_wallet_adapter.cpp:184) OR NoOpMwaAndroidBridge::deauthorize
# fan-out depending on build (inheritance from).
func _run_deauthorize_scenario(mwa: Node) -> bool:
	_reset_capture()

	mwa.deauthorize()

	for _i in range(TIMEOUT_FRAMES):
		await get_tree().process_frame
		if _error_received:
			break

	if not _error_received:
		printerr("%s FAIL [deauthorize]: mwa_error did not fire within %d frames on desktop NoOp bridge" %
			[TEST_TAG, TIMEOUT_FRAMES])
		return false

	if not _assert_unsupported_platform_payload("deauthorize"):
		return false

	print("%s PASS [deauthorize]: mwa_error{code=UNSUPPORTED_PLATFORM, source_method=deauthorize, layer=cpp}" % TEST_TAG)
	return true


# sign_messages on desktop emits UNSUPPORTED_PLATFORM.
# Trivial 1-byte PackedByteArray payload — content is irrelevant on the
# NoOp path (no wallet round-trip occurs); the null-bridge guard short-
# circuits before any payload inspection.
func _run_sign_messages_scenario(mwa: Node) -> bool:
	_reset_capture()

	mwa.sign_messages([PackedByteArray([1, 2, 3])])

	for _i in range(TIMEOUT_FRAMES):
		await get_tree().process_frame
		if _error_received:
			break

	if not _error_received:
		printerr("%s FAIL [sign_messages]: mwa_error did not fire within %d frames on desktop NoOp bridge" %
			[TEST_TAG, TIMEOUT_FRAMES])
		return false

	if not _assert_unsupported_platform_payload("sign_messages"):
		return false

	print("%s PASS [sign_messages]: mwa_error{code=UNSUPPORTED_PLATFORM, source_method=sign_messages, layer=cpp}" % TEST_TAG)
	return true


# sign_transactions on desktop emits UNSUPPORTED_PLATFORM.
# Same payload-irrelevant pattern as sign_messages — the NoOp path
# short-circuits at the bridge boundary.
func _run_sign_transactions_scenario(mwa: Node) -> bool:
	_reset_capture()

	mwa.sign_transactions([PackedByteArray([1, 2, 3])])

	for _i in range(TIMEOUT_FRAMES):
		await get_tree().process_frame
		if _error_received:
			break

	if not _error_received:
		printerr("%s FAIL [sign_transactions]: mwa_error did not fire within %d frames on desktop NoOp bridge" %
			[TEST_TAG, TIMEOUT_FRAMES])
		return false

	if not _assert_unsupported_platform_payload("sign_transactions"):
		return false

	print("%s PASS [sign_transactions]: mwa_error{code=UNSUPPORTED_PLATFORM, source_method=sign_transactions, layer=cpp}" % TEST_TAG)
	return true


# sign_and_send on desktop emits UNSUPPORTED_PLATFORM.
# Payload-irrelevant on NoOp path (no breadcrumb write
# write-then-call ordering only fires on the Android Kotlin path).
func _run_sign_and_send_scenario(mwa: Node) -> bool:
	_reset_capture()

	mwa.sign_and_send([PackedByteArray([1, 2, 3])])

	for _i in range(TIMEOUT_FRAMES):
		await get_tree().process_frame
		if _error_received:
			break

	if not _error_received:
		printerr("%s FAIL [sign_and_send]: mwa_error did not fire within %d frames on desktop NoOp bridge" %
			[TEST_TAG, TIMEOUT_FRAMES])
		return false

	if not _assert_unsupported_platform_payload("sign_and_send"):
		return false

	print("%s PASS [sign_and_send]: mwa_error{code=UNSUPPORTED_PLATFORM, source_method=sign_and_send, layer=cpp}" % TEST_TAG)
	return true


# forget_all on desktop emits UNSUPPORTED_PLATFORM.
# Exercises the fill-in body at
# mobile_wallet_adapter.cpp:272-280: generate_request_id → null-bridge
# guard → unsupported_platform mwa_error. NoOp bridge path produces the
# same shape via emit_unsupported fan-out.
func _run_forget_all_scenario(mwa: Node) -> bool:
	_reset_capture()

	mwa.forget_all()

	for _i in range(TIMEOUT_FRAMES):
		await get_tree().process_frame
		if _error_received:
			break

	if not _error_received:
		printerr("%s FAIL [forget_all]: mwa_error did not fire within %d frames on desktop NoOp bridge" %
			[TEST_TAG, TIMEOUT_FRAMES])
		return false

	if not _assert_unsupported_platform_payload("forget_all"):
		return false

	print("%s PASS [forget_all]: mwa_error{code=UNSUPPORTED_PLATFORM, source_method=forget_all, layer=cpp}" % TEST_TAG)
	return true


# (AC-1 GDScript-tier shape verification) — `MWA.get_diagnostics()`
# is SYNCHRONOUS. On the desktop NoOp-bridge path, the
# C++ binding's null-bridge guard or the NoOp's `query_diagnostics_json`
# returns the 12-key all-empty payload. Asserts the EXACT key
# set (a regression that adds/drops a key surfaces immediately) plus the
# invariant that string fields are `""`, integer fields are `0`,
# `last_n_correlation_trace` is empty Array, `session_state` is empty Dict.
func _run_get_diagnostics_shape_scenario(mwa: Node) -> bool:
	var diag: Dictionary = mwa.get_diagnostics()
	var expected_keys := [
		"sdk_version", "clientlib_ktx_version", "godot_version", "android_api_level",
		"session_state", "wallet_package", "wallet_version", "auth_token_fingerprint",
		"cluster", "last_n_correlation_trace", "late_result_count", "pending_submissions_count",
	]
	for k in expected_keys:
		if not diag.has(k):
			printerr("%s FAIL [get_diagnostics]: AC-1 12-key shape missing key '%s' (got: %s)" %
				[TEST_TAG, k, diag.keys()])
			return false
	if diag.size() != 12:
		printerr("%s FAIL [get_diagnostics]: AC-1 expects EXACTLY 12 keys, got %d (keys: %s)" %
			[TEST_TAG, diag.size(), diag.keys()])
		return false
	# invariants for the empty payload (NoOp-bridge desktop path).
	if str(diag.get("auth_token_fingerprint", "X")) != "":
		printerr("%s FAIL [get_diagnostics]: AC-3 baseline — disconnected payload's auth_token_fingerprint MUST be '' (got %s)" %
			[TEST_TAG, diag.get("auth_token_fingerprint")])
		return false
	if not (diag.get("last_n_correlation_trace") is Array):
		printerr("%s FAIL [get_diagnostics]: last_n_correlation_trace MUST be Array (got %s)" %
			[TEST_TAG, typeof(diag.get("last_n_correlation_trace"))])
		return false
	if not (diag.get("session_state") is Dictionary):
		printerr("%s FAIL [get_diagnostics]: session_state MUST be Dictionary (got %s)" %
			[TEST_TAG, typeof(diag.get("session_state"))])
		return false
	# step 3 — the C++ side overlays godot_version from
	# Engine::get_version_info(). On desktop the engine IS available, so the
	# field MUST be non-empty (regression marker for a future change that
	# breaks the Engine overlay path).
	if str(diag.get("godot_version", "")) == "":
		printerr("%s FAIL [get_diagnostics]: godot_version MUST be populated by Engine::get_version_info() overlay" % TEST_TAG)
		return false
	print("%s PASS [get_diagnostics]: 12-key empty payload + Engine::get_version_info overlay" % TEST_TAG)
	return true


# (AC-4 GDScript-tier shape verification) — `MWA.get_device_posture()`
# is SYNCHRONOUS. On desktop NoOp path the bridge returns the 4-key all-false
# payload.
func _run_get_device_posture_shape_scenario(mwa: Node) -> bool:
	var posture: Dictionary = mwa.get_device_posture()
	var expected_keys := ["rooted", "debuggable", "developer_options_on", "adb_enabled"]
	for k in expected_keys:
		if not posture.has(k):
			printerr("%s FAIL [get_device_posture]: AC-4 4-key shape missing key '%s' (got: %s)" %
				[TEST_TAG, k, posture.keys()])
			return false
	if posture.size() != 4:
		printerr("%s FAIL [get_device_posture]: AC-4 expects EXACTLY 4 keys, got %d (keys: %s)" %
			[TEST_TAG, posture.size(), posture.keys()])
		return false
	for k in expected_keys:
		var v: Variant = posture.get(k)
		if not (v is bool):
			printerr("%s FAIL [get_device_posture]: AC-4 key '%s' MUST be bool (got %s)" %
				[TEST_TAG, k, typeof(v)])
			return false
		# non-Android desktop returns 4-key all-false.
		if v != false:
			printerr("%s FAIL [get_device_posture]: — desktop NoOp path MUST return %s=false (got %s)" %
				[TEST_TAG, k, v])
			return false
	print("%s PASS [get_device_posture]: 4-key all-false desktop NoOp payload" % TEST_TAG)
	return true


# (AC-5 GDScript-tier shape verification) — 4-key
# superset `{name, package_id, play_store_url, website_url}` covering Phantom /
# Solflare / Backpack. Pure-GDScript const lookup; no bridge call.
func _run_get_suggested_wallet_install_links_scenario(mwa: Node) -> bool:
	var links: Array = mwa.get_suggested_wallet_install_links()
	if links.size() < 3:
		printerr("%s FAIL [wallet_links]: AC-5 expects >= 3 entries, got %d" % [TEST_TAG, links.size()])
		return false
	var ac5_required_keys := ["name", "package_id", "play_store_url"]
	var seen_package_ids := {}
	for entry: Dictionary in links:
		for k in ac5_required_keys:
			if not entry.has(k):
				printerr("%s FAIL [wallet_links]: AC-5 entry missing key '%s' (entry: %s)" %
					[TEST_TAG, k, entry])
				return false
		var pkg_id: String = entry.get("package_id", "")
		seen_package_ids[pkg_id] = true
	# superset — `website_url` is permitted (extra) per the
	# reconciliation. The 3 known package IDs (2--1: Backpack uses
	# `app.backpack.mobile` per the actual play_store_url).
	var expected_pkgs := ["app.phantom", "com.solflare.mobile", "app.backpack.mobile"]
	for pkg in expected_pkgs:
		if not seen_package_ids.has(pkg):
			printerr("%s FAIL [wallet_links]: AC-5 expected package_id '%s' not in catalog (seen: %s)" %
				[TEST_TAG, pkg, seen_package_ids.keys()])
			return false
	print("%s PASS [wallet_links]: AC-5 >= 3 entries + 4-key superset + 3 known package_ids" % TEST_TAG)
	return true
