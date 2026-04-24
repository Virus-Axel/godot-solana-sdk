// Story 2-1 T5 / T6 — real Android JNI implementation of MwaAndroidBridge.
//
// T5 landed the forward-direction call path (C++ → Kotlin) and the
// reverse-direction JNIEXPORT callbacks. T6 evolved the dispatcher surface
// to a `(String, Array)` signature per D-6, dissolving the T5 transitional
// `post_arity2` into the unified `post` + arity ladder, and added
// `MwaJniContext::query_session_state` as a synchronous pull of
// `MwaSessionState` for the C++ node's four state getters (plus
// `get_auth_token_fingerprint` surfaced for T7's MWA.gd facade).
//
// This TU is excluded from non-Android builds by the SConstruct platform
// filter (src/mwa/mwa_android_bridge_jni.cpp is filtered out when
// env["platform"] != "android"). Per D-11, NO `#ifdef __ANDROID__` inside
// this file — the platform gate is at SConstruct level.
//
// Architecture overview:
//   Forward path (GDScript → C++ → Kotlin plugin):
//     MobileWalletAdapter::mwa_connect(...)
//         → MwaAndroidBridgeJni::connect(request_id, identity, cluster, opts)
//             → JNI: CallStaticVoidMethod(plugin_companion,
//                   mwaAuthorizeFromJni, reqId, identityJson, cluster, chainId, timeoutMs)
//         The Kotlin @JvmStatic wrapper forwards to
//         GDExtensionAndroidPlugin.mwaAuthorize (the T4 @UsedByGodot instance
//         method). The Kotlin coroutine runs the authorize op.
//
//   Reverse path (Kotlin plugin → C++ → dispatcher):
//     DefaultNativeBridge.postConnectCompleted(...)
//         → GDExtensionAndroidPlugin.Companion.postConnectCompletedNative (external fun)
//             → JNIEXPORT Java_..._postConnectCompletedNative in this file
//                 → MwaJniContext::get_dispatcher()->post("connect_completed",
//                       Dictionary{request_id, result_dict})
//                 → D-5 Callable + D-6 Array ladder (T6) crosses to Godot main.
//
// JNI caching: JNI_OnLoad captures JavaVM*, the plugin companion class, and
// every @JvmStatic method ID. FindClass / GetStaticMethodID are expensive and
// fail on worker threads without a preceding AttachCurrentThread — so we
// cache during load-time on the class-loader's main JNIEnv.
//
// Lifetime / null-safety:
//   - If JNI_OnLoad fails to cache any symbol (class not found, method ID
//     null), every op emits an mwa_error{NOT_CONNECTED} envelope with
//     `developer_details="JNI symbol cache failed"` — honest actionable
//     error instead of a silent no-op.
//   - MwaJniContext::get_dispatcher() is `nullptr` before bridge ctor runs
//     or after dtor; JNIEXPORT callbacks `push_warning` and drop in that case.

#include "mwa_android_bridge_jni.hpp"

#include <jni.h>

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "generated/mwa_error_codes.hpp"
#include "godot_main_dispatcher.hpp"

namespace godot_solana_sdk::mwa {

namespace {

// ---------------- JavaVM + JNI symbol cache ----------------

JavaVM* g_jvm = nullptr;
jclass g_plugin_companion_class = nullptr; // GlobalRef

// @JvmStatic wrappers on the plugin companion — one per forward op.
jmethodID g_mid_mwa_authorize_from_jni = nullptr;
jmethodID g_mid_mwa_disconnect_from_jni = nullptr;
jmethodID g_mid_mwa_reauthorize_from_jni = nullptr;
jmethodID g_mid_mwa_deauthorize_from_jni = nullptr;
jmethodID g_mid_mwa_sign_messages_from_jni = nullptr;
jmethodID g_mid_mwa_sign_transactions_from_jni = nullptr;
jmethodID g_mid_mwa_sign_and_send_from_jni = nullptr;
jmethodID g_mid_mwa_forget_all_from_jni = nullptr;
jmethodID g_mid_mwa_get_diagnostics_from_jni = nullptr;
// Story 2-1 T6 — reverse-direction sync getter: returns JSON string snapshot
// of MwaSessionState for the 4 C++ state getters.
jmethodID g_mid_mwa_query_session_state_from_jni = nullptr;

// True once JNI_OnLoad populated the cache without error. Read by op methods
// to decide between real JNI call vs. `emit_jni_unavailable`.
std::atomic<bool> g_jni_ready{false};

// Scoped AttachCurrentThread — detaches in dtor iff WE attached. No-op if
// the calling thread was already attached (e.g., Godot main).
class ScopedJniAttach {
public:
    ScopedJniAttach() : env_(nullptr), did_attach_(false) {
        if (g_jvm == nullptr) { return; }
        const jint rc = g_jvm->GetEnv(reinterpret_cast<void**>(&env_), JNI_VERSION_1_6);
        if (rc == JNI_EDETACHED) {
            if (g_jvm->AttachCurrentThread(&env_, nullptr) == JNI_OK) {
                did_attach_ = true;
            } else {
                env_ = nullptr;
            }
        } else if (rc != JNI_OK) {
            env_ = nullptr;
        }
    }

