package com.godotengine.godot_solana_sdk.mwa.plugin

import android.net.Uri
import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.store.StorageCorruptException
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import io.mockk.every
import io.mockk.mockk
import io.mockk.mockkStatic
import io.mockk.slot
import io.mockk.unmockkStatic
import io.mockk.verify
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import org.godotengine.godot.Godot
import org.json.JSONObject
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertFalse
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Assertions.assertThrows
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File
import java.io.IOException
import java.security.GeneralSecurityException

/**
 * Story 4-3 — keystore corrupt-recovery + `reauth_required` emission
 * shape coverage at the plugin boundary.
 *
 * 7 cases mapped to AC-1 (DD-4-3-1, DD-4-3-1.a, DD-4-3-1.b) + DD-4-3-2:
 *   1. AUTHORIZE success-path corruption (line 1328 catch site) → currently
 *      emits `mwa_error{StorageCorrupt}`; T2 PIVOTs to `reauth_required{
 *      reason:"keystore_corrupt", source_method:"authorize"}`.
 *   2. REAUTHORIZE success-path corruption (line 720 catch site) → currently
 *      emits `mwa_error{StorageCorrupt}`; T2 PIVOTs to `reauth_required{
 *      reason:"keystore_corrupt", source_method:"reauthorize"}`.
 *   3. AUTHORIZE cache-LOOKUP path corruption (NEW catch site per DD-4-3-1.a)
 *      → today this propagates as uncaught Throwable → `mwa_error{
 *      PROTOCOL_ERROR}`; T2 wraps the first SecureTokenStore touchpoint in
 *      `withStorageOrReauthRequired(...)` so corruption surfaces as
 *      `reauth_required{reason:"keystore_corrupt"}` AND the wallet round-trip
 *      is NEVER attempted (fail-closed).
 *   4. REAUTHORIZE cache-LOOKUP path corruption (matches case 3 on the
 *      reauthorize branch) — symmetric across the 2×2 cross-product
 *      {authorize, reauthorize} × {GeneralSecurityException, IOException}
 *      with cases 1, 2 (per universal retrospect 2026-04-27 — symmetric
 *      defensive branches need symmetric tests).
 *   5. CAS-won terminal-slot invariant: when corrupt-recovery fires, ONLY
 *      `postReauthRequired` is called — no `postMwaError`, no
 *      `postConnectCompleted` / `postReauthorizeCompleted`. Per DD-4-1-3
 *      retrospective "flag-based post-finally branch" applied to corruption.
 *   6. Wrapper helper `withStorageOrReauthRequired` does NOT interpose on
 *      non-`StorageCorruptException` exceptions — they propagate unchanged
 *      and `postReauthRequired` is never called.
 *   7. `reauth_required` payload Dictionary shape — JSON parse + 5-key
 *      assertion per DD-4-3-1.b (`reason`, `request_id`, `source_method`,
 *      `developer_details`, `cause`).
 *
 * Plugin construction mirrors `MwaAndroidPluginReauthorizeTest` /
 * `AuthorizePathTest`: `@VisibleForTesting` ctor with injected mocks
 * (recording `NativeBridge`, throw-on-touch `SecureTokenStore`, etc.). The
 * `EncryptedSharedPreferences.create` static-mock pattern from
 * `SecureTokenStoreTest` is NOT used here — `storeProvider` returns a mock
 * `SecureTokenStore` whose methods throw `StorageCorruptException` directly.
 * Same correctness coverage (DD-4-3-1's catch is on `StorageCorruptException`,
 * not on `EncryptedSharedPreferences.create` per se), without the indirection.
 */
class MwaAndroidPluginKeystoreCorruptRecoveryTest {

    private val fixturesDir: File by lazy {
        val prop = System.getProperty("mwa.fixtures.dir")
            ?: error("system property 'mwa.fixtures.dir' must be set (wired in build.gradle.kts)")
        File(prop).also { require(it.isDirectory) { "fixtures dir missing: $it" } }
    }

    private lateinit var nativeBridge: NativeBridge
    private lateinit var senderMock: ActivityResultSender

