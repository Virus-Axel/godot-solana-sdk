#include "no_op_mwa_android_bridge.hpp"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/string.hpp>

#include "generated/mwa_error_codes.hpp"
#include "godot_main_dispatcher.hpp"

namespace godot_solana_sdk::mwa {

NoOpMwaAndroidBridge::NoOpMwaAndroidBridge(GodotMainDispatcher* dispatcher)
    : dispatcher_(nullptr) {  // default-init; filled in body if dispatcher is valid.
    // Loud null-check — UB-avoidance. ERR_FAIL_NULL_MSG returns void on failure,
    // leaving dispatcher_ at nullptr so subsequent op calls early-return.
    ERR_FAIL_NULL_MSG(dispatcher,
        "NoOpMwaAndroidBridge: dispatcher must be non-null.");
    dispatcher_ = dispatcher;
}

void NoOpMwaAndroidBridge::emit_unsupported(const godot::String& source_method,
                                            const godot::String& request_id) {
    if (dispatcher_ == nullptr) { return; }  // defensive; should have failed at ctor.
    godot::Dictionary payload;
    payload["request_id"] = request_id;
    payload["code"] = godot::String(code_name(MwaErrorCode::UNSUPPORTED_PLATFORM));
    godot::String os_name = godot::OS::get_singleton()->get_name();
    // `message` keeps the developer-readable form (required by AC-2 verbatim);
    // `user_message` is a UI-safe string with no internal identifiers;
    // `developer_details` carries the implementation-level breadcrumb.
    payload["message"] = godot::String("MWA is Android-only; OS.get_name()=") + os_name;
    payload["user_message"] =
        godot::String("Mobile wallet integration is only available on Android.");
    payload["developer_details"] =
        godot::String("Non-Android build: MwaAndroidBridge::create() returned NoOpMwaAndroidBridge on platform=")
        + os_name + godot::String(".");
    payload["recoverable"] = false;
    payload["retry_hint"] = godot::String("none");
    payload["layer"] = godot::String("cpp");
    payload["cause"] = godot::String("");
    payload["source_method"] = source_method;
    // 1-arity error/lifecycle signal — wrap payload in 1-elem Array.
    dispatcher_->post(godot::String("mwa_error"), godot::Array::make(payload));
}

godot::Dictionary NoOpMwaAndroidBridge::query_session_state() const {
    // Non-Android path never connects; state getters return empty defaults.
    godot::Dictionary out;
    out["is_connected"] = false;
    out["public_key"] = godot::String();
    out["cluster"] = godot::String();
    out["wallet_label"] = godot::String();
    out["auth_token_fingerprint"] = godot::String();
    return out;
}

void NoOpMwaAndroidBridge::connect(const godot::String& request_id,
                                   const godot::Dictionary& /*identity*/,
                                   const godot::String& /*cluster*/,
                                   const godot::Dictionary& /*opts*/) {
    emit_unsupported(godot::String("connect"), request_id);
}

void NoOpMwaAndroidBridge::reauthorize(const godot::String& request_id,
                                       const godot::Dictionary& /*opts*/) {
    emit_unsupported(godot::String("reauthorize"), request_id);
}

void NoOpMwaAndroidBridge::disconnect(const godot::String& request_id,
                                      const godot::Dictionary& /*opts*/) {
    emit_unsupported(godot::String("disconnect"), request_id);
}

void NoOpMwaAndroidBridge::deauthorize(const godot::String& request_id,
                                       const godot::Dictionary& /*opts*/) {
    emit_unsupported(godot::String("deauthorize"), request_id);
}

void NoOpMwaAndroidBridge::sign_messages(const godot::String& request_id,
                                         const godot::TypedArray<godot::PackedByteArray>& /*messages*/,
                                         const godot::Dictionary& /*opts*/) {
    emit_unsupported(godot::String("sign_messages"), request_id);
}

void NoOpMwaAndroidBridge::sign_transactions(const godot::String& request_id,
                                             const godot::TypedArray<godot::PackedByteArray>& /*transactions*/,
                                             const godot::Dictionary& /*opts*/) {
    emit_unsupported(godot::String("sign_transactions"), request_id);
}

void NoOpMwaAndroidBridge::sign_and_send(const godot::String& request_id,
                                         const godot::TypedArray<godot::PackedByteArray>& /*transactions*/,
                                         const godot::Dictionary& /*opts*/) {
    emit_unsupported(godot::String("sign_and_send"), request_id);
}

void NoOpMwaAndroidBridge::forget_all(const godot::String& request_id) {
    emit_unsupported(godot::String("forget_all"), request_id);
}

godot::String NoOpMwaAndroidBridge::query_diagnostics_json() const {
    // non-Android returns the 12-key
    // all-empty payload so GDScript callers can read MWA.get_diagnostics()
    // without platform branching. Shape mirrors
    // MwaDiagnosticsBuilder.emptyDiagnosticsJson on the Kotlin side.
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

godot::String NoOpMwaAndroidBridge::query_device_posture_json() const {
    // non-Android returns the 4-key all-false
    // payload. Mirrors MwaDevicePostureBuilder.emptyPostureJson on Kotlin.
    return godot::String(
        "{\"rooted\":false,"
        "\"debuggable\":false,"
        "\"developer_options_on\":false,"
        "\"adb_enabled\":false}");
}

}  // namespace godot_solana_sdk::mwa