    ~ScopedJniAttach() {
        if (did_attach_ && g_jvm != nullptr) {
            g_jvm->DetachCurrentThread();
        }
    }

    JNIEnv* env() const { return env_; }

    ScopedJniAttach(const ScopedJniAttach&) = delete;
    ScopedJniAttach& operator=(const ScopedJniAttach&) = delete;

private:
    JNIEnv* env_;
    bool did_attach_;
};

// ---------------- godot::String <-> jstring converters ----------------
//
// WARNING: these converters use JNI modified-UTF-8 (GetStringUTFChars /
// NewStringUTF), which diverges from standard UTF-8 for:
//   - embedded U+0000 (encoded as 0xC0 0x80 in modified, single 0x00 in standard)
//   - supplementary characters U+10000+ (emoji) — encoded as surrogate-pair 3-byte
//     sequences in modified vs. single 4-byte sequences in standard
// godot::String / godot::JSON produce standard UTF-8; feeding modified UTF-8 bytes
// to godot::String(const char*) silently mangles non-BMP input.
//
// CURRENT TRAFFIC — all strings crossing this boundary are ASCII-only:
//   - request_id: 8 lowercase hex chars
//   - identityJson / resultDictJson / errorDictJson / etc.: godot::JSON / Kotlin
//     JSONObject output escapes non-ASCII to \uXXXX sequences (JSON spec permits
//     both forms; the implementations happen to prefer escaped form for non-BMP)
//   - cluster / chainId: "devnet" / "solana:devnet" etc. — ASCII enum-like
//   - public_key / auth_token: base64 / base58 / hex — all ASCII
// Non-ASCII would only arrive if a wallet_label or identity.name with emoji
// reaches this boundary unescaped, which the current JSON serialization path
// prevents.
// CR-38 tracks migrating to GetStringChars/NewString (UTF-16) for future-proofing.
godot::String jstring_to_godot_string(JNIEnv* env, jstring jstr) {
    if (jstr == nullptr) { return godot::String(); }
    const char* utf = env->GetStringUTFChars(jstr, nullptr);
    if (utf == nullptr) { return godot::String(); }
    godot::String out(utf);
    env->ReleaseStringUTFChars(jstr, utf);
    return out;
}

jstring godot_string_to_jstring(JNIEnv* env, const godot::String& s) {
    const godot::CharString cs = s.utf8();
    return env->NewStringUTF(cs.get_data());
}

// Serialize a godot::Dictionary into a JSON string for forward-path JNI calls
// (identity dict → JNI string arg). Never logs the result.
godot::String serialize_dict_to_json(const godot::Dictionary& dict) {
    return godot::JSON::stringify(dict);
}

// Extract timeout_ms from opts dict (may be absent — returns 0, which
// GDExtensionAndroidPlugin's effectiveWatchdog maps to the internal default).
int64_t opts_timeout_ms(const godot::Dictionary& opts) {
    if (!opts.has("timeout_ms")) { return 0; }
    const godot::Variant v = opts["timeout_ms"];
    if (v.get_type() == godot::Variant::INT) {
        return static_cast<int64_t>(v);
    }
    return 0;
}

// Extract chain_id from opts dict; defaults to "solana:<cluster>" shape when
// absent (Kotlin side validates canonical form via MwaClientImpl's
// canonicalChainIdFor).
godot::String opts_chain_id(const godot::Dictionary& opts, const godot::String& cluster) {
    if (opts.has("chain_id")) {
        return godot::String(opts["chain_id"]);
    }
    return godot::String("solana:") + cluster;
}

// Build + post a PROTOCOL_ERROR mwa_error for the JNI-exception-swallowed case.
// Preserves the terminal-signal invariant: Kotlin throwing synchronously during
// dispatch no longer orphans the request_id — GDScript observes exactly-one
// terminal signal (the mwa_error here) for that request.
void emit_jni_exception_error(GodotMainDispatcher* dispatcher,
                              const godot::String& request_id,
                              const char* method_name) {
    if (dispatcher == nullptr) { return; }
    godot::Dictionary payload;
    payload["request_id"] = request_id;
    payload["code"] = godot::String("PROTOCOL_ERROR");
    payload["message"] = godot::String("MWA JNI dispatch threw");
    payload["user_message"] = godot::String("The wallet request could not be dispatched.");
    payload["developer_details"] = godot::String("JNI exception during ") + godot::String(method_name);
    payload["recoverable"] = true;
    payload["retry_hint"] = godot::String("retry_now");
    payload["layer"] = godot::String("cpp");
    payload["cause"] = godot::String("jni_exception");
    payload["source_method"] = godot::String(method_name);
    // D-6: 1-arity error signal — wrap payload in 1-elem Array.
    dispatcher->post(godot::String("mwa_error"), godot::Array::make(payload));
}

// ---------------- Forward-path JNI invocation ----------------

// Call a cached @JvmStatic void method that takes (reqId, identityJson,
// cluster, chainId, timeoutMs). Used for mwaAuthorizeFromJni and
// reauthorizeFromJni.
void call_authorize_shape(jmethodID mid,
                          GodotMainDispatcher* dispatcher,
                          const godot::String& request_id,
                          const godot::String& identity_json,
                          const godot::String& cluster,
                          const godot::String& chain_id,
                          int64_t timeout_ms,
                          const char* method_name) {
    if (mid == nullptr) { return; }
    ScopedJniAttach attach;
    JNIEnv* env = attach.env();
    if (env == nullptr) { return; }
    jstring j_req = godot_string_to_jstring(env, request_id);
    jstring j_identity = godot_string_to_jstring(env, identity_json);
    jstring j_cluster = godot_string_to_jstring(env, cluster);
    jstring j_chain = godot_string_to_jstring(env, chain_id);
    env->CallStaticVoidMethod(g_plugin_companion_class, mid, j_req, j_identity,
                              j_cluster, j_chain, static_cast<jlong>(timeout_ms));
    if (env->ExceptionCheck() == JNI_TRUE) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        // Preserve terminal-signal invariant (AC-6): orphan request_id would leave
        // the game with no signal fired. Emit mwa_error with PROTOCOL_ERROR so the
        // game's signal handlers see exactly one terminal signal.
        emit_jni_exception_error(dispatcher, request_id, method_name);
    }
    env->DeleteLocalRef(j_req);
    env->DeleteLocalRef(j_identity);
    env->DeleteLocalRef(j_cluster);
    env->DeleteLocalRef(j_chain);
}

// Call a cached @JvmStatic void method that takes a single (reqId) arg.
// Used for disconnect / deauthorize / forget_all / get_diagnostics.
void call_reqid_only_shape(jmethodID mid,
                           GodotMainDispatcher* dispatcher,
                           const godot::String& request_id,
                           const char* method_name) {
    if (mid == nullptr) { return; }
    ScopedJniAttach attach;
    JNIEnv* env = attach.env();
    if (env == nullptr) { return; }
    jstring j_req = godot_string_to_jstring(env, request_id);
    env->CallStaticVoidMethod(g_plugin_companion_class, mid, j_req);
    if (env->ExceptionCheck() == JNI_TRUE) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        emit_jni_exception_error(dispatcher, request_id, method_name);
    }
    env->DeleteLocalRef(j_req);
}

}  // anonymous namespace

