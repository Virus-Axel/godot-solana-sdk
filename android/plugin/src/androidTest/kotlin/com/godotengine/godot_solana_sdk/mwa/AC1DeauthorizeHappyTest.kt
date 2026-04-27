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
 * Story 4-1 T5 — AC-1 on-device: `MWA.deauthorize()` happy path.
 *
 * Flow:
 *   1. Fresh connect via `FakeMwaClient("success")` → SecureTokenStore
 *      populated with real EncryptedSharedPreferences-backed CacheRecord;
 *      MwaSessionState populated with auth state + identity fields.
 *   2. Call `plugin.mwaDeauthorize("deauthorize-ac1")` (FakeMwaClient
 *      `deauthorize_success.json` returns `MwaResult.Success(Unit)`).
 *   3. Assert:
 *      (a) `deauthorize_completed` fires within ≤3000ms (AC-1 wall-clock).
 *      (b) result-dict shape: `{request_id, remote_revoke_succeeded:true,
 *          local_cache_cleared:true, warning:""}` per arch.md:669 +
 *          DD-4-1-1.
 *      (c) MwaSessionState fully cleared (DD-4-1-2: `clear()`, NOT
 *          `clearOnLogout()` — identity wiped too).
 *      (d) SecureTokenStore.listAllKeys() returns empty list (DD-4-1-6
 *          multi-key wipe deleted the connect record under this identity).
 *      (e) `is_connected()` == false (publicKey wiped).
 *      (f) Terminal-signal invariant — no error/timeout alongside.
 *
 * Route B harness per Story 2-1 T9 D-T9-1 — in-process `FakeMwaClient`
 * injected via `@VisibleForTesting` ctor, no Fake Wallet APK install
 * (CR-45 inheritance).
 */
@RunWith(AndroidJUnit4::class)
class AC1DeauthorizeHappyTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File

    private val identityUri = "https://example.com/godot-mwa-deauth-ac1"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (4-1 AC-1)")
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
    fun ac1_deauthorizeHappyPathEmitsBothFlagsTrueAndWipesAllRecords() {
        // --- Stage 1: connect ------------------------------------------------
        plugin.mwaAuthorize(
            requestId = "req-ac1-deauth-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) {
                    nativeBridge.postConnectCompleted("req-ac1-deauth-connect", any())
                }
            }.isSuccess
        }

        // Sanity: pre-deauth state populated.
        val storeBefore = SecureTokenStore(ctx)
        val keysBefore = storeBefore.listAllKeys()
        assertEquals("Exactly 1 record cached post-connect", 1, keysBefore.size)
        assertTrue(
            "publicKey populated post-connect",
            GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58().isNotEmpty(),
        )
        assertEquals(
            "identityUri populated post-connect",
            identityUri,
            GDExtensionAndroidPlugin.sessionState.getIdentityUri(),
        )

        // --- Stage 2: deauthorize -------------------------------------------
        // FakeMwaClient.deauthorize_success.json returns MwaResult.Success(Unit);
        // the impl's else-branch try-block sets remoteSucceeded=true; finally
        // wipes via DD-4-1-6 listAllKeys filter; post-finally branch emits
        // deauthorize_completed with both flags true.
        plugin.mwaDeauthorize("deauthorize-ac1")

        val jsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) {
                    nativeBridge.postDeauthorizeCompleted("deauthorize-ac1", capture(jsonSlot))
                }
            }.isSuccess
        }

        // (a) deauthorize_completed payload — 4-key shape per arch.md:669 + DD-4-1-1.
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("deauthorize-ac1", payload.getString("request_id"))
        assertTrue(
            "remote_revoke_succeeded == true on happy path",
            payload.getBoolean("remote_revoke_succeeded"),
        )
        assertTrue(
            "local_cache_cleared == true (AC-NFR-SEC-5)",
            payload.getBoolean("local_cache_cleared"),
        )
        assertEquals(
            "warning empty on happy path",
            "",
            payload.getString("warning"),
        )

        // (b) MwaSessionState FULL clear per DD-4-1-2 (NOT clearOnLogout —
        //     identity is wiped too because the user's intent is "this device
        //     is no longer trusted with this wallet").
        assertNull("authToken wiped", GDExtensionAndroidPlugin.sessionState.getAuthToken())
        assertEquals("publicKeyBase58 wiped", "", GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58())
        assertEquals(
            "identityUri wiped (full clear, NOT clearOnLogout)",
            "",
            GDExtensionAndroidPlugin.sessionState.getIdentityUri(),
        )
        assertEquals(
            "identityName wiped",
            "",
            GDExtensionAndroidPlugin.sessionState.getIdentityName(),
        )
        assertEquals("clusterName wiped", "", GDExtensionAndroidPlugin.sessionState.getClusterName())
        assertEquals("walletLabel wiped", "", GDExtensionAndroidPlugin.sessionState.getWalletLabel())
        assertEquals(
            "authTokenFingerprint wiped",
            "",
            GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint(),
        )

        // (c) SecureTokenStore — DD-4-1-6 multi-key wipe deleted the matching
        //     CacheRecord. Real EncryptedSharedPreferences round-trip: if
        //     deauthorize hadn't actually persisted the deletion, this fails.
        val keysAfter = SecureTokenStore(ctx).listAllKeys()
        assertEquals(
            "All matching CacheRecords wiped on real EncryptedSharedPreferences",
            0,
            keysAfter.size,
        )

        // (d) is_connected() proxy — connectedKey wiped (DD-4-1-2 full clear).
        assertNull(
            "connectedKey wiped — is_connected() == false post-deauth",
            GDExtensionAndroidPlugin.sessionState.getConnectedKey(),
        )

        // (e) Terminal-signal invariant — no error/timeout alongside.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postDisconnectCompleted(any(), any()) }

        // (f) DD-4-1-1 sanity: the deauthorize path does NOT route to
        //     mwa_error on remote success either.
        assertFalse(
            "deauthorize_completed must have fired (not mwa_error) — DD-4-1-1",
            payload.has("code"),
        )
    }
}
