#include "wallet_adapter/wallet_adapter_signer.hpp"

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/method_bind.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "generated/mwa_error_codes.hpp"
#include "pubkey.hpp"
#include "wallet_adapter/wallet_adapter.hpp"

namespace godot_solana_sdk {

void WalletAdapterSigner::_bind_methods() {
	using namespace godot;

	ClassDB::bind_method(D_METHOD("set_wallet_adapter", "wallet_adapter"),
			&WalletAdapterSigner::set_wallet_adapter);
	ClassDB::bind_method(D_METHOD("get_wallet_adapter"),
			&WalletAdapterSigner::get_wallet_adapter);

	ClassDB::bind_method(D_METHOD("is_connected"),
			&WalletAdapterSigner::is_connected);
	ClassDB::bind_method(D_METHOD("get_public_key"),
			&WalletAdapterSigner::get_public_key);

	ClassDB::bind_method(D_METHOD("sign_messages", "messages_concat", "lengths", "request_id"),
			&WalletAdapterSigner::sign_messages);
	ClassDB::bind_method(D_METHOD("sign_transactions", "transactions_concat", "lengths", "request_id"),
			&WalletAdapterSigner::sign_transactions);

	ClassDB::bind_method(D_METHOD("_on_message_signed", "signature"),
			&WalletAdapterSigner::_on_message_signed);
	ClassDB::bind_method(D_METHOD("_on_signing_failed"),
			&WalletAdapterSigner::_on_signing_failed);
}

void WalletAdapterSigner::set_wallet_adapter(godot::Object *p_wa) {
	// Code-review HIGH 3: refuse mid-flight swaps. Without this guard, swapping
	// to a different WalletAdapter would leave disconnect_signals targeting the
	// NEW wa_ (no-op against the OLD one), leaking signal connections; swapping
	// to nullptr would strand the in-flight request forever (self_pin_ leaked,
	// caller never notified). Cleanest fix: reject the swap with a diagnostic.
	// Caller can call set_wallet_adapter again after the in-flight resolves.
	if (request_in_flight_) {
		WARN_PRINT_ED("WalletAdapterSigner::set_wallet_adapter: request in flight; swap rejected. Call again after sign completes/fails.");
		return;
	}

	if (p_wa == nullptr) {
		wa_ = nullptr;
		return;
	}
	auto *cast = godot::Object::cast_to<godot::WalletAdapter>(p_wa);
	if (cast == nullptr) {
		// Loud-fail diagnostic: silent nullification leaves the signer in a
		// non-functional state with no trail.
		WARN_PRINT_ED("WalletAdapterSigner::set_wallet_adapter: argument is not a WalletAdapter; signer left unconfigured.");
	}
	wa_ = cast;
}

godot::Object *WalletAdapterSigner::get_wallet_adapter() const {
	return wa_;
}

// NOTE: WalletAdapter::is_connected() is zero-arg (verified at
// include/wallet_adapter/wallet_adapter.hpp:153). The base Object class also
// declares bool is_connected(StringName, Callable) const with two REQUIRED args,
// so overload resolution picks WalletAdapter's zero-arg version unambiguously
// for `wa_->is_connected()`. No `using Object::is_connected` is required for our
// call site.
bool WalletAdapterSigner::is_connected() const {
	return wa_ != nullptr && wa_->is_connected();
}

godot::String WalletAdapterSigner::get_public_key() const {
	if (wa_ == nullptr || !wa_->is_connected()) {
		return godot::String();
	}
	// godot::Pubkey lives in `namespace godot` (verified at include/pubkey.hpp:16,
	// the closing `} //namespace godot` at line 494). godot::Pubkey::string_from_variant
	// (declared at include/pubkey.hpp:435) accepts a Variant carrying a Pubkey-typed
	// Resource — which is exactly what WalletAdapter::get_connected_key returns
	// (verified at src/wallet_adapter/wallet_adapter.cpp:447, body
	// `return Pubkey::new_from_bytes(connected_key);`). No conversion gap.
	return godot::Pubkey::string_from_variant(wa_->get_connected_key());
}

void WalletAdapterSigner::sign_messages(const godot::PackedByteArray &messages_concat,
		const godot::PackedInt32Array &lengths,
		const godot::String &request_id) {
	using namespace godot;

	if (wa_ == nullptr || !wa_->is_connected()) {
		// Code-review LOW 8: use generated MwaErrorCode names for parity with
		// LocalKeypairSigner (marker tracks the v1.2 namespace cleanup).
		// TODO: rename to SignerErrorCode in v1.2 cleanup.
		emit_signal("sign_failed", request_id,
				String(godot_solana_sdk::mwa::code_name(godot_solana_sdk::mwa::MwaErrorCode::NOT_CONNECTED)),
				String("WalletAdapterSigner has no connected WalletAdapter"));
		return;
	}
	if (request_in_flight_) {
		// "BUSY" not in MwaErrorCode enum; keep as string literal until error-codes.yaml
		// adds it (follow-up). // TODO: consider adding BUSY to enum.
		emit_signal("sign_failed", request_id,
				String("BUSY"),
				String("WalletAdapterSigner has another request in flight"));
		return;
	}
	if (lengths.is_empty()) {
		emit_signal("sign_completed", request_id, Array());
		return;
	}

	// Bounds pre-check: sum(lengths) must equal messages_concat.size(). Code-review
	// MED 7: emit sign_failed (caller hangs without it; previous ERR_FAIL_COND_MSG
	// returned silently, leaving HashMap leak). Matches LocalKeypairSigner's
	// PROTOCOL_ERROR convention.
	int64_t total_len = 0;
	for (int i = 0; i < lengths.size(); i++) {
		const int len = lengths[i];
		if (len < 0) {
			// TODO: rename to SignerErrorCode in v1.2 cleanup.
			emit_signal("sign_failed", request_id,
					String(godot_solana_sdk::mwa::code_name(godot_solana_sdk::mwa::MwaErrorCode::PROTOCOL_ERROR)),
					String("WalletAdapterSigner: negative length in lengths array"));
			return;
		}
		total_len += len;
	}
	if (total_len != messages_concat.size()) {
		// TODO: rename to SignerErrorCode in v1.2 cleanup.
		emit_signal("sign_failed", request_id,
				String(godot_solana_sdk::mwa::code_name(godot_solana_sdk::mwa::MwaErrorCode::PROTOCOL_ERROR)),
				String("WalletAdapterSigner: sum(lengths) does not match messages_concat.size()"));
		return;
	}

	request_in_flight_ = true;
	pending_request_id_ = request_id;
	pending_concat_ = messages_concat;
	pending_lengths_ = lengths;
	pending_signatures_ = Array();
	pending_offset_ = 0;
	self_pin_ = Ref<WalletAdapterSigner>(this);

	// Connect ONCE per request (not per slice). Connections are explicitly torn down
	// in finish_success() / fail(). One-shot connections per slice would leak siblings:
	// CONNECT_ONE_SHOT only removes the callback that fired; its sibling persists.
	// After N slices, N stale callbacks would accumulate and eventually fire into
	// freed memory once self_pin_ releases `this`.
	const Callable on_signed(this, "_on_message_signed");
	const Callable on_failed(this, "_on_signing_failed");
	wa_->connect("message_signed", on_signed);
	wa_->connect("signing_failed", on_failed);

	dispatch_next_slice();
}

void WalletAdapterSigner::sign_transactions(const godot::PackedByteArray &transactions_concat,
		const godot::PackedInt32Array &lengths,
		const godot::String &request_id) {
	// WalletAdapter::sign_message handles both message and transaction signing
	// (the wallet receives serialized bytes; the protocol distinction lives at the
	// caller). Delegating to sign_messages keeps this bridge minimal.
	sign_messages(transactions_concat, lengths, request_id);
}

void WalletAdapterSigner::dispatch_next_slice() {
	using namespace godot;

	const int slice_index = pending_signatures_.size();
	const int slice_len = pending_lengths_[slice_index];
	const PackedByteArray slice = pending_concat_.slice(pending_offset_, pending_offset_ + slice_len);
	pending_offset_ += slice_len;

	// signer_index is hardcoded to 0 — see header doc note on v1.1 limitation.
	wa_->sign_message(slice, 0);
}

void WalletAdapterSigner::_on_message_signed(const godot::PackedByteArray &signature) {
	if (!request_in_flight_) {
		return;
	}
	pending_signatures_.append(signature);
	if (pending_signatures_.size() >= pending_lengths_.size()) {
		finish_success();
	} else {
		dispatch_next_slice();
	}
}

void WalletAdapterSigner::_on_signing_failed() {
	if (!request_in_flight_) {
		return;
	}
	// Code-review LOW 8: WALLET_REJECTED is in MwaErrorCode enum (mwa_error_codes.hpp:9).
	// TODO: rename to SignerErrorCode in v1.2 cleanup.
	fail(godot::String(godot_solana_sdk::mwa::code_name(godot_solana_sdk::mwa::MwaErrorCode::WALLET_REJECTED)),
			godot::String("WalletAdapter emitted signing_failed"));
}

void WalletAdapterSigner::disconnect_signals() {
	using namespace godot;

	if (wa_ == nullptr) {
		return;
	}
	const Callable on_signed(this, "_on_message_signed");
	const Callable on_failed(this, "_on_signing_failed");
	// fix (Q2=(a) source fix, 2026-04-30): `WalletAdapter` declares
	// a zero-arg `is_connected()` (verified at include/wallet_adapter/wallet_
	// adapter.hpp:153) that shadows the inherited 2-arg `Object::is_connected
	// (StringName, Callable) const` (C++ name-hiding). The pre-existing inline
	// NOTE at lines 70-75 documented the resolution rule for the ZERO-arg call
	// sites (lines 77, 81, 98) but the 2-arg call sites here at 216, 219 were
	// not addressed. Cast through `Object *` so overload resolution finds the
	// 2-arg version. See docs/triage/.md.
	if (static_cast<godot::Object *>(wa_)->is_connected("message_signed", on_signed)) {
		wa_->disconnect("message_signed", on_signed);
	}
	if (static_cast<godot::Object *>(wa_)->is_connected("signing_failed", on_failed)) {
		wa_->disconnect("signing_failed", on_failed);
	}
}

void WalletAdapterSigner::finish_success() {
	const godot::String req_id = pending_request_id_;
	const godot::Array sigs = pending_signatures_;

	disconnect_signals();

	request_in_flight_ = false;
	pending_request_id_ = godot::String();
	pending_concat_ = godot::PackedByteArray();
	pending_lengths_ = godot::PackedInt32Array();
	pending_signatures_ = godot::Array();
	pending_offset_ = 0;

	emit_signal("sign_completed", req_id, sigs);

	// Release self-pin LAST so `this` survives the emit chain.
	self_pin_ = godot::Ref<WalletAdapterSigner>();
}

void WalletAdapterSigner::fail(const godot::String &error_code, const godot::String &message) {
	const godot::String req_id = pending_request_id_;

	disconnect_signals();

	request_in_flight_ = false;
	pending_request_id_ = godot::String();
	pending_concat_ = godot::PackedByteArray();
	pending_lengths_ = godot::PackedInt32Array();
	pending_signatures_ = godot::Array();
	pending_offset_ = 0;

	emit_signal("sign_failed", req_id, error_code, message);

	self_pin_ = godot::Ref<WalletAdapterSigner>();
}

} //namespace godot_solana_sdk
