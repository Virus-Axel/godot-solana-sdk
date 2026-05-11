#pragma once

#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/string.hpp"

#include "isigner.hpp"

namespace godot {
class Keypair;
} //namespace godot

namespace godot_solana_sdk {

/**
 * @brief Compat shim that wraps a @c godot::Keypair as an @c ISigner.
 *
 * Used by @c Transaction's deprecation path: when consumer code still passes a raw
 * @c Keypair into @c set_signers, the dispatch wraps it in a @c LocalKeypairSigner
 * before invoking the ISigner pipeline. Removed in v1.2 (Task 7's compile-define gate).
 *
 * @par Threading (AC-6, load-bearing)
 * @c sign_messages emits @c sign_completed @b synchronously via direct
 * @c emit_signal — NOT @c call_deferred. This preserves the pre-refactor invariant
 * that @c tx.signatures[index] is populated by the time @c tx.sign() returns.
 *
 * §2.3.1's "Godot main thread" guarantee is satisfied trivially: @c Keypair::sign_message
 * is synchronous on the calling thread, which is Godot main for any GDScript-driven
 * @c tx.sign() invocation. 's @c call_deferred mandate is scoped to cross-thread
 * Kotlin→GDScript marshalling (architecture §7.1), and does not apply here.
 *
 * @par Lifetime (Concern 4 resolution)
 * No self-pin needed. With synchronous emit, the local @c Ref<LocalKeypairSigner>
 * constructed in @c Transaction::sign_at_index is valid through the entire
 * @c emit_signal chain — the signal fires INSIDE @c sign_messages's call frame,
 * which fires INSIDE @c sign_at_index's call frame, which holds the local @c Ref
 * alive. Contrast: @c WalletAdapterSigner self-pins because @c WalletAdapter's
 * @c message_signed signal fires asynchronously after @c sign_message returns.
 */
class LocalKeypairSigner : public ISigner {
	GDCLASS_CUSTOM(LocalKeypairSigner, ISigner)

private:
	godot::Ref<godot::Keypair> kp_;

protected:
	/// Registers the GDScript-exposed methods (inherited @c ISigner signals are bound on the parent).
	static void _bind_methods();

public:
	LocalKeypairSigner() = default;
	~LocalKeypairSigner() override = default;

	/// Install the wrapped @c Keypair. The previous reference is released.
	void set_keypair(const godot::Ref<godot::Keypair> &kp);

	/// @return The currently wrapped @c Keypair (may be @c null if @ref set_keypair was never called).
	[[nodiscard]] godot::Ref<godot::Keypair> get_keypair() const;

	/// @copydoc ISigner::is_connected
	/// True iff a non-null @c Keypair is wrapped.
	[[nodiscard]] bool is_connected() const override;

	/// @copydoc ISigner::get_public_key
	[[nodiscard]] godot::String get_public_key() const override;

	/// @copydoc ISigner::sign_messages
	/// Emits @c sign_completed @b synchronously on the calling thread (the wrapped
	/// @c Keypair::sign_message is synchronous). See class-level threading note.
	void sign_messages(const godot::PackedByteArray &messages_concat,
			const godot::PackedInt32Array &lengths,
			const godot::String &request_id) override;

	/// @copydoc ISigner::sign_transactions
	/// Emits @c sign_completed @b synchronously on the calling thread. See class-level threading note.
	void sign_transactions(const godot::PackedByteArray &transactions_concat,
			const godot::PackedInt32Array &lengths,
			const godot::String &request_id) override;
};

} //namespace godot_solana_sdk
