#pragma once

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/string.hpp"

#include "isigner.hpp"

namespace godot {
class WalletAdapter;
} //namespace godot

namespace godot_solana_sdk {

/**
 * @brief Bridge that adapts the existing desktop @c godot::WalletAdapter to the
 * @c ISigner abstraction.
 *
 * requires "narrow ISigner abstraction implemented by both desktop WalletAdapter
 * and Android MWA"; this class is the desktop side of that contract.
 *
 * @par Threading
 * The underlying @c WalletAdapter::message_signed signal fires asynchronously (out of
 * @c sign_message's call frame), so this bridge uses a self-pin pattern: a
 * @c Ref<WalletAdapterSigner> is held internally for the duration of the in-flight
 * request, keeping @c this alive across the async boundary. Contrast: @c LocalKeypairSigner
 * (Task 4) does not self-pin because its @c sign_message path is synchronous (AC-6).
 *
 * @par v1.1 limitation
 * @c WalletAdapter does not support concurrent sign requests. This bridge enforces a
 * single in-flight request at a time; concurrent @c sign_messages calls fail with @c BUSY.
 * The @c signer_index argument passed to @c WalletAdapter::sign_message is hardcoded to 0
 * because @c ISigner's contract does not expose a per-message index. Callers needing
 * multi-signer transactions should drive the bridge directly per-signer.
 */
class WalletAdapterSigner : public ISigner {
	GDCLASS_CUSTOM(WalletAdapterSigner, ISigner)

private:
	godot::WalletAdapter *wa_ = nullptr;

	bool request_in_flight_ = false;
	godot::String pending_request_id_;
	godot::PackedByteArray pending_concat_;
	godot::PackedInt32Array pending_lengths_;
	godot::Array pending_signatures_;
	int pending_offset_ = 0;
	godot::Ref<WalletAdapterSigner> self_pin_;

	void dispatch_next_slice();
	void disconnect_signals();
	void finish_success();
	void fail(const godot::String &error_code, const godot::String &message);

protected:
	/// Registers the GDScript-exposed methods and binds the slot handlers
	/// @ref _on_message_signed / @ref _on_signing_failed.
	static void _bind_methods();

public:
	WalletAdapterSigner() = default;
	~WalletAdapterSigner() override = default;

	/**
	 * Install the underlying desktop @c WalletAdapter. Accepts @c godot::Object* to keep
	 * the public API decoupled from the @c WalletAdapter type at the binding layer;
	 * an internal @c cast_to populates the typed pointer. Disconnects any previous
	 * adapter's signals before swapping. Must not be called while a request is in flight.
	 */
	void set_wallet_adapter(godot::Object *p_wa);

	/// @return The currently installed @c WalletAdapter as a @c godot::Object*, or @c nullptr
	/// if @ref set_wallet_adapter was never called.
	[[nodiscard]] godot::Object *get_wallet_adapter() const;

	/// @copydoc ISigner::is_connected
	/// True iff a non-null @c WalletAdapter is installed and reports itself connected.
	[[nodiscard]] bool is_connected() const override;

	/// @copydoc ISigner::get_public_key
	[[nodiscard]] godot::String get_public_key() const override;

	/// @copydoc ISigner::sign_messages
	/// Drives the wrapped @c WalletAdapter slice-by-slice; emits @c sign_completed
	/// asynchronously once every message has been signed. Concurrent calls return
	/// @c sign_failed with code @c BUSY (v1.1 single-in-flight limit).
	void sign_messages(const godot::PackedByteArray &messages_concat,
			const godot::PackedInt32Array &lengths,
			const godot::String &request_id) override;

	/// @copydoc ISigner::sign_transactions
	/// Routes through the same single-in-flight path as @ref sign_messages.
	void sign_transactions(const godot::PackedByteArray &transactions_concat,
			const godot::PackedInt32Array &lengths,
			const godot::String &request_id) override;

	/// Slot wired to @c WalletAdapter::message_signed. Appends @p signature to the pending
	/// result and either advances to the next slice or finalizes the request.
	void _on_message_signed(const godot::PackedByteArray &signature);

	/// Slot wired to @c WalletAdapter::signing_failed. Emits @c sign_failed and releases
	/// the in-flight slot (clears the self-pin).
	void _on_signing_failed();
};

} //namespace godot_solana_sdk
