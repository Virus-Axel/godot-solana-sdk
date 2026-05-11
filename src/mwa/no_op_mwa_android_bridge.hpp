// Non-Android implementation of MwaAndroidBridge. Every op emits
// mwa_error{code=UNSUPPORTED_PLATFORM} via the injected dispatcher
// (Task 2, AC-2).
#ifndef GODOT_SOLANA_SDK_MWA_NO_OP_MWA_ANDROID_BRIDGE_HPP
#define GODOT_SOLANA_SDK_MWA_NO_OP_MWA_ANDROID_BRIDGE_HPP

#include "mwa_android_bridge.hpp"

namespace godot_solana_sdk::mwa {

class GodotMainDispatcher;

/**
 * @brief Non-Android stub implementation of MwaAndroidBridge.
 *
 * Every op overrides emits `mwa_error{code=UNSUPPORTED_PLATFORM}` via the
 * injected dispatcher. Selected at link time on linux/windows/macos/web/ios
 * by `platform_selector.cpp` so consumer GDScript can still call MWA ops on
 * non-Android targets and receive a structured error rather than a crash
 * (Task 2, AC-2).
 */
class NoOpMwaAndroidBridge : public MwaAndroidBridge {
public:
    /// Construct the stub with a non-owning @p dispatcher used to emit
    /// @c mwa_error{code=UNSUPPORTED_PLATFORM} for every op.
    explicit NoOpMwaAndroidBridge(GodotMainDispatcher* dispatcher);
    ~NoOpMwaAndroidBridge() override = default;

    /// @copydoc MwaAndroidBridge::connect
    /// On this stub: emits @c mwa_error{UNSUPPORTED_PLATFORM}.
    void connect(const godot::String& request_id,
                 const godot::Dictionary& identity,
                 const godot::String& cluster,
                 const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::reauthorize
    /// On this stub: emits @c mwa_error{UNSUPPORTED_PLATFORM}.
    void reauthorize(const godot::String& request_id,
                     const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::disconnect
    /// On this stub: emits @c mwa_error{UNSUPPORTED_PLATFORM}.
    void disconnect(const godot::String& request_id,
                    const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::deauthorize
    /// On this stub: emits @c mwa_error{UNSUPPORTED_PLATFORM}.
    void deauthorize(const godot::String& request_id,
                     const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::sign_messages
    /// On this stub: emits @c mwa_error{UNSUPPORTED_PLATFORM}.
    void sign_messages(const godot::String& request_id,
                       const godot::TypedArray<godot::PackedByteArray>& messages,
                       const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::sign_transactions
    /// On this stub: emits @c mwa_error{UNSUPPORTED_PLATFORM}.
    void sign_transactions(const godot::String& request_id,
                           const godot::TypedArray<godot::PackedByteArray>& transactions,
                           const godot::Dictionary& opts) override;
    /// @copydoc MwaAndroidBridge::sign_and_send
    /// On this stub: emits @c mwa_error{UNSUPPORTED_PLATFORM}.
    void sign_and_send(const godot::String& request_id,
                       const godot::TypedArray<godot::PackedByteArray>& transactions,
                       const godot::Dictionary& opts) override;

    /// @copydoc MwaAndroidBridge::forget_all
    /// On this stub: emits @c mwa_error{UNSUPPORTED_PLATFORM}.
    void forget_all(const godot::String& request_id) override;

    /// @copydoc MwaAndroidBridge::query_diagnostics_json
    /// On this stub: returns the 12-key all-empty JSON shape.
    godot::String query_diagnostics_json() const override;
    /// @copydoc MwaAndroidBridge::query_device_posture_json
    /// On this stub: returns the 4-key all-@c false JSON shape.
    godot::String query_device_posture_json() const override;

    /// @copydoc MwaAndroidBridge::query_session_state
    /// On this stub: returns empty defaults (never connected on non-Android).
    godot::Dictionary query_session_state() const override;

    NoOpMwaAndroidBridge(const NoOpMwaAndroidBridge&) = delete;
    NoOpMwaAndroidBridge& operator=(const NoOpMwaAndroidBridge&) = delete;

private:
    /// Build and post the standard @c mwa_error{UNSUPPORTED_PLATFORM, source_method, request_id} payload.
    void emit_unsupported(const godot::String& source_method,
                          const godot::String& request_id);

    GodotMainDispatcher* dispatcher_;  // non-owning — dispatcher outlives the bridge.
};

}  // namespace godot_solana_sdk::mwa

#endif  // GODOT_SOLANA_SDK_MWA_NO_OP_MWA_ANDROID_BRIDGE_HPP
