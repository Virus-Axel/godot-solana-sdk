#include "mwa/mobile_wallet_adapter.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <memory>
#include <random>

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "generated/mwa_error_codes.hpp"
#include "mwa_android_bridge.hpp"

namespace {
// Buffer for the 8-hex-char request id + 1 NUL terminator.
constexpr std::size_t REQUEST_ID_BUFFER_SIZE = 9;

// generate exactly 8 lowercase hex chars ([0-9a-f]{8}) per op-method call.
// Matches Kotlin corrId format (UUID.randomUUID().toString().take(8).lowercase()).
// Source pick: std::random_device + std::mt19937 — portable, no scene-tree/engine
// dependency, no godot-cpp gen/ Crypto class requirement. thread_local keeps
// construction cost off the hot path without cross-thread contention.
godot::String generate_request_id() {
	static thread_local std::random_device random_device;
	static thread_local std::mt19937 gen(random_device());
	std::uniform_int_distribution<std::uint32_t> dist(0U, std::numeric_limits<std::uint32_t>::max());
	const std::uint32_t random_value = dist(gen);
	std::array<char, REQUEST_ID_BUFFER_SIZE> buf{};
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg,cert-err33-c) —
	// snprintf is the standard portable formatted-to-buffer call; the return value
	// is the length that would be written (always 8 here for "%08x" of a uint32_t),
	// not a status code that needs checking.
	std::snprintf(buf.data(), buf.size(), "%08x", random_value);
	return godot::String(buf.data());
}

// build the AC-3 mwa_error payload for the null-bridge pre-op branch.
// Shape mirrors NoOpMwaAndroidBridge::emit_unsupported (src/mwa/no_op_mwa_android_bridge.cpp:21-43)
// so consumers see an identical mwa_error{UNSUPPORTED_PLATFORM, ...} envelope regardless of
// which path fired (non-Android NoOp vs. MWA_TESTING null-bridge). See Guardrails
// § + §"Signal payload semantic" (lines 110-111) for key rationale. developer_details
// MUST be exactly "Kotlin plugin not loaded" — T5.3 asserts on this. `message` also carries
// the same string to satisfy AC-3 prose (`message="Kotlin plugin not loaded"`); this key
// matches NoOp's shape which likewise populates `message` at
// src/mwa/no_op_mwa_android_bridge.cpp:25-39 — cross-bridge consistency so consumers keying
// off `message` work identically across both paths. The arch §3.2 canonical payload lists
// only `user_message` + `developer_details`; the `message` key is a NoOp-originated
// extension that this T3 branch preserves rather than drops. Reconciliation (pick a single
// canonical shape across NoOp + T3 + downstream 2-1/5-5 docs) is tracked as a CR in T7
// close-out — NOT resolved here.
godot::Dictionary build_unsupported_platform_payload(const godot::String &request_id,
		const godot::String &source_method) {
	godot::Dictionary payload;
	payload["request_id"] = request_id;
	payload["code"] = godot::String(godot_solana_sdk::mwa::code_name(godot_solana_sdk::mwa::MwaErrorCode::UNSUPPORTED_PLATFORM));
	payload["message"] = godot::String("Kotlin plugin not loaded");
	payload["user_message"] = godot::String("Mobile wallet isn't available on this device.");
	payload["developer_details"] = godot::String("Kotlin plugin not loaded");
	payload["recoverable"] = false;
	payload["retry_hint"] = godot::String("none");
	payload["layer"] = godot::String("cpp");
	payload["cause"] = godot::String("");
	payload["source_method"] = source_method;
	return payload;
}

