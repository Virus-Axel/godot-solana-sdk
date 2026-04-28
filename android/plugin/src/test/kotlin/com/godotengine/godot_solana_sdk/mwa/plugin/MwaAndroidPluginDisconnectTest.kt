package com.godotengine.godot_solana_sdk.mwa.plugin

import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
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
import org.junit.jupiter.api.Assertions.assertArrayEquals
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Assertions.assertNull
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import plugin.walletadapterandroid.GDExtensionAndroidPlugin

/**
 * Story 2-3 T1+T2 — `GDExtensionAndroidPlugin.mwaDisconnect` coverage.
 *
 * 4 scenarios:
 *   1. AC-1 happy path — wipe auth-only MwaSessionState + retain SecureTokenStore
 *   2. AC-2 preconditions — post-disconnect state is what Story 2-2 silent
 *      reauth will consume (identity + cluster Int preserved; CacheKey
 *      re-derivable from caller args; store still holds the CacheRecord)
 *   3. AC-3 double-disconnect — two distinct requestIds → two emissions
 *   4. Defensive — duplicate-requestId → PROTOCOL_ERROR (not disconnect_completed)
 *
 * Plugin is built via the `@VisibleForTesting` ctor with injected collaborators,
 * identical to [AuthorizePathTest]. `mwaClientFactory` is set to throw because
 * DD-2-3-1 says the disconnect path does NOT touch the client — if the impl
 * accidentally invokes the factory, the test fails loudly.
 *
 * `SecureTokenStore` is a thin map-backed fake: `putToken` stores into a local
 * `MutableMap<CacheKey, CacheRecord>`; `getToken` reads from it. This lets us
 * assert "CacheRecord retained post-disconnect" mechanically without touching
 * Android Keystore / EncryptedSharedPreferences at unit-test time. Real
 * EncryptedSharedPreferences retention is covered on-device in T5 androidTest.
 */
class MwaAndroidPluginDisconnectTest {

    private lateinit var nativeBridge: NativeBridge
    private lateinit var senderMock: ActivityResultSender
    private lateinit var storedRecords: MutableMap<CacheKey, CacheRecord>
    private lateinit var store: SecureTokenStore

    // Pre-connect identity / cluster / chainId / walletPackage mirror Story 2-1
    // T9 androidTest fixtures. Values are arbitrary but STABLE across tests —
    // the CacheKey re-derivation assertion (AC-2) relies on feeding the same
    // 4-tuple into CacheKey(...) twice to verify same toHash().
    private val preIdentityUri = "https://game.example/godot-mwa"
    private val preIconUri = "https://game.example/icon.png"
    private val preIdentityName = "Test Game"
    private val preClusterInt = 2
    private val preClusterString = "devnet"
    private val preChainId = "solana:devnet"
    private val preWalletPackage = "com.wallet.phantom"
    private val preWalletLabel = "Phantom"
    private val preWalletIconUri = "https://phantom.app/icon.png"
    private val prePublicKey = "AbcDefGhiJkl1234567890AbcDefGhiJkl1234567890"
    private val preFingerprint = "deadbeef"
    private val preAuthTokenBytes = "tok-secret-12345".toByteArray(Charsets.UTF_8)

