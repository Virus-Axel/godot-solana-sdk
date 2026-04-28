package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import io.mockk.mockk
import io.mockk.slot
import io.mockk.spyk
import io.mockk.verify
import io.mockk.verifyOrder
import org.json.JSONObject
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 3-3 T5 — AC-1 + AC-2 happy E2E on-device: `MWA.sign_and_send(...)`
 * returns the tx signatures through the unified terminal signal path AND
 * the pending-submission breadcrumb is written-ahead-then-cleared per
 * DD-3-3-B / DD-3-3-C ordering invariants.
 *
 * Validates (on emulator, with real coroutines + real EncryptedSharedPreferences
 * + real Keystore + Route B in-process FakeMwaClient — D-T9-1 inheritance):
 *
 *   1. Full connect via `FakeMwaClient("success")` — assert `connect_completed`
 *      fires and a `CacheRecord` lands in `SecureTokenStore` (preconditions
 *      for `mwaSignAndSendTransactions`'s DD-3-1-6-inherited NOT_CONNECTED
 *      preflight + AC-6 cluster-bleed lookup to NOT trigger).
 *   2. Rebuild plugin with `scenario = "success"` token AND a `spyk`-wrapped
 *      [SecureTokenStore] over the real ESP (the same instance Stage 1 wrote
 *      to). The spy records `putPendingSubmission` /
 *      `removePendingSubmission` calls without diverging from the real
 *      Keystore-backed behavior — preserves the AC-6 cluster-bleed lookup
 *      against Stage 1's cached record while proving DD-3-3-B / DD-3-3-C
 *      ordering at the androidTest tier.
 *   3. Call `plugin.mwaSignAndSendTransactions("e2e-sas-ac1",
 *      listOf(byteArrayOf(0x01, 0x02)), 10_000L)` via the test seam.
 *   4. Assert within 5s (via `PluginTestHarness.awaitCondition`):
 *      - `postSignAndSendCompleted` fires exactly once with
 *        `request_id == "e2e-sas-ac1"` (AC-1 correlation invariant).
 *      - `result.signatures` is a 1-string JSON array; the entry is a
 *        non-empty base58 string matching the fixture's
 *        `response.payload.signatures[0]` byte-for-byte (FakeMwaClient
 *        decodes the fixture via `parseSignAndSendResponse`;
 *        `buildSignAndSendSuccessJson` re-emits as a JSON string array).
 *        NOTE: signatures are base58, NOT base64 — distinct from the
 *        wire format used in `signed_payloads` / `signed_transactions`
 *        per DD-3-3-E.
 *      - `result.submitted_at` is a Long matching the injected `clock()`
 *        value (1_700_000_000_000L per harness default).
 *      - `result.confirmation_status` is the literal `"submitted"` per
 *        AC-1.
 *      - DD-3-3-B / DD-3-3-C ordering: `verifyOrder` asserts
 *        `putPendingSubmission` is called BEFORE
 *        `removePendingSubmission`, AND `removePendingSubmission` is
 *        called BEFORE `postSignAndSendCompleted` fires (AC-3 cleanup
 *        invariant).
 *      - Zero `postMwaError` / `postMwaTimeout` captures (terminal-signal
 *        invariant per DD-15).
 *
 * **Payload-shape distinctness (DD-3-3-E):** the success payload is the
 * 4-key shape `{request_id, signatures, submitted_at, confirmation_status}`
 * — NOT the 2-key `{request_id, signed_payloads | signed_transactions}`
 * shape from `sign_messages_completed` / `sign_transactions_completed`.
 * Negative-key checks below catch a regression where
 * `buildSignSuccessJson` (the 2-key builder) gets accidentally wired
 * into `postSignAndSendCompleted` instead of `buildSignAndSendSuccessJson`.
 *
 * **Real-wallet invariant deferred (CR-45-class):** AC-1's stricter
 * "the signatures correspond to a real cluster submission" invariant is
 * verifiable ONLY against a real wallet and a real RPC endpoint. The
 * `sign_and_send_success.json` fixture carries a placeholder base58
 * signature ("5CrhasMLvBzCo8...") — round-tripped intact through the
 * plumbing but never carrying a real chain submission. The structural
 * assertion here proves the FORWARD AND REVERSE plumbing (Kotlin →
 * FakeMwaClient → handleSignAndSendSuccess → buildSignAndSendSuccessJson →
 * nativeBridge.postSignAndSendCompleted) works end-to-end on-device.
 * The on-chain validity is the responsibility of the manual release-gate
 * procedure against Phantom/Solflare in Story 3-3's pre-Gate-5 close-out
 * window (parallel to Story 3-1 / 3-2's procedure).
 */
@RunWith(AndroidJUnit4::class)
class AC1SignAndSendHappyE2ETest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File
    private lateinit var sharedStore: SecureTokenStore

    private val identityUri = "https://example.com/godot-mwa-signsend-ac1"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (3-3 AC-1)")
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
        // Single shared real ESP-backed store; the spy in Stage 2 wraps THIS
        // instance so Stage 1's putToken-cached CacheRecord is observable to
        // the AC-6 cluster-bleed lookup in Stage 3.
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
    fun ac1_signAndSendEmitsCompletedSignalWithFixtureSignatures() {
        // --- Stage 1: connect ---------------------------------------------------
        plugin.mwaAuthorize(
            requestId = "req-sas-ac1-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-sas-ac1-connect", any()) }
            }.isSuccess
        }

        val keysAfterConnect = sharedStore.listAllKeys()
        assertEquals("Exactly 1 record cached post-connect", 1, keysAfterConnect.size)
        assertNotNull(
            "CacheRecord present in SecureTokenStore after connect",
            sharedStore.getToken(keysAfterConnect.single()),
        )
        assertNotNull(
            "sessionState.getAuthToken() must not be null after successful connect",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )

        // --- Stage 2: spy-wrap the real store + rebuild plugin ------------------
        // spyk delegates to real methods by default — putPendingSubmission /
        // removePendingSubmission still write to the real EncryptedSharedPreferences,
        // and listAllKeys returns the Stage 1 cached record so the AC-6
        // cluster-bleed preflight passes. The spy adds call-recording so we
        // can verifyOrder the DD-3-3-B / DD-3-3-C ordering invariants.
        val spiedStore = spyk(sharedStore)
        val signPlugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
            storeOverride = spiedStore,
        )

        // --- Stage 3: call mwaSignAndSendTransactions ---------------------------
        signPlugin.mwaSignAndSendTransactions(
            requestId = "e2e-sas-ac1",
            transactions = listOf(byteArrayOf(0x01, 0x02)),
            timeoutMs = 10_000L,
        )

        // --- Stage 4: assert within 5s -----------------------------------------
        val jsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(5_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postSignAndSendCompleted("e2e-sas-ac1", capture(jsonSlot)) }
            }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)

        // AC-1: request_id round-trips from the call site to the terminal signal.
        assertEquals("request_id in payload", "e2e-sas-ac1", payload.getString("request_id"))

        // DD-3-3-E: 4-key shape (signatures + submitted_at + confirmation_status).
        // Distinct from the 2-key sign_messages / sign_transactions completed shapes.
        assertTrue(
            "DD-3-3-E: payload must have signatures",
            payload.has("signatures"),
        )
        assertTrue(
            "DD-3-3-E: payload must have submitted_at",
            payload.has("submitted_at"),
        )
        assertTrue(
            "DD-3-3-E: payload must have confirmation_status",
            payload.has("confirmation_status"),
        )

        // DD-3-3-E negative-key guards: catch a regression where
        // buildSignSuccessJson (2-key builder for sign_messages /
        // sign_transactions) gets accidentally wired into
        // postSignAndSendCompleted instead of buildSignAndSendSuccessJson.
        assertTrue(
            "DD-3-3-E: must NOT have signed_payloads (sign_messages key)",
            !payload.has("signed_payloads"),
        )
        assertTrue(
            "DD-3-3-E: must NOT have signed_transactions (sign_transactions key)",
            !payload.has("signed_transactions"),
        )
        assertTrue(
            "DD-3-3-E: success payload must NOT carry source_method (mwa_error-only per A-14)",
            !payload.has("source_method"),
        )
        assertTrue(
            "DD-3-3-E: success payload must NOT carry auth_token",
            !payload.has("auth_token"),
        )
        assertTrue(
            "DD-3-3-E: success payload must NOT carry auth_token_fingerprint (connect/reauth-only)",
            !payload.has("auth_token_fingerprint"),
        )

        val signatures = payload.getJSONArray("signatures")
        assertEquals(
            "AC-1: signatures.size must equal input transactions.size (1 input → 1 signature)",
            1,
            signatures.length(),
        )

        // AC-1 / fixture-byte-fidelity: signature is the base58 string from
        // sign_and_send_success.json's response.payload.signatures[0].
        // NOTE: base58 (Solana tx-signature convention), NOT base64. If the
        // fixture is regenerated against a real wallet for a future Story 5-x
        // release-gate refresh, update this hardcoded literal — the test
        // catches divergence loudly so the regeneration is intentional and
        // audited.
        assertEquals(
            "AC-1: signatures[0] must match sign_and_send_success.json fixture (base58)",
            "5CrhasMLvBzCo8bfv6WpPzGTVrrAENZrwUtsSENXkpBJcsaQyBFbLDxLK2wpTtQhMwUS2UvuyJrqN2D2jqV5gn1T",
            signatures.getString(0),
        )

        // AC-1: submitted_at is a Long matching the injected harness clock().
        assertEquals(
            "AC-1: submitted_at must equal harness clock() (1_700_000_000_000L)",
            1_700_000_000_000L,
            payload.getLong("submitted_at"),
        )

        // AC-1: confirmation_status is the literal "submitted" today.
        assertEquals(
            "AC-1: confirmation_status must be the literal \"submitted\"",
            "submitted",
            payload.getString("confirmation_status"),
        )

        // DD-3-3-B (write-then-call) + DD-3-3-C (cleanup-before-terminal):
        // putPendingSubmission must be called BEFORE removePendingSubmission;
        // removePendingSubmission must be called BEFORE the terminal
        // postSignAndSendCompleted. The full ordering proves the breadcrumb
        // mechanic at the integration tier (the unit tier covers the same
        // ordering with mocked stores; this tier proves it survives the
        // dispatcher / coroutine round-trip on a real ESP).
        verifyOrder {
            spiedStore.putPendingSubmission("e2e-sas-ac1", any())
            spiedStore.removePendingSubmission("e2e-sas-ac1")
            nativeBridge.postSignAndSendCompleted("e2e-sas-ac1", any())
        }

        // Terminal-signal invariant per DD-15 — no concurrent error/timeout
        // alongside the completion. AC-1 is the success path; either signal
        // here would be a regression. The other 2 sign signals must also stay
        // silent — postSignTransactionsCompleted should never fire on a
        // sign_and_send call (regression guard for accidental signal-name
        // swap in handleSignAndSendSuccess).
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postSignMessagesCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postSignTransactionsCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postPendingSubmissionFound(any()) }
    }
}