// ---------------- MwaJniContext impl (+ CR-41 teardown barrier) ----------------
//
// CR-41 resolution (Story 2-1 T10): JNIEXPORT callbacks that post through the
// dispatcher take a CallbackLease before touching the pointer. The lease's
// ctor atomically increments `g_in_flight_callbacks` iff `g_draining` is
// clear. `unregister_dispatcher` sets `g_draining`, spin-waits (bounded
// ~200ms) for the counter to reach zero, then clears the pointer. Net: no
// lease-holder is ever mid-`post()` when the dispatcher object is destroyed.
// See CR-41 in concerns.md for the full hazard framing.

namespace {
std::atomic<GodotMainDispatcher*> g_dispatcher{nullptr};
std::atomic<int> g_in_flight_callbacks{0};
std::atomic<bool> g_draining{false};

constexpr int DRAIN_TIMEOUT_MS = 200;
constexpr int DRAIN_POLL_US = 100;
}  // anonymous namespace

void MwaJniContext::register_dispatcher(GodotMainDispatcher* dispatcher) {
    // Store the new pointer BEFORE clearing the draining flag — so if a
    // stale callback somehow observes the pointer transition it gets the
    // new (valid) dispatcher, not a torn read.
    g_dispatcher.store(dispatcher, std::memory_order_release);
    g_draining.store(false, std::memory_order_release);
}

void MwaJniContext::unregister_dispatcher() {
    // Phase 1: signal draining — new lease acquisitions will fail from this
    // point on. Release-order pairs with the CallbackLease ctor's acquire.
    g_draining.store(true, std::memory_order_release);

    // Phase 2: spin-wait (bounded) for in-flight leases to drop. Each
    // existing lease finishes its `dispatcher->post(...)` call and
    // decrements the counter in its dtor. We yield the CPU between polls
    // so a worker thread holding a lease can make progress.
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(DRAIN_TIMEOUT_MS);
    while (g_in_flight_callbacks.load(std::memory_order_acquire) > 0 &&
           std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::microseconds(DRAIN_POLL_US));
    }

    const int stragglers = g_in_flight_callbacks.load(std::memory_order_acquire);
    if (stragglers > 0) {
        godot::UtilityFunctions::push_warning(
            godot::String("MWA: dispatcher unregister drain timed out with ") +
            godot::itos(stragglers) +
            godot::String(" in-flight callback(s); proceeding at UAF risk (CR-41 bound exceeded)."));
    }

    // Phase 3: clear the pointer. Any stragglers past the deadline still
    // hold the OLD pointer in their lease and will hit a destroyed
    // dispatcher object — best-effort drop per CR-41's bounded-wait contract.
    g_dispatcher.store(nullptr, std::memory_order_release);
}

