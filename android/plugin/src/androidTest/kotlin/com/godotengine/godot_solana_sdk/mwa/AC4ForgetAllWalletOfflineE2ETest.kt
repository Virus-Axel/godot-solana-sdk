package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.plugin.InflightMap
import com.godotengine.godot_solana_sdk.mwa.plugin.MwaDiagnostics
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import io.mockk.every
import io.mockk.mockk
import io.mockk.verify
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import org.godotengine.godot.Godot
import org.json.JSONObject
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File
import java.security.KeyStore

/**
 * Story 4-2 T5 — AC-4 wallet-offline E2E on-device: `MWA.forget_all()`
 * completes the local wipe + MasterKey rotation within the 10-second
 * AC-1 budget EVEN WHEN one of the cached wallets' per-wallet remote
 * deauthorize attempt fails (DD-4-2-5 continue-on-error).
 *
 * Models the threat scenario: a user invokes `forget_all` to wipe every
 * wallet binding on their device, but one of the wallets is offline /
 * uninstalled / network-unreachable at the moment of the call. The
 * security guarantee is that the LOCAL wipe completes regardless — the
 * device must not retain working auth_tokens just because a remote
 * revoke could not reach the wallet.
 *
 * Validates (on emulator, with real coroutines + real
 * `EncryptedSharedPreferences` + real Android Keystore + Route B in-process
 * `FakeMwaClient` per Story 2-1 T9 D-T9-1):
 *
 *   1. Full connect via `FakeMwaClient("success")` populates `sessionState`
 *      and lands ONE `CacheRecord`.
 *   2. Pre-seed two ADDITIONAL `CacheRecord`s under different
 *      walletPackages so the wipe loop has 3 records to iterate.
 *   3. Rebuild plugin with a counter-based `mwaClientFactory` that returns
 *      `FakeMwaClient("network_offline")` for the SECOND deauth call (no
 *      such fixture exists → `FakeMwaClient.runOp` throws
 *      `IllegalStateException("no fixture for scenario 'network_offline'")`)
 *      and `FakeMwaClient("success")` for the others. Mirror the
 *      MwaAndroidPluginForgetAllTest AC-4 unit test factory shape.
 *   4. Capture wallclock at `t0` and call `plugin.mwaForgetAll(
 *      "e2e-fa-ac4")`. The mwaForgetAll body's
 *      `try { withTimeoutOrNull(...) { client.deauthorize(...) } }
 *      catch (ex: Throwable) { SdkLog.w(...) }` continues the loop past
 *      the offline wallet (DD-4-2-5).
 *   5. Assert within 10s:
 *      - `store.listAllKeys().isEmpty()` — all 3 records wiped despite the
 *        offline wallet's deauth failing (AC-4 invariant: local wipe is
 *        independent of remote outcome).
 *      - `KeyStore.containsAlias(MASTER_KEY_ALIAS) == false` — DD-4-2-4
 *        rotation completes despite the per-wallet deauth-loop exception.
 *      - `sessionState.getAuthToken() == null` — full clear independent of
 *        remote outcome.
 *      - Total wallclock < 10_000ms (AC-4 budget mirrors AC-1: per-wallet
 *        timeout is 2s × 3 wallets = 6s worst-case; comfortably under 10s).
 *      - `factoryCallCount >= 4` — at least 1 connect + 3 deauth (the loop
 *        continued past the offline wallet's exception per DD-4-2-5; loop
 *        skip on exception would yield only 2 deauth calls).
 *      - ZERO terminal-error signals fire (`postMwaError`, `postMwaTimeout`,
 *        `postReauthRequired`) — DD-4-2-6 LOCKS the wrapper bypass; offline
 *        wallets surface as SdkLog.w warnings ONLY, never as terminal-error
 *        signals to the caller.
 *
 * **SdkLog.w assertion deferred (CR-class manual-only):** AC-4's spec
 * requires a `SdkLog.w` for the offline wallet. The androidTest tier
 * cannot easily assert logcat content (would require parsing
 * `Runtime.exec("logcat")` output — flaky and tier-inappropriate). The
 * unit test in MwaAndroidPluginForgetAllTest covers the
 * `mockkStatic(Log::class)` capture path. Here we assert via the FALL-
 * BACK invariant per the story T5 spec line 273: "fall back to: total
 * time still < 10s and final state is clean".
 */
