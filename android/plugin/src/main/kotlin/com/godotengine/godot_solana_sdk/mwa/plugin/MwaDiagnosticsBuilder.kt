package com.godotengine.godot_solana_sdk.mwa.plugin

import com.godotengine.godot_solana_sdk.mwa.session.MwaSessionState

/**
 * Story 5-2 (DD-5-2-1 LOCKED) — diagnostics-payload builder.
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
 * DD-34); the raw `auth_token` bytes never appear in the JSON. The `public_key`
 * and Base58 encoded forms are NOT included in this payload (they're surfaced
 * separately via `MWA.get_public_key()`); embedding the public key here would
 * be redundant + arguably PII-adjacent under a strict reading of AC-3.
 *
 * **Non-Android empty-Dict semantics (DD-5-2-3):** [emptyDiagnosticsJson]
 * returns the 12-key all-empty shape (string fields = `""`, int fields = `0`,
 * `last_n_correlation_trace` = `[]`, `session_state` = `{}`); used when the
 * plugin instance is null OR when called on non-Android via the no-op bridge.
 *
 * T2 fills in both bodies. T1 ships the API surface so tests compile against it.
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
    ): String = TODO("Story 5-2 T2 fills in")

    fun emptyDiagnosticsJson(): String = TODO("Story 5-2 T2 fills in")
}