    @BeforeEach
    fun setup() {
        // Mirrors AuthorizePathTest setup — companion init touches android.util.Log,
        // which we mock before first touch.
        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0

        nativeBridge = mockk(relaxed = true)
        senderMock = mockk(relaxed = true)

        // Map-backed fake store — small enough inline that a dedicated FakeSecureTokenStore
        // class would be over-engineering for 4 tests. The key invariant being tested is that
        // `mwaDisconnect` does NOT call any mutating method on the store; the map's state
        // before == after.
        storedRecords = mutableMapOf()
        store = mockk(relaxed = true) {
            every { putToken(any(), any()) } answers {
                val key: CacheKey = firstArg()
                val record: CacheRecord = secondArg()
                storedRecords[key] = record
            }
            every { getToken(any()) } answers {
                val key: CacheKey = firstArg()
                storedRecords[key]
            }
            every { getOrCreatePerInstallSalt() } returns ByteArray(32) { 0x01 }
        }

        // Every test starts with a clean session.
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @AfterEach
    fun teardown() {
        unmockkStatic(Log::class)
    }

    /**
     * Build a plugin with a client-factory that throws — DD-2-3-1 says the
     * disconnect path must NEVER invoke the MwaClient. If this lambda runs,
     * the test fails with a clear message.
     */
    private fun buildPlugin(inflightOverride: InflightMap = InflightMap()): GDExtensionAndroidPlugin {
        val godot = mockk<Godot>(relaxed = true)
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.Default)
        return GDExtensionAndroidPlugin(
            godot = godot,
            mwaClientFactory = {
                error(
                    "DD-2-3-1 violation: disconnect path invoked mwaClientFactory. " +
                        "Disconnect is a pure client-side wipe and MUST NOT touch the MwaClient.",
                )
            },
            storeProvider = { store },
            nativeBridge = nativeBridge,
            senderProvider = {
                error(
                    "DD-2-3-1 violation: disconnect path invoked senderProvider. " +
                        "Disconnect does not launch a wallet Activity.",
                )
            },
            scope = scope,
            clock = { 1_700_000_000_000L },
            inflightMap = inflightOverride,
            diagnostics = MwaDiagnostics(),
            // Tests run without an Android main looper — use Unconfined so
            // `withContext(mainDispatcher)` runs the body in the caller's thread.
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

    /**
     * Pre-seeds `MwaSessionState` with a post-connect snapshot so tests can
     * assert what changes (auth-only fields) vs what stays (identity +
     * cluster Int). Uses the plugin's singleton `sessionState` object because
     * that's the same instance `mwaDisconnect` mutates via `clearOnLogout()`.
     */
    private fun seedPostConnectSession() {
        GDExtensionAndroidPlugin.sessionState.apply {
            setIdentity(preIdentityUri, preIconUri, preIdentityName)
            setCluster(preClusterInt)
            setClusterName(preClusterString)
            setAuthToken(SecretString(preAuthTokenBytes.copyOf()))
            setAuthTokenFingerprint(preFingerprint)
            setWalletIconUri(preWalletIconUri)
            setPublicKeyBase58(prePublicKey)
            setWalletLabel(preWalletLabel)
            setKey(ByteArray(32) { 0x42 })
        }
    }

    /** Pre-seed the fake store with a CacheRecord keyed by the same 4-tuple
     * Story 2-2 will use for silent-reauth lookup. Returns the seeded record
     * so tests can byte-compare post-disconnect retrieval. */
    private fun seedCacheRecord(): Pair<CacheKey, CacheRecord> {
        val key = CacheKey(preClusterString, preChainId, preWalletPackage, preIdentityUri)
        val record = CacheRecord(
            schema = 1,
            authToken = SecretString(preAuthTokenBytes.copyOf()),
            walletPackage = preWalletPackage,
            walletLabel = preWalletLabel,
            walletIconUri = preWalletIconUri,
            publicKey = prePublicKey,
            cluster = preClusterString,
            chainId = preChainId,
            identityUri = preIdentityUri,
            issuedAtMs = 1_700_000_000_000L,
            lastUsedAtMs = 1_700_000_000_000L,
        )
        store.putToken(key, record)
        return key to record
    }

    @Test
    fun `AC-1 disconnect wipes session state but retains SecureTokenStore entry`() {
        val plugin = buildPlugin()
        seedPostConnectSession()
        val (cacheKey, storedRecord) = seedCacheRecord()
        val jsonSlot = slot<String>()

        plugin.mwaDisconnect("req-ac1")

        awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postDisconnectCompleted("req-ac1", capture(jsonSlot)) }
            }.isSuccess
        }

        // (a) Result payload shape — 2-key minimal per A-12 + DD-2-3-4.
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-ac1", payload.getString("request_id"))
        assertEquals("disconnect", payload.getString("source_method"))

