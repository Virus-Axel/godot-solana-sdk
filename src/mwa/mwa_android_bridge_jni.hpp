// Android JNI implementation of MwaAndroidBridge (real JNI).
// This header is only INCLUDED on Android (platform_selector.cpp gates the
// include under its sanctioned `#ifdef __ANDROID__`). The CPP TU is excluded
// from non-Android builds by the SConstruct platform filter.
#ifndef GODOT_SOLANA_SDK_MWA_MWA_ANDROID_BRIDGE_JNI_HPP
#define GODOT_SOLANA_SDK_MWA_MWA_ANDROID_BRIDGE_JNI_HPP

#include "mwa_android_bridge.hpp"

namespace godot_solana_sdk::mwa {

class GodotMainDispatcher;

/**
 * Thread-safe singleton that holds the active [GodotMainDispatcher] pointer so
 * JNIEXPORT callbacks (Kotlin → C++) can route `postConnectCompletedNative` /
 * `postMwaErrorNative` / etc. through the dispatcher without threading the
 * pointer through the JVM.
 *
 * Lifetime: [MwaAndroidBridgeJni] calls [register_dispatcher] in its ctor and
 * [unregister_dispatcher] in its dtor.
 *
 * teardown-race barrier:
 *   An in-flight callback counter plus a draining flag protect the
 * hazard-class identified by (JNIEXPORT callback already past the
 *   null-check + mid-`dispatcher->post(...)` when the owning node is freed).
 *   JNIEXPORT callbacks acquire a scoped RAII lease via
 *   [acquire_callback_lease] BEFORE touching the dispatcher pointer;
 *   [unregister_dispatcher] sets the draining flag and spin-waits (bounded
 *   ~200ms) until the counter drops to zero, so no lease-holder is mid-post
 *   when the dispatcher object is destroyed.
 *
 *   The lease contract is EXCLUSIVE to the JNIEXPORT reverse-path helpers
 *   (`post_arity1`, `post_arity2_completed`); the forward-path op methods on
 *   [MwaAndroidBridgeJni] use the `dispatcher_` MEMBER pointer and are
 *   main-thread-only — the bridge object cannot be destroyed while main is
 *   executing one of its own methods, so no race.
 *
 * Ownership: the dispatcher is NOT owned here — it's owned by the
 * [MobileWalletAdapter] node that constructed it. [get_dispatcher] returns a
 * non-owning raw pointer; callers MUST not `delete` it.
 */
class MwaJniContext {
public:
    /**
     * Register the active dispatcher and clear the draining flag. Called from the
     * @c MwaAndroidBridgeJni ctor. A preceding @ref unregister_dispatcher MUST
     * have completed (its bounded spin-drain guarantees zero in-flight callbacks
     * at that point); re-registering without an intervening unregister overwrites
     * the pointer and accepts the ambiguity for any still-leased call.
     */
    static void register_dispatcher(GodotMainDispatcher* dispatcher);

    /**
     * Signal draining + spin-wait (bounded ~200 ms) for in-flight callbacks to
     * release their leases, then clear the dispatcher pointer. After return the
     * in-flight counter is zero (or the timeout fired with a logged warning).
     * Called from the @c MwaAndroidBridgeJni dtor.
     */
    static void unregister_dispatcher();

    /**
     * Non-owning accessor. Prefer @ref CallbackLease for JNIEXPORT callbacks —
     * this bare accessor returns a pointer SNAPSHOT that can become dangling
     * after return. Retained for diagnostic/test-only callers that do not post
     * through the dispatcher.
     */
    static GodotMainDispatcher* get_dispatcher();

    // RAII lease. Holding this guard PINS the dispatcher against a
    // concurrent [unregister_dispatcher]. Usage:
    //
    //   MwaJniContext::CallbackLease lease;
    //   if (lease.dispatcher() == nullptr) { ...dropped...; return; }
    //   lease.dispatcher()->post(...);
    //   // destructor decrements the in-flight counter.
    //
    // `dispatcher()` returns non-null iff we successfully acquired (not
    // draining + a dispatcher was registered). A null `dispatcher()` means
    // the callback should drop cleanly.
    /**
     * @brief RAII lease on a JNI callback in-flight slot.
     *
     * Construction increments the in-flight counter; destruction decrements
     * it. While at least one lease is alive, the JVM cannot detach the
     * thread without finishing the callback. `dispatcher()` returns the
     * registered `GodotMainDispatcher*` or nullptr if shutdown is in
     * progress (caller drops cleanly in the nullptr case).
     */
    class CallbackLease {
    public:
        CallbackLease();
        ~CallbackLease();
        CallbackLease(const CallbackLease&) = delete;
        CallbackLease& operator=(const CallbackLease&) = delete;
        CallbackLease(CallbackLease&&) = delete;
        CallbackLease& operator=(CallbackLease&&) = delete;

