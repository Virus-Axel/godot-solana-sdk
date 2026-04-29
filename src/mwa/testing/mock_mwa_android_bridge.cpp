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

godot::String MockMwaAndroidBridge::query_diagnostics_json() const {
    // Story 5-2 T3 — mirrors NoOp's 12-key all-empty payload. Tests that
    // exercise non-empty diagnostics will (in T5+) extend this to a
    // setter-injected snapshot; today this default keeps existing test
    // fixtures unchanged.
    return godot::String(
        "{\"sdk_version\":\"\","
        "\"clientlib_ktx_version\":\"\","
        "\"godot_version\":\"\","
        "\"android_api_level\":0,"
        "\"session_state\":{},"
        "\"wallet_package\":\"\","
        "\"wallet_version\":\"\","
        "\"auth_token_fingerprint\":\"\","
        "\"cluster\":\"\","
        "\"last_n_correlation_trace\":[],"
        "\"late_result_count\":0,"
        "\"pending_submissions_count\":0}");
}

godot::String MockMwaAndroidBridge::query_device_posture_json() const {
    // Story 5-2 T3 — mirrors NoOp's 4-key all-false payload.
    return godot::String(
        "{\"rooted\":false,"
        "\"debuggable\":false,"
        "\"developer_options_on\":false,"
        "\"adb_enabled\":false}");
}

godot::Array MockMwaAndroidBridge::get_recorded_calls() const {
    return recorded_calls_;
}

void MockMwaAndroidBridge::clear_calls() {
    recorded_calls_.clear();
}

godot::Dictionary MockMwaAndroidBridge::query_session_state() const {
    // Tests stage the snapshot via set_session_state_for_testing. When unset,
    // return the same empty-default shape NoOp uses so default-cast code paths
    // (non-connected state) work without explicit test setup.
    if (session_state_snapshot_.is_empty()) {
        godot::Dictionary out;
        out["is_connected"] = false;
        out["public_key"] = godot::String();
        out["cluster"] = godot::String();
        out["wallet_label"] = godot::String();
        out["auth_token_fingerprint"] = godot::String();
        return out;
    }
    return session_state_snapshot_.duplicate();
}

void MockMwaAndroidBridge::set_session_state_for_testing(const godot::Dictionary& snapshot) {
    session_state_snapshot_ = snapshot.duplicate();
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
        // D-6: dispatcher->post now takes a godot::Array. For the mock's
        // generalized signal-driver, treat every caller payload as a 1-arity
        // wrap. Tests that need 2-arity (*_completed) signals should call
        // dispatcher->post directly or extend this helper with an arity param.
        dispatcher->post(signal_copy, godot::Array::make(payload_copy));
    });

    worker.join();  // synchronous for tests: post executes before caller proceeds.
}

}  // namespace godot_solana_sdk::mwa
