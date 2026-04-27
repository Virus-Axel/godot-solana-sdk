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
 * Story 3-1 T5 — AC-2 happy E2E on-device: `MWA.sign_messages(...)` returns
 * the signed-payload list through the unified terminal signal path.
 *
 * Validates (on emulator, with real coroutines + real EncryptedSharedPreferences
 * + real Keystore + Route B in-process FakeMwaClient):
 *
 *   1. Full connect via `FakeMwaClient("success")` — assert `connect_completed`
 *      fires and a `CacheRecord` lands in `SecureTokenStore` (preconditions for
 *      `mwaSignMessages`'s DD-3-1-6 NOT_CONNECTED preflight to NOT trigger).
 *   2. Rebuild plugin with the same `scenario = "success"` token — the next
 *      FakeMwaClient resolves `sign_messages_success.json` when
 *      `mwaSignMessages` calls the factory (op="sign_messages").
 *   3. Call `plugin.mwaSignMessages("e2e-ac2", listOf("hello".toByteArray(),
 *      "world".toByteArray()), 10_000L)` via the test seam.
 *   4. Assert within 5s (via `PluginTestHarness.awaitCondition`):
 *      - `postSignMessagesCompleted` fires exactly once with
 *        `request_id == "e2e-ac2"` (AC-2 correlation invariant — terminal
 *        signal carries the caller-supplied request_id round-trip).
 *      - `result.signed_payloads` is a JSON array of size 2 (matches input
 *        messages count) containing base64 strings (RFC 4648 §4) that match
 *        the fixture's `response.payload.signed_payloads` byte-for-byte
 *        (FakeMwaClient base64-decodes the fixture into ByteArrays;
 *        `buildSignSuccessJson` re-encodes via java.util.Base64 — the
 *        round-trip is identity for valid base64).
 *      - Each base64-decoded payload is non-empty.
 *      - Zero `postMwaError` / `postMwaTimeout` captures (terminal-signal
 *        invariant per DD-15 — AC-2 is the success path; any error or
 *        timeout would be a regression).
 *
 * **Real-wallet invariant deferred (CR-45-class):**
 * AC-2's stricter "decoded length ≥ input length + 64 (signature appended)"
 * invariant from the production contract is verifiable ONLY against real
 * ed25519-signing wallet output. The `sign_messages_success.json` fixture
 * carries placeholder strings ("signed-message-one"/"signed-message-two",
 * 18 bytes each) — round-tripped intact through the plumbing but never
 * length-prefixed with a real 64-byte signature. The structural assertion
 * here (size==2 + non-empty + matches fixture) proves the FORWARD AND
 * REVERSE plumbing (Kotlin → FakeMwaClient → handleSignMessagesSuccess →
 * buildSignSuccessJson → nativeBridge.postSignMessagesCompleted) works
 * end-to-end on-device. The byte-length invariant is asserted in two
 * other places: T6 contract test (FakeMwaClient vs recorded-real-clientlib
 * adapter agreement, DD-3-1-7) and the planned manual release-gate run
 * against Phantom/Solflare in Story 3-1's pre-Gate-5 close-out window.
 *
 * **Activity-stack assertion (parallel to AC1ReauthorizeSilentTest):**
 * Route B is in-process — no Intent is ever fired to a wallet. The
 * `nativeBridge` mock capturing exactly one `postSignMessagesCompleted`
 * and zero `postMwaError` / `postMwaTimeout` proves the FakeMwaClient
 * path completed without ever dispatching to a wallet. The dumpsys /
 * ActivityMonitor approach is reserved for the manual release-gate
 * procedure on a physical device.
 */
@RunWith(AndroidJUnit4::class)
class AC2SignMessagesHappyE2ETest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File

    private val identityUri = "https://example.com/godot-mwa-signmsg-ac2"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (3-1 AC-2)")
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
    fun ac2_signMessagesEmitsCompletedSignalWithFixturePayloads() {
        // --- Stage 1: connect ---------------------------------------------------
        plugin.mwaAuthorize(
            requestId = "req-ac2-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-ac2-connect", any()) }
            }.isSuccess
        }

        // Pre-condition: a cached auth token must exist before sign_messages can
        // pass DD-3-1-6 preflight (sessionState.getAuthToken() != null path).
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

        // --- Stage 2: rebuild plugin with sign_messages_success scenario -------
        // Same SecureTokenStore + sessionState (shared via ctx). The
        // `scenario = "success"` token causes mwaClientFactory() to create a
        // FakeMwaClient(...).withScenario("success"), which resolves to
        // sign_messages_success.json when mwaSignMessages calls the factory
        // (op="sign_messages").
        val signPlugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
        )

        // --- Stage 3: call mwaSignMessages -------------------------------------
        signPlugin.mwaSignMessages(
            requestId = "e2e-ac2",
            messages = listOf("hello".toByteArray(), "world".toByteArray()),
            timeoutMs = 10_000L,
        )

        // --- Stage 4: assert within 5s -----------------------------------------
        val jsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(5_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postSignMessagesCompleted("e2e-ac2", capture(jsonSlot)) }
            }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)

        // AC-2: request_id round-trips from the call site to the terminal signal.
        assertEquals("request_id in payload", "e2e-ac2", payload.getString("request_id"))

        // AC-2 / DD-3-1-5: signed_payloads is the only other key in the success
        // payload. No source_method (mwa_error-only per A-14), no auth_token
        // (secret material does not cross this boundary — DD-3-1-5).
        assertTrue("payload must have signed_payloads", payload.has("signed_payloads"))
        assertTrue(
            "AC-2 / DD-3-1-5: success payload must NOT carry source_method (that's mwa_error-only per A-14)",
            !payload.has("source_method"),
        )
        assertTrue(
            "AC-2 / DD-3-1-5: success payload must NOT carry auth_token (secret material per DD-3-1-5)",
            !payload.has("auth_token"),
        )

        // DD-3-1-5 / Guardrails: the sign_messages success payload is exactly
        // 2 keys ({request_id, signed_payloads}). The connect/reauthorize
        // success payload is 6 keys per DD-2-2-4 ({request_id, public_key,
        // auth_token_fingerprint, wallet_label, wallet_icon_uri, cluster}).
        // The Guardrails forbid a sibling/replacement `buildSuccessJson` —
        // these 5 negative checks catch a regression where buildSuccessJson
        // (the 6-key auth builder) gets accidentally wired into
        // postSignMessagesCompleted instead of buildSignSuccessJson (the
        // 2-key sign builder).
        assertTrue(
            "DD-3-1-5: sign_messages success payload must NOT carry auth_token_fingerprint (connect/reauthorize-only per DD-2-2-4)",
            !payload.has("auth_token_fingerprint"),
        )
        assertTrue(
            "DD-3-1-5: sign_messages success payload must NOT carry public_key (connect/reauthorize-only)",
            !payload.has("public_key"),
        )
        assertTrue(
            "DD-3-1-5: sign_messages success payload must NOT carry wallet_label (connect/reauthorize-only)",
            !payload.has("wallet_label"),
        )
        assertTrue(
            "DD-3-1-5: sign_messages success payload must NOT carry wallet_icon_uri (connect/reauthorize-only)",
            !payload.has("wallet_icon_uri"),
        )
        assertTrue(
            "DD-3-1-5: sign_messages success payload must NOT carry cluster (connect/reauthorize-only)",
            !payload.has("cluster"),
        )

        val signedPayloads = payload.getJSONArray("signed_payloads")

        // AC-2: signed_payloads.size matches input messages.size (1:1 per-message
        // signature). The unit-tier test (MwaAndroidPluginSignMessagesTest. AC-2)
        // covers the same shape; this on-device run additionally proves the
        // forward-and-reverse JNI/dispatcher path (T3 JNIEXPORT, T4 facade) end-to-end.
        assertEquals(
            "AC-2: signed_payloads.size must equal input messages.size",
            2,
            signedPayloads.length(),
        )

        // AC-2 / fixture-byte-fidelity: each entry is a non-empty base64 string
        // matching the fixture's `response.payload.signed_payloads` after round-trip
        // (FakeMwaClient base64-decodes fixture entries → buildSignSuccessJson
        // re-encodes via java.util.Base64). Real-wallet "≥ input + 64" byte-length
        // invariant is deferred to T6 contract test + manual release-gate (kdoc above).
        //
        // Hardcoded from `testdata/mwa-fixtures/sign_messages_success.json`'s
        // `response.payload.signed_payloads` ("c2lnbmVkLW1lc3NhZ2Utb25l" decodes
        // to "signed-message-one"; "c2lnbmVkLW1lc3NhZ2UtdHdv" → "signed-message-two").
        // If the fixture is regenerated (e.g., recorded against a real wallet
        // for the T6 contract test or a future Story 5-x release-gate refresh),
        // update this list — the test catches divergence loudly so the
        // regeneration is intentional and audited.
        val expectedBase64 = listOf("c2lnbmVkLW1lc3NhZ2Utb25l", "c2lnbmVkLW1lc3NhZ2UtdHdv")
        for (i in 0 until signedPayloads.length()) {
            val entry = signedPayloads.getString(i)
            assertEquals(
                "signed_payloads[$i] must match sign_messages_success.json fixture (round-trip identity)",
                expectedBase64[i],
                entry,
            )
            val decoded = Base64.getDecoder().decode(entry)
            assertTrue(
                "signed_payloads[$i] decoded bytes must be non-empty",
                decoded.isNotEmpty(),
            )
        }

        // Terminal-signal invariant per DD-15 — no concurrent error/timeout
        // alongside the completion. AC-2 is the success path; either signal
        // here would be a regression (e.g., a watchdog firing late or a
        // double-route through nativeBridge.postMwaError).
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
