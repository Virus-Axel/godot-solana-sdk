// Android JNI implementation of MwaAndroidBridge (Story 2-1 T5 — real JNI).
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
 * Story 2-1 T10 — CR-41 teardown-race barrier:
 *   An in-flight callback counter plus a draining flag protect the
 *   hazard-class identified by CR-41 (JNIEXPORT callback already past the
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
    // Register the active dispatcher + clear the draining flag. Called from
    // [MwaAndroidBridgeJni] ctor. A preceding
    // [unregister_dispatcher] MUST have completed (its bounded spin-drain
    // guarantees zero in-flight callbacks at that point); re-registering
    // without an intervening unregister overwrites the pointer and accepts
    // the ambiguity for any still-leased call.
    static void register_dispatcher(GodotMainDispatcher* dispatcher);

    // Signal draining + spin-wait (bounded ~200ms) for in-flight callbacks
    // to release their leases, then clear the dispatcher pointer. After
    // this returns the counter is zero (or the timeout fired with a
    // logged warning — see CR-41). Called from [MwaAndroidBridgeJni] dtor.
    static void unregister_dispatcher();

    // Non-owning accessor. Prefer [acquire_callback_lease] for JNIEXPORT
    // callbacks — this bare accessor returns a POINTER SNAPSHOT that can
    // become dangling after return. Retained for diagnostic/test-only
    // callers that do not post through the dispatcher.
    static GodotMainDispatcher* get_dispatcher();

    // CR-41 RAII lease. Holding this guard PINS the dispatcher against a
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
    class CallbackLease {
    public:
        CallbackLease();
        ~CallbackLease();
        CallbackLease(const CallbackLease&) = delete;
        CallbackLease& operator=(const CallbackLease&) = delete;
        CallbackLease(CallbackLease&&) = delete;
        CallbackLease& operator=(CallbackLease&&) = delete;
        GodotMainDispatcher* dispatcher() const { return dispatcher_; }

    private:
        GodotMainDispatcher* dispatcher_;
        bool acquired_;
    };

    // Story 2-1 T6 — synchronous JNI round-trip to the Kotlin plugin that
    // returns an atomic snapshot of `MwaSessionState` (is_connected,
    // public_key, cluster, wallet_label, auth_token_fingerprint). Called
    // from MwaAndroidBridgeJni::query_session_state() on the Godot main
    // thread. Returns an empty Dictionary if the JNI symbol cache is not
    // ready, if the plugin instance is null, or if the CallStatic throws —
    // all recoverable "not yet connected" states that match NoOp's shape.
    static godot::Dictionary query_session_state();
};

class MwaAndroidBridgeJni : public MwaAndroidBridge {
public:
    explicit MwaAndroidBridgeJni(GodotMainDispatcher* dispatcher);
    ~MwaAndroidBridgeJni() override;

    // 7 MWA ops — override signatures exactly match MwaAndroidBridge's pure-virtuals.
    void connect(const godot::String& request_id,
                 const godot::Dictionary& identity,
                 const godot::String& cluster,
                 const godot::Dictionary& opts) override;
    void reauthorize(const godot::String& request_id,
                     const godot::Dictionary& opts) override;
    void disconnect(const godot::String& request_id,
                    const godot::Dictionary& opts) override;
    void deauthorize(const godot::String& request_id,
                     const godot::Dictionary& opts) override;
    void sign_messages(const godot::String& request_id,
                       const godot::TypedArray<godot::PackedByteArray>& messages,
                       const godot::Dictionary& opts) override;
    void sign_transactions(const godot::String& request_id,
                           const godot::TypedArray<godot::PackedByteArray>& transactions,
                           const godot::Dictionary& opts) override;
    void sign_and_send(const godot::String& request_id,
                       const godot::TypedArray<godot::PackedByteArray>& transactions,
                       const godot::Dictionary& opts) override;

    // 2 lifecycle ops
    void forget_all(const godot::String& request_id) override;
    void get_diagnostics(const godot::String& request_id) override;

    // Story 2-1 T6 — delegates to MwaJniContext::query_session_state() which
    // performs a synchronous JNI round-trip to MwaSessionState (Kotlin).
    godot::Dictionary query_session_state() const override;

    MwaAndroidBridgeJni(const MwaAndroidBridgeJni&) = delete;
    MwaAndroidBridgeJni& operator=(const MwaAndroidBridgeJni&) = delete;

private:
    // Helper: emit NOT_CONNECTED when JNI_OnLoad failed and callers need an
    // actionable error envelope (not a silent drop).
    void emit_jni_unavailable(const godot::String& source_method,
                              const godot::String& request_id);

    GodotMainDispatcher* dispatcher_;  // non-owning — dispatcher outlives the bridge.
};

}  // namespace godot_solana_sdk::mwa

#endif  // GODOT_SOLANA_SDK_MWA_MWA_ANDROID_BRIDGE_JNI_HPP
