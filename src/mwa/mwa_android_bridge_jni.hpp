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
 * Lifetime: [MwaAndroidBridgeJni] calls [register] in its ctor and
 * [unregister] in its dtor. Re-entrancy-safe via atomic store — multiple
 * register calls from different bridges replace the previous pointer without
 * UB; an unregister paired with a live dispatcher reset to `nullptr` so
 * JNIEXPORT functions can detect the teardown and drop cleanly.
 *
 * Ownership: the dispatcher is NOT owned here — it's owned by the
 * [MobileWalletAdapter] node that constructed it. [get_dispatcher] returns a
 * non-owning raw pointer; callers MUST not `delete` it.
 */
class MwaJniContext {
public:
    // Register the active dispatcher. Idempotent: subsequent registers
    // overwrite the previous pointer atomically. Called from
    // [MwaAndroidBridgeJni] ctor.
    static void register_dispatcher(GodotMainDispatcher* dispatcher);

    // Clear the stored pointer. After this, [get_dispatcher] returns
    // `nullptr` and JNIEXPORT callbacks drop with a `push_warning`. Called
    // from [MwaAndroidBridgeJni] dtor.
    static void unregister_dispatcher();

    // Non-owning accessor for JNIEXPORT callbacks. May return `nullptr` if
    // the ctor hasn't run or the dtor already did — callers MUST null-check.
    static GodotMainDispatcher* get_dispatcher();
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
