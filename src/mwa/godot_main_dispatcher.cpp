#include "godot_main_dispatcher.hpp"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#ifdef MWA_TESTING
#include <godot_cpp/core/mutex_lock.hpp>   // MutexLock RAII for drain_for_testing
#endif

namespace godot_solana_sdk::mwa {

GodotMainDispatcher::GodotMainDispatcher(godot::Object* target) {
    // Loud null-check — UB-avoidance (nullptr deref on target->get_instance_id()).
    // ERR_FAIL_NULL_MSG returns void on failure, leaving emit_signal_callable_
    // default-constructed (invalid Callable). Subsequent post() calls through
    // an invalid Callable produce an internal godot-cpp warning and are dropped.
    ERR_FAIL_NULL_MSG(target,
        "GodotMainDispatcher: target must be a non-null live godot::Object* at construction.");
    // bind Callable carrying target+method+ObjectID as an atomic handle.
    // Callable internally holds an ObjectID for lifetime-aware dispatch — a
    // freed target produces a no-op internal warning inside godot-cpp rather
    // than a UAF (closure).
    emit_signal_callable_ = godot::Callable(target, "emit_signal");
#ifdef MWA_TESTING
    // target_id_ retained only for drain_for_testing's synchronous
    // dispatch path (Callable::call_deferred is asynchronous — the test
    // harness contract expects sync emit after workers joined).
    target_id_ = target->get_instance_id();
#endif
}

void GodotMainDispatcher::post(const godot::String& signal_name,
                               const godot::Array& args) {
#ifdef MWA_TESTING
    // enqueue for synchronous drain_for_testing. The host test binary has
    // no engine loop, so call_deferred would be a black hole. Test calls
    // drain_for_testing() to flush after std::thread workers have joined.
    //
    // key rename: `"payload"` (Dictionary) → `"args"` (Array).
    //
    // CR-T6-2: validate arity at ENQUEUE time so tests fail loudly at the
    // call site that produced the bad Array, not later inside
    // drain_for_testing(). Mirrors the production ladder's default arm so
    // the MWA_TESTING branch doesn't silently accept a 3-arity Array that
    // the production path would reject. Keep symmetric with the drain
    // ladder — if production grows to arity 3, bump this guard too.
    {
        const int n_in = args.size();
        if (n_in != 1 && n_in != 2) {
            ERR_FAIL_MSG(godot::vformat(
                "GodotMainDispatcher::post (MWA_TESTING): unsupported arity %d for signal '%s' (expected 1 or 2)",
                n_in, signal_name));
        }
    }
    godot::MutexLock lock(drain_mutex_);
    godot::Dictionary entry;
    entry["signal_name"] = signal_name;
    entry["args"] = args;
    pending_.push_back(entry);
    return;
#else
    // arity ladder: godot-cpp's Callable exposes `call_deferred` as a
    // vararg template (extension_api.json: `is_vararg: true`) but NO
    // `call_deferredv(Array)` equivalent — `callv` is synchronous. Hand-rolling
    // a `const Variant**` argv is not an option on the public surface. So we
    // unpack the Array at the call site with a bounded ladder; `default`
    // fires ERR_FAIL_MSG so an out-of-contract arity cannot ship silently.
    //
    // Arity upper bound is 2, bounded by (7 `*_completed` signals carry
    // 2 params; 4 error/lifecycle signals carry 1). If a future story needs
    // arity 3, it MUST file a new amendment AND extend this ladder AND the
    // parallel ladder in drain_for_testing() below.
    const int n = args.size();
    switch (n) {
        case 1:
            emit_signal_callable_.call_deferred(signal_name, args[0]);
            break;
        case 2:
            emit_signal_callable_.call_deferred(signal_name, args[0], args[1]);
            break;
        default:
            ERR_FAIL_MSG(godot::vformat(
                "GodotMainDispatcher::post: unsupported arity %d for signal '%s' (expected 1 or 2)",
                n, signal_name));
    }
#endif
}

#ifdef MWA_TESTING
void GodotMainDispatcher::drain_for_testing() {
    // Snapshot under lock, emit outside. emit_signal callbacks may re-enter
    // post() → must not hold drain_mutex_ across arbitrary user code.
    godot::Array snapshot;
    {
        godot::MutexLock lock(drain_mutex_);
        snapshot = pending_.duplicate();
        pending_.clear();
    }

    // drain_for_testing resolves through ObjectDB::get_instance against
    // the MWA_TESTING-only `target_id_` field, not through emit_signal_callable_.
    // Callable::call_deferred is asynchronous; the harness contract is
    // synchronous dispatch after workers joined.
    godot::Object* target = godot::ObjectDB::get_instance(target_id_);
    if (target == nullptr) {
        // Parity with production: target freed → drop silently for
        // deterministic tests.
        return;
    }

    const int n = snapshot.size();
    for (int i = 0; i < n; ++i) {
        godot::Dictionary entry = snapshot[i];
        godot::String signal_name = entry["signal_name"];
        godot::Array args = entry["args"];
        // parallel arity ladder — SYMMETRIC with the production ladder
        // above. Must grow in lockstep with production post() or the
        // retired-harness guarantee (retain-source clause) breaks.
        const int arity = args.size();
        switch (arity) {
            case 1:
                target->emit_signal(signal_name, args[0]);
                break;
            case 2:
                target->emit_signal(signal_name, args[0], args[1]);
                break;
            default:
                ERR_FAIL_MSG(godot::vformat(
                    "GodotMainDispatcher::drain_for_testing: unsupported arity %d for signal '%s' (expected 1 or 2)",
                    arity, signal_name));
        }
    }
}

godot::Array GodotMainDispatcher::snapshot_pending_for_testing() const {
    // Copy pending_ under lock. godot::Array::duplicate() produces a deep copy
    // (the nested Dictionary entries are also copied) — caller gets a stable
    // snapshot unaffected by subsequent post() calls from other threads.
    godot::MutexLock lock(drain_mutex_);
    return pending_.duplicate();
}
#endif

}  // namespace godot_solana_sdk::mwa
