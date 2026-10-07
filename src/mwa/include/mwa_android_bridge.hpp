// ABI hygiene: this header must contain only godot:: types — header-lint enforces.
// See docs/architecture.md §7.1.1 and AC-1.
#ifndef GODOT_SOLANA_SDK_MWA_ANDROID_BRIDGE_HPP
#define GODOT_SOLANA_SDK_MWA_ANDROID_BRIDGE_HPP

#include <cstdint>

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot_solana_sdk::mwa {

class GodotMainDispatcher;

/**
 * Platform-agnostic C++ bridge for the Android Mobile Wallet Adapter.
 *
 * The ABC fronts either `MwaAndroidBridgeJni` (Android) or `NoOpMwaAndroidBridge`
 * (non-Android) via the factory @ref create. Every op method takes a
 * @c request_id for correlation and returns void — results arrive
 * asynchronously as signals emitted through the injected @c GodotMainDispatcher.
 *
 * Ownership & lifetime contract:
 *   - @ref create returns a naked @c MwaAndroidBridge* that the CALLER OWNS.
 *     The bridge is heap-allocated with `new`; the caller is responsible for
 *     deleting it (typically via `unique_ptr<MwaAndroidBridge>` stored
 *     inside the owning node — `<memory>` is allowed in impl files; it is
 * banned only in public headers per). Double-delete and leaks are
 *     caller-side bugs.
 *   - The @c dispatcher passed to @ref create is held by the impl as a
 *     non-owning raw pointer for the bridge's lifetime. The CALLER MUST ensure
 *     the dispatcher outlives the bridge. In practice the owning node
 *     constructs the dispatcher and the bridge in a sequence where the
 *     dispatcher is declared first (destroyed last); deleting the dispatcher
 *     before the bridge is UB at the next @c post call.
 *   - Rationale: `unique_ptr` would express ownership at the type level
 * but requires `<memory>` in the public header, which forbids
 * (ABI hygiene). Documentation on the declaration is the
 *     contract surface.
 */
class MwaAndroidBridge {
public:
    virtual ~MwaAndroidBridge() = default;

    /**
     * Factory. Returns a caller-owned heap-allocated bridge (non-Android: no-op
     * impl; Android: JNI stub through , real JNI in +).
     * @param dispatcher MUST be non-null and MUST outlive the returned bridge.
     *                   The impl holds a non-owning pointer to it.
     * @return caller-owned `MwaAndroidBridge*` — `delete` when done.
     *         Typical usage: `unique_ptr<MwaAndroidBridge> bridge_{MwaAndroidBridge::create(&dispatcher_)};`
     *         stored inside the owning node (impl-side; `<memory>` OK there).
     */
    static MwaAndroidBridge* create(GodotMainDispatcher* dispatcher);

    /**
     * Begin a wallet-connect (authorize) op. Asynchronous; the terminal result is delivered
     * via @c connect_completed / @c mwa_error / @c mwa_timeout signals carrying @p request_id.
     * @param request_id caller-supplied correlation id (8-lowercase-hex).
     * @param identity wallet-facing identity dict (@c name, @c icon_uri, @c identity_uri).
     * @param cluster e.g. @c "devnet" / @c "mainnet-beta".
     * @param opts optional overrides (@c timeout_ms, @c chain_id).
     */
    virtual void connect(const godot::String& request_id,
                         const godot::Dictionary& identity,
                         const godot::String& cluster,
                         const godot::Dictionary& opts) = 0;

    /**
     * Silent re-authorization using the cached auth_token (no wallet UI). Asynchronous; result
     * arrives on @c reauthorize_completed / @c mwa_error / @c mwa_timeout.
     * @param request_id caller-supplied correlation id.
     * @param opts optional @c {timeout_ms}.
     */
    virtual void reauthorize(const godot::String& request_id,
                             const godot::Dictionary& opts) = 0;

    /**
     * Tear down the active session (local-only — token cache retained). Asynchronous; result
     * arrives on @c disconnect_completed / @c mwa_error.
     */
    virtual void disconnect(const godot::String& request_id,
                            const godot::Dictionary& opts) = 0;

    /**
     * Revoke the wallet's auth_token AND wipe the local cache for this identity. Always wipes
     * local state. Result arrives on @c deauthorize_completed (always — carries
     * @c remote_revoke_succeeded flag) or @c mwa_error for catastrophic local failures only.
     */
    virtual void deauthorize(const godot::String& request_id,
                             const godot::Dictionary& opts) = 0;

    /**
     * Sign one or more raw message buffers with the connected wallet (wallet UI shown).
     * Result arrives on @c sign_messages_completed / @c mwa_error / @c mwa_timeout.
     * @param request_id caller-supplied correlation id.
     * @param messages 1+ message byte buffers.
     * @param opts optional @c {timeout_ms}.
     */
    virtual void sign_messages(const godot::String& request_id,
                               const godot::TypedArray<godot::PackedByteArray>& messages,
                               const godot::Dictionary& opts) = 0;

    /**
     * Sign one or more serialized Solana transactions (wallet UI shown).
     * Result arrives on @c sign_transactions_completed / @c mwa_error / @c mwa_timeout.
     * @param request_id caller-supplied correlation id.
     * @param transactions 1+ serialized transaction byte buffers.
     * @param opts optional @c {timeout_ms}.
     */
    virtual void sign_transactions(const godot::String& request_id,
                                   const godot::TypedArray<godot::PackedByteArray>& transactions,
                                   const godot::Dictionary& opts) = 0;

    /**
     * Sign one or more transactions and submit them via the wallet to the cluster RPC.
     * Writes an encrypted breadcrumb before the wallet round-trip for crash recovery
     * (see @c pending_submission_found). Result arrives on @c sign_and_send_completed /
     * @c mwa_error / @c mwa_timeout / @c reauth_required.
     * @param request_id caller-supplied correlation id.
     * @param transactions 1+ serialized transaction byte buffers.
     * @param opts optional @c {timeout_ms}.
     */
    virtual void sign_and_send(const godot::String& request_id,
                               const godot::TypedArray<godot::PackedByteArray>& transactions,
                               const godot::Dictionary& opts) = 0;

    /**
     * GDPR/CCPA "Sign out everywhere" reset — best-effort remote deauth + full local wipe +
     * Keystore master-key rotation + cancellation of every in-flight op. Fire-and-forget;
     * no completion signal is emitted by this bridge.
     */
    virtual void forget_all(const godot::String& request_id) = 0;

    /**
     * synchronous pull of the AC-1 12-key
     * diagnostics payload as a JSON String for @c MobileWalletAdapter::get_diagnostics
     * to parse into a Dictionary. Replaces the async
     * @c get_diagnostics(request_id) seam (deleted) — architecture §6.2 has
     * always specified @c get_diagnostics() as SYNC ≤1ms; the async stub was
     * a placeholder that drifted from the spec.
     *
     * Impl contract:
     * - NoOp returns the 12-key all-empty JSON String.
     *   - Jni delegates to the Kotlin @c mwaQueryDiagnosticsFromJni companion
     *     entry; returns the 12-key all-empty JSON on any JNI failure path.
     *
     * Synchronous by design: callers are on the Godot main thread; must be
     * cheap (≤1ms — no coroutine, no disk, no network).
     */
    [[nodiscard]] virtual godot::String query_diagnostics_json() const = 0;

    /**
     * (AC-4) — synchronous pull of the 4-key device posture
     * payload as a JSON String for @c MobileWalletAdapter::get_device_posture
     * to parse into a Dictionary.
     *
     * Impl contract:
     * - NoOp returns the 4-key all-false JSON String.
     *   - Jni delegates to the Kotlin @c mwaQueryDevicePostureFromJni
     *     companion entry; returns the 4-key all-false JSON on any JNI
     *     failure path.
     */
    [[nodiscard]] virtual godot::String query_device_posture_json() const = 0;

    /**
     * synchronous pull of current session state for
     * @c MobileWalletAdapter's four state getters (+ surface).
     *
     * Returns a @c godot::Dictionary with these keys (all required):
     *   - @c is_connected : @c bool
     *   - @c public_key : @c godot::String (base58)
     *   - @c cluster : @c godot::String (e.g. "devnet")
     *   - @c wallet_label : @c godot::String
     *   - @c auth_token_fingerprint : @c godot::String (8 lowercase hex chars; empty pre-connect)
     *
     * Impl contract:
     *   - NoOp returns empty defaults (false / empty strings) on all platforms.
     *   - Jni returns the atomic snapshot via
     *     @c MwaJniContext::query_session_state (single JNI round-trip that
     *     reads @c MwaSessionState fields under its synchronized lock).
     *
     * Synchronous by design: callers are @c mwa_is_connected / @c get_public_key
     * / @c get_cluster / @c get_wallet_label / @c get_auth_token_fingerprint on
     * the Godot main thread — no coroutine / signal hop. Must be cheap.
     */
    [[nodiscard]] virtual godot::Dictionary query_session_state() const = 0;
};

}  // namespace godot_solana_sdk::mwa

#endif  // GODOT_SOLANA_SDK_MWA_ANDROID_BRIDGE_HPP