    // Stable identity values mirroring Story 2-1 / 2-2 / 4-1 fixtures.
    private val identityUri = "https://example.com/godot-mwa-sample"
    private val identityName = "Test Game"
    private val iconUri = "https://example.com/icon.png"
    private val walletPackage = "com.wallet.app"
    private val walletLabel = "TestWallet"
    private val walletIconUri = "https://wallet.example/icon.png"
    private val publicKey = "AbcDefGhiJkl1234567890AbcDefGhiJkl1234567890"
    private val authTokenBytes = "dGVzdC1hdXRoLXRva2VuLXJlYXV0aA==".toByteArray(Charsets.UTF_8)
    private val salt = ByteArray(32) { 0x01 }

    private val identityJson = JSONObject().apply {
        put("name", identityName)
        put("icon_uri", iconUri)
        put("identity_uri", identityUri)
    }.toString()

    @BeforeEach
    fun setup() {
        // Companion init touches android.util.Log; mock before first companion touch.
        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0

        // ConnectionIdentity construction goes through Uri.parse(...) — mock to a
        // relaxed mock so JVM-side construction succeeds without an Android runtime.
        mockkStatic(Uri::class)
        every { Uri.parse(any()) } returns mockk(relaxed = true)

        nativeBridge = mockk(relaxed = true)
        senderMock = mockk(relaxed = true)
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @AfterEach
    fun teardown() {
        unmockkStatic(Log::class)
        unmockkStatic(Uri::class)
    }

    private fun buildPlugin(
        clientFactory: () -> MwaClient = { error("unexpected mwaClientFactory invocation") },
        store: SecureTokenStore = mockk(relaxed = true),
        inflightOverride: InflightMap = InflightMap(),
    ): GDExtensionAndroidPlugin {
        val godot = mockk<Godot>(relaxed = true)
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.Default)
        return GDExtensionAndroidPlugin(
            godot = godot,
            mwaClientFactory = clientFactory,
            storeProvider = { store },
            nativeBridge = nativeBridge,
            senderProvider = { senderMock },
            scope = scope,
            clock = { 1_700_000_000_000L },
            inflightMap = inflightOverride,
            diagnostics = MwaDiagnostics(),
            // Tests run without an Android main looper — Unconfined runs withContext
            // in the caller's thread.
            mainDispatcher = Dispatchers.Unconfined,
        )
    }

    /** Polls [check] every 5ms up to [timeoutMs] ms; errors otherwise. */
    private fun awaitCondition(timeoutMs: Long, check: () -> Boolean) {
        val deadline = System.currentTimeMillis() + timeoutMs
        while (System.currentTimeMillis() < deadline) {
            if (check()) return
            Thread.sleep(5)
        }
        error("condition not met within ${timeoutMs}ms")
    }

    /** Builds a CacheRecord matching the canonical (cluster, chainId, walletPackage, identityUri). */
    private fun buildSeedRecord(cluster: String = "devnet", chainId: String = "solana:devnet"): Pair<CacheKey, CacheRecord> {
        val key = CacheKey(cluster, chainId, walletPackage, identityUri)
        val record = CacheRecord(
            schema = 1,
            authToken = SecretString(authTokenBytes.copyOf()),
            walletPackage = walletPackage,
            walletLabel = walletLabel,
            walletIconUri = walletIconUri,
            publicKey = publicKey,
            cluster = cluster,
            chainId = chainId,
            identityUri = identityUri,
            issuedAtMs = 1_700_000_000_000L,
            lastUsedAtMs = 1_700_000_000_000L,
        )
        return key to record
    }

    // ---------------- Case 1 — AUTHORIZE success-path corruption (line 1328 catch site) ----------------

    @Test
    fun `case 1 - authorize success path corruption fires reauth_required with source_method=authorize`() {
        // Store mock that throws StorageCorruptException on putToken — simulates corruption
        // surfacing AFTER the wallet round-trip succeeds, mid-handleSuccess.
        val corruptStore = mockk<SecureTokenStore>(relaxed = true) {
            every { putToken(any(), any()) } throws StorageCorruptException(
                GeneralSecurityException("simulated Tink AEAD failure on putToken"),
            )
            every { getOrCreatePerInstallSalt() } returns salt
        }
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            store = corruptStore,
        )
        val jsonSlot = slot<String>()

