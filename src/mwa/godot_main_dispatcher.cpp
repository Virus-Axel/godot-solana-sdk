#include "godot_main_dispatcher.hpp"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#ifdef MWA_TESTING
#include <godot_cpp/core/mutex_lock.hpp>   // MutexLock RAII for drain_for_testing
#endif

namespace godot_solana_sdk::mwa {

GodotMainDispatcher::GodotMainDispatcher(godot::Object* target)
    : target_id_() {  // default-init to invalid ObjectID; filled in body if target is valid.
    // Loud null-check — UB-avoidance (nullptr deref on target->get_instance_id()).
    // ERR_FAIL_NULL_MSG returns void on failure, leaving target_id_ at its invalid default.
    // Subsequent post() calls will hit the null ObjectDB::get_instance branch and warn.
    ERR_FAIL_NULL_MSG(target,
        "GodotMainDispatcher: target must be a non-null live godot::Object* at construction.");
    target_id_ = target->get_instance_id();
}

void GodotMainDispatcher::post(const godot::String& signal_name,
                               const godot::Dictionary& payload) {
#ifdef MWA_TESTING
    // D-2: enqueue for synchronous drain_for_testing(). The host test binary has
    // no engine loop, so call_deferred would be a black hole. Test calls
    // drain_for_testing() to flush after std::thread workers have joined.
    godot::MutexLock lock(drain_mutex_);
    godot::Dictionary entry;
    entry["signal_name"] = signal_name;
    entry["payload"] = payload;
    pending_.push_back(entry);
    return;
#else
    // Thread-safe handle resolution (Godot 4): ObjectDB::get_instance is the C++ API
    // for resolving an ObjectID; Object::get_instance_from_id is the GDScript-facing
    // equivalent. Repo convention (see src/transaction/transaction.cpp:361) uses
    // ObjectDB::get_instance — mirror that here.
    //
    // Race note: see class-level thread-safety contract — caller must guarantee
    // `target` outlives every in-flight post() call. The null-check below only
    // catches the post-teardown case (ObjectID resolves to null); a concurrent
    // free between the null-check and the virtual dispatch is caller-prevented.
    godot::Object* target = godot::ObjectDB::get_instance(target_id_);
    if (target == nullptr) {
        // AC-6: target freed or never registered — drop with a warning.
        // Signal name is included so the lost emission is identifiable in logs.
        // "MWA: " tag prefix mirrors the §7.2 logging convention (Kotlin SdkLog parity).
        // UtilityFunctions::push_warning is thread-safe in Godot 4 (PrintHandler lock).
        godot::UtilityFunctions::push_warning(
            godot::String("MWA: dispatcher post after destroy (signal=") + signal_name +
            godot::String(")"));
        return;
    }

    // DD-22: every signal to GDScript crosses to the Godot main thread via
    // call_deferred("emit_signal", ...). Never call emit_signal directly here.
    target->call_deferred(godot::StringName("emit_signal"), signal_name, payload);
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

    godot::Object* target = godot::ObjectDB::get_instance(target_id_);
    if (target == nullptr) {
        // Parity with production post(): target freed → drop silently (the
        // equivalent warning already fires from post()'s production path;
        // in test mode we want a clean no-op for deterministic tests).
        return;
    }

    const int n = snapshot.size();
    for (int i = 0; i < n; ++i) {
        godot::Dictionary entry = snapshot[i];
        godot::String signal_name = entry["signal_name"];
        godot::Dictionary payload = entry["payload"];
        // Under MWA_TESTING, direct emit_signal is safe because we're
        // synchronously invoked from the test's main thread AFTER worker
        // threads have joined. No thread-marshalling concern.
        target->emit_signal(signal_name, payload);
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