// 12-key all-empty diagnostics Dictionary returned
// when the bridge is null (MWA_TESTING null-bridge guard). Mirrors the
// JSON-string shape produced by MwaDiagnosticsBuilder.emptyDiagnosticsJson on
// the Kotlin side + the namespace-local build_empty_diagnostics_json on the
// JNI side; here the same 12-key set is emitted directly as a Dictionary so
// the GDScript caller sees a consistent shape regardless of the bridge.
godot::Dictionary build_empty_diagnostics_dict() {
	godot::Dictionary out;
	out["sdk_version"] = godot::String();
	out["clientlib_ktx_version"] = godot::String();
	out["godot_version"] = godot::String();
	out["android_api_level"] = static_cast<int64_t>(0);
	out["session_state"] = godot::Dictionary();
	out["wallet_package"] = godot::String();
	out["wallet_version"] = godot::String();
	out["auth_token_fingerprint"] = godot::String();
	out["cluster"] = godot::String();
	out["last_n_correlation_trace"] = godot::Array();
	out["late_result_count"] = static_cast<int64_t>(0);
	out["pending_submissions_count"] = static_cast<int64_t>(0);
	return out;
}

godot::Dictionary build_empty_posture_dict() {
	godot::Dictionary out;
	out["rooted"] = false;
	out["debuggable"] = false;
	out["developer_options_on"] = false;
	out["adb_enabled"] = false;
	return out;
}

// parse a JSON string from the bridge into a Dictionary.
// Falls back to the supplied empty-shape builder on parse failure or non-Dict
// root so the caller receives the spec'd shape regardless of upstream errors.
godot::Dictionary parse_dict_from_bridge_json(const godot::String &json_str,
		godot::Dictionary (*empty_fallback)(),
		const char *source_label) {
	godot::Ref<godot::JSON> json;
	json.instantiate();
	if (json->parse(json_str) != godot::OK) {
		godot::UtilityFunctions::push_warning(
				godot::String("MWA: ") + godot::String(source_label) + godot::String(" JSON parse failed"));
		return empty_fallback();
	}
	const godot::Variant data = json->get_data();
	if (data.get_type() != godot::Variant::DICTIONARY) {
		godot::UtilityFunctions::push_warning(
				godot::String("MWA: ") + godot::String(source_label) + godot::String(" JSON root is not a dict"));
		return empty_fallback();
	}
	return godot::Dictionary(data);
}
} // anonymous namespace

