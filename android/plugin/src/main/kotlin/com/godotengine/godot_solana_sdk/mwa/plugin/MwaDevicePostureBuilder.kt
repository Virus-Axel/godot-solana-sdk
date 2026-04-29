package com.godotengine.godot_solana_sdk.mwa.plugin

import android.content.Context

/**
 * Story 5-2 (DD-5-2-1 LOCKED) — device-posture-payload builder.
 *
 * Produces the AC-4 4-key Dictionary as a JSON String for sync transport
 * across the JNI seam (`mwaQueryDevicePostureFromJni: () -> String` → C++
 * `MwaAndroidBridgeJni::query_device_posture_json` → `godot::JSON::parse` →
 * `Dictionary`).
 *
 * **AC-4 4-key shape (top-level — all fields `bool`):**
 * ```
 * {
 *   "rooted": Bool,                  // Build.TAGS.contains("test-keys")
 *   "debuggable": Bool,              // (applicationInfo.flags and FLAG_DEBUGGABLE) != 0
 *   "developer_options_on": Bool,    // Settings.Secure.DEVELOPMENT_SETTINGS_ENABLED != 0
 *   "adb_enabled": Bool              // Settings.Global.ADB_ENABLED != 0
 * }
 * ```
 *
 * **Non-authoritative (per AC-4 spec language):** the 4 signals are inexpensive
 * heuristics, NOT cryptographic attestation. A determined attacker can mask
 * each signal at the OS level. Do NOT use as a security gate; surface as
 * informational metadata only (e.g., for support-team triage).
 *
 * **Non-Android empty-Dict semantics (DD-5-2-3):** [emptyPostureJson] returns
 * the 4-key all-`false` shape; used when the plugin instance is null OR when
 * called on non-Android via the no-op bridge.
 *
 * T2 fills in both bodies. T1 ships the API surface so tests compile against it.
 */
object MwaDevicePostureBuilder {

    fun buildDevicePostureJson(context: Context): String = TODO("Story 5-2 T2 fills in")

    fun emptyPostureJson(): String = TODO("Story 5-2 T2 fills in")
}
