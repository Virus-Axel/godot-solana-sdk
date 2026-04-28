package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import io.mockk.mockk
import io.mockk.verify
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
 * Story 4-2 T5 — AC-1 happy E2E on-device: `MWA.forget_all()` wipes every
 * cached `CacheRecord`, rotates the encryption MasterKey (deletes the
 * Android Keystore alias), and clears `MwaSessionState` within the AC-1
 * 10-second wallclock budget.
 *
 * Validates (on emulator, with real coroutines + real
 * `EncryptedSharedPreferences` + real Android Keystore + Route B in-process
 * `FakeMwaClient` per Story 2-1 T9 D-T9-1):
 *
 *   1. Full connect via `FakeMwaClient("success")` populates `sessionState`
 *      (auth_token, publicKey, walletLabel) AND lands ONE `CacheRecord`
 *      keyed by the `authorize_success.json` fixture's `wallet_package`
 *      ("com.solanamobile.fakewallet"). Connect's `MasterKey.create(...)`
 *      side-effect installs the `godot-sdk-mwa-master-key-v1` alias in the
 *      real Android Keystore — this is the key forget_all rotates.
 *   2. Pre-seed two ADDITIONAL `CacheRecord`s under different
 *      walletPackages ("app.phantom", "app.solflare") via `store.putToken`
 *      so the wipe loop has 3 records to iterate (matching the AC-1 unit
 *      test's seed shape but on the REAL EncryptedSharedPreferences).
 *   3. Capture wallclock at `t0 = System.currentTimeMillis()` and call
 *      `plugin.mwaForgetAll("e2e-fa-ac1")`. The call returns synchronously;
 *      the wipe runs on the plugin's `scope.launch + forgetAllMutex.withLock`.
 *   4. Assert within 10s (AC-1 budget — `t1 = System.currentTimeMillis();
 *      assert t1 - t0 < 10_000L`):
 *      - `store.listAllKeys().isEmpty()` — every stored CacheRecord wiped
 *        (proves `store.deleteAll` round-trips through the REAL Keystore-
 *        backed `EncryptedSharedPreferences`, not just the unit-tier fake).
 *      - `KeyStore.getInstance("AndroidKeyStore").containsAlias(
 *        SecureTokenStore.MASTER_KEY_ALIAS) == false` — DD-4-2-4 / DD-4-2-9
 *        rotation: the alias is deleted, forcing the next `storeProvider(
 *        ctx)` call to regenerate a fresh MasterKey. This is the highest-
 *        leverage AC-1 assertion — only achievable on a real Android
 *        Keystore (the unit tier mocks `KeyStore.getInstance`).
 *      - `sessionState.getAuthToken() == null` — DD-4-1-2 LOCKED
 *        inheritance: full clear (NOT clearOnLogout — that's the disconnect
 *        path's wipe that preserves identity).
 *      - `sessionState.getPublicKeyBase58() == ""` — additional sessionState
 *        wipe coverage matching AC3TokenExpiredWipeTest's pattern.
 *      - ZERO terminal-error signals fire (`postMwaError`, `postMwaTimeout`,
 *        `postReauthRequired`) — DD-4-2-8 LOCKS that forget_all has no
 *        completion signal AND happy path has no error signals either; the
 *        evidence is the post-condition state inspection above.
 *
 * **Real-Keystore deletion invariant (CR-58-class):** the `containsAlias ==
 * false` assertion is the load-bearing AC-1 evidence on this tier. The unit
 * test verifies `verify(atLeast = 1) { keystoreMock.deleteEntry(...) }` —
 * which proves the call was made but NOT that the real Android Keystore
 * actually removed the key material. This androidTest closes that gap.
 *
 * **Real-wallet invariant deferred (CR-45-class):** AC-1's stricter "the
 * wallet's per-wallet `client.deauthorize(...)` actually revoked at the
 * remote" invariant is verifiable ONLY against a real wallet. The
 * `deauthorize_success.json` fixture round-trips a placeholder Success
 * result intact through the per-wallet deauth loop — proving the FORWARD
 * plumbing (Kotlin → FakeMwaClient → handleDeauthorizeResult). The on-
 * chain revocation is the responsibility of the manual release-gate
 * procedure against Phantom/Solflare/Backpack in the pre-Gate-5 close-out
 * window (parallel to Story 2-2 / 3-1 / 3-2 / 3-3 procedures).
 */
@RunWith(AndroidJUnit4::class)
class AC1ForgetAllHappyE2ETest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File
    private lateinit var sharedStore: SecureTokenStore

    private val identityUri = "https://example.com/godot-mwa-forgetall-ac1"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (4-2 AC-1)")
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
        // Single shared real ESP-backed store; Stage 2 pre-seeds extra records
        // on the same instance so forget_all's listAllKeys returns 3 entries.
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
    fun ac1_forgetAllWipesAllRecordsRotatesMasterKeyAndClearsSessionWithin10s() {
        // --- Stage 1: connect ---------------------------------------------------
        plugin.mwaAuthorize(
            requestId = "req-fa-ac1-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-fa-ac1-connect", any()) }
            }.isSuccess
        }

        val keysAfterConnect = sharedStore.listAllKeys()
        assertEquals("Exactly 1 record cached post-connect", 1, keysAfterConnect.size)
        assertNotNull(
            "sessionState.getAuthToken() must not be null after successful connect",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )
        // DD-4-2-4: connect's MasterKey.create installs the alias in the real
        // Android Keystore — pre-condition for the forget_all rotation assertion.
        val keystorePre = KeyStore.getInstance("AndroidKeyStore").apply { load(null) }
        assertTrue(
            "MasterKey alias must exist post-connect (forget_all rotation pre-condition)",
            keystorePre.containsAlias(SecureTokenStore.MASTER_KEY_ALIAS),
        )

        // --- Stage 2: pre-seed 2 additional CacheRecords on the real ESP --------
        // Different walletPackages from the connect-stored "com.solanamobile.
        // fakewallet" so all 3 keys are unique. CacheKey shape per Story 2-1:
        // (cluster, chainId, walletPackage, identityUri).
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

        // --- Stage 3: call mwaForgetAll with wallclock instrumentation ----------
        val t0 = System.currentTimeMillis()
        plugin.mwaForgetAll("e2e-fa-ac1")

        // --- Stage 4: assert within 10s -----------------------------------------
        // AC-1 budget: total wallclock for stage 3+4 must be < 10_000ms. The
        // poll-loop awaitCondition uses 20ms tick — the actual elapsed time
        // captured below proves we landed under budget.
        PluginTestHarness.awaitCondition(10_000L) {
            sharedStore.listAllKeys().isEmpty() &&
                GDExtensionAndroidPlugin.sessionState.getAuthToken() == null
        }
        val elapsedMs = System.currentTimeMillis() - t0

        assertTrue(
            "AC-1: store.listAllKeys() must be empty post-forget_all (got ${sharedStore.listAllKeys().size})",
            sharedStore.listAllKeys().isEmpty(),
        )
        assertNull(
            "AC-1: sessionState.getAuthToken() must be null post-clear (DD-4-1-2 full clear)",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )
        assertEquals(
            "AC-1: sessionState.getPublicKeyBase58() must be empty post-clear",
            "",
            GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(),
        )

        // DD-4-2-4 / DD-4-2-9 — MasterKey alias must be DELETED from the real
        // Android Keystore. This is the load-bearing E2E assertion that the
        // unit tier cannot make (the unit test mocks KeyStore.getInstance).
        val keystorePost = KeyStore.getInstance("AndroidKeyStore").apply { load(null) }
        assertFalse(
            "AC-1: MasterKey alias must be deleted from real Android Keystore post-forget_all (DD-4-2-4)",
            keystorePost.containsAlias(SecureTokenStore.MASTER_KEY_ALIAS),
        )

        // AC-1 budget: total stage 3+4 wallclock < 10_000ms.
        assertTrue(
            "AC-1: forget_all + assertions must complete within 10s budget (got ${elapsedMs}ms)",
            elapsedMs < 10_000L,
        )

        // DD-4-2-8: forget_all has no completion signal. Happy path has zero
        // terminal-error signals either — the evidence is the post-condition
        // state inspection above. Negative-key guards catch a regression
        // where forget_all accidentally fires mwa_error / mwa_timeout /
        // reauth_required on the success path.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postReauthRequired(any()) }
    }
}