        plugin.mwaAuthorize("req-c1", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        // RED in T1: line 1328 still emits emitFailure(...MwaError.StorageCorrupt...) → postMwaError;
        // postReauthRequired is NEVER called → this verify times out → test fails.
        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postReauthRequired(capture(jsonSlot)) } }.isSuccess
        }

        // (a) Payload shape — DD-4-3-1.b 5-key Dictionary.
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("keystore_corrupt", payload.getString("reason"))
        assertEquals("req-c1", payload.getString("request_id"))
        assertEquals("authorize", payload.getString("source_method"))
        assertTrue(
            payload.getString("developer_details").contains("GeneralSecurityException"),
            "developer_details must mention the underlying exception class",
        )
        assertEquals("STORAGE_CORRUPT", payload.getString("cause"))

        // (b) Terminal-signal invariant — no dual-emit per DD-4-3-1 / InflightMap CAS.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }
    }

    // ---------------- Case 2 — REAUTHORIZE success-path corruption (line 720 catch site) ----------------

    @Test
    fun `case 2 - reauthorize success path corruption fires reauth_required with source_method=reauthorize`() {
        val (cacheKey, cachedRecord) = buildSeedRecord()
        // Store seeded with one record so the 3-tuple filter matches; putToken (called
        // mid-handleReauthSuccess) throws StorageCorruptException — corruption surfaces
        // AFTER the wallet round-trip.
        val corruptStore = mockk<SecureTokenStore>(relaxed = true) {
            every { listAllKeys() } returns listOf(cacheKey)
            every { getToken(cacheKey) } returns cachedRecord
            every { putToken(any(), any()) } throws StorageCorruptException(
                GeneralSecurityException("simulated Tink AEAD failure on reauthorize putToken"),
            )
            every { getOrCreatePerInstallSalt() } returns salt
        }
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            store = corruptStore,
        )
        val jsonSlot = slot<String>()

        plugin.mwaReauthorize("req-c2", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        // RED in T1: line 720 still emits emitFailureReauth(...MwaError.StorageCorrupt...) → postMwaError.
        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postReauthRequired(capture(jsonSlot)) } }.isSuccess
        }

        val payload = JSONObject(jsonSlot.captured)
        assertEquals("keystore_corrupt", payload.getString("reason"))
        assertEquals("req-c2", payload.getString("request_id"))
        assertEquals("reauthorize", payload.getString("source_method"))
        assertTrue(
            payload.getString("developer_details").contains("GeneralSecurityException"),
            "developer_details must mention the underlying exception class",
        )
        assertEquals("STORAGE_CORRUPT", payload.getString("cause"))

        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }
    }

    // ---------------- Case 3 — AUTHORIZE cache-LOOKUP path corruption (NEW catch per DD-4-3-1.a) ----------------

    @Test
    fun `case 3 - authorize cache-lookup corruption fires reauth_required and never invokes mwaClient`() {
        // Store mock throws on EVERY method touch — first SecureTokenStore touchpoint in
        // mwaAuthorize must surface as reauth_required, NOT as Throwable → PROTOCOL_ERROR.
        val corruptStore = mockk<SecureTokenStore>(relaxed = true) {
            every { listAllKeys() } throws StorageCorruptException(
                IOException("simulated Tink proto deserialization failure (IOException parent)"),
            )
            every { getToken(any()) } throws StorageCorruptException(
                IOException("simulated Tink proto deserialization failure (IOException parent)"),
            )
            every { getOrCreatePerInstallSalt() } throws StorageCorruptException(
                IOException("simulated Tink proto deserialization failure (IOException parent)"),
            )
            every { putToken(any(), any()) } throws StorageCorruptException(
                IOException("simulated Tink proto deserialization failure (IOException parent)"),
            )
        }
        // Recording client factory — assert it is NEVER invoked (fail-closed: don't
        // attempt the wallet round-trip if local storage is corrupt).
        var clientFactoryInvocations = 0
        val plugin = buildPlugin(
            clientFactory = {
                clientFactoryInvocations++
                FakeMwaClient(fixturesDir).withScenario("success")
            },
            store = corruptStore,
        )
        val jsonSlot = slot<String>()

        plugin.mwaAuthorize("req-c3", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postReauthRequired(capture(jsonSlot)) } }.isSuccess
        }

        val payload = JSONObject(jsonSlot.captured)
        assertEquals("keystore_corrupt", payload.getString("reason"))
        assertEquals("req-c3", payload.getString("request_id"))
        assertEquals("authorize", payload.getString("source_method"))
        assertTrue(
            payload.getString("developer_details").contains("IOException"),
            "developer_details must mention the underlying exception class",
        )

        // (b) Fail-closed: clientlib-ktx authorize MUST NEVER be attempted (DD-4-3-1.a).
        assertEquals(
            0,
            clientFactoryInvocations,
            "mwaClientFactory must NEVER be invoked when storage is corrupt at cache-lookup time",
        )
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }
    }

    // ---------------- Case 4 — REAUTHORIZE cache-LOOKUP corruption (symmetric 2×2 cross-product) ----------------

    @Test
    fun `case 4 - reauthorize cache-lookup corruption fires reauth_required and never invokes mwaClient`() {
        // Symmetric to case 3 on the reauthorize branch; uses GeneralSecurityException
        // to complete the {authorize, reauthorize} × {GSE, IOException} 2×2 cross-product
        // alongside cases 1 (auth/GSE), 2 (reauth/GSE), 3 (auth/IOException). This case
        // covers reauth/GSE — actually wait, case 2 is reauth/GSE; this is reauth/IOException.
        // Per spec must_have: cases 1-4 collectively cover the 2×2 cross-product.
        val corruptStore = mockk<SecureTokenStore>(relaxed = true) {
            every { listAllKeys() } throws StorageCorruptException(
                IOException("simulated Tink IO failure on reauthorize cache-lookup"),
            )
            every { getToken(any()) } throws StorageCorruptException(
                IOException("simulated Tink IO failure on reauthorize cache-lookup"),
            )
        }
        var clientFactoryInvocations = 0
        val plugin = buildPlugin(
            clientFactory = {
                clientFactoryInvocations++
                FakeMwaClient(fixturesDir).withScenario("success")
            },
            store = corruptStore,
        )
        val jsonSlot = slot<String>()

        plugin.mwaReauthorize("req-c4", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postReauthRequired(capture(jsonSlot)) } }.isSuccess
        }

        val payload = JSONObject(jsonSlot.captured)
        assertEquals("keystore_corrupt", payload.getString("reason"))
        assertEquals("req-c4", payload.getString("request_id"))
        assertEquals("reauthorize", payload.getString("source_method"))
        assertTrue(
            payload.getString("developer_details").contains("IOException"),
            "developer_details must mention the underlying exception class",
        )

        assertEquals(
            0,
            clientFactoryInvocations,
            "mwaClientFactory must NEVER be invoked when storage is corrupt at reauthorize cache-lookup time",
        )
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }
    }

    // ---------------- Case 5 — CAS-won terminal-slot invariant (no dual-emit) ----------------

    @Test
    fun `case 5 - corrupt-recovery emits ONLY postReauthRequired and never dual-emits postMwaError`() {
        // Drive the same path as case 1 (authorize success → putToken corruption) but
        // assert the no-dual-emit invariant explicitly. Per DD-4-1-3 retrospective:
        // when the CAS is won by reauth_required, no other terminal signal may fire.
        val corruptStore = mockk<SecureTokenStore>(relaxed = true) {
            every { putToken(any(), any()) } throws StorageCorruptException(
                GeneralSecurityException("simulated Tink AEAD failure post-CAS"),
            )
            every { getOrCreatePerInstallSalt() } returns salt
        }
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            store = corruptStore,
        )

        plugin.mwaAuthorize("req-c5", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postReauthRequired(any()) } }.isSuccess
        }

        // Hard invariant — no other terminal signals on the same requestId.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postMwaCancelledLifecycle(any()) }
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }
    }

    // ---------------- Case 6 — Wrapper does NOT interpose on non-StorageCorruptException ----------------

    @Test
    fun `case 6 - withStorageOrReauthRequired rethrows non-StorageCorruptException uncaught`() {
        // Direct contract test on the wrapper helper — NOT via a public entry point.
        // Reflection: the helper is private. Test the inverse via the public surface:
        // throw a RuntimeException from a store call site reached BEFORE the wrapper;
        // observe that postReauthRequired is NEVER called (the wrapper would have
        // converted a StorageCorruptException to reauth_required, but a generic
        // RuntimeException must propagate to the outer Throwable handler →
        // mwa_error{PROTOCOL_ERROR}).
        val nonCorruptStore = mockk<SecureTokenStore>(relaxed = true) {
            every { listAllKeys() } throws RuntimeException("not-a-storage-error: synthetic non-corrupt failure")
            every { getToken(any()) } throws RuntimeException("not-a-storage-error: synthetic non-corrupt failure")
        }
        val plugin = buildPlugin(
            clientFactory = { error("clientFactory must not be invoked when store throws on lookup") },
            store = nonCorruptStore,
        )

        plugin.mwaReauthorize("req-c6", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        // The outer Throwable handler in mwaReauthorize fires postMwaError{PROTOCOL_ERROR}
        // — assert that's the path taken, NOT reauth_required.
        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postMwaError(any()) } }.isSuccess
        }

        // (a) Hard invariant: postReauthRequired must NEVER be invoked for non-storage errors.
        verify(exactly = 0) { nativeBridge.postReauthRequired(any()) }

        // (b) Sanity: also assert NO success-path emissions (terminal-signal invariant on the failure path).
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }
    }

    // ---------------- Case 7 — reauth_required Dictionary 5-key shape (DD-4-3-1.b) ----------------

    @Test
    fun `case 7 - reauth_required payload Dictionary has exactly the 5 DD-4-3-1_b keys`() {
        // Mechanically assert the JSON parses to a JSONObject with the 5 expected keys.
        // Driven through the case-1 path (authorize success → putToken corruption) so
        // request_id and source_method are populated (non-empty per the op-context branch).
        val corruptStore = mockk<SecureTokenStore>(relaxed = true) {
            every { putToken(any(), any()) } throws StorageCorruptException(
                GeneralSecurityException("simulated Tink AEAD failure for shape test"),
            )
            every { getOrCreatePerInstallSalt() } returns salt
        }
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            store = corruptStore,
        )
        val jsonSlot = slot<String>()

        plugin.mwaAuthorize("req-c7", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postReauthRequired(capture(jsonSlot)) } }.isSuccess
        }

        val payload = JSONObject(jsonSlot.captured)

        // (a) Each of the 5 DD-4-3-1.b keys is present.
        assertTrue(payload.has("reason"), "DD-4-3-1.b key 'reason' must be present")
        assertTrue(payload.has("request_id"), "DD-4-3-1.b key 'request_id' must be present")
        assertTrue(payload.has("source_method"), "DD-4-3-1.b key 'source_method' must be present")
        assertTrue(payload.has("developer_details"), "DD-4-3-1.b key 'developer_details' must be present")
        assertTrue(payload.has("cause"), "DD-4-3-1.b key 'cause' must be present")

        // (b) Exactly 5 keys — no extras.
        assertEquals(
            5,
            payload.length(),
            "DD-4-3-1.b: payload must contain EXACTLY 5 keys; got ${payload.keys().asSequence().toList()}",
        )

        // (c) reason is the AC-1 contract discriminator — the only value defined for this story.
        assertEquals("keystore_corrupt", payload.getString("reason"))

        // (d) cause uses the codegen-sourced literal per C-4-3-G — drift-protected if
        // codegen ever changes the StorageCorrupt code string.
        assertEquals("STORAGE_CORRUPT", payload.getString("cause"))

        // (e) Op-context fields populated for this op-driven case (per DD-4-3-1.b empty-string-default note,
        // these are populated when corruption fires inside an op).
        assertEquals("req-c7", payload.getString("request_id"))
        assertEquals("authorize", payload.getString("source_method"))
        assertNotNull(payload.getString("developer_details"))
        assertFalse(
            payload.getString("developer_details").isEmpty(),
            "developer_details must be non-empty for op-context cases",
        )

        // (f) Self-check — assertThrows is unused above; reference it here for import-hygiene
        // sanity (some kotlin compilers warn on unused imports). The assertion is a no-op
        // test contract: parsing valid JSON does not throw.
        assertThrows(org.json.JSONException::class.java) {
            JSONObject(jsonSlot.captured).getString("nonexistent_key")
        }
    }
}
