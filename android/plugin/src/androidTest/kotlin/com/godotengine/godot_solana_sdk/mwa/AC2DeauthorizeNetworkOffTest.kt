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
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 4-1 T5 — AC-2 on-device: `MWA.deauthorize()` with the wallet's
 * remote-revoke endpoint failing (network off / NETWORK_OFFLINE).
 *
 * AC-2 spec: `deauthorize_completed{remote_revoke_succeeded: false,
 * local_cache_cleared: true, warning: "remote_unreachable"}` fires; the
 * SecureTokenStore record is STILL removed (AC-NFR-SEC-5 — local wipe
 * unconditional). Per DD-4-1-1, remote-revoke failure routes to
 * `deauthorize_completed`, NOT to `mwa_error`.
 *
 * Two-stage plugin construction:
 *   - Stage 1 plugin: scenario="success" — connects via authorize_success.json
 *     to populate MwaSessionState (singleton) + SecureTokenStore (real
 *     EncryptedSharedPreferences).
 *   - Stage 2 plugin: scenario="remote_unreachable" — fresh
 *     GDExtensionAndroidPlugin instance with FakeMwaClient configured to
 *     return MwaResult.Failure(NetworkOffline) for deauthorize via
 *     deauthorize_remote_unreachable.json. The MwaSessionState singleton +
 *     real SecureTokenStore carry across plugin instances, so the deauth
 *     path sees the connect-state set up in Stage 1.
 *
 * Per CR-45 inheritance: in-process FakeMwaClient (Route B), no Fake Wallet
 * APK install. Real EncryptedSharedPreferences round-trip earns this test's
 * seat over the unit-tier MwaAndroidPluginDeauthorizeTest case 2 — if the
 * impl's wipe is somehow no-op'd at the on-disk layer, this test catches it.
 */
@RunWith(AndroidJUnit4::class)
class AC2DeauthorizeNetworkOffTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var fixturesDir: File

    private val identityUri = "https://example.com/godot-mwa-deauth-ac2"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (4-1 AC-2)")
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
    }

    @After
    fun teardown() {
        runCatching { SecureTokenStore(ctx).deleteAll() }
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @Test
    fun ac2_deauthorizeWithRemoteUnreachableStillWipesAndEmitsWarning() {
        // --- Stage 1: connect with success scenario --------------------------
        val pluginConnect = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
        )
        pluginConnect.mwaAuthorize(
            requestId = "req-ac2-deauth-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) {
                    nativeBridge.postConnectCompleted("req-ac2-deauth-connect", any())
                }
            }.isSuccess
        }

        // Sanity: store has the cached record + sessionState populated.
        val storeBefore = SecureTokenStore(ctx)
        assertEquals(
            "Exactly 1 record cached post-connect",
            1,
            storeBefore.listAllKeys().size,
        )
        assertTrue(
            "publicKey populated post-connect",
            GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58().isNotEmpty(),
        )

        // --- Stage 2: deauthorize with remote_unreachable scenario -----------
        // Fresh plugin instance with FakeMwaClient configured to return
        // MwaResult.Failure(NetworkOffline) on deauthorize. MwaSessionState
        // (singleton) + SecureTokenStore (real on-disk) are unchanged from
        // Stage 1, so the deauth path snapshots the populated authToken +
        // identityUri and proceeds into the else-branch try-block.
        val pluginDeauth = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "remote_unreachable",
            nativeBridge = nativeBridge,
        )
        pluginDeauth.mwaDeauthorize("deauthorize-ac2")

        val jsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) {
                    nativeBridge.postDeauthorizeCompleted("deauthorize-ac2", capture(jsonSlot))
                }
            }.isSuccess
        }

        // (a) AC-2 payload: remote_revoke_succeeded:false +
        //     local_cache_cleared:true + warning:"remote_unreachable".
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("deauthorize-ac2", payload.getString("request_id"))
        assertFalse(
            "remote_revoke_succeeded == false on NetworkOffline failure",
            payload.getBoolean("remote_revoke_succeeded"),
        )
        assertTrue(
            "local_cache_cleared == true regardless of remote outcome (AC-NFR-SEC-5)",
            payload.getBoolean("local_cache_cleared"),
        )
        assertEquals(
            "warning == remote_unreachable on remote failure (DD-4-1-1)",
            "remote_unreachable",
            payload.getString("warning"),
        )

        // (b) Real on-disk wipe — HIGHEST-LEVERAGE assertion. If the impl's
        //     finally-block wipe was somehow no-op'd at the on-disk layer
        //     (e.g. deleteToken is in the try block, not finally), this fails.
        val keysAfter = SecureTokenStore(ctx).listAllKeys()
        assertEquals(
            "All matching CacheRecords wiped despite remote-revoke failure — " +
                "real EncryptedSharedPreferences round-trip",
            0,
            keysAfter.size,
        )

        // (c) MwaSessionState fully cleared (DD-4-1-2 full clear).
        assertNull(
            "authToken wiped despite remote failure",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )
        assertEquals(
            "identityUri wiped (full clear)",
            "",
            GDExtensionAndroidPlugin.sessionState.getIdentityUri(),
        )
        assertEquals(
            "publicKeyBase58 wiped",
            "",
            GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(),
        )

        // (d) AC-2 + DD-4-1-1: remote failure routes to deauthorize_completed,
        //     NOT to mwa_error.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
