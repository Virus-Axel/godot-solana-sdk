package com.godotengine.godot_solana_sdk.mwa.plugin

import com.godotengine.godot_solana_sdk.mwa.session.MwaSessionState
import org.json.JSONArray
import org.json.JSONObject

/**
 * diagnostics-payload builder.
 *
 * Produces the AC-1 12-key Dictionary as a JSON String for sync transport
 * across the JNI seam (`mwaQueryDiagnosticsFromJni: () -> String` → C++
 * `MwaAndroidBridgeJni::query_diagnostics_json` → `godot::JSON::parse` →
 * `Dictionary`).
 *
 * **AC-1 12-key shape (top-level):**
 * ```
 * {
 *   "sdk_version": String,
 *   "clientlib_ktx_version": String,
 *   "godot_version": String,
 *   "android_api_level": Int,
 *   "session_state": Object,            // nested 5-key from MwaSessionState.snapshotSessionStateJson()
 *   "wallet_package": String,           // "" pre-connect or if not surfaced by sessionState
 *   "wallet_version": String,           // "" pre-connect or if not surfaced
 *   "auth_token_fingerprint": String,   // 8 lowercase hex chars; "" pre-connect
 *   "cluster": String,                  // "" pre-connect
 *   "last_n_correlation_trace": Array,  // 0-20 entries each with AC-2 5-key shape
 *   "late_result_count": Int,
 *   "pending_submissions_count": Int
 * }
 * ```
 *
 * **AC-2 correlation-trace entry shape (each Array element):**
 * ```
 * {request_id, source_method, terminal_signal, elapsed_ms, timestamp_ms}
 * ```
 *
 * **AC-3 PII guarantee:** the only token-derived field in the payload is
 * `auth_token_fingerprint` (HKDF-SHA256 truncated to 4 bytes / 8 hex chars per
 *); the raw `auth_token` bytes never appear in the JSON. The `public_key`
 * and Base58 encoded forms are NOT included in this payload (they're surfaced
 * separately via `MWA.get_public_key()`); embedding the public key here would
 * be redundant + arguably PII-adjacent under a strict reading of AC-3.
 *
 * **Non-Android empty-Dict semantics:** [emptyDiagnosticsJson]
 * returns the 12-key all-empty shape (string fields = `""`, int fields = `0`,
 * `last_n_correlation_trace` = `[]`, `session_state` = `{}`); used when the
 * plugin instance is null OR when called on non-Android via the no-op bridge.
 */
internal object MwaDiagnosticsBuilder {

    fun buildDiagnosticsJson(
        sessionState: MwaSessionState,
        diagnostics: MwaDiagnostics,
        pendingSubmissionsCount: Int,
        sdkVersion: String,
        ktxVersion: String,
        godotVersion: String,
        androidApiLevel: Int,
    ): String {
        // Per: nest the 5-key sessionState snapshot under `session_state`
        // by parsing the JSON the existing accessor produces (cheap reuse — keeps
        // a single source-of-truth shape across `mwaQuerySessionStateFromJni` and
        // `mwaQueryDiagnosticsFromJni`).
        val sessionObj = JSONObject(sessionState.snapshotSessionStateJson())

        // wallet_package + wallet_version are NOT surfaced by sessionState
        // (clientlib-ktx 2.0.3's AuthorizationResult has no walletPackage; see
        // MwaClientImpl.kt:285). Empty strings per fallback semantics.

        val traceArr = JSONArray()
        for (entry in diagnostics.lastNCorrelationTrace) {
            traceArr.put(
                JSONObject().apply {
                    put("request_id", entry.requestId)
                    put("source_method", entry.sourceMethod)
                    put("terminal_signal", entry.terminalSignal)
                    put("elapsed_ms", entry.elapsedMs)
                    put("timestamp_ms", entry.timestampMs)
                },
            )
        }

        return JSONObject().apply {
            put("sdk_version", sdkVersion)
            put("clientlib_ktx_version", ktxVersion)
            put("godot_version", godotVersion)
            put("android_api_level", androidApiLevel)
            put("session_state", sessionObj)
            put("wallet_package", "")
            put("wallet_version", "")
            put("auth_token_fingerprint", sessionState.getAuthTokenFingerprint())
            put("cluster", sessionState.getClusterName())
            put("last_n_correlation_trace", traceArr)
            put("late_result_count", diagnostics.lateResultCount)
            put("pending_submissions_count", pendingSubmissionsCount)
        }.toString()
    }

    fun emptyDiagnosticsJson(): String = JSONObject().apply {
        put("sdk_version", "")
        put("clientlib_ktx_version", "")
        put("godot_version", "")
        put("android_api_level", 0)
        put("session_state", JSONObject())
        put("wallet_package", "")
        put("wallet_version", "")
        put("auth_token_fingerprint", "")
        put("cluster", "")
        put("last_n_correlation_trace", JSONArray())
        put("late_result_count", 0L)
        put("pending_submissions_count", 0)
    }.toString()
}
