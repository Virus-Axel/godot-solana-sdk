// Story 1-5 Task 6 fixture — intentional violations of pattern 6
// (emit-signal-outside-dispatcher). Self-test run asserts pattern 6 fires.
//
// Real MWA C++ code MUST NOT contain any of these lines outside
// src/mwa/godot_main_dispatcher.cpp (the sole TU allowed to call emit_signal
// directly, via drain_for_testing under MWA_TESTING). Every other path routes
// through dispatcher.post() → call_deferred (production) or the pending_
// queue (test mode).
//
// The "emit_signal" STRING literal inside call_deferred (seen in the real
// production dispatcher as `target->call_deferred(StringName("emit_signal"),
// ...)`) is NOT matched by pattern 6 because the identifier there is not
// followed by '(' — the '"' terminator intervenes. This fixture exercises
// the three real call-site shapes.
this->emit_signal("mwa_error");
target->emit_signal(signal_name, payload);
emit_signal("connect_completed", request_id, result);
