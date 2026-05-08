#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/string.hpp"

#include "solana_utils.hpp"

namespace godot_solana_sdk {

/**
 * @brief Abstract signer interface for Solana transactions.
 *
 * Both the desktop @c WalletAdapter (via @c WalletAdapterSigner bridge) and the Android
 * @c MobileWalletAdapter implement this seam. @c Transaction depends only on @c Ref<ISigner>
 * — no reverse dependency from @c godot-solana-sdk core to MWA.
 *
 * @par Threading contract (§2.3.1)
 * Methods return immediately; results arrive via signals on the Godot main thread. Implementations
 * that hop threads (Android JNI) MUST use @c call_deferred via @c GodotMainDispatcher.
 * Implementations that stay on the calling thread (e.g., @c LocalKeypairSigner whose underlying
 * @c Keypair::sign_message is synchronous) MAY emit synchronously — the §2.3.1 "main thread"
 * guarantee is satisfied trivially when no thread hop occurs (AC-6).
 *
 * @par Ownership
 * Engine-managed via @c godot::Ref<>. Implementations are reference-counted.
 */
class ISigner : public godot::RefCounted {
	GDCLASS_CUSTOM(ISigner, godot::RefCounted)

protected:
	static void _bind_methods();

public:
	[[nodiscard]] virtual bool is_connected() const = 0;
	[[nodiscard]] virtual godot::String get_public_key() const = 0;

	virtual void sign_messages(const godot::PackedByteArray &messages_concat,
			const godot::PackedInt32Array &lengths,
			const godot::String &request_id) = 0;

	virtual void sign_transactions(const godot::PackedByteArray &transactions_concat,
			const godot::PackedInt32Array &lengths,
			const godot::String &request_id) = 0;

	// Signals defined in _bind_methods():
	//   sign_completed(String request_id, Array signatures)
	//   sign_failed(String request_id, String error_code, String message)

	~ISigner() override = default;
};

} //namespace godot_solana_sdk
