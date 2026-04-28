package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import io.mockk.mockk
import io.mockk.slot
import io.mockk.verify
import io.mockk.verifyOrder
import org.json.JSONArray
import org.json.JSONObject
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 3-3 T5 — AC-5 pending_submission_found on-device: a stale
 * sign_and_send breadcrumb pre-seeded BEFORE connect surfaces as a
 * one-shot `pending_submission_found` signal AFTER the success
 * `connect_completed` per DD-3-3-D ordering invariant.
 *
 * Validates (on emulator, with real coroutines + real EncryptedSharedPreferences
 * + real Keystore + Route B in-process FakeMwaClient — D-T9-1 inheritance):
 *
 *   1. Pre-seed `secureTokenStore.putPendingSubmission("e2e-sas-stale",
 *      <breadcrumb dict>)` BEFORE the plugin connects. Breadcrumb's
 *      `identity_uri` matches the connect's identity so DD-3-3-D's
 *      identity-scoped scan filter selects it.
 *   2. Run `plugin.mwaAuthorize(...)` to success via `FakeMwaClient("success")`.
 *      Stage 2's `handleSuccess` post-`postConnectCompleted` invokes
 *      `scope.launch { scanPendingSubmissions(identityUri) }` per T2 wire-up.
 *   3. Assert (verifyOrder): `postConnectCompleted` fires FIRST, then
 *      `postPendingSubmissionFound` with the expected 6-key payload, then
 *      the breadcrumb is removed (one-shot per AC-5).
 *
 * **DD-3-3-E payload-shape distinctness:** the `pending_submission_found`
 * signal's payload is the 6-key shape `{request_id, op_type,
 * started_at_ms, tx_count, tx_preview_hashes, recommendation}` — drops
 * the breadcrumb-internal `cluster` + `identity_uri` fields and adds the
 * literal `recommendation: "check_chain_for_signatures"`. Negative-key
 * guards below catch a regression where `buildPendingSubmissionFoundJson`
 * forgot to drop the breadcrumb-internal fields or accidentally emitted
 * the breadcrumb dict directly.
 *
 * **One-shot semantics (AC-5 + DD-3-3-D):** after the emit, the
 * breadcrumb is removed. A re-connect would NOT re-fire — covered at the
 * unit tier in `MwaAndroidPluginSignAndSendTest.AC-5`. This test focuses
 * on the integration of T2's `scanPendingSubmissions` + the JNI/dispatcher
 * path (T3 `postPendingSubmissionFoundNative` JNIEXPORT + T4 MWA.gd
 * relay) end-to-end on a real ESP.
 *
 * **No spy needed:** the test uses the real ESP via PluginTestHarness's
 * default `storeProvider` path (Route B harness). The pre-seed write
 * goes to the real ESP; the scan reads from the same ESP; the cleanup
 * removes from the same ESP. Final-state assertion via
 * `secureTokenStore.getPendingSubmission(...)` returning null confirms
 * the one-shot cleanup landed on disk.
 */
@RunWith(AndroidJUnit4::class)
class AC5PendingSubmissionFoundOnNextConnectTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File
    private lateinit var sharedStore: SecureTokenStore

    private val identityUri = "https://example.com/godot-mwa-pending-ac5"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (3-3 AC-5)")
        put("icon_uri", "https://example.com/icon.png")
        put("identity_uri", identityUri)
    }.toString()

    @Before
    fun setup() {
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        val testCtx = InstrumentationRegistry.getInstrumentation().context
        fixturesDir = FakeWalletFixtures.extractToCacheDir(testCtx)
        SecureTokenStore(ctx).deleteAll()
        GDExtensionAndroidPlugin.sessionState.clear()

        nativeBridge = mockk(relaxed = true)
        sharedStore = SecureTokenStore(ctx)
        plugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
            storeOverride = sharedStore,
        )
    }

    @After
    fun teardown() {
        runCatching { SecureTokenStore(ctx).deleteAll() }
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @Test
    fun ac5_staleBreadcrumbSurfacesPendingSubmissionFoundAfterConnect() {
        // --- Stage 1: pre-seed stale breadcrumb under matching identity_uri ----
        val staleBreadcrumb = JSONObject().apply {
            put("request_id", "e2e-sas-stale")
            put("op_type", "sign_and_send")
            put("cluster", "devnet")
            put("identity_uri", identityUri)
            put("started_at_ms", 1_699_000_000_000L)
            put("tx_count", 1)
            val previewArr = JSONArray()
            // 64-char lowercase-hex SHA-256 placeholder (pre-seed bypasses the
            // breadcrumb-write codepath; the shape only needs to match the
            // `pending_submission_found` payload contract for the assertions
            // below — DD-3-3-F enforces this at write time, not at read time).
            previewArr.put("a".repeat(64))
            put("tx_preview_hashes", previewArr)
        }.toString()
        sharedStore.putPendingSubmission("e2e-sas-stale", staleBreadcrumb)

        // Sanity check: the pre-seed landed on the real ESP.
        val preSeedRecovered = sharedStore.getPendingSubmission("e2e-sas-stale")
        assertEquals(staleBreadcrumb, preSeedRecovered)

        // --- Stage 2: run mwaAuthorize to success ------------------------------
        plugin.mwaAuthorize(
            requestId = "req-pending-ac5-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )

        // Wait until BOTH connect_completed AND pending_submission_found have
        // fired. The scan is launched via scope.launch AFTER
        // postConnectCompleted per DD-3-3-D, so the second signal arrives
        // strictly after the first.
        val pendingJsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(5_000L) {
            runCatching {
                verify(exactly = 1) {
                    nativeBridge.postPendingSubmissionFound(capture(pendingJsonSlot))
                }
            }.isSuccess
        }

        // --- Stage 3: verifyOrder + payload assertions --------------------------
        verifyOrder {
            nativeBridge.postConnectCompleted("req-pending-ac5-connect", any())
            nativeBridge.postPendingSubmissionFound(any())
        }

        val pending = JSONObject(pendingJsonSlot.captured)

        // DD-3-3-E 6-key shape assertions.
        assertEquals(
            "AC-5: request_id from pre-seeded breadcrumb",
            "e2e-sas-stale",
            pending.getString("request_id"),
        )
        assertEquals(
            "AC-5: op_type literal",
            "sign_and_send",
            pending.getString("op_type"),
        )
        assertEquals(
            "AC-5: started_at_ms from pre-seeded breadcrumb",
            1_699_000_000_000L,
            pending.getLong("started_at_ms"),
        )
        assertEquals(
            "AC-5: tx_count from pre-seeded breadcrumb",
            1,
            pending.getInt("tx_count"),
        )
        assertEquals(
            "AC-5: tx_preview_hashes carries the pre-seeded array",
            1,
            pending.getJSONArray("tx_preview_hashes").length(),
        )
        assertEquals(
            "AC-5: recommendation must be the literal \"check_chain_for_signatures\"",
            "check_chain_for_signatures",
            pending.getString("recommendation"),
        )

        // DD-3-3-E negative-key guards: cluster + identity_uri are
        // breadcrumb-internal correlation metadata, NOT user-facing on the
        // signal. A regression that emitted the breadcrumb dict directly
        // (instead of going through buildPendingSubmissionFoundJson) would
        // leak these fields. The unit tier covers this too — the on-device
        // verification proves the negative-shape survives the JNI/dispatcher
        // round-trip.
        assertTrue(
            "AC-5 / DD-3-3-E: pending payload must NOT carry cluster",
            !pending.has("cluster"),
        )
        assertTrue(
            "AC-5 / DD-3-3-E: pending payload must NOT carry identity_uri",
            !pending.has("identity_uri"),
        )

        // AC-5 one-shot: the breadcrumb is removed after the emit. Read back
        // from the real ESP to prove the cleanup landed on disk. A regression
        // that forgot the cleanupBreadcrumb call inside scanPendingSubmissions
        // would leave the entry; a re-connect would re-fire the signal
        // (covered at the unit tier as a verify(exactly = 1) over multiple
        // mwaAuthorize calls).
        assertNull(
            "AC-5 one-shot: stale breadcrumb must be removed post-emit",
            sharedStore.getPendingSubmission("e2e-sas-stale"),
        )

        // Terminal-signal invariant — no error/timeout alongside the success.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
