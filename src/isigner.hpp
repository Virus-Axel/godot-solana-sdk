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
	/// Registers the GDScript-exposed signals (@c sign_completed, @c sign_failed) on the engine class DB.
	static void _bind_methods();

public:
	/// @return @c true iff this signer currently holds an active session capable of signing.
	[[nodiscard]] virtual bool is_connected() const = 0;

	/// @return Base58-encoded public key of the active signer, or an empty string when not connected.
	[[nodiscard]] virtual godot::String get_public_key() const = 0;

	/**
	 * Sign one or more raw messages. Non-blocking; completion is delivered via the @c sign_completed
	 * or @c sign_failed signal carrying the same @p request_id.
	 *
	 * @param messages_concat concatenated byte buffer of all messages back-to-back.
	 * @param lengths length (in bytes) of each message in @p messages_concat; @c lengths.size()
	 *                determines the message count.
	 * @param request_id caller-supplied correlation id, echoed on the terminal signal.
	 */
	virtual void sign_messages(const godot::PackedByteArray &messages_concat,
			const godot::PackedInt32Array &lengths,
			const godot::String &request_id) = 0;

	/**
	 * Sign one or more serialized Solana transactions. Non-blocking; completion is delivered via
	 * the @c sign_completed or @c sign_failed signal carrying the same @p request_id.
	 *
	 * @param transactions_concat concatenated byte buffer of serialized transactions back-to-back.
	 * @param lengths length (in bytes) of each transaction in @p transactions_concat.
	 * @param request_id caller-supplied correlation id, echoed on the terminal signal.
	 */
	virtual void sign_transactions(const godot::PackedByteArray &transactions_concat,
			const godot::PackedInt32Array &lengths,
			const godot::String &request_id) = 0;

	// Signals defined in _bind_methods():
	//   sign_completed(String request_id, Array signatures)
	//   sign_failed(String request_id, String error_code, String message)

	~ISigner() override = default;
};

} //namespace godot_solana_sdk