GodotMainDispatcher* MwaJniContext::get_dispatcher() {
    return g_dispatcher.load(std::memory_order_acquire);
}

// CR-41 lease: ctor order is (counter-increment, draining-check, pointer-load,
// counter-decrement-on-failure). The counter increment PRECEDES the draining
// check so that `unregister_dispatcher` cannot observe counter=0 between a
// racing ctor's draining-check and its post() call.
MwaJniContext::CallbackLease::CallbackLease()
    : dispatcher_(nullptr), acquired_(false) {
    g_in_flight_callbacks.fetch_add(1, std::memory_order_acq_rel);
    if (g_draining.load(std::memory_order_acquire)) {
        // Draining in progress — back out the pre-increment and leave
        // `dispatcher_` null so callers drop cleanly.
        g_in_flight_callbacks.fetch_sub(1, std::memory_order_acq_rel);
        return;
    }
    GodotMainDispatcher* p = g_dispatcher.load(std::memory_order_acquire);
    if (p == nullptr) {
        g_in_flight_callbacks.fetch_sub(1, std::memory_order_acq_rel);
        return;
    }
    dispatcher_ = p;
    acquired_ = true;
}

MwaJniContext::CallbackLease::~CallbackLease() {
    if (acquired_) {
        g_in_flight_callbacks.fetch_sub(1, std::memory_order_acq_rel);
    }
}

// Story 2-1 T6 — empty-defaults snapshot. Matches the shape NoOp returns so
// consumers default-cast to "not connected" on any JNI failure path.
static godot::Dictionary build_empty_session_state() {
    godot::Dictionary out;
    out["is_connected"] = false;
    out["public_key"] = godot::String();
    out["cluster"] = godot::String();
    out["wallet_label"] = godot::String();
    out["auth_token_fingerprint"] = godot::String();
    return out;
}

godot::Dictionary MwaJniContext::query_session_state() {
    if (!g_jni_ready.load(std::memory_order_acquire) ||
        g_mid_mwa_query_session_state_from_jni == nullptr) {
        return build_empty_session_state();
    }
    ScopedJniAttach attach;
    JNIEnv* env = attach.env();
    if (env == nullptr) {
        return build_empty_session_state();
    }
    // CallStaticObjectMethod -> jstring JSON payload. Kotlin's
    // `mwaQuerySessionStateFromJni` reads MwaSessionState under a single
    // `@Synchronized` lock, so the 5-key JSON is a coherent snapshot.
    jobject result_obj = env->CallStaticObjectMethod(
        g_plugin_companion_class, g_mid_mwa_query_session_state_from_jni);
    if (env->ExceptionCheck() == JNI_TRUE) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        if (result_obj != nullptr) { env->DeleteLocalRef(result_obj); }
        return build_empty_session_state();
    }
    if (result_obj == nullptr) {
        return build_empty_session_state();
    }
    const godot::String json_str =
        jstring_to_godot_string(env, static_cast<jstring>(result_obj));
    env->DeleteLocalRef(result_obj);

    godot::Ref<godot::JSON> json;
    json.instantiate();
    if (json->parse(json_str) != godot::OK) {
        godot::UtilityFunctions::push_warning(
            godot::String("MWA: query_session_state JSON parse failed"));
        return build_empty_session_state();
    }
    const godot::Variant data = json->get_data();
    if (data.get_type() != godot::Variant::DICTIONARY) {
        godot::UtilityFunctions::push_warning(
            godot::String("MWA: query_session_state JSON root is not a dict"));
        return build_empty_session_state();
    }
    return godot::Dictionary(data);
}

// ---------------- MwaAndroidBridgeJni impl ----------------

MwaAndroidBridgeJni::MwaAndroidBridgeJni(GodotMainDispatcher* dispatcher)
    : dispatcher_(nullptr) {
    ERR_FAIL_NULL_MSG(dispatcher,
        "MwaAndroidBridgeJni: dispatcher must be non-null.");
    dispatcher_ = dispatcher;
    MwaJniContext::register_dispatcher(dispatcher);
}

MwaAndroidBridgeJni::~MwaAndroidBridgeJni() {
    MwaJniContext::unregister_dispatcher();
}

