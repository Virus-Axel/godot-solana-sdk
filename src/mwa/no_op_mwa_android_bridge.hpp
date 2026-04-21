// Non-Android implementation of MwaAndroidBridge. Every op emits
// mwa_error{code=UNSUPPORTED_PLATFORM} via the injected dispatcher
// (Story 1-4 Task 2, AC-2).
#ifndef GODOT_SOLANA_SDK_MWA_NO_OP_MWA_ANDROID_BRIDGE_HPP
#define GODOT_SOLANA_SDK_MWA_NO_OP_MWA_ANDROID_BRIDGE_HPP

#include "mwa_android_bridge.hpp"

namespace godot_solana_sdk::mwa {

class GodotMainDispatcher;

class NoOpMwaAndroidBridge : public MwaAndroidBridge {
public:
    explicit NoOpMwaAndroidBridge(GodotMainDispatcher* dispatcher);
    ~NoOpMwaAndroidBridge() override = default;

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

    NoOpMwaAndroidBridge(const NoOpMwaAndroidBridge&) = delete;
    NoOpMwaAndroidBridge& operator=(const NoOpMwaAndroidBridge&) = delete;

private:
    // Helper: build the standard mwa_error{UNSUPPORTED_PLATFORM, ...} payload and post.
    void emit_unsupported(const godot::String& source_method,
                          const godot::String& request_id);

    GodotMainDispatcher* dispatcher_;  // non-owning — dispatcher outlives the bridge.
};

}  // namespace godot_solana_sdk::mwa

#endif  // GODOT_SOLANA_SDK_MWA_NO_OP_MWA_ANDROID_BRIDGE_HPP
