package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import io.mockk.mockk
import io.mockk.slot
import io.mockk.verify
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
import java.util.Base64

/**
 * Story 3-2 T5 — AC-1 happy E2E on-device: `MWA.sign_transactions(...)` returns
 * the signed-transaction list through the unified terminal signal path.
 *
 * Validates (on emulator, with real coroutines + real EncryptedSharedPreferences
 * + real Keystore + Route B in-process FakeMwaClient — D-T9-1 inheritance):
 *
 *   1. Full connect via `FakeMwaClient("success")` — assert `connect_completed`
 *      fires and a `CacheRecord` lands in `SecureTokenStore` (preconditions for
 *      `mwaSignTransactions`'s DD-3-1-6-inherited NOT_CONNECTED preflight to
 *      NOT trigger).
 *   2. Rebuild plugin with the same `scenario = "success"` token — the next
 *      FakeMwaClient resolves `sign_transactions_success.json` when
 *      `mwaSignTransactions` calls the factory (op="sign_transactions").
 *   3. Call `plugin.mwaSignTransactions("e2e-st-ac1", listOf(byteArrayOf(0x01,
 *      0x02), byteArrayOf(0x03, 0x04)), 10_000L)` via the test seam.
 *   4. Assert within 5s (via `PluginTestHarness.awaitCondition`):
 *      - `postSignTransactionsCompleted` fires exactly once with
 *        `request_id == "e2e-st-ac1"` (AC-1 correlation invariant — terminal
 *        signal carries the caller-supplied request_id round-trip).
 *      - `result.signed_transactions` is a JSON array of size 2 (matches input
 *        transactions count) containing base64 strings (RFC 4648 §4) that match
 *        the fixture's `response.payload.signed_payloads` byte-for-byte
 *        (FakeMwaClient base64-decodes the fixture into ByteArrays;
 *        `buildSignSuccessJson(payloadKey="signed_transactions")` re-encodes
 *        via java.util.Base64 — the round-trip is identity for valid base64).
 *      - Each base64-decoded payload is non-empty.
 *      - Zero `postMwaError` / `postMwaTimeout` captures (terminal-signal
 *        invariant per DD-15 — AC-1 is the success path; any error or
 *        timeout would be a regression).
 *
 * **Payload-key rename invariant (DD-3-2-3):** The fixture file's response
 * key is `signed_payloads` (op-agnostic — `parseSignResponse` is shared
 * across `sign_messages` / `sign_transactions` / `sign_and_send`). The
 * Kotlin side renames the array's JSON key to `signed_transactions` in
 * `buildSignSuccessJson(payloadKey="signed_transactions")` per DD-3-2-3 +
 * D-3-2-1 — the renaming happens in the success path on the way to the
 * GDScript-facing signal, not in the fixture. The negative-key checks below
 * (`!payload.has("signed_payloads")`) prove the rename happened correctly:
 * a regression that forgot the `payloadKey =` arg would emit the wire-format
 * key `signed_payloads` and fail this test loud, before T6's contract-test
 * tier ever runs.
 *
 * **Real-wallet invariant deferred (CR-45-class):**
 * AC-1's stricter "decoded length corresponds to a valid signed Solana
 * Transaction with one or more populated ed25519 signatures" invariant from
 * the production contract is verifiable ONLY against real wallet output.
 * The `sign_transactions_success.json` fixture carries placeholder strings
 * ("tx-one-signed"/"tx-two-signed", 13 bytes each) — round-tripped intact
 * through the plumbing but never carrying real signature bytes. The
 * structural assertion here (size==2 + non-empty + matches fixture) proves
 * the FORWARD AND REVERSE plumbing (Kotlin → FakeMwaClient →
 * handleSignTransactionsSuccess → buildSignSuccessJson →
 * nativeBridge.postSignTransactionsCompleted) works end-to-end on-device.
 * The byte-length / signature-validity invariant is the responsibility of
 * the manual release-gate procedure against Phantom/Solflare in Story 3-2's
 * pre-Gate-5 close-out window (parallel to Story 3-1's procedure).
 *
 * **Activity-stack assertion (parallel to AC2SignMessagesHappyE2ETest):**
 * Route B is in-process — no Intent is ever fired to a wallet. The
 * `nativeBridge` mock capturing exactly one `postSignTransactionsCompleted`
 * and zero `postMwaError` / `postMwaTimeout` proves the FakeMwaClient
 * path completed without ever dispatching to a wallet. The dumpsys /
 * ActivityMonitor approach is reserved for the manual release-gate
 * procedure on a physical device.
 */