void MwaAndroidBridgeJni::emit_jni_unavailable(const godot::String& source_method,
                                                const godot::String& request_id) {
    if (dispatcher_ == nullptr) { return; }
    godot::Dictionary payload;
    payload["request_id"] = request_id;
    payload["code"] = godot::String(code_name(MwaErrorCode::NOT_CONNECTED));
    payload["message"] = godot::String("MWA JNI bridge not initialized");
    payload["user_message"] = godot::String("Mobile wallet is temporarily unavailable. Please restart the app.");
    payload["developer_details"] = godot::String(
        "JNI_OnLoad did not populate the plugin companion class or method ids; "
        "check System.loadLibrary and the Kotlin plugin class path.");
    payload["recoverable"] = false;
    payload["retry_hint"] = godot::String("none");
    payload["layer"] = godot::String("cpp");
    payload["cause"] = godot::String("jni_uninitialized");
    payload["source_method"] = source_method;
    // D-6: 1-arity error signal — wrap payload in 1-elem Array.
    dispatcher_->post(godot::String("mwa_error"), godot::Array::make(payload));
}

void MwaAndroidBridgeJni::connect(const godot::String& request_id,
                                  const godot::Dictionary& identity,
                                  const godot::String& cluster,
                                  const godot::Dictionary& opts) {
    if (!g_jni_ready.load(std::memory_order_acquire)) {
        emit_jni_unavailable(godot::String("connect"), request_id);
        return;
    }
    call_authorize_shape(g_mid_mwa_authorize_from_jni, dispatcher_,
                         request_id, serialize_dict_to_json(identity),
                         cluster, opts_chain_id(opts, cluster),
                         opts_timeout_ms(opts), "connect");
}

void MwaAndroidBridgeJni::reauthorize(const godot::String& request_id,
                                      const godot::Dictionary& opts) {
    if (!g_jni_ready.load(std::memory_order_acquire)) {
        emit_jni_unavailable(godot::String("reauthorize"), request_id);
        return;
    }
    // Story 2-2 lands the full reauth path; empty identity + cluster for now.
    call_authorize_shape(g_mid_mwa_reauthorize_from_jni, dispatcher_,
                         request_id, godot::String(""), godot::String(""),
                         godot::String(""), opts_timeout_ms(opts), "reauthorize");
}

void MwaAndroidBridgeJni::disconnect(const godot::String& request_id,
                                     const godot::Dictionary& /*opts*/) {
    if (!g_jni_ready.load(std::memory_order_acquire)) {
        emit_jni_unavailable(godot::String("disconnect"), request_id);
        return;
    }
    call_reqid_only_shape(g_mid_mwa_disconnect_from_jni, dispatcher_, request_id, "disconnect");
}

void MwaAndroidBridgeJni::deauthorize(const godot::String& request_id,
                                      const godot::Dictionary& /*opts*/) {
    if (!g_jni_ready.load(std::memory_order_acquire)) {
        emit_jni_unavailable(godot::String("deauthorize"), request_id);
        return;
    }
    call_reqid_only_shape(g_mid_mwa_deauthorize_from_jni, dispatcher_, request_id, "deauthorize");
}

void MwaAndroidBridgeJni::sign_messages(const godot::String& request_id,
                                        const godot::TypedArray<godot::PackedByteArray>& /*messages*/,
                                        const godot::Dictionary& /*opts*/) {
    if (!g_jni_ready.load(std::memory_order_acquire)) {
        emit_jni_unavailable(godot::String("sign_messages"), request_id);
        return;
    }
    call_reqid_only_shape(g_mid_mwa_sign_messages_from_jni, dispatcher_, request_id, "sign_messages");
}

void MwaAndroidBridgeJni::sign_transactions(const godot::String& request_id,
                                            const godot::TypedArray<godot::PackedByteArray>& /*transactions*/,
                                            const godot::Dictionary& /*opts*/) {
    if (!g_jni_ready.load(std::memory_order_acquire)) {
        emit_jni_unavailable(godot::String("sign_transactions"), request_id);
        return;
    }
    call_reqid_only_shape(g_mid_mwa_sign_transactions_from_jni, dispatcher_, request_id, "sign_transactions");
}

void MwaAndroidBridgeJni::sign_and_send(const godot::String& request_id,
                                        const godot::TypedArray<godot::PackedByteArray>& /*transactions*/,
                                        const godot::Dictionary& /*opts*/) {
    if (!g_jni_ready.load(std::memory_order_acquire)) {
        emit_jni_unavailable(godot::String("sign_and_send"), request_id);
        return;
    }
    call_reqid_only_shape(g_mid_mwa_sign_and_send_from_jni, dispatcher_, request_id, "sign_and_send");
}

void MwaAndroidBridgeJni::forget_all(const godot::String& request_id) {
    if (!g_jni_ready.load(std::memory_order_acquire)) {
        emit_jni_unavailable(godot::String("forget_all"), request_id);
        return;
    }
    call_reqid_only_shape(g_mid_mwa_forget_all_from_jni, dispatcher_, request_id, "forget_all");
}

void MwaAndroidBridgeJni::get_diagnostics(const godot::String& request_id) {
    if (!g_jni_ready.load(std::memory_order_acquire)) {
        emit_jni_unavailable(godot::String("get_diagnostics"), request_id);
        return;
    }
    call_reqid_only_shape(g_mid_mwa_get_diagnostics_from_jni, dispatcher_, request_id, "get_diagnostics");
}