namespace godot_solana_sdk {

void MobileWalletAdapter::_bind_methods() {
	using godot::ClassDB;
	using godot::D_METHOD;
	using godot::MethodInfo;
	using godot::PropertyInfo;
	using godot::Variant;

	// 11 signals — shapes per Guardrails signals table (post-revision):
	//   - 7 *_completed: 2 params (request_id: String, result: Dictionary) —
	// request_id is first-class typed per threading, not buried in a
	// dict. Amendment documents the escalation from arch §3.2's
	//     uniform 1-Dict spec; 2-1 MWA.gd + 5-5 API docs must follow this.
	//   - 4 error/lifecycle: 1 param (payload: Dictionary).
	// Order matches AC-1 verbatim.
	ADD_SIGNAL(MethodInfo("connect_completed",
			PropertyInfo(Variant::STRING, "request_id"),
			PropertyInfo(Variant::DICTIONARY, "result")));
	ADD_SIGNAL(MethodInfo("reauthorize_completed",
			PropertyInfo(Variant::STRING, "request_id"),
			PropertyInfo(Variant::DICTIONARY, "result")));
	ADD_SIGNAL(MethodInfo("disconnect_completed",
			PropertyInfo(Variant::STRING, "request_id"),
			PropertyInfo(Variant::DICTIONARY, "result")));
	ADD_SIGNAL(MethodInfo("deauthorize_completed",
			PropertyInfo(Variant::STRING, "request_id"),
			PropertyInfo(Variant::DICTIONARY, "result")));
	ADD_SIGNAL(MethodInfo("sign_messages_completed",
			PropertyInfo(Variant::STRING, "request_id"),
			PropertyInfo(Variant::DICTIONARY, "result")));
	ADD_SIGNAL(MethodInfo("sign_transactions_completed",
			PropertyInfo(Variant::STRING, "request_id"),
			PropertyInfo(Variant::DICTIONARY, "result")));
	ADD_SIGNAL(MethodInfo("sign_and_send_completed",
			PropertyInfo(Variant::STRING, "request_id"),
			PropertyInfo(Variant::DICTIONARY, "result")));
	ADD_SIGNAL(MethodInfo("mwa_error", PropertyInfo(Variant::DICTIONARY, "payload")));
	ADD_SIGNAL(MethodInfo("mwa_timeout", PropertyInfo(Variant::DICTIONARY, "payload")));
	ADD_SIGNAL(MethodInfo("mwa_cancelled_lifecycle", PropertyInfo(Variant::DICTIONARY, "payload")));
	ADD_SIGNAL(MethodInfo("reauth_required", PropertyInfo(Variant::DICTIONARY, "payload")));
	// 1-param `pending_submission_found` lifecycle signal.
	// Fires on next successful connect/reauthorize AFTER the success signal if a
	// stale sign_and_send breadcrumb survives a process death (AC-5). One-shot:
	// each breadcrumb produces a single emission then clears. Payload is the
	// 6-key dict {request_id, op_type, started_at_ms, tx_count,
	// tx_preview_hashes, recommendation:"check_chain_for_signatures"}. Parallel
	// in arity to mwa_error / mwa_timeout / mwa_cancelled_lifecycle / reauth_required.
	ADD_SIGNAL(MethodInfo("pending_submission_found", PropertyInfo(Variant::DICTIONARY, "payload")));

	// 7 ops — rename: mwa_connect, mwa_disconnect (collide with
	// godot::Object::connect / disconnect). T2 wires delegation.
	ClassDB::bind_method(D_METHOD("mwa_connect", "identity", "cluster", "opts"), &MobileWalletAdapter::mwa_connect);
	ClassDB::bind_method(D_METHOD("reauthorize", "opts"), &MobileWalletAdapter::reauthorize);
	ClassDB::bind_method(D_METHOD("mwa_disconnect"), &MobileWalletAdapter::mwa_disconnect);
	ClassDB::bind_method(D_METHOD("deauthorize"), &MobileWalletAdapter::deauthorize);
	ClassDB::bind_method(D_METHOD("sign_messages", "messages", "opts"), &MobileWalletAdapter::sign_messages);
	ClassDB::bind_method(D_METHOD("sign_transactions", "transactions", "opts"), &MobileWalletAdapter::sign_transactions);
	ClassDB::bind_method(D_METHOD("sign_and_send", "transactions", "opts"), &MobileWalletAdapter::sign_and_send);

	// 4 state getters — rename: mwa_is_connected (collides with
	// godot::Object::is_connected). wires to bridge snapshot.
	ClassDB::bind_method(D_METHOD("mwa_is_connected"), &MobileWalletAdapter::mwa_is_connected);
	ClassDB::bind_method(D_METHOD("get_public_key"), &MobileWalletAdapter::get_public_key);
	ClassDB::bind_method(D_METHOD("get_cluster"), &MobileWalletAdapter::get_cluster);
	ClassDB::bind_method(D_METHOD("get_wallet_label"), &MobileWalletAdapter::get_wallet_label);
	// AC-7 fingerprint getter forwarded by MWA.gd facade.
	ClassDB::bind_method(D_METHOD("get_auth_token_fingerprint"), &MobileWalletAdapter::get_auth_token_fingerprint);

	// 2 utility + 1 NEW posture surface (AC-4).
	ClassDB::bind_method(D_METHOD("get_diagnostics"), &MobileWalletAdapter::get_diagnostics);
	ClassDB::bind_method(D_METHOD("get_device_posture"), &MobileWalletAdapter::get_device_posture);
	ClassDB::bind_method(D_METHOD("forget_all"), &MobileWalletAdapter::forget_all);
}

MobileWalletAdapter::MobileWalletAdapter() :
		dispatcher_(this),
		bridge_(std::unique_ptr<mwa::MwaAndroidBridge>(mwa::MwaAndroidBridge::create(&dispatcher_))) {}

// 7 ops — generate request_id and delegate to bridge. Bridge method names
// stay original (rename is node-binding-only). Disconnect/deauthorize pass
// an empty Dictionary for opts — bridge signatures require the arg even though
// the node bindings take none. T3 adds the null-bridge guard above each
// delegation line.
godot::String MobileWalletAdapter::mwa_connect(const godot::Dictionary &identity, const godot::String &cluster, const godot::Dictionary &opts) {
	const godot::String request_id = generate_request_id();
	if (bridge_ == nullptr) {
		// 1-arity error signal — wrap payload in 1-elem Array.
		dispatcher_.post(godot::String("mwa_error"),
				godot::Array::make(build_unsupported_platform_payload(request_id, godot::String("connect"))));
		return request_id;
	}
	bridge_->connect(request_id, identity, cluster, opts);
	return request_id;
}

void MobileWalletAdapter::reauthorize(const godot::Dictionary &opts) {
	const godot::String request_id = generate_request_id();
	if (bridge_ == nullptr) {
		dispatcher_.post(godot::String("mwa_error"),
				godot::Array::make(build_unsupported_platform_payload(request_id, godot::String("reauthorize"))));
		return;
	}
	bridge_->reauthorize(request_id, opts);
}

void MobileWalletAdapter::mwa_disconnect() {
	const godot::String request_id = generate_request_id();
	if (bridge_ == nullptr) {
		dispatcher_.post(godot::String("mwa_error"),
				godot::Array::make(build_unsupported_platform_payload(request_id, godot::String("disconnect"))));
		return;
	}
	bridge_->disconnect(request_id, godot::Dictionary());
}

void MobileWalletAdapter::deauthorize() {
	const godot::String request_id = generate_request_id();
	if (bridge_ == nullptr) {
		dispatcher_.post(godot::String("mwa_error"),
				godot::Array::make(build_unsupported_platform_payload(request_id, godot::String("deauthorize"))));
		return;
	}
	bridge_->deauthorize(request_id, godot::Dictionary());
}

void MobileWalletAdapter::sign_messages(const godot::TypedArray<godot::PackedByteArray> &messages, const godot::Dictionary &opts) {
	const godot::String request_id = generate_request_id();
	if (bridge_ == nullptr) {
		dispatcher_.post(godot::String("mwa_error"),
				godot::Array::make(build_unsupported_platform_payload(request_id, godot::String("sign_messages"))));
		return;
	}
	bridge_->sign_messages(request_id, messages, opts);
}

void MobileWalletAdapter::sign_transactions(const godot::TypedArray<godot::PackedByteArray> &transactions, const godot::Dictionary &opts) {
	const godot::String request_id = generate_request_id();
	if (bridge_ == nullptr) {
		dispatcher_.post(godot::String("mwa_error"),
				godot::Array::make(build_unsupported_platform_payload(request_id, godot::String("sign_transactions"))));
		return;
	}
	bridge_->sign_transactions(request_id, transactions, opts);
}

void MobileWalletAdapter::sign_and_send(const godot::TypedArray<godot::PackedByteArray> &transactions, const godot::Dictionary &opts) {
	const godot::String request_id = generate_request_id();
	if (bridge_ == nullptr) {
		dispatcher_.post(godot::String("mwa_error"),
				godot::Array::make(build_unsupported_platform_payload(request_id, godot::String("sign_and_send"))));
		return;
	}
	bridge_->sign_and_send(request_id, transactions, opts);
}

// 4 state getters — T6 wires via bridge->query_session_state() (atomic
// snapshot: NoOp returns empty defaults on non-Android; Jni round-trips to
// MwaSessionState via MwaJniContext::query_session_state on Android). The
// null-bridge branch (MWA_TESTING case) returns the same empty-default
// values NoOp's snapshot would produce.
bool MobileWalletAdapter::mwa_is_connected() const {
	if (bridge_ == nullptr) { return false; }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return static_cast<bool>(snapshot.get("is_connected", false));
}

godot::String MobileWalletAdapter::get_public_key() const {
	if (bridge_ == nullptr) { return {}; }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return godot::String(snapshot.get("public_key", godot::String()));
}

godot::String MobileWalletAdapter::get_cluster() const {
	if (bridge_ == nullptr) { return {}; }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return godot::String(snapshot.get("cluster", godot::String()));
}

godot::String MobileWalletAdapter::get_wallet_label() const {
	if (bridge_ == nullptr) { return {}; }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return godot::String(snapshot.get("wallet_label", godot::String()));
}

godot::String MobileWalletAdapter::get_auth_token_fingerprint() const {
	if (bridge_ == nullptr) { return {}; }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return godot::String(snapshot.get("auth_token_fingerprint", godot::String()));
}

// synchronous diagnostics pull. Replaces
// the empty-Dict stub. Bridge is queried synchronously for a JSON
// String (same pattern as `query_session_state`); here we parse it into a
// Dictionary AND overlay `godot_version` from `Engine::get_version_info()` so
// the Kotlin side does not need to know the engine version (it already
// populates the other 11 keys via MwaDiagnosticsBuilder).
//
// `is_supported()` is NOT called here — the bridge contract already covers
// non-Android via the NoOp impl returning the 12-key all-empty JSON shape per
// Null-bridge (MWA_TESTING) returns the same empty Dict directly.
godot::Dictionary MobileWalletAdapter::get_diagnostics() {
	godot::Dictionary out;
	if (bridge_ == nullptr) {
		out = build_empty_diagnostics_dict();
	} else {
		const godot::String json_str = bridge_->query_diagnostics_json();
		out = parse_dict_from_bridge_json(json_str, &build_empty_diagnostics_dict, "get_diagnostics");
	}
	// step 3 — overlay godot_version from Engine::get_version_info
	// so the Kotlin side's "" placeholder (set in
	// GDExtensionAndroidPlugin.buildDiagnosticsJsonForJni) is replaced with the
	// actual engine version on the read side. Falls back to "" if the engine
	// singleton is unavailable (early init / non-engine test harness).
	godot::String godot_version_str;
	if (godot::Engine *engine = godot::Engine::get_singleton()) {
		const godot::Dictionary version_info = engine->get_version_info();
		if (version_info.has("string")) {
			godot_version_str = godot::String(version_info["string"]);
		}
	}
	out["godot_version"] = godot_version_str;
	return out;
}

// (AC-4) — synchronous device posture pull. Mirrors
// `get_diagnostics`: bridge returns the 4-key JSON, we parse + fall back to
// the empty-shape dict on any failure.
godot::Dictionary MobileWalletAdapter::get_device_posture() {
	if (bridge_ == nullptr) {
		return build_empty_posture_dict();
	}
	const godot::String json_str = bridge_->query_device_posture_json();
	return parse_dict_from_bridge_json(json_str, &build_empty_posture_dict, "get_device_posture");
}

// fill in the empty stub.
// Mirrors the mwa_disconnect / deauthorize sibling pattern at this TU
// (generate_request_id → null-bridge guard → bridge delegation), with two
// differences: (a) the bridge call is 1-arg per
// MwaAndroidBridge::forget_all (no opts Dictionary — distinct from
// disconnect/deauthorize 2-arg signatures); (b) the unsupported-platform
// payload's source_method literal is "forget_all". NO ADD_SIGNAL row is
// added — LOCKS that forget_all has no completion signal; AC-1
// evidence is post-condition state inspection (listAllKeys empty +
// MasterKey alias gone + sessionState cleared).
void MobileWalletAdapter::forget_all() {
	const godot::String request_id = generate_request_id();
	if (bridge_ == nullptr) {
		dispatcher_.post(godot::String("mwa_error"),
				godot::Array::make(build_unsupported_platform_payload(request_id, godot::String("forget_all"))));
		return;
	}
	bridge_->forget_all(request_id);
}

#ifdef MWA_TESTING
void MobileWalletAdapter::set_bridge_for_testing(std::unique_ptr<mwa::MwaAndroidBridge> bridge) {
	bridge_ = std::move(bridge);
}

mwa::GodotMainDispatcher &MobileWalletAdapter::dispatcher_for_testing() {
	return dispatcher_;
}
#endif

} //namespace godot_solana_sdk
