// ABI hygiene: this header must contain only godot:: types — header-lint enforces.
// See docs/architecture.md §7.1.1 and AC-1.
#ifndef GODOT_SOLANA_SDK_MWA_GODOT_MAIN_DISPATCHER_HPP
#define GODOT_SOLANA_SDK_MWA_GODOT_MAIN_DISPATCHER_HPP

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string.hpp>

#ifdef MWA_TESTING
#include <godot_cpp/classes/mutex.hpp>
#endif

namespace godot_solana_sdk::mwa {

/**
 * Thread-marshalling primitive: every MWA signal emission to GDScript crosses
 * to the Godot main thread through a @c GodotMainDispatcher. Bridge
 * implementations (no-op, JNI) hold a raw pointer to this dispatcher and
 * invoke @ref post from any thread.
 *
 * evolution (+ resolution):
 *   The production path no longer performs raw-pointer virtual dispatch on a
 *   reconstructed @c godot::Object*. Instead, the ctor binds a
 *   @c godot::Callable that carries target+method+ObjectID as an atomic
 *   handle; @ref post forwards through @c Callable::call_deferred. A freed
 *   target produces a no-op internal warning inside godot-cpp, not a UAF —
 * 's ABA hazard is structurally closed.
 *
 *   @ref post now takes a @c godot::Array payload. A hard-coded arity ladder
 *   over {1, 2} unpacks it into the vararg @c call_deferred. Arity is bounded
 * by (1 for error/lifecycle signals; 2 for @c *_completed signals);
 * any arity outside {1, 2} fires @c ERR_FAIL_MSG at the caller.
 *
 * Preconditions:
 *   - The @c target argument to the constructor must be non-null and live.
 *     Enforced with @c ERR_FAIL_NULL_MSG in the ctor (loud failure at debug,
 *     not UB). If the null-check fires, the Callable stays default-constructed
 *     and @ref post calls produce internal godot-cpp warnings when invoked.
 *
 * Thread-safety contract:
 *   - @ref post is safe to call from any thread. Callable dispatch resolves
 *     the target atomically and posts to Godot's main message queue; no
 *     raw-pointer hop in the caller's thread.
 *   - Under @c MWA_TESTING, @ref post enqueues into @c pending_ (mutex-guarded)
 *     and @c drain_for_testing resolves through @c ObjectDB::get_instance
 * synchronously against the retained @c target_id_ — matches the
 *     host-test harness contract (call after workers joined, expects sync
 * @c emit_signal). Preserved per 's retain-source clause for the
 * future headless-Godot tier.
 *
 * Lifetime:
 *   - The dispatcher is owned by the target node (constructed in the node's
 * ctor, destroyed with the node) and is NOT Godot-registered (no
 *     script-exposed class macro).
 *
 * Copy/move: deleted — a dispatcher is bound to a single node instance.
 */
class GodotMainDispatcher {
public:
    /**
     * Bind the dispatcher to a target node. Constructs an internal
     * @c godot::Callable carrying @p target plus its @c ObjectID so subsequent
     * @ref post calls can resolve the target atomically across threads.
     *
     * @param target the owning Godot @c Object whose @c emit_signal slot will
     *               receive forwarded calls. Must be non-null and live; a null
     *               @p target triggers @c ERR_FAIL_NULL_MSG and leaves the
     *               internal Callable default-constructed (subsequent
     *               @ref post calls produce godot-cpp internal warnings).
     */
    explicit GodotMainDispatcher(godot::Object* target);

    /**
     * Post a signal emission to the Godot main thread.
     *
     * @param signal_name the GDScript-registered signal name.
     * @param args length-1 or length-2 @c godot::Array matching the signal
     *             registration. Arity outside {1, 2} fires @c ERR_FAIL_MSG.
     *
     * arity contract:
     *   - 1-element array: error/lifecycle signals (@c mwa_error,
     *     @c mwa_timeout, @c mwa_cancelled_lifecycle, @c reauth_required).
     *     Build with @c godot::Array::make(payload_dict).
     *   - 2-element array: @c *_completed signals
     *     (@c connect_completed, @c reauthorize_completed, etc.).
     *     Build with @c godot::Array::make(request_id_string, result_dict).
     */
    void post(const godot::String& signal_name, const godot::Array& args);

    // Copy/move semantics: dispatcher is node-owned, not meant to move. Move
    // ops are implicitly deleted by the copy=delete, but declaring them
    // explicitly is hygiene for a class holding a value-member `godot::Mutex`
    // (see ownership note below + `_THREAD_SAFE_CLASS_` sanction cited there).
    GodotMainDispatcher(const GodotMainDispatcher&) = delete;
    GodotMainDispatcher& operator=(const GodotMainDispatcher&) = delete;
    GodotMainDispatcher(GodotMainDispatcher&&) = delete;
    GodotMainDispatcher& operator=(GodotMainDispatcher&&) = delete;

#ifdef MWA_TESTING
    // test-only synchronous drain. Under MWA_TESTING, `post` enqueues into
    // `pending_` (mutex-guarded) instead of calling `call_deferred` — the host
    // test binary has no Godot engine loop to drain it. Test code calls this
    // method after any worker thread (e.g.
    // MockMwaAndroidBridge::drive_signal_from_thread) has been joined, to
    // synchronously invoke target->emit_signal with (name, args-unpacked) for
    // each queued entry. Production builds (MWA_TESTING undefined) do NOT
    // declare this method; `post()` uses `Callable::call_deferred` as above.
    //
    // Thread-safety: emit loop runs OUTSIDE the mutex (signal handlers may
    // re-enter post() — must not hold the lock across arbitrary callback code).
    // Snapshot+clear under lock, emit from snapshot without lock.
    //
    // arity ladder: uses an identical {1, 2} ladder against the
    // post-resolved Object::emit_signal path — symmetrical with the
    // production Callable::call_deferred ladder in post(). If one ladder
    // grows, the other MUST grow in the same commit or the retired-harness
    // guarantee breaks.
    void drain_for_testing();

    // Returns a COPY of `pending_` under lock without mutating state. Tests use
    // this to assert on posted-but-not-yet-drained entries (signal_name +
    // args) without invoking emit_signal — in host-mode test binaries
    // emit_signal has no connected callbacks to observe, so snapshot-then-drain
    // is the practical assertion pattern. Each element is a
    // godot::Dictionary{"signal_name": String, "args": Array}. T6 renamed the
    // args-key from "payload" (Dictionary) to "args" (Array).
    godot::Array snapshot_pending_for_testing() const;
#endif

private:
    // production field: Callable carrying target+method+ObjectID as an
    // atomic handle. Bound in ctor to `godot::Callable(target, "emit_signal")`.
    // Replaces the raw `ObjectDB::get_instance(target_id_) → target->call_deferred`
    // hop that identified as the ABA-hazard seam.
    godot::Callable emit_signal_callable_;

#ifdef MWA_TESTING
    // target_id_ is MWA_TESTING-only — retained for drain_for_testing
    // because Callable::call_deferred is asynchronous (posts to the message
    // queue) and the test harness contract requires synchronous dispatch after
    // workers join. Resolved via `godot::ObjectDB::get_instance(target_id_)`
    // inside drain_for_testing() only; production path never reads this field.
    godot::ObjectID target_id_;

    // {signal_name, args} entries awaiting drain. godot::Array + godot::Mutex
    // (Godot 4 value-member pattern per godot-cpp/include/godot_cpp/core/mutex_lock.hpp:52
    // `mutable Mutex _thread_safe_;`) instead of the STL `vector`+`mutex` pair
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