@RunWith(AndroidJUnit4::class)
class AC1SignTransactionsHappyE2ETest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File

    private val identityUri = "https://example.com/godot-mwa-signtxn-ac1"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (3-2 AC-1)")
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
        plugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
        )
    }

    @After
    fun teardown() {
        runCatching { SecureTokenStore(ctx).deleteAll() }
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @Test
    fun ac1_signTransactionsEmitsCompletedSignalWithFixturePayloads() {
        // --- Stage 1: connect ---------------------------------------------------
        plugin.mwaAuthorize(
            requestId = "req-st-ac1-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-st-ac1-connect", any()) }
            }.isSuccess
        }

        // Pre-condition: a cached auth token must exist before sign_transactions
        // can pass DD-3-1-6-inherited preflight (sessionState.getAuthToken() != null).
        val store = SecureTokenStore(ctx)
        val keysAfterConnect = store.listAllKeys()
        assertEquals("Exactly 1 record cached post-connect", 1, keysAfterConnect.size)
        assertNotNull(
            "CacheRecord present in SecureTokenStore after connect",
            store.getToken(keysAfterConnect.single()),
        )
        assertNotNull(
            "sessionState.getAuthToken() must not be null after successful connect",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )

        // --- Stage 2: rebuild plugin with sign_transactions_success scenario ---
        // Same SecureTokenStore + sessionState (shared via ctx). The
        // `scenario = "success"` token causes mwaClientFactory() to create a
        // FakeMwaClient(...).withScenario("success"), which resolves to
        // sign_transactions_success.json when mwaSignTransactions calls the
        // factory (op="sign_transactions" — the FakeMwaClient resolution path
        // is `${op}_${scenario}.json` per FakeMwaClient.kt:155).
        val signPlugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
        )

        // --- Stage 3: call mwaSignTransactions ---------------------------------
        signPlugin.mwaSignTransactions(
            requestId = "e2e-st-ac1",
            transactions = listOf(byteArrayOf(0x01, 0x02), byteArrayOf(0x03, 0x04)),
            timeoutMs = 10_000L,
        )

        // --- Stage 4: assert within 5s -----------------------------------------
        val jsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(5_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postSignTransactionsCompleted("e2e-st-ac1", capture(jsonSlot)) }
            }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)

        // AC-1: request_id round-trips from the call site to the terminal signal.
        assertEquals("request_id in payload", "e2e-st-ac1", payload.getString("request_id"))

        // DD-3-2-3: signed_transactions is the renamed payload key (NOT
        // signed_payloads — the wire-format key from the fixture). The Kotlin
        // side renames via buildSignSuccessJson(payloadKey="signed_transactions").
        assertTrue(
            "DD-3-2-3: payload must have signed_transactions (renamed from wire-format signed_payloads)",
            payload.has("signed_transactions"),
        )
        assertTrue(
            "DD-3-2-3 regression guard: payload must NOT have signed_payloads " +
                "(the rename to signed_transactions must happen in buildSignSuccessJson " +
                "— a regression that forgot payloadKey= would leak the wire-format key)",
            !payload.has("signed_payloads"),
        )
        assertTrue(
            "DD-3-2-3 / DD-3-1-5: success payload must NOT carry source_method (that's mwa_error-only per A-14)",
            !payload.has("source_method"),
        )
        assertTrue(
            "DD-3-2-3 / DD-3-1-5: success payload must NOT carry auth_token (secret material)",
            !payload.has("auth_token"),
        )

        // DD-3-2-3 / Guardrails: the sign_transactions success payload is
        // exactly 2 keys ({request_id, signed_transactions}). The
        // connect/reauthorize success payload is 6 keys per DD-2-2-4
        // ({request_id, public_key, auth_token_fingerprint, wallet_label,
        // wallet_icon_uri, cluster}). Mirror the AC2SignMessagesHappyE2ETest
        // negative-key checks: catch a regression where buildSuccessJson (the
        // 6-key auth builder) gets accidentally wired into
        // postSignTransactionsCompleted instead of buildSignSuccessJson.
        assertTrue(
            "DD-3-2-3: sign_transactions success payload must NOT carry auth_token_fingerprint (connect/reauthorize-only per DD-2-2-4)",
            !payload.has("auth_token_fingerprint"),
        )
        assertTrue(
            "DD-3-2-3: sign_transactions success payload must NOT carry public_key (connect/reauthorize-only)",
            !payload.has("public_key"),
        )
        assertTrue(
            "DD-3-2-3: sign_transactions success payload must NOT carry wallet_label (connect/reauthorize-only)",
            !payload.has("wallet_label"),
        )
        assertTrue(
            "DD-3-2-3: sign_transactions success payload must NOT carry wallet_icon_uri (connect/reauthorize-only)",
            !payload.has("wallet_icon_uri"),
        )
        assertTrue(
            "DD-3-2-3: sign_transactions success payload must NOT carry cluster (connect/reauthorize-only)",
            !payload.has("cluster"),
        )

        val signedTransactions = payload.getJSONArray("signed_transactions")

        // AC-1: signed_transactions.size matches input transactions.size (1:1
        // per-transaction signature). The unit-tier test
        // (MwaAndroidPluginSignTransactionsTest. AC-1) covers the same shape;
        // this on-device run additionally proves the forward-and-reverse
        // JNI/dispatcher path (T3 JNIEXPORT, T4 facade) end-to-end.
        assertEquals(
            "AC-1: signed_transactions.size must equal input transactions.size",
            2,
            signedTransactions.length(),
        )

        // AC-1 / fixture-byte-fidelity: each entry is a non-empty base64 string
        // matching the fixture's `response.payload.signed_payloads` after
        // round-trip (FakeMwaClient base64-decodes fixture entries →
        // buildSignSuccessJson re-encodes via java.util.Base64). Real-wallet
        // signature-validity invariant is deferred to manual release-gate
        // procedure (kdoc above).
        //
        // Hardcoded from `testdata/mwa-fixtures/sign_transactions_success.json`'s
        // `response.payload.signed_payloads` ("dHgtb25lLXNpZ25lZA==" decodes
        // to "tx-one-signed"; "dHgtdHdvLXNpZ25lZA==" → "tx-two-signed").
        // If the fixture is regenerated (e.g., recorded against a real wallet
        // for a future Story 5-x release-gate refresh), update this list —
        // the test catches divergence loudly so the regeneration is
        // intentional and audited.
        val expectedBase64 = listOf("dHgtb25lLXNpZ25lZA==", "dHgtdHdvLXNpZ25lZA==")
        for (i in 0 until signedTransactions.length()) {
            val entry = signedTransactions.getString(i)
            assertEquals(
                "signed_transactions[$i] must match sign_transactions_success.json fixture (round-trip identity)",
                expectedBase64[i],
                entry,
            )
            val decoded = Base64.getDecoder().decode(entry)
            assertTrue(
                "signed_transactions[$i] decoded bytes must be non-empty",
                decoded.isNotEmpty(),
            )
        }

        // Terminal-signal invariant per DD-15 — no concurrent error/timeout
        // alongside the completion. AC-1 is the success path; either signal
        // here would be a regression (e.g., a watchdog firing late or a
        // double-route through nativeBridge.postMwaError). The other 2 sign
        // signals must also stay silent — postSignMessagesCompleted should
        // never fire on a sign_transactions call (regression guard for
        // accidental signal-name swap in handleSignTransactionsSuccess).
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postSignMessagesCompleted(any(), any()) }
    }
}
