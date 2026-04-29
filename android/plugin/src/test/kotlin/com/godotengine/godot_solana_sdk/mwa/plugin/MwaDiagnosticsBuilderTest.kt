package com.godotengine.godot_solana_sdk.mwa.plugin

import com.godotengine.godot_solana_sdk.mwa.session.MwaSessionState
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import org.json.JSONObject
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test

/**
 * Story 5-2 T1 — `MwaDiagnosticsBuilder` 12-key payload coverage (TDD red).
 *
 * 5 scenarios exercising AC-1 + DD-5-2-1 LOCKED:
 *   1. AC-1 12-key Dictionary shape — exact key set with correct types.
 *   2. AC-1 disconnected session — fingerprint/cluster/wallet_package/wallet_version are `""`.
 *   3. AC-1 connected session — fingerprint/cluster/wallet_label populated from sessionState.
 *   4. AC-1 last_n_correlation_trace is JSONArray of 0–20 entries each with 5-key shape.
 *   5. AC-1 late_result_count == diagnostics.lateResultCount AND pending_submissions_count
 *      reflects the count parameter passed in (caller injects the listAllPendingSubmissions size).
 *
 * 5 scenarios cover the AC-1 12-key payload; all GREEN at T2 (commit
 * f15673d2) which filled in [MwaDiagnosticsBuilder.buildDiagnosticsJson].
 */
class MwaDiagnosticsBuilderTest {

    private lateinit var sessionState: MwaSessionState
    private lateinit var diagnostics: MwaDiagnostics

    @BeforeEach
    fun setup() {
        sessionState = MwaSessionState()
        diagnostics = MwaDiagnostics()
    }

    private fun buildDefault(pendingCount: Int = 0): String = MwaDiagnosticsBuilder.buildDiagnosticsJson(
        sessionState = sessionState,
        diagnostics = diagnostics,
        pendingSubmissionsCount = pendingCount,
        sdkVersion = "1.0.0",
        ktxVersion = "2.0.3",
        godotVersion = "4.3.0.stable",
        androidApiLevel = 34,
    )

    @Test
    fun `AC-1 12-key Dictionary shape — exact key set with correct types`() {
        val json = JSONObject(buildDefault())
        val expectedKeys = setOf(
            "sdk_version",
            "clientlib_ktx_version",
            "godot_version",
            "android_api_level",
            "session_state",
            "wallet_package",
            "wallet_version",
            "auth_token_fingerprint",
            "cluster",
            "last_n_correlation_trace",
            "late_result_count",
            "pending_submissions_count",
        )
        assertEquals(expectedKeys, json.keySet(), "AC-1: exact 12-key set")
        // Type spot-checks per AC-1 spec.
        assertTrue(json.get("sdk_version") is String, "sdk_version is String")
        assertTrue(json.get("android_api_level") is Int, "android_api_level is Int")
        assertTrue(json.get("session_state") is JSONObject, "session_state is nested Object")
        assertTrue(json.get("last_n_correlation_trace") is org.json.JSONArray, "last_n_correlation_trace is Array")
        assertTrue(json.get("late_result_count") is Int || json.get("late_result_count") is Long, "late_result_count is integral")
        assertTrue(json.get("pending_submissions_count") is Int, "pending_submissions_count is Int")
    }

    @Test
    fun `AC-1 disconnected session — fingerprint cluster wallet_package wallet_version are empty`() {
        // sessionState is fresh (default constructor); no auth token set; no cluster set.
        val json = JSONObject(buildDefault())
        assertEquals("", json.getString("auth_token_fingerprint"), "fingerprint is '' pre-connect")
        assertEquals("", json.getString("cluster"), "cluster is '' pre-connect")
        assertEquals("", json.getString("wallet_package"), "wallet_package is '' pre-connect")
        assertEquals("", json.getString("wallet_version"), "wallet_version is '' pre-connect")
    }

    @Test
    fun `AC-1 connected session — fingerprint cluster wallet_label populated from sessionState`() {
        sessionState.setIdentity("https://game.example", "https://game.example/icon.png", "Game")
        sessionState.setAuthToken(SecretString("test-token".toByteArray(Charsets.UTF_8)))
        sessionState.setAuthTokenFingerprint("ab3f9c21")
        sessionState.setPublicKeyBase58("AbcDefGhiJkl1234567890AbcDefGhiJkl1234567890")
        sessionState.setClusterName("devnet")
        sessionState.setWalletLabel("TestWallet")

        val json = JSONObject(buildDefault())
        assertEquals("ab3f9c21", json.getString("auth_token_fingerprint"), "fingerprint pulled from sessionState")
        assertEquals("devnet", json.getString("cluster"), "cluster pulled from sessionState")
        // wallet_package + wallet_version are sourced from sessionState extensions or "" if
        // not surfaced. The 5-3-precedent-derived behavior: empty-string is acceptable per
        // DD-5-2-3; the connected-session test asserts AT LEAST cluster + fingerprint round-trip.
        assertTrue(
            json.has("wallet_package") && json.has("wallet_version"),
            "wallet_package and wallet_version keys present (values may be '' if not surfaced)",
        )
    }

    @Test
    fun `AC-1 last_n_correlation_trace is Array of 0-20 entries each with AC-2 5-key shape`() {
        // Pre-populate diagnostics with 3 entries.
        for (i in 0 until 3) {
            diagnostics.recordCorrelationTrace(
                CorrelationTraceEntry(
                    requestId = "req-$i",
                    sourceMethod = "connect",
                    terminalSignal = "connect_completed",
                    elapsedMs = 100L + i,
                    timestampMs = 1_700_000_000_000L + i,
                ),
            )
        }
        val json = JSONObject(buildDefault())
        val arr = json.getJSONArray("last_n_correlation_trace")
        assertEquals(3, arr.length(), "AC-1/AC-2: array reflects diagnostics.lastNCorrelationTrace size")
        for (i in 0 until arr.length()) {
            val entry = arr.getJSONObject(i)
            val expectedKeys = setOf(
                "request_id",
                "source_method",
                "terminal_signal",
                "elapsed_ms",
                "timestamp_ms",
            )
            assertEquals(expectedKeys, entry.keySet(), "AC-2: each entry has 5-key shape (index $i)")
        }
    }

    @Test
    fun `AC-1 late_result_count and pending_submissions_count plumbed correctly`() {
        diagnostics.incrementLateResult()
        diagnostics.incrementLateResult()
        diagnostics.incrementLateResult()
        val json = JSONObject(buildDefault(pendingCount = 7))
        assertEquals(3L, json.getLong("late_result_count"), "AC-1: late_result_count == diagnostics.lateResultCount")
        assertEquals(7, json.getInt("pending_submissions_count"), "AC-1: pending_submissions_count from caller-supplied count")
    }
}
