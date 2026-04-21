#include "mock_mwa_android_bridge.hpp"

#include <thread>

#if defined(__linux__) && !defined(__ANDROID__)
#include <pthread.h>
#elif defined(__APPLE__)
#include <pthread.h>
#endif

#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

#include "godot_main_dispatcher.hpp"

namespace godot_solana_sdk::mwa {

MockMwaAndroidBridge::MockMwaAndroidBridge(GodotMainDispatcher* dispatcher)
    : dispatcher_(dispatcher) {}  // recorded_calls_ default-constructs.

void MockMwaAndroidBridge::record(const godot::String& method,
                                  const godot::String& request_id,
                                  godot::Dictionary args) {
    godot::Dictionary entry;
    entry["method"] = method;
    entry["request_id"] = request_id;
    entry["args"] = args;
    recorded_calls_.append(entry);
}

void MockMwaAndroidBridge::connect(const godot::String& request_id,
                                   const godot::Dictionary& identity,
                                   const godot::String& cluster,
                                   const godot::Dictionary& opts) {
    godot::Dictionary args;
    args["identity"] = identity;
    args["cluster"] = cluster;
    args["opts"] = opts;
    record(godot::String("connect"), request_id, args);
}

void MockMwaAndroidBridge::reauthorize(const godot::String& request_id,
                                       const godot::Dictionary& opts) {
    godot::Dictionary args;
    args["opts"] = opts;
    record(godot::String("reauthorize"), request_id, args);
}

void MockMwaAndroidBridge::disconnect(const godot::String& request_id,
                                      const godot::Dictionary& opts) {
    godot::Dictionary args;
    args["opts"] = opts;
    record(godot::String("disconnect"), request_id, args);
}

void MockMwaAndroidBridge::deauthorize(const godot::String& request_id,
                                       const godot::Dictionary& opts) {
    godot::Dictionary args;
    args["opts"] = opts;
    record(godot::String("deauthorize"), request_id, args);
}

void MockMwaAndroidBridge::sign_messages(
    const godot::String& request_id,
    const godot::TypedArray<godot::PackedByteArray>& messages,
    const godot::Dictionary& opts) {
    godot::Dictionary args;
    args["messages"] = messages;
    args["opts"] = opts;
    record(godot::String("sign_messages"), request_id, args);
}

void MockMwaAndroidBridge::sign_transactions(
    const godot::String& request_id,
    const godot::TypedArray<godot::PackedByteArray>& transactions,
    const godot::Dictionary& opts) {
    godot::Dictionary args;
    args["transactions"] = transactions;
    args["opts"] = opts;
    record(godot::String("sign_transactions"), request_id, args);
}

void MockMwaAndroidBridge::sign_and_send(
    const godot::String& request_id,
    const godot::TypedArray<godot::PackedByteArray>& transactions,
    const godot::Dictionary& opts) {
    godot::Dictionary args;
    args["transactions"] = transactions;
    args["opts"] = opts;
    record(godot::String("sign_and_send"), request_id, args);
}

void MockMwaAndroidBridge::forget_all(const godot::String& request_id) {
    godot::Dictionary args;
    record(godot::String("forget_all"), request_id, args);
}

void MockMwaAndroidBridge::get_diagnostics(const godot::String& request_id) {
    godot::Dictionary args;
    record(godot::String("get_diagnostics"), request_id, args);
}

godot::Array MockMwaAndroidBridge::get_recorded_calls() const {
    return recorded_calls_;
}

void MockMwaAndroidBridge::clear_calls() {
    recorded_calls_.clear();
}

void MockMwaAndroidBridge::drive_signal_from_thread(const godot::String& thread_name,
                                                    const godot::String& signal_name,
                                                    godot::Dictionary payload) {
    ERR_FAIL_NULL_MSG(dispatcher_,
        "MockMwaAndroidBridge::drive_signal_from_thread requires a non-null dispatcher (construct with one).");

    // Capture CharString by VALUE into the lambda closure (UB avoidance).
    //
    // A naïve `name_utf8 = thread_name.utf8().get_data()` captures a const char*
    // into a temporary godot::CharString that is destroyed at the end of the full
    // expression constructing std::thread — BEFORE the worker thread runs. Reading
    // that pointer inside the lambda is a use-after-free (often invisible on
    // tolerant allocators; tripped by ASAN/MSAN; intermittent under stress).
    //
    // Capturing the CharString by value gives the lambda ownership; .get_data()
    // inside the lambda yields a pointer into still-alive storage. Freed when the
    // lambda (and hence the owning std::thread) is destroyed after worker.join().
    //
    // Note on godot::Dictionary ref-semantics: `payload_copy = payload` captures
    // a copy of the Dictionary handle, which is ref-counted — the underlying data
    // is shared with the caller. If the caller mutates `payload` between this
    // function returning and the Godot main thread draining the deferred call,
    // the emitted signal observes the mutated state. Test authors typically do
    // not mutate post-post; documented in the header contract.
    std::thread worker([dispatcher = dispatcher_,
                        name_cs = thread_name.utf8(),
                        signal_copy = signal_name,
                        payload_copy = payload]() {
        const char* name_utf8 = name_cs.get_data();
        // Best-effort thread-naming. Linux limits names to 15 chars (16 w/ NUL);
        // longer names fail with ERANGE (return value intentionally unused —
        // thread names are for debuggers, not correctness). All guards exclude
        // __ANDROID__ so AC-5 grep (Task 6) remains quiet on compound forms
        // (verified 2026-04-21 against Task 6's regex — compound `#if A && !B`
        // is not matched); mock never compiles on Android production anyway
        // (src/mwa/testing/ is outside SConstruct's production source glob).
#if defined(__linux__) && !defined(__ANDROID__)
        // bionic vs glibc both provide this signature.
        pthread_setname_np(pthread_self(), name_utf8);
#elif defined(__APPLE__)
        // macOS: self only, no pthread_t arg.
        pthread_setname_np(name_utf8);
#else
        // Windows / other: no-op. Thread name is for debugging; functional behavior is unaffected.
        (void)name_utf8;
#endif
        dispatcher->post(signal_copy, payload_copy);
    });

    worker.join();  // synchronous for tests: post executes before caller proceeds.
}

}  // namespace godot_solana_sdk::mwa
