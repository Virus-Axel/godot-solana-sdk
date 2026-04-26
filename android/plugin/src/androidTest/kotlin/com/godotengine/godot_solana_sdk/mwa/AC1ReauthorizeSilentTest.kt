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

/**
 * Story 2-2 T5 — AC-1 + AC-6 on-device: `MWA.reauthorize()` silently
 * restores the session without any wallet UI and preserves the fingerprint.
 *
 * Validates (on emulator, with real coroutines + real EncryptedSharedPreferences
 * + real Keystore):
 *
 *   1. Full connect via `FakeMwaClient("success")` — assert `connect_completed`
 *      fires and a `CacheRecord` lands in `SecureTokenStore`.
 *   2. Capture `prePublicKey = sessionState.getPublicKeyBase58()` and
 *      `prePrint = sessionState.getAuthTokenFingerprint()` before reauthorize.
 *   3. Switch FakeMwaClient scenario to `reauthorize_success` by rebuilding the
 *      plugin with that scenario (the store state is shared via the same ctx).
 *   4. Call `plugin.mwaReauthorize("rea-on-device", identityJson, "devnet",
 *      "solana:devnet", 5000L)` via the test seam.
 *   5. Assert within 3s (via `PluginTestHarness.awaitCondition`):
 *      - `reauthorize_completed` fires exactly once with
 *        `request_id == "rea-on-device"` and `result.public_key == prePublicKey`
 *        (AC-1 same-public-key invariant).
 *      - `result.auth_token_fingerprint == prePrint` (AC-6 in-band assertion).
 *      - `sessionState.getAuthTokenFingerprint() == prePrint` (AC-6 getter).
 *      - `sessionState.getAuthToken() != null`.
 *      - `sessionState.getPublicKeyBase58() == prePublicKey`.
 *      - Zero `mwa_error` / `mwa_timeout` captures.
 *
 * **Activity-stack assertion (AC-1 "zero wallet transitions"):**
 * This test uses a `PluginTestHarness`-injected `FakeMwaClient` (Route B per
 * D-T9-1 / CR-45) which runs entirely in-process — the real Fake Wallet APK
 * is never launched. Therefore the android Activity back-stack NEVER shows
 * `com.solanamobile.fakewallet` during the test window by construction.
 * Both `UiDevice.executeShellCommand("dumpsys activity activities")` and
 * `Instrumentation.addMonitor(IntentFilter, ...)` are technically available
 * from this test (the Instrumentation reference is right there via
 * `InstrumentationRegistry.getInstrumentation()`), but in Route B both would
 * always report 0 transitions because no Intent is ever fired — they would
 * add zero information. The zero-transition guarantee is therefore asserted
 * structurally: the `nativeBridge` mock captures EXACTLY one
 * `postReauthorizeCompleted` and ZERO `postMwaError` / `postMwaTimeout`,
 * which proves the in-process FakeMwaClient path completed without ever
 * dispatching an Intent to a wallet. The `adb dumpsys` / ActivityMonitor
 * approach is reserved for the T6 manual release-gate procedure (AC-5),
 * which runs against a real Phantom on a physical device where wallet
 * Activity transitions are observable and meaningful.
 */
