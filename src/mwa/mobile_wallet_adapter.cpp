#include "mwa/mobile_wallet_adapter.hpp"

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/method_bind.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

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
	// godot::Object::is_connected). Return empty defaults until Story 2-1.
	ClassDB::bind_method(D_METHOD("mwa_is_connected"), &MobileWalletAdapter::mwa_is_connected);
	ClassDB::bind_method(D_METHOD("get_public_key"), &MobileWalletAdapter::get_public_key);
	ClassDB::bind_method(D_METHOD("get_cluster"), &MobileWalletAdapter::get_cluster);
	ClassDB::bind_method(D_METHOD("get_wallet_label"), &MobileWalletAdapter::get_wallet_label);

	// 2 utility — stubs in 1-5.
	ClassDB::bind_method(D_METHOD("get_diagnostics"), &MobileWalletAdapter::get_diagnostics);
	ClassDB::bind_method(D_METHOD("forget_all"), &MobileWalletAdapter::forget_all);
}

MobileWalletAdapter::MobileWalletAdapter() :
		dispatcher_(this),
		bridge_(std::unique_ptr<mwa::MwaAndroidBridge>(mwa::MwaAndroidBridge::create(&dispatcher_))) {}

// 7 ops — empty bodies; T2 wires bridge delegation + request_id generation.
void MobileWalletAdapter::mwa_connect(const godot::Dictionary & /*identity*/, const godot::String & /*cluster*/, const godot::Dictionary & /*opts*/) {}

void MobileWalletAdapter::reauthorize(const godot::Dictionary & /*opts*/) {}

void MobileWalletAdapter::mwa_disconnect() {}

void MobileWalletAdapter::deauthorize() {}

void MobileWalletAdapter::sign_messages(const godot::TypedArray<godot::PackedByteArray> & /*messages*/, const godot::Dictionary & /*opts*/) {}

void MobileWalletAdapter::sign_transactions(const godot::TypedArray<godot::PackedByteArray> & /*transactions*/, const godot::Dictionary & /*opts*/) {}

void MobileWalletAdapter::sign_and_send(const godot::TypedArray<godot::PackedByteArray> & /*transactions*/, const godot::Dictionary & /*opts*/) {}

// 4 state getters — empty defaults in 1-5.
bool MobileWalletAdapter::mwa_is_connected() const {
	return false;
}

godot::String MobileWalletAdapter::get_public_key() const {
	return godot::String();
}

godot::String MobileWalletAdapter::get_cluster() const {
	return godot::String();
}

godot::String MobileWalletAdapter::get_wallet_label() const {
	return godot::String();
}

// 2 utility — stubs in 1-5.
godot::Dictionary MobileWalletAdapter::get_diagnostics() {
	return godot::Dictionary();
}

void MobileWalletAdapter::forget_all() {}

} //namespace godot_solana_sdk
