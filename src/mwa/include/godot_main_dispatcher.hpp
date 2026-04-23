// ABI hygiene: this header must contain only godot:: types — header-lint (Story 1-4) enforces.
// See docs/architecture.md §7.1.1 (DD-26) and Story 1-4 AC-1.
#ifndef GODOT_SOLANA_SDK_MWA_GODOT_MAIN_DISPATCHER_HPP
#define GODOT_SOLANA_SDK_MWA_GODOT_MAIN_DISPATCHER_HPP

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

#ifdef MWA_TESTING
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/variant/array.hpp>
#endif

namespace godot_solana_sdk::mwa {

/**
 * Thread-marshalling primitive: every MWA signal emission to GDScript crosses
 * to the Godot main thread through a @c GodotMainDispatcher (DD-22). Bridge
 * implementations (no-op, JNI) hold a raw pointer to this dispatcher and
 * invoke @ref post from any thread; the dispatcher resolves the target via a
 * thread-safe @c godot::ObjectID handle and marshals the emission with
 * @c call_deferred("emit_signal", ...).
 *
 * Preconditions:
 *   - The @c target argument to the constructor must be non-null and live.
 *     Enforced with @c ERR_FAIL_NULL_MSG in the ctor (loud failure at debug,
 *     not UB). If the null-check fires, @ref post becomes a no-op that warns.
 *
 * Thread-safety contract:
 *   - @ref post is safe to call from any thread while the @b target outlives
 *     every in-flight @ref post call. The dispatcher itself guarantees the
 *     ObjectID→pointer resolution is atomic (Godot 4 @c ObjectDB uses an
 *     internal mutex), but the subsequent @c target->call_deferred virtual
 *     dispatch is a member call on a raw pointer — if another thread frees
 *     @c target between the null-check and the virtual dispatch, behavior is
 *     undefined. Callers MUST arrange for the target to outlive all threads
 *     that can possibly call @ref post. For MWA this means the Godot-side
 *     wrapper node must not be freed while JNI callbacks are in flight.
 *   - @c godot::UtilityFunctions::push_warning is thread-safe in Godot 4
 *     (synchronized via @c PrintHandler's internal lock) and remains safe to
 *     invoke from the bridge's non-main threads in the post-destroy path.
 *
 * TODO(story-2-1): the current TOCTOU window is inert in Story 1-4 (no-op
 *   + test paths never race) but WILL surface when the real JNI flow lands
 *   — JNI worker threads have lifetimes NOT synchronized with Godot main,
 *   so `_exit_tree` → node freed → JNI callback still in flight → dispatcher
 *   resolves stale pointer → call_deferred on freed memory → UAF. Story 2-1
 *   MUST specify the caller-side teardown protocol:
 *     (a) `_exit_tree` sets a destroying-flag and joins/cancels all in-flight
 *         JNI callbacks before the node destructor returns, OR
 *     (b) switch to a handle-carrying type (e.g. `godot::Callable` bound to
 *         `emit_signal` + target ObjectID), eliminating the raw-pointer hop.
 *   See retrospect/universal.md "Mutable holder + in-flight async state ⇒
 *   guard every setter against the state machine" — same lifecycle hazard
 *   class. Do not ship Story 2-1 without resolving this.
 *
 * Lifetime:
 *   - The dispatcher stores the target's @c godot::ObjectID, NOT a raw
 *     pointer, so it remains safe to call @ref post after the target is
 *     freed: the ObjectID will resolve to @c nullptr, the post is dropped
 *     with a @c push_warning identifying the signal name, and no crash
 *     occurs (AC-6).
 *   - The dispatcher is owned by the target node (constructed in the node's
 *     ctor, destroyed with the node) and is NOT Godot-registered (A-11 — no
 *     script-exposed class macro).
 *
 * Copy/move: deleted — a dispatcher is bound to a single node instance.
 *
 * Under @c MWA_TESTING, the class additionally exposes @ref drain_for_testing,
 * a synchronous harness for host-binary tests that lack a Godot engine loop.
 */
class GodotMainDispatcher {
public:
    explicit GodotMainDispatcher(godot::Object* target);
    void post(const godot::String& signal_name, const godot::Dictionary& payload);

