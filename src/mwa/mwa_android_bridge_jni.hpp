// Android JNI implementation of MwaAndroidBridge (STUB for Story 1-4 Task 2).
// Real JNI calls arrive in Story 2-1. Currently every op emits
// mwa_error{code=NOT_CONNECTED} + logs a stub notice. NO actual JNI calls,
// NO JavaVM*, NO AttachCurrentThread.
#ifndef GODOT_SOLANA_SDK_MWA_MWA_ANDROID_BRIDGE_JNI_HPP
#define GODOT_SOLANA_SDK_MWA_MWA_ANDROID_BRIDGE_JNI_HPP

#include "mwa_android_bridge.hpp"

namespace godot_solana_sdk::mwa {

class GodotMainDispatcher;

class MwaAndroidBridgeJni : public MwaAndroidBridge {
public:
    explicit MwaAndroidBridgeJni(GodotMainDispatcher* dispatcher);
    ~MwaAndroidBridgeJni() override = default;

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
    // Helper: log the stub notice + build and post an mwa_error{NOT_CONNECTED} payload.
    void emit_not_connected_stub(const godot::String& source_method,
                                 const godot::String& request_id);

    GodotMainDispatcher* dispatcher_;  // non-owning — dispatcher outlives the bridge.
};

}  // namespace godot_solana_sdk::mwa

#endif  // GODOT_SOLANA_SDK_MWA_MWA_ANDROID_BRIDGE_JNI_HPP
