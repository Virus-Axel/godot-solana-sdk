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
 * @c GodotMainDispatcher. Arch §2.2 component B; thread-marshalling.
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

	/**
	 * Initiate a wallet connect (authorize). Non-blocking; the wallet UI is
	 * shown and completion arrives via the @c connect_completed, @c mwa_error,
	 * or @c mwa_timeout signal.
	 *
	 * @param identity dictionary @c {"name": String, "icon_uri": String, "identity_uri": String}
	 *                 displayed by the wallet during signing prompts.
	 * @param cluster one of @c "devnet", @c "testnet", or @c "mainnet-beta".
	 * @param opts optional overrides @c {"timeout_ms": int, "chain_id": String};
	 *             omitted keys fall to internal defaults.
	 * @return the generated 8-lowercase-hex @c request_id echoed on the terminal signal.
	 *
	 * Renamed from the plan-spec @c connect to avoid shadowing @c Node::connect.
	 */
	godot::String mwa_connect(const godot::Dictionary &identity, const godot::String &cluster, const godot::Dictionary &opts);

	/**
	 * Silent re-authorization using the cached auth token (no wallet UI).
	 * Completion arrives via @c reauthorize_completed on success or @c mwa_error
	 * (@c NOT_CONNECTED / @c TOKEN_EXPIRED) / @c mwa_timeout otherwise.
	 *
	 * @param opts optional @c {"timeout_ms": int}; omitted keys fall to defaults.
	 */
	void reauthorize(const godot::Dictionary &opts);

	/**
	 * Tear down the active session. Completion arrives via @c disconnect_completed
	 * (or @c mwa_error on protocol failure). Local cache is retained — see
	 * @c deauthorize for full token revocation.
	 *
	 * Renamed from the plan-spec @c disconnect to avoid shadowing @c Node::disconnect.
	 */
	void mwa_disconnect();

	/**
	 * Revoke the wallet's auth_token and wipe local cache for this identity.
	 * Always clears local state regardless of remote outcome (threat model:
	 * returned/stolen device must not retain a working auth_token).
	 * Completion arrives via @c deauthorize_completed with
	 * @c {remote_revoke_succeeded, local_cache_cleared, warning}.
	 * Catastrophic local failures surface as @c mwa_error with @c PROTOCOL_ERROR.
	 */
	void deauthorize();

	/**
	 * Sign one or more byte-array messages with the connected wallet (wallet UI
	 * is shown for user confirmation). Completion arrives via
	 * @c sign_messages_completed carrying signed payloads, or @c mwa_error
	 * (@c NOT_CONNECTED / @c USER_CANCELED) / @c mwa_timeout otherwise.
	 *
	 * @param messages 1+ message byte buffers.
	 * @param opts optional @c {"timeout_ms": int}.
	 */
	void sign_messages(const godot::TypedArray<godot::PackedByteArray> &messages, const godot::Dictionary &opts);

	/**
	 * Sign one or more serialized Solana transactions (wallet UI shown).
	 * Completion arrives via @c sign_transactions_completed with the
	 * @c signed_transactions array, or @c mwa_error (@c NOT_CONNECTED /
	 * @c WALLET_REJECTED) / @c mwa_timeout otherwise.
	 *
	 * @param transactions 1+ serialized transaction byte buffers.
	 * @param opts optional @c {"timeout_ms": int}.
	 */
	void sign_transactions(const godot::TypedArray<godot::PackedByteArray> &transactions, const godot::Dictionary &opts);

	/**
	 * Sign one or more transactions AND submit them via the wallet to the
	 * cluster's RPC (single round-trip). Writes an encrypted breadcrumb to
	 * survive mid-flight process death — see the @c pending_submission_found
	 * signal for recovery semantics. Completion arrives via
	 * @c sign_and_send_completed with @c {signatures (base58), submitted_at,
	 * confirmation_status}, or @c mwa_error (@c NOT_CONNECTED, @c WALLET_REJECTED,
	 * @c NETWORK_OFFLINE, @c RPC_FAILED) / @c mwa_timeout / @c reauth_required.
	 *
	 * @param transactions 1+ serialized transaction byte buffers.
	 * @param opts optional @c {"timeout_ms": int}.
	 */
	void sign_and_send(const godot::TypedArray<godot::PackedByteArray> &transactions, const godot::Dictionary &opts);

	/// @return @c true after a successful connect, @c false otherwise. Backed by @c MwaSessionState.
	/// Renamed from the plan-spec @c is_connected to avoid shadowing @c Node::is_connected.
	[[nodiscard]] bool mwa_is_connected() const;

	/// @return Base58 public key of the connected wallet, or empty string pre-connect / on non-Android.
	[[nodiscard]] godot::String get_public_key() const;

	/// @return Cluster name (e.g. @c "devnet") of the active session, or empty string pre-connect.
	[[nodiscard]] godot::String get_cluster() const;

	/// @return Wallet-reported human-readable label, or empty string pre-connect.
	[[nodiscard]] godot::String get_wallet_label() const;

	/**
	 * @return 8-lowercase-hex HKDF-SHA256 fingerprint of the current @c auth_token, or empty
	 * string pre-connect. The fingerprint is stable within a single install lifecycle and
	 * rotates after @c deauthorize or @c forget_all (the per-install salt is rotated).
	 * Never the raw token — safe to attach to bug reports.
	 */
	[[nodiscard]] godot::String get_auth_token_fingerprint() const;

	/**
	 * @return A 12-key support-engineer triage snapshot @c {sdk_version, clientlib_ktx_version,
	 * godot_version, android_api_level, session_state, wallet_package, wallet_version,
	 * auth_token_fingerprint, cluster, last_n_correlation_trace, late_result_count,
	 * pending_submissions_count}. Zero auth-token bytes / signing key material / PII —
	 * safe to attach to bug reports. Returns the same shape with empty values on non-Android.
	 */
	[[nodiscard]] godot::Dictionary get_diagnostics();

	/**
	 * @return 4-key dictionary @c {rooted, debuggable, developer_options_on, adb_enabled}
	 * (all @c bool). Heuristic only — sourced from @c Build.TAGS / @c ApplicationInfo.FLAG_DEBUGGABLE
	 * / @c Settings.Secure.DEVELOPMENT_SETTINGS_ENABLED / @c Settings.Global.ADB_ENABLED.
	 *
	 * @warning Non-authoritative; a determined attacker can mask each signal at the OS level.
	 * Do NOT use as a security gate. Returns 4-key all-@c false on non-Android.
	 */
	[[nodiscard]] godot::Dictionary get_device_posture();

	/**
	 * GDPR/CCPA "Sign out everywhere" hard reset. Wipes every cached wallet token across all
	 * identities, rotates the encryption master key (Android Keystore alias), cancels every
	 * in-flight op via @c mwa_cancelled_lifecycle, and best-effort remote-deauthorizes each
	 * cached wallet (offline wallets do not block the local wipe).
	 *
	 * Fire-and-forget — no completion signal. Verify via @c mwa_is_connected returning @c false.
	 */
	void forget_all();

#ifdef MWA_TESTING
	// test-only. Replaces the owned bridge with a caller-supplied one
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
