#include "local_keypair_signer.hpp"

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/method_bind.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "generated/mwa_error_codes.hpp"
#include "keypair.hpp"

namespace godot_solana_sdk {

void LocalKeypairSigner::_bind_methods() {
	using namespace godot;

	ClassDB::bind_method(D_METHOD("set_keypair", "kp"),
			&LocalKeypairSigner::set_keypair);
	ClassDB::bind_method(D_METHOD("get_keypair"),
			&LocalKeypairSigner::get_keypair);

	ClassDB::bind_method(D_METHOD("is_connected"),
			&LocalKeypairSigner::is_connected);
	ClassDB::bind_method(D_METHOD("get_public_key"),
			&LocalKeypairSigner::get_public_key);

	ClassDB::bind_method(D_METHOD("sign_messages", "messages_concat", "lengths", "request_id"),
			&LocalKeypairSigner::sign_messages);
	ClassDB::bind_method(D_METHOD("sign_transactions", "transactions_concat", "lengths", "request_id"),
			&LocalKeypairSigner::sign_transactions);
}

void LocalKeypairSigner::set_keypair(const godot::Ref<godot::Keypair> &kp) {
	kp_ = kp;
}

godot::Ref<godot::Keypair> LocalKeypairSigner::get_keypair() const {
	return kp_;
}

bool LocalKeypairSigner::is_connected() const {
	return kp_.is_valid() && !kp_->get_public_string().is_empty();
}

godot::String LocalKeypairSigner::get_public_key() const {
	if (!kp_.is_valid()) {
		return godot::String();
	}
	return kp_->get_public_string();
}

void LocalKeypairSigner::sign_messages(const godot::PackedByteArray &messages_concat,
		const godot::PackedInt32Array &lengths,
		const godot::String &request_id) {
	using namespace godot;

	if (!is_connected()) {
		// MwaErrorCode is in namespace godot_solana_sdk::mwa (verified at
		// src/generated/mwa_error_codes.hpp:5). Reused here for non-MWA error reporting.
		// TODO: rename to SignerErrorCode in v1.2 cleanup.
		emit_signal("sign_failed", request_id,
				String(godot_solana_sdk::mwa::code_name(godot_solana_sdk::mwa::MwaErrorCode::NOT_CONNECTED)),
				String("LocalKeypairSigner has no Keypair"));
		return;
	}

	// Defensive bounds: sum(lengths) must equal messages_concat.size().
	int64_t total_len = 0;
	for (int i = 0; i < lengths.size(); i++) {
		const int len = lengths[i];
		if (len < 0) {
			emit_signal("sign_failed", request_id,
					String(godot_solana_sdk::mwa::code_name(godot_solana_sdk::mwa::MwaErrorCode::PROTOCOL_ERROR)),
					String("LocalKeypairSigner: negative length in lengths array"));
			return;
		}
		total_len += len;
	}
	if (total_len != messages_concat.size()) {
		emit_signal("sign_failed", request_id,
				String(godot_solana_sdk::mwa::code_name(godot_solana_sdk::mwa::MwaErrorCode::PROTOCOL_ERROR)),
				String("LocalKeypairSigner: sum(lengths) does not match messages_concat.size()"));
		return;
	}

	Array sigs;
	int offset = 0;
	for (int i = 0; i < lengths.size(); i++) {
		const int len = lengths[i];
		const PackedByteArray slice = messages_concat.slice(offset, offset + len);
		sigs.append(kp_->sign_message(slice));
		offset += len;
	}

	// Synchronous emit — preserves AC-6 (signatures populated before sign() returns).
	// See class doc-comment for the §2.3.1 / scope clarification.
	emit_signal("sign_completed", request_id, sigs);
}

void LocalKeypairSigner::sign_transactions(const godot::PackedByteArray &transactions_concat,
		const godot::PackedInt32Array &lengths,
		const godot::String &request_id) {
	// Solana transactions sign the same as messages from this wrapper's POV;
	// the caller pre-serializes. Delegating keeps the implementation minimal.
	sign_messages(transactions_concat, lengths, request_id);
}

} //namespace godot_solana_sdk