godot::Dictionary MwaAndroidBridgeJni::query_session_state() const {
    // Delegates to the file-scope helper so non-bridge callers (e.g. diagnostic
    // tooling in Story 5-2) can reach the same snapshot without instantiating
    // a bridge. On any JNI failure path the helper returns empty defaults —
    // the getter then reports "not connected" rather than crashing.
    return MwaJniContext::query_session_state();
}

}  // namespace godot_solana_sdk::mwa

// ---------------- JNI_OnLoad + JNIEXPORT callbacks ----------------
//
// JNI boundary functions live in the top-level C linkage so the JVM can
// resolve them by mangled name. Everything inside
// `godot_solana_sdk::mwa` is C++ with name mangling and not directly
// callable from the JVM.

extern "C" {

using godot_solana_sdk::mwa::MwaJniContext;
using godot_solana_sdk::mwa::GodotMainDispatcher;

// --- forward-path: cache the plugin companion class + method IDs ---

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/) {
    godot_solana_sdk::mwa::g_jvm = vm;
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    jclass local = env->FindClass("plugin/walletadapterandroid/GDExtensionAndroidPlugin$Companion");
    if (local == nullptr) {
        // FindClass left a pending exception; clear to avoid spilling into
        // future JNI calls on this thread.
        if (env->ExceptionCheck() == JNI_TRUE) { env->ExceptionClear(); }
        return JNI_VERSION_1_6;  // Cache not populated; g_jni_ready stays false, ops emit NOT_CONNECTED.
    }
    godot_solana_sdk::mwa::g_plugin_companion_class =
        static_cast<jclass>(env->NewGlobalRef(local));
    env->DeleteLocalRef(local);

    // (reqId: String, identityJson: String, cluster: String, chainId: String, timeoutMs: Long): Unit
    const char* authorize_sig = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;J)V";
    godot_solana_sdk::mwa::g_mid_mwa_authorize_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaAuthorizeFromJni", authorize_sig);
    godot_solana_sdk::mwa::g_mid_mwa_reauthorize_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaReauthorizeFromJni", authorize_sig);

    // (reqId: String): Unit
    const char* reqid_only_sig = "(Ljava/lang/String;)V";
    godot_solana_sdk::mwa::g_mid_mwa_disconnect_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaDisconnectFromJni", reqid_only_sig);
    godot_solana_sdk::mwa::g_mid_mwa_deauthorize_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaDeauthorizeFromJni", reqid_only_sig);
    godot_solana_sdk::mwa::g_mid_mwa_sign_messages_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaSignMessagesFromJni", reqid_only_sig);
    godot_solana_sdk::mwa::g_mid_mwa_sign_transactions_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaSignTransactionsFromJni", reqid_only_sig);
    godot_solana_sdk::mwa::g_mid_mwa_sign_and_send_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaSignAndSendFromJni", reqid_only_sig);
    godot_solana_sdk::mwa::g_mid_mwa_forget_all_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaForgetAllFromJni", reqid_only_sig);
    godot_solana_sdk::mwa::g_mid_mwa_get_diagnostics_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaGetDiagnosticsFromJni", reqid_only_sig);

    // Story 2-1 T6 — sync getter returns String (Ljava/lang/String;).
    godot_solana_sdk::mwa::g_mid_mwa_query_session_state_from_jni =
        env->GetStaticMethodID(godot_solana_sdk::mwa::g_plugin_companion_class,
                               "mwaQuerySessionStateFromJni", "()Ljava/lang/String;");

    if (env->ExceptionCheck() == JNI_TRUE) { env->ExceptionClear(); }

    // Readiness: class + at least the authorize method must be resolved.
    // Missing optional methods (reauth, sign, etc.) are acceptable during
    // Story 2-1 T5 (only authorize is fully wired); the op methods null-check
    // their specific mid before each call.
    godot_solana_sdk::mwa::g_jni_ready.store(
        godot_solana_sdk::mwa::g_plugin_companion_class != nullptr &&
            godot_solana_sdk::mwa::g_mid_mwa_authorize_from_jni != nullptr,
        std::memory_order_release);

    return JNI_VERSION_1_6;
}

// --- reverse-path: callbacks from Kotlin NativeBridge via external fun ---

