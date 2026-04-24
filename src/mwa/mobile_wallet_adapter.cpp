#include "mwa/mobile_wallet_adapter.hpp"

#include <cstdio>
#include <cstdint>
#include <random>

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/method_bind.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "generated/mwa_error_codes.hpp"

namespace {
// D-4: generate exactly 8 lowercase hex chars ([0-9a-f]{8}) per op-method call.
// Matches Story 1-6 Kotlin corrId format (UUID.randomUUID().toString().take(8).lowercase()).
// Source pick: std::random_device + std::mt19937 — portable, no scene-tree/engine
// dependency, no godot-cpp gen/ Crypto class requirement. thread_local keeps
// construction cost off the hot path without cross-thread contention.
godot::String generate_request_id() {
	static thread_local std::random_device rd;
	static thread_local std::mt19937 gen(rd());
	std::uniform_int_distribution<std::uint32_t> dist(0U, 0xFFFFFFFFU);
	const std::uint32_t r = dist(gen);
	char buf[9];
	std::snprintf(buf, sizeof(buf), "%08x", r);
	return godot::String(buf);
}

// D-3: build the AC-3 mwa_error payload for the null-bridge pre-op branch.
// Shape mirrors NoOpMwaAndroidBridge::emit_unsupported (src/mwa/no_op_mwa_android_bridge.cpp:21-43)
// so consumers see an identical mwa_error{UNSUPPORTED_PLATFORM, ...} envelope regardless of
// which path fired (non-Android NoOp vs. MWA_TESTING null-bridge). See Story 1-5 Guardrails
// §D-3 + §"Signal payload semantic" (lines 110-111) for key rationale. developer_details
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
} // anonymous namespace

namespace godot_solana_sdk {

void MobileWalletAdapter::_bind_methods() {
	using godot::ClassDB;
	using godot::D_METHOD;
	using godot::MethodInfo;
	using godot::PropertyInfo;
	using godot::Variant;

	// 11 signals — shapes per Story 1-5 Guardrails signals table (post-revision):
	//   - 7 *_completed: 2 params (request_id: String, result: Dictionary) —
	//     request_id is first-class typed per D-4 threading, not buried in a
	//     dict. Amendment A-12 documents the escalation from arch §3.2's
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

	// 7 ops — D-1 rename: mwa_connect, mwa_disconnect (collide with
	// godot::Object::connect / disconnect). T2 wires delegation.
	ClassDB::bind_method(D_METHOD("mwa_connect", "identity", "cluster", "opts"), &MobileWalletAdapter::mwa_connect);
	ClassDB::bind_method(D_METHOD("reauthorize", "opts"), &MobileWalletAdapter::reauthorize);
	ClassDB::bind_method(D_METHOD("mwa_disconnect"), &MobileWalletAdapter::mwa_disconnect);
	ClassDB::bind_method(D_METHOD("deauthorize"), &MobileWalletAdapter::deauthorize);
	ClassDB::bind_method(D_METHOD("sign_messages", "messages", "opts"), &MobileWalletAdapter::sign_messages);
	ClassDB::bind_method(D_METHOD("sign_transactions", "transactions", "opts"), &MobileWalletAdapter::sign_transactions);
	ClassDB::bind_method(D_METHOD("sign_and_send", "transactions", "opts"), &MobileWalletAdapter::sign_and_send);

	// 4 state getters — D-1 rename: mwa_is_connected (collides with
	// godot::Object::is_connected). Story 2-1 T6 wires to bridge snapshot.
	ClassDB::bind_method(D_METHOD("mwa_is_connected"), &MobileWalletAdapter::mwa_is_connected);
	ClassDB::bind_method(D_METHOD("get_public_key"), &MobileWalletAdapter::get_public_key);
	ClassDB::bind_method(D_METHOD("get_cluster"), &MobileWalletAdapter::get_cluster);
	ClassDB::bind_method(D_METHOD("get_wallet_label"), &MobileWalletAdapter::get_wallet_label);
	// Story 2-1 T6 — AC-7 fingerprint getter forwarded by MWA.gd facade (T7).
	ClassDB::bind_method(D_METHOD("get_auth_token_fingerprint"), &MobileWalletAdapter::get_auth_token_fingerprint);

	// 2 utility — stubs in 1-5.
	ClassDB::bind_method(D_METHOD("get_diagnostics"), &MobileWalletAdapter::get_diagnostics);
	ClassDB::bind_method(D_METHOD("forget_all"), &MobileWalletAdapter::forget_all);
}

MobileWalletAdapter::MobileWalletAdapter() :
		dispatcher_(this),
		bridge_(std::unique_ptr<mwa::MwaAndroidBridge>(mwa::MwaAndroidBridge::create(&dispatcher_))) {}

// 7 ops — generate request_id (D-4) and delegate to bridge. Bridge method names
// stay original (D-1 rename is node-binding-only). Disconnect/deauthorize pass
// an empty Dictionary for opts — bridge signatures require the arg even though
// the node bindings take none. T3 adds the null-bridge guard above each
// delegation line.
godot::String MobileWalletAdapter::mwa_connect(const godot::Dictionary &identity, const godot::String &cluster, const godot::Dictionary &opts) {
	const godot::String request_id = generate_request_id();
	if (bridge_ == nullptr) {
		// D-6: 1-arity error signal — wrap payload in 1-elem Array.
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
// null-bridge branch (D-3 MWA_TESTING case) returns the same empty-default
// values NoOp's snapshot would produce.
bool MobileWalletAdapter::mwa_is_connected() const {
	if (bridge_ == nullptr) { return false; }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return static_cast<bool>(snapshot.get("is_connected", false));
}

godot::String MobileWalletAdapter::get_public_key() const {
	if (bridge_ == nullptr) { return godot::String(); }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return godot::String(snapshot.get("public_key", godot::String()));
}

godot::String MobileWalletAdapter::get_cluster() const {
	if (bridge_ == nullptr) { return godot::String(); }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return godot::String(snapshot.get("cluster", godot::String()));
}

godot::String MobileWalletAdapter::get_wallet_label() const {
	if (bridge_ == nullptr) { return godot::String(); }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return godot::String(snapshot.get("wallet_label", godot::String()));
}

godot::String MobileWalletAdapter::get_auth_token_fingerprint() const {
	if (bridge_ == nullptr) { return godot::String(); }
	const godot::Dictionary snapshot = bridge_->query_session_state();
	return godot::String(snapshot.get("auth_token_fingerprint", godot::String()));
}

// 2 utility — stubs in 1-5.
godot::Dictionary MobileWalletAdapter::get_diagnostics() {
	return godot::Dictionary();
}

void MobileWalletAdapter::forget_all() {}

#ifdef MWA_TESTING
void MobileWalletAdapter::set_bridge_for_testing(std::unique_ptr<mwa::MwaAndroidBridge> bridge) {
	bridge_ = std::move(bridge);
}

mwa::GodotMainDispatcher &MobileWalletAdapter::dispatcher_for_testing() {
	return dispatcher_;
}
#endif

} //namespace godot_solana_sdk
