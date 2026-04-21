#include "mwa_android_bridge_jni.hpp"

#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "generated/mwa_error_codes.hpp"
#include "godot_main_dispatcher.hpp"

namespace godot_solana_sdk::mwa {

MwaAndroidBridgeJni::MwaAndroidBridgeJni(GodotMainDispatcher* dispatcher)
    : dispatcher_(nullptr) {  // default-init; filled in body if dispatcher is valid.
    // Loud null-check — UB-avoidance. ERR_FAIL_NULL_MSG returns void on failure,
    // leaving dispatcher_ at nullptr so subsequent op calls early-return.
    ERR_FAIL_NULL_MSG(dispatcher,
        "MwaAndroidBridgeJni: dispatcher must be non-null.");
    dispatcher_ = dispatcher;
}

void MwaAndroidBridgeJni::emit_not_connected_stub(const godot::String& source_method,
                                                  const godot::String& request_id) {
    if (dispatcher_ == nullptr) { return; }  // defensive; should have failed at ctor.

    // 1) Log the stub notice via print_line (no severity distinction in C++ yet; Kotlin
    //    SdkLog exists but a C++ parallel is not yet shipped — see story Dev Notes).
    godot::UtilityFunctions::print_line(
        godot::String("MWA: op=") + source_method +
        godot::String(" request_id=") + request_id +
        godot::String(" (JNI stub — real implementation arrives in Story 2-1)"));

    // 2) Build + post the NOT_CONNECTED mwa_error.
    //
    // Payload discipline for the 1-4 JNI stub (differs from a live-adapter
    // NOT_CONNECTED that Story 2-1+ will emit):
    //   - `recoverable = false` and `retry_hint = "none"`: retry cannot
    //     succeed in this SDK version — the JNI bridge has no implementation
    //     yet. A caller that retries on `recoverable + retry_hint=retry_now`
    //     would spin forever; `false` prevents the loop AND disambiguates the
    //     stub payload from a real live-adapter NOT_CONNECTED (which ships
    //     `recoverable=true, retry_hint="connect"` when Story 2-1 lands).
    //   - `user_message` says "not yet available in this build" — honest and
    //     actionable without leaking internal "stub" vocabulary into UI
    //     toasts / error dialogs. The "stub" breadcrumb is in
    //     `developer_details` (technical log field) instead.
    godot::Dictionary payload;
    payload["request_id"] = request_id;
    payload["code"] = godot::String(code_name(MwaErrorCode::NOT_CONNECTED));
    payload["message"] =
        godot::String("MWA JNI bridge stub — real implementation arrives in Story 2-1");
    payload["user_message"] =
        godot::String("MWA is not yet available in this build.");
    payload["developer_details"] =
        godot::String("Story 1-4 JNI stub; no real bridge yet. ")
        + godot::String("connect/authorize impl lands in Story 2-1. ")
        + godot::String("Retry will not succeed in this SDK version.");
    payload["recoverable"] = false;
    payload["retry_hint"] = godot::String("none");
    payload["layer"] = godot::String("cpp");
    payload["cause"] = godot::String("");
    payload["source_method"] = source_method;
    dispatcher_->post(godot::String("mwa_error"), payload);
}

void MwaAndroidBridgeJni::connect(const godot::String& request_id,
                                  const godot::Dictionary& /*identity*/,
                                  const godot::String& /*cluster*/,
                                  const godot::Dictionary& /*opts*/) {
    emit_not_connected_stub(godot::String("connect"), request_id);
}

void MwaAndroidBridgeJni::reauthorize(const godot::String& request_id,
                                      const godot::Dictionary& /*opts*/) {
    emit_not_connected_stub(godot::String("reauthorize"), request_id);
}

void MwaAndroidBridgeJni::disconnect(const godot::String& request_id,
                                     const godot::Dictionary& /*opts*/) {
    emit_not_connected_stub(godot::String("disconnect"), request_id);
}

void MwaAndroidBridgeJni::deauthorize(const godot::String& request_id,
                                      const godot::Dictionary& /*opts*/) {
    emit_not_connected_stub(godot::String("deauthorize"), request_id);
}

void MwaAndroidBridgeJni::sign_messages(const godot::String& request_id,
                                        const godot::TypedArray<godot::PackedByteArray>& /*messages*/,
                                        const godot::Dictionary& /*opts*/) {
    emit_not_connected_stub(godot::String("sign_messages"), request_id);
}

void MwaAndroidBridgeJni::sign_transactions(const godot::String& request_id,
                                            const godot::TypedArray<godot::PackedByteArray>& /*transactions*/,
                                            const godot::Dictionary& /*opts*/) {
    emit_not_connected_stub(godot::String("sign_transactions"), request_id);
}

void MwaAndroidBridgeJni::sign_and_send(const godot::String& request_id,
                                        const godot::TypedArray<godot::PackedByteArray>& /*transactions*/,
                                        const godot::Dictionary& /*opts*/) {
    emit_not_connected_stub(godot::String("sign_and_send"), request_id);
}

void MwaAndroidBridgeJni::forget_all(const godot::String& request_id) {
    emit_not_connected_stub(godot::String("forget_all"), request_id);
}

void MwaAndroidBridgeJni::get_diagnostics(const godot::String& request_id) {
    emit_not_connected_stub(godot::String("get_diagnostics"), request_id);
}

}  // namespace godot_solana_sdk::mwa
