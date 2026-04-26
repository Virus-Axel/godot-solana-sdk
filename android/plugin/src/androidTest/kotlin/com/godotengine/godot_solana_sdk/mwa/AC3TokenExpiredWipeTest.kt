package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import io.mockk.mockk
import io.mockk.slot
import io.mockk.verify
import org.json.JSONObject
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 2-2 T5 — AC-3 on-device: `MWA.reauthorize()` with a TOKEN_EXPIRED
 * wallet response performs a graceful wipe of the `SecureTokenStore` record
 * on REAL `EncryptedSharedPreferences` (not a mock).
 *
 * This is the highest-leverage AC-3 assertion: the unit test tier uses a
 * map-backed fake store which always succeeds by construction — so AC-3's
 * `store.deleteToken(key)` call must be exercised against the real
 * Keystore-backed `EncryptedSharedPreferences` to prove the disk deletion
 * actually works. If `SecureTokenStore.deleteToken` were broken (e.g., it
 * silently swallowed the write), the unit test would still pass but this
 * test would fail on `store.getToken(cacheKey) != null`.
 *
 * Flow:
 *   1. Full connect via `FakeMwaClient("success")` → assert `connect_completed`
 *      fires and a `CacheRecord` lands in `SecureTokenStore`; capture the
 *      `CacheKey` from `listAllKeys()` for the post-wipe assertion.
 *   2. Rebuild the plugin with `scenario = "token_expired"` (same `ctx` →
 *      same shared `SecureTokenStore`).
 *   3. Call `plugin.mwaReauthorize("rea-expired", ...)`.
 *   4. Assert within 3s:
 *      - `NativeBridge` captured ONE `postMwaError` whose JSON carries
 *        `{code: "TOKEN_EXPIRED", source_method: "reauthorize",
 *        retry_hint: "connect", request_id: "rea-expired"}` plus all
 *        other A-14 keys.
 *      - `store.getToken(cacheKey) == null` — graceful wipe on REAL
 *        EncryptedSharedPreferences (first production caller of
 *        `SecureTokenStore.deleteToken` per story DD-2-2-2).
 *      - `store.listAllKeys().size == 0` — the sole stored record is gone.
 *      - `sessionState.getAuthToken() == null`.
 *      - `sessionState.getPublicKeyBase58() == ""`.
 *      - ZERO `reauthorize_completed` captures.
 */
@RunWith(AndroidJUnit4::class)
class AC3TokenExpiredWipeTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File

    private val identityUri = "https://example.com/godot-mwa-reauth-ac3"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (2-2 AC-3)")
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
    fun ac3_tokenExpiredWipesSecureTokenStoreRecordOnRealEncryptedPrefs() {
        // --- Stage 1: connect ---------------------------------------------------
        plugin.mwaAuthorize(
            requestId = "req-ac3-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-ac3-connect", any()) }
            }.isSuccess
        }

        // Capture the CacheKey from the stored record. This is the key whose
        // on-disk record must be null after TOKEN_EXPIRED wipe.
        val store = SecureTokenStore(ctx)
        val keysAfterConnect = store.listAllKeys()
        assertEquals("Exactly 1 record cached post-connect", 1, keysAfterConnect.size)
        val cacheKey: CacheKey = keysAfterConnect.single()
        assertNotNull(
            "CacheRecord present before reauthorize",
            store.getToken(cacheKey),
        )

        // --- Stage 2: rebuild plugin with token_expired scenario -----------------
        // Rebuild using the same ctx (same SecureTokenStore) but with the
        // reauthorize_token_expired fixture. The connect-phase CacheRecord stays
        // on disk; mwaReauthorize will discover it via the 3-tuple filter
        // (cluster, chainId, identityUri) and then receive TOKEN_EXPIRED from
        // the FakeMwaClient, triggering handleTokenExpired → deleteToken.
        //
        // Same `ctx` → same SecureTokenStore on disk; the companion-scoped
        // GDExtensionAndroidPlugin.sessionState is also shared between both
        // plugin instances. After expiredPlugin.mwaReauthorize, sessionState
        // assertions read the state that handleTokenExpired wrote via
        // clearOnLogout (the singleton, not per-instance state).
        val expiredPlugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "token_expired",
            nativeBridge = nativeBridge,
        )

        // --- Stage 3: call mwaReauthorize with the expired-token scenario ---------
        expiredPlugin.mwaReauthorize(
            requestId = "rea-expired",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 5_000L,
        )

        // --- Stage 4: assert within 3s -------------------------------------------
        val errorSlot = slot<String>()
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(errorSlot)) }
            }.isSuccess
        }
        val errorPayload = JSONObject(errorSlot.captured)

        // A-14 error shape: TOKEN_EXPIRED with correct routing fields.
        assertEquals("error code is TOKEN_EXPIRED", "TOKEN_EXPIRED", errorPayload.getString("code"))
        assertEquals(
            "source_method is reauthorize",
            "reauthorize",
            errorPayload.getString("source_method"),
        )
        assertEquals(
            "retry_hint is connect for TOKEN_EXPIRED",
            "connect",
            errorPayload.getString("retry_hint"),
        )
        assertEquals(
            "request_id in error payload matches the call",
            "rea-expired",
            errorPayload.getString("request_id"),
        )
        // All 10 A-14 keys must be present.
        assertTrue("A-14: 'message' key present", errorPayload.has("message"))
        assertTrue("A-14: 'user_message' key present", errorPayload.has("user_message"))
        assertTrue("A-14: 'developer_details' key present", errorPayload.has("developer_details"))
        assertTrue("A-14: 'recoverable' key present", errorPayload.has("recoverable"))
        assertTrue("A-14: 'layer' key present", errorPayload.has("layer"))
        // 'cause' key is always present in the A-14 envelope; for handleTokenExpired
        // the value is JSONObject.NULL (no underlying exception). Asserting key
        // presence guards against a regression that drops the field — downstream
        // consumers rely on it for error-routing.
        assertTrue("A-14: 'cause' key present", errorPayload.has("cause"))

        // AC-3 graceful wipe — HIGHEST-LEVERAGE assertion (device-only).
        // Real EncryptedSharedPreferences round-trip: if handleTokenExpired
        // did not call store.deleteToken(key), this assertion fails.
        // First production caller of SecureTokenStore.deleteToken (Story 2-2
        // Context section + Verified Interfaces row).
        assertNull(
            "AC-3 graceful wipe: store.getToken(cacheKey) MUST be null after TOKEN_EXPIRED — " +
                "deleteToken must remove the record from real EncryptedSharedPreferences",
            store.getToken(cacheKey),
        )

        // Store cardinality: the only stored record is the one just wiped.
        assertEquals(
            "store.listAllKeys() must be empty after TOKEN_EXPIRED wipe",
            0,
            store.listAllKeys().size,
        )

        // Session state auth-only fields wiped via clearOnLogout. NOTE: this
        // androidTest asserts the state OUTCOME only — both wipes have
        // completed by the time we read. DD-2-2-2 ORDERING (CAS → clearOnLogout
        // → deleteToken → postMwaError) is asserted by the unit-test tier
        // (MwaAndroidPluginReauthorizeTest AC-3) via mockk verifyOrder + the
        // sessionState-snapshot pattern at deleteToken-call time.
        assertNull(
            "sessionState.getAuthToken() must be null after TOKEN_EXPIRED",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )
        assertEquals(
            "sessionState.getPublicKeyBase58() must be empty after TOKEN_EXPIRED",
            "",
            GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(),
        )

        // Terminal-signal invariant: reauthorize_completed must NOT fire alongside error.
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
