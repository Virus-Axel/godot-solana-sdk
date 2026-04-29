package com.godotengine.godot_solana_sdk.mwa.plugin

import android.content.Context
import android.content.pm.ApplicationInfo
import android.os.Build
import android.provider.Settings
import org.json.JSONObject

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
 */
object MwaDevicePostureBuilder {

    fun buildDevicePostureJson(context: Context): String {
        val rooted = try {
            Build.TAGS?.contains("test-keys") == true
        } catch (_: Throwable) {
            false
        }

        val debuggable = (context.applicationInfo.flags and ApplicationInfo.FLAG_DEBUGGABLE) != 0

        // Settings.Secure / Settings.Global throw SettingNotFoundException when
        // the key is unset on the host. Wrapping with try/catch ensures the
        // builder returns the 4-key shape on every device, including unit-test
        // hosts where the static helpers may not resolve deterministically.
        val developerOptionsOn = try {
            Settings.Secure.getInt(
                context.contentResolver,
                Settings.Secure.DEVELOPMENT_SETTINGS_ENABLED,
                0,
            ) != 0
        } catch (_: Throwable) {
            false
        }

        val adbEnabled = try {
            Settings.Global.getInt(
                context.contentResolver,
                Settings.Global.ADB_ENABLED,
                0,
            ) != 0
        } catch (_: Throwable) {
            false
        }

        return JSONObject().apply {
            put("rooted", rooted)
            put("debuggable", debuggable)
            put("developer_options_on", developerOptionsOn)
            put("adb_enabled", adbEnabled)
        }.toString()
    }

    fun emptyPostureJson(): String = JSONObject().apply {
        put("rooted", false)
        put("debuggable", false)
        put("developer_options_on", false)
        put("adb_enabled", false)
    }.toString()
}
