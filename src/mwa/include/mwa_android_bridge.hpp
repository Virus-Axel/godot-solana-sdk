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
 *     banned only in public headers per DD-26). Double-delete and leaks are
 *     caller-side bugs.
 *   - The @c dispatcher passed to @ref create is held by the impl as a
 *     non-owning raw pointer for the bridge's lifetime. The CALLER MUST ensure
 *     the dispatcher outlives the bridge. In practice the owning node
 *     constructs the dispatcher and the bridge in a sequence where the
 *     dispatcher is declared first (destroyed last); deleting the dispatcher
 *     before the bridge is UB at the next @c post call.
 *   - Rationale: `unique_ptr` would express ownership at the type level
 *     but requires `<memory>` in the public header, which DD-26 forbids
 *     (ABI hygiene). Documentation on the declaration is the DD-26-compatible
 *     contract surface.
 */
class MwaAndroidBridge {
public:
    virtual ~MwaAndroidBridge() = default;

    /**
     * Factory. Returns a caller-owned heap-allocated bridge (non-Android: no-op
     * impl; Android: JNI stub through Story 1-4, real JNI in Story 2-1+).
     * @param dispatcher MUST be non-null and MUST outlive the returned bridge.
     *                   The impl holds a non-owning pointer to it.
     * @return caller-owned `MwaAndroidBridge*` — `delete` when done.
     *         Typical usage: `unique_ptr<MwaAndroidBridge> bridge_{MwaAndroidBridge::create(&dispatcher_)};`
     *         stored inside the owning node (impl-side; `<memory>` OK there).
     */
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
