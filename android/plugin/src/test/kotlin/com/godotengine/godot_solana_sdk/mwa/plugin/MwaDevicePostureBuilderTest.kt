package com.godotengine.godot_solana_sdk.mwa.plugin

import android.content.Context
import android.content.pm.ApplicationInfo
import io.mockk.every
import io.mockk.mockk
import org.json.JSONObject
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test

/**
 * Story 5-2 T1 — `MwaDevicePostureBuilder` 4-key payload coverage (TDD red).
 *
 * 3 scenarios exercising AC-4:
 *   1. AC-4 4-key Dictionary shape — exact key set with all `bool` types.
 *   2. AC-4 `debuggable` reflects the `applicationInfo.flags & FLAG_DEBUGGABLE`
 *      bit (only deterministic bit testable at unit tier without instrumentation).
 *   3. AC-4 [emptyPostureJson] returns 4-key all-`false` shape (DD-5-2-3 LOCKED
 *      non-Android empty-Dict semantics).
 *
 * **Reflection-based fields (rooted / developer_options_on / adb_enabled)
 * are NOT directly tested at unit tier** — `Build.TAGS` is a static field +
 * `Settings.Secure.getInt` / `Settings.Global.getInt` are static helpers
 * that can't be deterministically mocked without ROBOLECTRIC or PowerMockK.
 * Those fields' wiring is covered transitively at the GDScript integration
 * tier in T5's `tools/test_device_posture.gd`. D-5-2-T1-3 Rule 1 documents
 * this scope-down.
 *
 * **TDD red baseline (T1):** all 3 tests fail at runtime against the
 * `TODO("Story 5-2 T2 fills in")` body in [MwaDevicePostureBuilder.buildDevicePostureJson]
 * + [MwaDevicePostureBuilder.emptyPostureJson]. T2 turns them GREEN.
 */
class MwaDevicePostureBuilderTest {

    @Test
    fun `AC-4 4-key Dictionary shape with all bool types`() {
        // Build a relaxed-mock Context so `buildDevicePostureJson` can call
        // `applicationInfo.flags` without an Android runtime. Settings.Secure /
        // Settings.Global static calls will not deterministically resolve under
        // unit-tier — the impl must guard via try/catch + return false on
        // SettingNotFoundException OR pre-mock the static surfaces. Test focuses
        // on shape + bool-type invariant (deferred field-value cases to T5).
        val ctx: Context = mockk(relaxed = true)
        every { ctx.applicationInfo } returns ApplicationInfo().apply { flags = 0 }
        every { ctx.contentResolver } returns mockk(relaxed = true)

        val json = JSONObject(MwaDevicePostureBuilder.buildDevicePostureJson(ctx))
        val expectedKeys = setOf("rooted", "debuggable", "developer_options_on", "adb_enabled")
        assertEquals(expectedKeys, json.keySet(), "AC-4: exact 4-key set")
        for (key in expectedKeys) {
            assertTrue(json.get(key) is Boolean, "AC-4: '$key' is Boolean")
        }
    }

    @Test
    fun `AC-4 debuggable reflects FLAG_DEBUGGABLE bit on applicationInfo flags`() {
        // FLAG_DEBUGGABLE bit set → debuggable == true.
        val ctxDebug: Context = mockk(relaxed = true)
        every { ctxDebug.applicationInfo } returns ApplicationInfo().apply {
            flags = ApplicationInfo.FLAG_DEBUGGABLE
        }
        every { ctxDebug.contentResolver } returns mockk(relaxed = true)
        assertEquals(
            true,
            JSONObject(MwaDevicePostureBuilder.buildDevicePostureJson(ctxDebug)).getBoolean("debuggable"),
            "AC-4: FLAG_DEBUGGABLE set → debuggable == true",
        )

        // FLAG_DEBUGGABLE bit clear → debuggable == false.
        val ctxRelease: Context = mockk(relaxed = true)
        every { ctxRelease.applicationInfo } returns ApplicationInfo().apply { flags = 0 }
        every { ctxRelease.contentResolver } returns mockk(relaxed = true)
        assertEquals(
            false,
            JSONObject(MwaDevicePostureBuilder.buildDevicePostureJson(ctxRelease)).getBoolean("debuggable"),
            "AC-4: FLAG_DEBUGGABLE clear → debuggable == false",
        )
    }

    @Test
    fun `AC-4 emptyPostureJson returns 4-key all-false shape`() {
        // DD-5-2-3 LOCKED: non-Android / instance==null path returns 4-key all-false.
        val json = JSONObject(MwaDevicePostureBuilder.emptyPostureJson())
        val expectedKeys = setOf("rooted", "debuggable", "developer_options_on", "adb_enabled")
        assertEquals(expectedKeys, json.keySet(), "DD-5-2-3: empty payload has 4-key shape")
        assertEquals(false, json.getBoolean("rooted"), "DD-5-2-3: rooted == false")
        assertEquals(false, json.getBoolean("debuggable"), "DD-5-2-3: debuggable == false")
        assertEquals(false, json.getBoolean("developer_options_on"), "DD-5-2-3: developer_options_on == false")
        assertEquals(false, json.getBoolean("adb_enabled"), "DD-5-2-3: adb_enabled == false")
    }
}