@RunWith(AndroidJUnit4::class)
class AC1ReauthorizeSilentTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File

    private val identityUri = "https://example.com/godot-mwa-reauth-ac1"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (2-2 AC-1)")
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
    fun ac1_reauthorizeCompletedFiresWithRetainedPublicKeyAndPreservedFingerprint() {
        // --- Stage 1: connect ---------------------------------------------------
        plugin.mwaAuthorize(
            requestId = "req-ac1-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-ac1-connect", any()) }
            }.isSuccess
        }

        // Verify the CacheRecord was persisted (AC-1 pre-condition: a cached auth
        // token must exist before reauthorize can succeed silently).
        val store = SecureTokenStore(ctx)
        val keysAfterConnect = store.listAllKeys()
        assertEquals("Exactly 1 record cached post-connect", 1, keysAfterConnect.size)
        assertNotNull(
            "CacheRecord present in SecureTokenStore after connect",
            store.getToken(keysAfterConnect.single()),
        )

        // --- Stage 2: capture pre-reauthorize state ---------------------------------
        // Snapshot public_key + fingerprint BEFORE reauth. Both must survive unchanged
        // after a successful silent reauthorize (AC-1 same-public-key; AC-6 fingerprint).
        val prePublicKey = GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58()
        val prePrint = GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint()
        assertTrue("publicKey populated post-connect", prePublicKey.isNotEmpty())
        assertTrue("fingerprint populated post-connect", prePrint.isNotEmpty())
        assertEquals("fingerprint is 8 lowercase hex chars", 8, prePrint.length)
        assertTrue(
            "fingerprint chars are lowercase hex",
            prePrint.all { it in '0'..'9' || it in 'a'..'f' },
        )

        // --- Stage 3: rebuild plugin with reauthorize_success scenario ---------------
        // The same SecureTokenStore instance (via ctx) is shared across plugin instances.
        // The `scenario = "success"` token causes mwaClientFactory() to create
        // FakeMwaClient(...).withScenario("success"), which resolves to
        // reauthorize_success.json when mwaReauthorize calls the factory (op="reauthorize").
        val reauthPlugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
        )

        // --- Stage 4: call mwaReauthorize --------------------------------------------
        reauthPlugin.mwaReauthorize(
            requestId = "rea-on-device",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 5_000L,
        )

        // --- Stage 5: assert within 3s -----------------------------------------------
        val jsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postReauthorizeCompleted("rea-on-device", capture(jsonSlot)) }
            }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)

        // AC-1: request_id matches the call.
        assertEquals("request_id in payload", "rea-on-device", payload.getString("request_id"))

        // AC-1: public_key is identical to the pre-reauthorize value (same session identity).
        assertEquals(
            "AC-1: public_key must equal pre-reauthorize value (same-public-key invariant)",
            prePublicKey,
            payload.getString("public_key"),
        )

        // AC-6 in-band: fingerprint in the reauthorize_completed payload matches pre-reauth.
        assertEquals(
            "AC-6 in-band: auth_token_fingerprint in payload must equal prePrint",
            prePrint,
            payload.getString("auth_token_fingerprint"),
        )

        // DD-2-2-4 success payload shape — full 6-key coverage. The 3 keys
        // already asserted above are request_id, public_key, auth_token_fingerprint.
        // The remaining 3 (wallet_label, wallet_icon_uri, cluster) come from
        // the cached CacheRecord (handleReauthSuccess uses cached.walletLabel /
        // cached.walletIconUri; cluster echoes the request arg).
        assertTrue(
            "DD-2-2-4: wallet_label key present in success payload",
            payload.has("wallet_label"),
        )
        assertTrue(
            "DD-2-2-4: wallet_icon_uri key present in success payload",
            payload.has("wallet_icon_uri"),
        )
        assertEquals(
            "DD-2-2-4: cluster matches the mwaReauthorize request arg",
            "devnet",
            payload.getString("cluster"),
        )

        // DD-2-2-4 negative invariant — `source_method` is mwa_error-only per A-14.
        // A copy-paste from buildErrorJson into buildSuccessJson would slip
        // a source_method key into the success payload; this guard catches it.
        assertTrue(
            "DD-2-2-4: success payload must NOT carry source_method (that's mwa_error-only per A-14)",
            !payload.has("source_method"),
        )

        // AC-6 getter: session state reflects the same fingerprint post-reauth.
        assertEquals(
            "AC-6 getter: sessionState fingerprint must equal prePrint after reauthorize",
            prePrint,
            GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint(),
        )

        // AC-1: session state public_key unchanged.
        assertEquals(
            "AC-1: sessionState publicKeyBase58 must equal prePublicKey after reauthorize",
            prePublicKey,
            GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(),
        )

        // Auth token is present post-reauth (session fully restored).
        assertNotNull(
            "sessionState.getAuthToken() must not be null after successful reauthorize",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )

        // Terminal-signal invariant: no concurrent error/timeout alongside the completion.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
