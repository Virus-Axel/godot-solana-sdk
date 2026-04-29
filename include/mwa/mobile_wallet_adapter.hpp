#ifndef GODOT_SOLANA_SDK_MOBILE_WALLET_ADAPTER_HPP
#define GODOT_SOLANA_SDK_MOBILE_WALLET_ADAPTER_HPP

#include <memory>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "godot_main_dispatcher.hpp"
#include "mwa_android_bridge.hpp"
#include "solana_utils.hpp"

namespace godot_solana_sdk {

/**
 * @brief Godot-facing MWA node. Registers 11 signals + 13 methods (7 ops + 4
 * state getters + 2 utility) and delegates op calls to @c MwaAndroidBridge via
 * @c GodotMainDispatcher. Arch §2.2 component B; DD-22 thread-marshalling.
 *
 * @par Ownership
 * Owns one @c GodotMainDispatcher (constructed with @c this as target) and one
 * @c std::unique_ptr<MwaAndroidBridge> produced by @c MwaAndroidBridge::create.
 * Declaration order is dispatcher-first, bridge-second so destruction (reverse
 * order) tears down the bridge before the dispatcher it points to.
 */
class MobileWalletAdapter : public godot::Node {
	GDCLASS_CUSTOM(MobileWalletAdapter, godot::Node)

private:
	mwa::GodotMainDispatcher dispatcher_;
	std::unique_ptr<mwa::MwaAndroidBridge> bridge_;

protected:
	/**
	 * @brief Register 11 signals + 13 GDScript-callable methods.
	 */
	static void _bind_methods();

public:
	MobileWalletAdapter();
	~MobileWalletAdapter() override = default;

	// Copy/move suppression comes from GDCLASS_CUSTOM (private empty operator=)
	// + owned std::unique_ptr<MwaAndroidBridge> (deletes implicit copy). No
	// explicit `= delete` here: it would collide with the GDCLASS-generated
	// `private: void operator=(const m_class&) {}` on redefinition.

	// 7 ops — delegate to bridge (T2 wires bodies). D-1 rename applied to
	// mwa_connect / mwa_disconnect to avoid godot::Object inherited-method
	// collision.
	//
	// Story 2-1 T7 (Rule 2 expansion from T6): mwa_connect returns the
	// generated 8-hex-char request_id so the GDScript MWA.gd facade can
	// return the correlation ID to callers (T7 spec: `connect(...) -> String`).
	// The remaining 6 op methods stay void — the facade exposes `void` for
	// them per the T7 spec.
	godot::String mwa_connect(const godot::Dictionary &identity, const godot::String &cluster, const godot::Dictionary &opts);
	void reauthorize(const godot::Dictionary &opts);
	void mwa_disconnect();
	void deauthorize();
	void sign_messages(const godot::TypedArray<godot::PackedByteArray> &messages, const godot::Dictionary &opts);
	void sign_transactions(const godot::TypedArray<godot::PackedByteArray> &transactions, const godot::Dictionary &opts);
	void sign_and_send(const godot::TypedArray<godot::PackedByteArray> &transactions, const godot::Dictionary &opts);

	// 4 state getters — Story 2-1 T6 wires these to MwaSessionState via
	// bridge->query_session_state() (JNI round-trip on Android; empty defaults
	// on non-Android / MWA_TESTING null bridge). D-1 rename applied to
	// mwa_is_connected.
	[[nodiscard]] bool mwa_is_connected() const;
	[[nodiscard]] godot::String get_public_key() const;
	[[nodiscard]] godot::String get_cluster() const;
	[[nodiscard]] godot::String get_wallet_label() const;

	// Story 2-1 T6 — AC-7 fingerprint surface for MWA.gd facade (T7). Reads
	// the same MwaSessionState snapshot as the 4 state getters above; returns
	// empty string pre-connect or on any JNI failure path. 8 lowercase hex
	// chars post-connect, as computed by AuthTokenFingerprint in T3.
	[[nodiscard]] godot::String get_auth_token_fingerprint() const;

	// 2 utility — Story 5-2 wires get_diagnostics; Story 4-2 wires forget_all.
	[[nodiscard]] godot::Dictionary get_diagnostics();
	// Story 5-2 T3 (AC-4) — synchronous device posture surface. Returns the
	// 4-key Dictionary `{rooted, debuggable, developer_options_on, adb_enabled}`
	// (all bool). Non-authoritative — sourced via Android Build/Settings APIs;
	// see AC-4 + DD-32 runbook reference.
	[[nodiscard]] godot::Dictionary get_device_posture();
	void forget_all();

#ifdef MWA_TESTING
	// D-3: test-only. Replaces the owned bridge with a caller-supplied one
	// (or nullptr to exercise the AC-3 UNSUPPORTED_PLATFORM pre-op branch).
	// Production builds do not define MWA_TESTING; this symbol does not exist
	// in production. CI grep-ban `mwa-testing-define` (T6) guards against
	// MWA_TESTING leaking into release builds.
	void set_bridge_for_testing(std::unique_ptr<mwa::MwaAndroidBridge> bridge);

	// Test-only reference to the owned GodotMainDispatcher. Tests reach this
	// to call drain_for_testing() / snapshot_pending_for_testing() and to
	// construct a MockMwaAndroidBridge sharing the same dispatcher instance.
	// Symmetric with set_bridge_for_testing — both gated by MWA_TESTING.
	mwa::GodotMainDispatcher &dispatcher_for_testing();
#endif
};

} //namespace godot_solana_sdk

#endif // GODOT_SOLANA_SDK_MOBILE_WALLET_ADAPTER_HPP