// Hand-construct a minimal mwa_error when a payload JSON fails to parse. Used
// by both arity-1 and arity-2 callback helpers so GDScript always sees a
// well-formed mwa_error — a mangled emission isn't silently suppressed and a
// malformed *_completed isn't delivered with an empty result dict.
//
// CR-T6-8 trade-off (Story 2-1 T6 review): for arity-1 paths the request_id
// lives INSIDE the payload JSON. When parse fails we have no trustworthy way
// to recover it — attempting to string-match a `request_id` out of mangled
// bytes would propagate corruption (the wrong request could be marked
// TERMINATED in the Kotlin InflightMap). So `request_id_maybe` is the empty
// string for arity-1 failures. Consequence: the specific original request
// stays PENDING in InflightMap forever (until the watchdog timeout fires
// from the Kotlin side). Acceptable in this cascading-failure scenario:
// a mangled payload is itself a symptom of a deeper fault; the deterministic
// user-visible mwa_error preserves the terminal-signal invariant globally
// even if one specific request_id can't be correlated. We log the seam label
// and payload byte-length (NOT the body — the body might carry auth_token
// ciphertext or similar secret material) so operators can correlate the
// failure post-hoc. Future editors: do NOT "fix" this by pulling a
// substring-matched request_id out of the mangled string.
static void post_parse_failure_error(GodotMainDispatcher* dispatcher,
                                     const godot::String& request_id_maybe,
                                     const char* seam_label,
                                     int mangled_payload_length = -1) {
    if (mangled_payload_length >= 0) {
        godot::UtilityFunctions::push_warning(
            godot::String("MWA: post_parse_failure seam=") + godot::String(seam_label) +
            godot::String(" mangled_bytes=") + godot::itos(mangled_payload_length) +
            godot::String(" request_id=") + (request_id_maybe.is_empty()
                ? godot::String("<unrecoverable; arity-1 parse failure>")
                : request_id_maybe));
    }
    godot::Dictionary fallback;
    fallback["request_id"] = request_id_maybe;
    fallback["code"] = godot::String("PROTOCOL_ERROR");
    fallback["message"] = godot::String("MWA payload JSON parse failed");
    fallback["user_message"] = godot::String("The wallet response could not be processed.");
    fallback["developer_details"] =
        godot::String("JNI payload parse failed at seam=") + godot::String(seam_label);
    fallback["recoverable"] = true;
    fallback["retry_hint"] = godot::String("retry_now");
    fallback["layer"] = godot::String("cpp");
    fallback["cause"] = godot::String("json_parse_error");
    fallback["source_method"] = godot::String(seam_label);
    // D-6: 1-arity error signal — wrap fallback in 1-elem Array.
    dispatcher->post(godot::String("mwa_error"), godot::Array::make(fallback));
}

// Parse a JSON string and return (ok, dict). On parse failure pushes a warning
// and returns (false, {}). Callers decide whether to fall back to a
// hand-constructed error or pass through the empty dict.
static bool try_parse_payload(const godot::String& json_str,
                              const char* seam_label,
                              godot::Dictionary& out) {
    godot::Ref<godot::JSON> json;
    json.instantiate();
    if (json->parse(json_str) != godot::OK) {
        godot::UtilityFunctions::push_warning(
            godot::String("MWA: JNI payload JSON parse failed at seam=") + godot::String(seam_label));
        return false;
    }
    const godot::Variant data = json->get_data();
    if (data.get_type() != godot::Variant::DICTIONARY) {
        godot::UtilityFunctions::push_warning(
            godot::String("MWA: JNI payload JSON root is not a dict at seam=") + godot::String(seam_label));
        return false;
    }
    out = godot::Dictionary(data);
    return true;
}

// Helper: parse payload JSON + post to dispatcher as arity-1 (1-param error/lifecycle
// signals per A-12). Used by mwa_error / mwa_timeout / mwa_cancelled_lifecycle /
// reauth_required. NEVER logs the payload body — only the seam label (which is
// the signal name: public identifier, not secret). Payload variable naming matches
// the grep-ban pattern 8.
// On parse failure emits a hand-constructed mwa_error to preserve the terminal-
// signal invariant (AC-6) instead of delivering an empty-payload emission.
static void post_arity1(JNIEnv* env,
                        jstring payload_json,
                        const char* signal_name,
                        const char* seam_label) {
    // CR-41: acquire lease BEFORE touching dispatcher. Lease dtor releases
    // the in-flight counter even if we early-return below.
    MwaJniContext::CallbackLease lease;
    GodotMainDispatcher* dispatcher = lease.dispatcher();
    if (dispatcher == nullptr) {
        godot::UtilityFunctions::push_warning(
            godot::String("MWA: ") + seam_label +
            godot::String(" fired before bridge registered or during teardown drain; dropping"));
        return;
    }
    const godot::String json_str =
        godot_solana_sdk::mwa::jstring_to_godot_string(env, payload_json);
    godot::Dictionary payload;
    if (!try_parse_payload(json_str, seam_label, payload)) {
        // Hand-construct — don't pass the mangled payload through. Callers
        // of mwa_error specifically: even if the payload that failed WAS
        // an mwa_error, we still emit the hand-constructed fallback so
        // downstream shapes are well-formed.
        //
        // CR-T6-8: arity-1 path — request_id is embedded IN the payload
        // JSON; parse failure leaves it unrecoverable. We log the seam +
        // mangled-byte length so operators can correlate post-hoc; the
        // specific original request_id stays PENDING in Kotlin's
        // InflightMap until the watchdog fires. See post_parse_failure_error
        // kdoc for the trade-off rationale.
        post_parse_failure_error(dispatcher, godot::String(""), seam_label, json_str.length());
        return;
    }
    // D-6: 1-arity error/lifecycle signal — wrap payload in 1-elem Array.
    dispatcher->post(godot::String(signal_name), godot::Array::make(payload));
}