    /**
     * Story 2-1 T5 transitional — arity-2 call_deferred for `*_completed`
     * signals per A-12. GDScript sees the signal as
     * `connect_completed(request_id, result)` — two typed params, matching
     * the `ADD_SIGNAL(MethodInfo(...))` registration in
     * `MobileWalletAdapter::_bind_methods`.
     *
     * REMOVED in T6: T6 evolves [post] to take `godot::Array` +
     * a hard-coded `{1, 2}` arity ladder (D-6); this helper dissolves into
     * that single unified entry point. Do NOT add new callers here beyond
     * the T5 JNIEXPORT `postConnectCompletedNative` path — any extension
     * points the T6 evolution naturally.
     */
    void post_arity2(const godot::String& signal_name,
                     const godot::String& request_id,
                     const godot::Dictionary& result);

    // Copy/move semantics: dispatcher is node-owned, not meant to move. Move
    // ops are implicitly deleted by the copy=delete, but declaring them
    // explicitly is hygiene for a class holding a value-member `godot::Mutex`
    // (see :110 ownership note + `_THREAD_SAFE_CLASS_` sanction cited there).
    GodotMainDispatcher(const GodotMainDispatcher&) = delete;
    GodotMainDispatcher& operator=(const GodotMainDispatcher&) = delete;
    GodotMainDispatcher(GodotMainDispatcher&&) = delete;
    GodotMainDispatcher& operator=(GodotMainDispatcher&&) = delete;

#ifdef MWA_TESTING
    // D-2: test-only synchronous drain. Under MWA_TESTING, `post()` enqueues into
    // `pending_` (mutex-guarded) instead of calling `call_deferred` — the host
    // test binary has no Godot engine loop to drain it. Test code calls this
    // method after any worker thread (e.g.
    // MockMwaAndroidBridge::drive_signal_from_thread) has been joined, to
    // synchronously invoke target->emit_signal with (name, payload) for each queued
    // entry. Production builds (MWA_TESTING undefined) do NOT declare this
    // method; `post()` uses `call_deferred` as before.
    //
    // Thread-safety: emit loop runs OUTSIDE the mutex (signal handlers may
    // re-enter post() — must not hold the lock across arbitrary callback code).
    // Snapshot+clear under lock, emit from snapshot without lock.
    void drain_for_testing();

    // Returns a COPY of `pending_` under lock without mutating state. Tests use
    // this to assert on posted-but-not-yet-drained entries (signal_name +
    // payload) without invoking emit_signal — in host-mode test binaries
    // emit_signal has no connected callbacks to observe, so snapshot-then-drain
    // is the practical assertion pattern. Each element is a
    // godot::Dictionary{"signal_name": String, "payload": Dictionary}.
    godot::Array snapshot_pending_for_testing() const;
#endif

private:
    godot::ObjectID target_id_;

#ifdef MWA_TESTING
    // D-2: {signal_name, payload} entries awaiting drain. godot::Array + godot::Mutex
    // (Godot 4 value-member pattern per godot-cpp/include/godot_cpp/core/mutex_lock.hpp:52
    // `mutable Mutex _thread_safe_;`) instead of the STL `vector`+`mutex` pair — DD-26
    // allow-list compliance (src/mwa/include/ restricted to godot:: + <cstdint>/<cstddef>).
    //
    // Value-member rationale for godot::Mutex (which inherits godot::RefCounted
    // and is normally held via godot::Ref<>): sanctioned directly by godot-cpp's
    // own `_THREAD_SAFE_CLASS_` macro (mutex_lock.hpp:52), which expands inside
    // arbitrary Object subclasses to `mutable Mutex _thread_safe_;` — the same
    // shape we use here. The Mutex wrapper's default ctor registers the OS
    // mutex through the extension interface at GodotMainDispatcher construction
    // (which happens after the GDExtension has bound, because the dispatcher is
    // owned by a Godot-registered Node whose ctor runs post-bind). Copy/move
    // ops on GodotMainDispatcher are explicitly `= delete` above, so the Mutex
    // value-member cannot be duplicated or moved out of a live dispatcher.
    godot::Array pending_;
    mutable godot::Mutex drain_mutex_;
#endif
};

}  // namespace godot_solana_sdk::mwa

#endif  // GODOT_SOLANA_SDK_MWA_GODOT_MAIN_DISPATCHER_HPP