        // (b) MwaSessionState auth-only wipe — DD-2-3-1 + clearOnLogout contract.
        assertNull(GDExtensionAndroidPlugin.sessionState.getAuthToken(), "authToken")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(), "publicKeyBase58")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getClusterName(), "clusterName (String)")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getWalletLabel(), "walletLabel")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint(), "authTokenFingerprint")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getWalletIconUri(), "walletIconUri")
        assertNull(GDExtensionAndroidPlugin.sessionState.getConnectedKey(), "connectedKey")

        // (c) Retention invariant — SecureTokenStore entry under the SAME CacheKey
        // is still present and byte-equal to the pre-seeded source-of-truth
        // (highest-leverage AC-1 assertion per concern C4 / I-T1-1).
        //
        // WHY compare against `preAuthTokenBytes` (constant) and not
        // `storedRecord.authToken.reveal()` (object field): the fake store
        // returns the SAME CacheRecord instance that was putToken'd — comparing
        // `retrieved.authToken.reveal()` to `storedRecord.authToken.reveal()`
        // is `x == x`, true by identity regardless of whether disconnect
        // mutated anything. Comparing against the pre-seeded constant catches
        // hypothetical in-place mutation (e.g. a zeroing bug).
        val retrieved = store.getToken(cacheKey)
        assertNotNull(retrieved, "CacheRecord retained post-disconnect")
        assertArrayEquals(
            preAuthTokenBytes,
            retrieved!!.authToken.reveal(),
            "auth_token bytes match pre-seeded source-of-truth post-disconnect",
        )
        assertEquals(preWalletPackage, retrieved.walletPackage, "walletPackage preserved")
        assertEquals(prePublicKey, retrieved.publicKey, "publicKey preserved")
        assertEquals(preClusterString, retrieved.cluster, "cluster preserved")
        assertEquals(preChainId, retrieved.chainId, "chainId preserved")
        assertEquals(preIdentityUri, retrieved.identityUri, "identityUri preserved")
        // `storedRecord` is kept in scope only to anchor the fact that
        // putToken was called; it must NOT appear on the right-hand side of
        // any assertion (see note above).
        assertNotNull(storedRecord, "seedCacheRecord() ran")

        // (d) DD-2-3-1 + §8.3 retention invariant — direct runtime evidence
        // (complements the grep-static check in must_haves.truths): disconnect
        // MUST NOT call any mutating store method. I-T1-2 resolution.
        verify(exactly = 0) { store.deleteToken(any()) }
        verify(exactly = 0) { store.deleteAll() }

        // (e) Terminal-signal invariant — no other terminal signal alongside.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }
    }

    @Test
    fun `AC-2 post-disconnect state satisfies silent-reauth preconditions`() {
        val plugin = buildPlugin()
        seedPostConnectSession()
        val (originalKey, storedRecord) = seedCacheRecord()

        plugin.mwaDisconnect("req-ac2")

        awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postDisconnectCompleted("req-ac2", any()) }
            }.isSuccess
        }

        // (i) Auth-only wipe (same as AC-1's (b) for independence).
        assertNull(GDExtensionAndroidPlugin.sessionState.getAuthToken())
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58())
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getClusterName())
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getWalletLabel())
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint())

        // (ii) Identity preservation — Story 2-2 reauthorize will NOT have to re-ask
        // the caller for (identityUri, iconUri, identityName, cluster Int); they
        // are in MwaSessionState.
        assertEquals(preIdentityUri, GDExtensionAndroidPlugin.sessionState.getIdentityUri())
        assertEquals(preIconUri, GDExtensionAndroidPlugin.sessionState.getIconUri())
        assertEquals(preIdentityName, GDExtensionAndroidPlugin.sessionState.getIdentityName())
        assertEquals(preClusterInt, GDExtensionAndroidPlugin.sessionState.getCluster())

        // (iii) CacheKey re-derivability (DD-2-3-5 + concern C2). The key Story 2-2's
        // silent reauth will construct from FRESH caller arguments must produce the
        // same toHash() as the key under which the record was stored — and the store
        // lookup must return the retained record.
        val rederivedKey = CacheKey(preClusterString, preChainId, preWalletPackage, preIdentityUri)
        assertEquals(
            originalKey.toHash(),
            rederivedKey.toHash(),
            "CacheKey.toHash() is stable across call sites given the same 4-tuple",
        )
        val retrievedViaRederivedKey = store.getToken(rederivedKey)
        assertNotNull(retrievedViaRederivedKey, "CacheRecord retrievable via re-derived CacheKey")
        // I-T1-1: compare against pre-seeded constant, not storedRecord (same object).
        assertArrayEquals(
            preAuthTokenBytes,
            retrievedViaRederivedKey!!.authToken.reveal(),
            "auth_token bytes match pre-seeded constant when looked up via re-derived key",
        )
        assertEquals(preClusterString, retrievedViaRederivedKey.cluster)
        assertNotNull(storedRecord, "seedCacheRecord() ran")

        // I-T1-2: DD-2-3-1 + §8.3 runtime evidence — disconnect path MUST NOT
        // invoke any mutating store method. This complements the grep-static
        // check in must_haves.truths.
        verify(exactly = 0) { store.deleteToken(any()) }
        verify(exactly = 0) { store.deleteAll() }

        // STORY-2-2-HANDOFF: full reauth round-trip asserted by Story 2-2 AC-1;
        // this test asserts preconditions only. CacheKey is re-derivable from
        // caller-provided args (see DD-2-3-5), not from MwaSessionState.
        //
        // NOTE (I-T1-4): on-disk retention is covered by T5 androidTest
        // `AC1DisconnectRetainsTokenTest` against the real
        // EncryptedSharedPreferences-backed store. This unit test verifies
        // mock-level contract shape only.
    }

    @Test
    fun `AC-3 double disconnect emits disconnect_completed twice without error`() {
        val plugin = buildPlugin()
        seedPostConnectSession()
        seedCacheRecord()

        plugin.mwaDisconnect("req-ac3-first")
        awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postDisconnectCompleted("req-ac3-first", any()) }
            }.isSuccess
        }

        // Second call with a DIFFERENT requestId. Per DD-2-3-2, each call mints its
        // own request_id and produces its own emission — idempotence is at the STATE
        // level (wipe is a no-op on already-clear state), not at the SIGNAL level.
        plugin.mwaDisconnect("req-ac3-second")
        awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postDisconnectCompleted("req-ac3-second", any()) }
            }.isSuccess
        }

        // Both emissions landed — verify total count == 2.
        verify(exactly = 2) { nativeBridge.postDisconnectCompleted(any(), any()) }

        // No other terminal signal alongside either call.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }

        // State remains clean after the second call (wipe on already-clear is a no-op).
        assertNull(GDExtensionAndroidPlugin.sessionState.getAuthToken())
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58())
    }

    @Test
    fun `duplicate requestId emits PROTOCOL_ERROR and does not wipe state`() {
        val fakeInflight = mockk<InflightMap>(relaxed = true)
        // Simulate a duplicate: register returns false on the first call.
        every { fakeInflight.register(any(), any(), any()) } returns false

        val plugin = buildPlugin(inflightOverride = fakeInflight)
        seedPostConnectSession()

        val errorSlot = slot<String>()
        plugin.mwaDisconnect("req-dup")

        awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(errorSlot)) }
            }.isSuccess
        }

        val err = JSONObject(errorSlot.captured)
        assertEquals("req-dup", err.getString("request_id"))
        assertEquals("PROTOCOL_ERROR", err.getString("code"))
        assertEquals("disconnect", err.getString("source_method"))
        assertEquals("kotlin", err.getString("layer"))
        assertEquals("duplicate_request_id", err.getString("cause"))
        // A-14 10-key envelope intact.
        assertTrue(err.has("message"))
        assertTrue(err.has("user_message"))
        assertTrue(err.has("developer_details"))
        assertTrue(err.has("recoverable"))
        assertTrue(err.has("retry_hint"))

        // State must NOT be wiped — duplicate requestId is a caller contract error;
        // the ordering discipline DD-2-3-2 guarantees no state mutation precedes a
        // rejected register.
        assertNotNull(GDExtensionAndroidPlugin.sessionState.getAuthToken(), "authToken preserved on reject")
        assertEquals(prePublicKey, GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58())

        // No disconnect_completed emission — terminal-signal invariant honored.
        verify(exactly = 0) { nativeBridge.postDisconnectCompleted(any(), any()) }
    }
}
