#include <godot_cpp/classes/engine.hpp>

#include "test_validator.hpp"

namespace godot {

void TestValidator::_bind_methods() {
    ClassDB::add_signal("TestValidator", MethodInfo("fees_stabilized"));
}

void TestValidator::queue_start(){
    UtilityFunctions::print("TestValidator is ready.");
    int res = start_validator(1);
    UtilityFunctions::print("start_validator returned: " + String::num_int64(res));
}

void TestValidator::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    thread.instantiate();
    thread->start(callable_mp(this, &TestValidator::queue_start), Thread::PRIORITY_NORMAL);
}

void TestValidator::_process(double delta){
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    if (thread.is_null()){
        return;
    }

    if(!thread->is_alive()){
        thread->wait_to_finish();
        UtilityFunctions::print("TestValidator thread has finished.");
        thread.unref();
        emit_signal("fees_stabilized");
    }
}

void TestValidator::_notification(int p_what) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    switch (p_what) {
        case NOTIFICATION_EXIT_TREE: {
            if (thread.is_valid()) {
                thread->wait_to_finish();
            }

            thread.unref();
        }
    }
}

} //namespace godot