        /// @return The registered dispatcher pointer, or @c nullptr if the
        /// lease did not acquire (shutdown in progress / no dispatcher set).
        /// Safe to use while the lease object is alive.
        GodotMainDispatcher* dispatcher() const { return dispatcher_; }

    private:
        GodotMainDispatcher* dispatcher_;
        bool acquired_;
    };

    /**
     * Synchronous JNI round-trip to the Kotlin plugin returning an atomic
     * snapshot of @c MwaSessionState (@c is_connected, @c public_key,
     * @c cluster, @c wallet_label, @c auth_token_fingerprint). Called from
     * @c MwaAndroidBridgeJni's @c query_session_state override on the Godot main thread.
     * Returns an empty Dictionary if the JNI symbol cache is not ready, the
     * plugin instance is null, or the JVM call throws — all recoverable
     * "not yet connected" states that match the @c NoOp shape.
     */
    static godot::Dictionary query_session_state();
};

/**
 * @brief Real Android JNI implementation of MwaAndroidBridge.
 *
 * Forwards each MWA op into the JVM-side `GDExtensionAndroidPlugin` via JNI;
 * the Kotlin layer drives the wallet-handshake + transaction-signing flows
 * against the Solana Mobile clientlib-ktx. Results are marshalled back to
 * the Godot main thread via the injected `GodotMainDispatcher`.
 *
 * Only included on Android — `platform_selector.cpp` gates the include
 * under its sanctioned `#ifdef __ANDROID__`; the .cpp TU is excluded from
 * non-Android builds by the SConstruct platform filter.
 */
class MwaAndroidBridgeJni : public MwaAndroidBridge {
public:
    /**
     * Construct the JNI bridge and register @p dispatcher with @ref MwaJniContext
     * so JNIEXPORT reverse callbacks can route through it.
     * @param dispatcher non-owning; MUST outlive this bridge instance.
     */
    explicit MwaAndroidBridgeJni(GodotMainDispatcher* dispatcher);

    /**
     * Drain in-flight JNI callbacks (bounded ~200 ms spin) and unregister the
     * dispatcher from @ref MwaJniContext before destruction.
     */
    ~MwaAndroidBridgeJni() override;

    /// @copydoc MwaAndroidBridge::connect
    void connect(const godot::String& request_id,
                 const godot::Dictionary& identity,
                 const godot::String& cluster,
                 const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::reauthorize
    void reauthorize(const godot::String& request_id,
                     const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::disconnect
    void disconnect(const godot::String& request_id,
                    const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::deauthorize
    void deauthorize(const godot::String& request_id,
                     const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::sign_messages
    void sign_messages(const godot::String& request_id,
                       const godot::TypedArray<godot::PackedByteArray>& messages,
                       const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::sign_transactions
    void sign_transactions(const godot::String& request_id,
                           const godot::TypedArray<godot::PackedByteArray>& transactions,
                           const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::sign_and_send
    void sign_and_send(const godot::String& request_id,
                       const godot::TypedArray<godot::PackedByteArray>& transactions,
                       const godot::Dictionary& opts) override;

    /// @copydoc MwaAndroidBridge::forget_all
    void forget_all(const godot::String& request_id) override;

    /// @copydoc MwaAndroidBridge::query_diagnostics_json
    godot::String query_diagnostics_json() const override;
    /// @copydoc MwaAndroidBridge::query_device_posture_json
    godot::String query_device_posture_json() const override;

    /// @copydoc MwaAndroidBridge::query_session_state
    /// Delegates to @ref MwaJniContext::query_session_state for the synchronous JNI round-trip.
    godot::Dictionary query_session_state() const override;

    MwaAndroidBridgeJni(const MwaAndroidBridgeJni&) = delete;
    MwaAndroidBridgeJni& operator=(const MwaAndroidBridgeJni&) = delete;

private:
    /**
     * Emit @c NOT_CONNECTED through @p dispatcher_ when JNI symbol resolution failed
     * (@c JNI_OnLoad bug) and callers need an actionable error envelope instead of a silent drop.
     */
    void emit_jni_unavailable(const godot::String& source_method,
                              const godot::String& request_id);

    GodotMainDispatcher* dispatcher_;  // non-owning — dispatcher outlives the bridge.
};

}  // namespace godot_solana_sdk::mwa

#endif  // GODOT_SOLANA_SDK_MWA_MWA_ANDROID_BRIDGE_JNI_HPP
