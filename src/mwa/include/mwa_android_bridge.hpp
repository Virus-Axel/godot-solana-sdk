// ABI hygiene: this header must contain only godot:: types — header-lint (Story 1-4) enforces.
// See docs/architecture.md §7.1.1 (DD-26) and Story 1-4 AC-1.
#ifndef GODOT_SOLANA_SDK_MWA_ANDROID_BRIDGE_HPP
#define GODOT_SOLANA_SDK_MWA_ANDROID_BRIDGE_HPP

#include <cstdint>

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot_solana_sdk::mwa {

class GodotMainDispatcher;

class MwaAndroidBridge {
public:
    virtual ~MwaAndroidBridge() = default;

    static MwaAndroidBridge* create(GodotMainDispatcher* dispatcher);

    virtual void connect(const godot::String& request_id,
                         const godot::Dictionary& identity,
                         const godot::String& cluster,
                         const godot::Dictionary& opts) = 0;

    virtual void reauthorize(const godot::String& request_id,
                             const godot::Dictionary& opts) = 0;

    virtual void disconnect(const godot::String& request_id,
                            const godot::Dictionary& opts) = 0;

    virtual void deauthorize(const godot::String& request_id,
                             const godot::Dictionary& opts) = 0;

    virtual void sign_messages(const godot::String& request_id,
                               const godot::TypedArray<godot::PackedByteArray>& messages,
                               const godot::Dictionary& opts) = 0;

    virtual void sign_transactions(const godot::String& request_id,
                                   const godot::TypedArray<godot::PackedByteArray>& transactions,
                                   const godot::Dictionary& opts) = 0;

    virtual void sign_and_send(const godot::String& request_id,
                               const godot::TypedArray<godot::PackedByteArray>& transactions,
                               const godot::Dictionary& opts) = 0;

    virtual void forget_all(const godot::String& request_id) = 0;

    virtual void get_diagnostics(const godot::String& request_id) = 0;
};

}  // namespace godot_solana_sdk::mwa

#endif  // GODOT_SOLANA_SDK_MWA_ANDROID_BRIDGE_HPP