// Helper: post to dispatcher as arity-2 `*_completed` signal per A-12. GDScript
// sees `connect_completed(request_id, result)` — two typed parameters matching
// the `ADD_SIGNAL` registration. T6: dispatcher->post now takes Array; we
// wrap (req_id, result) in a 2-elem Array and the production ladder unpacks
// it into `call_deferred(signal_name, req_id, result)`.
// On parse failure emits mwa_error instead of connect_completed with empty dict
// — the terminal-signal invariant fires, and the mwa_error carries the
// actual failure mode (AC-6).
static void post_arity2_completed(JNIEnv* env,
                                  jstring req_id_jstr,
                                  jstring result_json_jstr,
                                  const char* signal_name,
                                  const char* seam_label) {
    // CR-41: acquire lease BEFORE touching dispatcher. Lease dtor releases
    // the in-flight counter even if we early-return below.
    MwaJniContext::CallbackLease lease;
    GodotMainDispatcher* dispatcher = lease.dispatcher();
    if (dispatcher == nullptr) {
        godot::UtilityFunctions::push_warning(
            godot::String("MWA: ") + seam_label +
            godot::String(" fired before bridge registered or during teardown drain; dropping"));
        return;
    }
    const godot::String req_id =
        godot_solana_sdk::mwa::jstring_to_godot_string(env, req_id_jstr);
    const godot::String json_str =
        godot_solana_sdk::mwa::jstring_to_godot_string(env, result_json_jstr);
    godot::Dictionary result;
    if (!try_parse_payload(json_str, seam_label, result)) {
        // Arity-2 path — request_id was the separate JNI arg and IS
        // recoverable; pass it through so the fallback mwa_error carries
        // the correct correlation.
        post_parse_failure_error(dispatcher, req_id, seam_label, json_str.length());
        return;
    }
    // D-6: 2-arity *_completed signal — Array::make(req_id, result).
    dispatcher->post(godot::String(signal_name), godot::Array::make(req_id, result));
}

JNIEXPORT void JNICALL
Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_postConnectCompletedNative(
    JNIEnv* env, jobject /*companion*/, jstring reqId, jstring resultDictJson) {
    post_arity2_completed(env, reqId, resultDictJson, "connect_completed", "postConnectCompletedNative");
}

// Story 2-3 T3 — 2-param `disconnect_completed` JNIEXPORT per A-12.
// Kotlin side: GDExtensionAndroidPlugin.Companion.postDisconnectCompletedNative (external fun)
// → this JNIEXPORT → post_arity2_completed helper → dispatcher->post(
//       "disconnect_completed", Array::make(req_id, result_dict))
// under CR-41 CallbackLease — parallel to postConnectCompletedNative above.
//
// The result_dict carries `{request_id, source_method: "disconnect"}` only —
// no secret material on the disconnect path. On JSON parse failure (unlikely;
// the Kotlin side builds a 2-key JSON via JSONObject), post_parse_failure_error
// falls back to mwa_error{PROTOCOL_ERROR, source_method="disconnect",
// cause="parse_failure"} via the existing helper — preserves the
// terminal-signal invariant (AC-3 idempotence remains safe because each
// call carries its own request_id per DD-2-3-4).
JNIEXPORT void JNICALL
Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_postDisconnectCompletedNative(
    JNIEnv* env, jobject /*companion*/, jstring reqId, jstring resultDictJson) {
    post_arity2_completed(env, reqId, resultDictJson, "disconnect_completed", "postDisconnectCompletedNative");
}

JNIEXPORT void JNICALL
Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_postMwaErrorNative(
    JNIEnv* env, jobject /*companion*/, jstring errorDictJson) {
    post_arity1(env, errorDictJson, "mwa_error", "postMwaErrorNative");
}

JNIEXPORT void JNICALL
Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_postMwaTimeoutNative(
    JNIEnv* env, jobject /*companion*/, jstring timeoutDictJson) {
    post_arity1(env, timeoutDictJson, "mwa_timeout", "postMwaTimeoutNative");
}

JNIEXPORT void JNICALL
Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_postMwaCancelledLifecycleNative(
    JNIEnv* env, jobject /*companion*/, jstring cancelledDictJson) {
    post_arity1(env, cancelledDictJson, "mwa_cancelled_lifecycle", "postMwaCancelledLifecycleNative");
}

JNIEXPORT void JNICALL
Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_postReauthRequiredNative(
    JNIEnv* env, jobject /*companion*/, jstring reauthDictJson) {
    post_arity1(env, reauthDictJson, "reauth_required", "postReauthRequiredNative");
}

}  // extern "C"