@RunWith(AndroidJUnit4::class)
class AC4ForgetAllWalletOfflineE2ETest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var connectPlugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File
    private lateinit var sharedStore: SecureTokenStore

    private val identityUri = "https://example.com/godot-mwa-forgetall-ac4"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (4-2 AC-4)")
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
        // Stage 1 plugin uses the standard PluginTestHarness with
        // scenario="success" so the connect's authorize call succeeds.
        // Stage 3 rebuilds with a counter-based factory.
        connectPlugin = PluginTestHarness.freshPlugin(
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
    fun ac4_forgetAllCompletesLocalWipeDespiteOneOfflineWalletWithin10s() {
        // --- Stage 1: connect ---------------------------------------------------
        connectPlugin.mwaAuthorize(
            requestId = "req-fa-ac4-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-fa-ac4-connect", any()) }
            }.isSuccess
        }

        assertEquals("Stage 1 precondition: 1 record post-connect", 1, sharedStore.listAllKeys().size)
        assertNotNull(
            "Stage 1 precondition: sessionState.getAuthToken() set post-connect",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )

        // --- Stage 2: pre-seed 2 additional records on the real ESP -------------
        for ((pkg, label) in listOf("app.phantom" to "Phantom", "app.solflare" to "Solflare")) {
            val key = CacheKey(
                cluster = "devnet",
                chainId = "solana:devnet",
                walletPackage = pkg,
                identityUri = identityUri,
            )
            val record = CacheRecord(
                schema = CacheRecord.SCHEMA_V1,
                authToken = SecretString("seeded-auth-$pkg".toByteArray(Charsets.UTF_8)),
                walletPackage = pkg,
                walletLabel = label,
                walletIconUri = "https://example.com/$pkg-icon.png",
                publicKey = "AbcDefGhiJkl1234567890AbcDefGhiJkl1234567890",
                cluster = "devnet",
                chainId = "solana:devnet",
                identityUri = identityUri,
                issuedAtMs = 1_700_000_000_000L,
                lastUsedAtMs = 1_700_000_000_000L,
            )
            sharedStore.putToken(key, record)
        }
        assertEquals(
            "Stage 2 precondition: 3 cached records (1 from connect + 2 seeded)",
            3,
            sharedStore.listAllKeys().size,
        )

        // --- Stage 3: rebuild plugin with counter-based factory -----------------
        // The 2nd deauth call returns FakeMwaClient("network_offline") — no
        // such fixture exists, so FakeMwaClient.loadFixture throws
        // IllegalStateException at runtime. mwaForgetAll's catch (Throwable)
        // logs the failure and continues the loop (DD-4-2-5).
        var factoryCallCount = 0
        val offlinePlugin = buildOfflinePlugin(
            sharedStore = sharedStore,
            nativeBridge = nativeBridge,
            clientFactory = {
                factoryCallCount++
                when (factoryCallCount) {
                    2 -> FakeMwaClient(fixturesDir).withScenario("network_offline")
                    else -> FakeMwaClient(fixturesDir).withScenario("success")
                }
            },
        )

        // --- Stage 4: call mwaForgetAll with wallclock instrumentation ----------
        val t0 = System.currentTimeMillis()
        offlinePlugin.mwaForgetAll("e2e-fa-ac4")

        // --- Stage 5: assert within 10s -----------------------------------------
        PluginTestHarness.awaitCondition(10_000L) {
            sharedStore.listAllKeys().isEmpty() &&
                GDExtensionAndroidPlugin.sessionState.getAuthToken() == null
        }
        val elapsedMs = System.currentTimeMillis() - t0

        // AC-4 invariant: local wipe completes regardless of one offline wallet.
        assertTrue(
            "AC-4: store.listAllKeys() must be empty post-forget_all despite one offline wallet " +
                "(got ${sharedStore.listAllKeys().size} record(s) remaining)",
            sharedStore.listAllKeys().isEmpty(),
        )
        assertNull(
            "AC-4: sessionState.getAuthToken() must be null post-clear (independent of remote outcome)",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )

        // DD-4-2-4 / DD-4-2-9 — MasterKey alias deletion must complete despite
        // the per-wallet deauth-loop exception. The runCatching wrappers in
        // mwaForgetAll guarantee step independence: a deauth-loop exception
        // does NOT abort the keystore-side rotation.
        val keystorePost = KeyStore.getInstance("AndroidKeyStore").apply { load(null) }
        assertFalse(
            "AC-4: MasterKey alias deleted despite offline wallet (DD-4-2-4 keystore-side rotation completes)",
            keystorePost.containsAlias(SecureTokenStore.MASTER_KEY_ALIAS),
        )

        // AC-4 budget: < 10_000ms. Per-wallet timeout is 2s × 3 wallets =
        // 6s worst-case (the offline wallet throws immediately on missing
        // fixture, NOT on timeout — but even if it timed out we'd be under
        // budget).
        assertTrue(
            "AC-4: forget_all + assertions must complete within 10s budget despite offline wallet " +
                "(got ${elapsedMs}ms)",
            elapsedMs < 10_000L,
        )

        // DD-4-2-5: per-wallet deauth loop continued past the offline
        // wallet's exception. factoryCallCount captures all 3 deauth-loop
        // mwaClientFactory invocations (the connect ran on connectPlugin
        // with a separate factory, so this counter starts fresh at 0 for
        // offlinePlugin). Loop-skip-on-exception would yield only 2.
        assertTrue(
            "DD-4-2-5: per-wallet deauth loop must continue on offline-wallet exception " +
                "(got $factoryCallCount factory calls; expected ≥3 for all 3 records)",
            factoryCallCount >= 3,
        )

        // DD-4-2-6 / DD-4-2-8: forget_all bypasses the
        // withStorageOrReauthRequired wrapper AND has no completion signal.
        // Offline wallets surface as SdkLog.w warnings only, never as
        // terminal-error signals to the caller.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postReauthRequired(any()) }
    }

    // ------------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------------

    /**
     * Inline plugin construction matching [PluginTestHarness.freshPlugin]
     * but accepting a custom [clientFactory] for per-call scenario
     * switching. PluginTestHarness uses a single fixed scenario; AC-4
     * needs the offline scenario for ONE deauth call only, so this test
     * inlines the construction. If a future test needs the same shape,
     * promoting this to PluginTestHarness as an overload is appropriate.
     */
    private fun buildOfflinePlugin(
        sharedStore: SecureTokenStore,
        nativeBridge: NativeBridge,
        clientFactory: () -> MwaClient,
    ): GDExtensionAndroidPlugin {
        val godot = mockk<Godot>(relaxed = true).also {
            every { it.getActivity() } returns mockk(relaxed = true)
        }
        val senderMock = mockk<ActivityResultSender>(relaxed = true)
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.Default)
        return GDExtensionAndroidPlugin(
            godot = godot,
            mwaClientFactory = clientFactory,
            storeProvider = { _ -> sharedStore },
            nativeBridge = nativeBridge,
            senderProvider = { senderMock },
            scope = scope,
            clock = { 1_700_000_000_000L },
            inflightMap = InflightMap(),
            diagnostics = MwaDiagnostics(),
            mainDispatcher = Dispatchers.Unconfined,
        )
    }
}
