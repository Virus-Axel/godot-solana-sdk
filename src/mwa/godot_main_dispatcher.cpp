#include "godot_main_dispatcher.hpp"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

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
}

}  // namespace godot_solana_sdk::mwa
