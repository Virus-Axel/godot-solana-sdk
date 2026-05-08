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
import io.mockk.verifyOrder
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.async
import kotlinx.coroutines.awaitAll
import kotlinx.coroutines.runBlocking
import org.godotengine.godot.Godot
import org.json.JSONObject
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNull
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.security.KeyStore

/**
 * Story 4-2 T1 — `GDExtensionAndroidPlugin.mwaForgetAll` coverage (TDD red).
 *
 * 6 scenarios exercising the LOCKED design decisions DD-4-2-1..-9:
 *   1. AC-1 happy path — 3 cached tokens (3 different walletPackages) →
 *      mwaForgetAll → within 10s: listAllKeys.isEmpty == true; MasterKey
 *      alias was deleted via KeyStore.deleteEntry; sessionState clear;
 *      no terminal-error signal fires.
 *   2. AC-2 cancels in-flight — pre-seed inflightMap.register("inflight-sm-1",
 *      clock(), "sign_messages"); call mwaForgetAll. Assert verifyOrder:
 *      nativeBridge.postMwaCancelledLifecycle(any) BEFORE store.deleteAll
 *      (DD-4-2-3 + AC-2 ordering). Captured payload asserts {request_id:
 *      "inflight-sm-1", source_method: "sign_messages", reason:
 *      "forget_all_invoked"}.
 *   3. AC-3 mutex serializes concurrent forget_all + connect — two
 *      coroutines via runBlocking + async + awaitAll. Final state assertion:
 *      either connect completed first then forget_all wiped its work, OR
 *      forget_all completed first then connect saw clean state. NO partial
 *      state observable (DD-4-2-2 Mutex invariant).
 *   4. AC-4 wallet offline — pre-seed 3 records; FakeMwaClient(network_offline)
 *      for one wallet, FakeMwaClient(success) for the other 2. mwaForgetAll
 *      should: log SdkLog.w for the offline wallet AND complete the local
 *      wipe regardless. Loop continues — other 2 wallets' deauth attempts
 *      ARE invoked (verified via mwaClientFactory invocation count or
 *      relaxed-mock verify).
 *   5. getToken StorageCorruptException for ONE record — pre-seed 3 records
 *      but mock store.getToken to throw on the 2nd. Assert: 1st + 3rd records'
 *      deauth attempts invoked (skip-and-continue per DD-4-2-5); local wipe
 *      proceeds.
 *   6. listAllKeys StorageCorruptException bypasses wrapper — mock
 *      store.listAllKeys to throw. Assert: nativeBridge.postReauthRequired
 *      is NEVER invoked (DD-4-2-6 LOCKS bypass); the wipe still proceeds
 *      (KeyStore.deleteEntry IS invoked).
 *
 * **TDD red baseline (T1):** all 6 tests fail at runtime against the
 * `TODO("Story 4-2 T2 fills in")` body in
 * [GDExtensionAndroidPlugin.mwaForgetAll] +
 * [GDExtensionAndroidPlugin.buildCancelledLifecycleJson]. T2 turns them
 * GREEN.
 *
 * Plugin is built via the `@VisibleForTesting` ctor with injected
 * collaborators (identical to [MwaAndroidPluginSignAndSendTest] /
 * [MwaAndroidPluginDeauthorizeTest]):
 *   - recording [NativeBridge] mock
 *   - map-backed fake [SecureTokenStore] via `storeProvider` — exercises
 *     the storage layer at unit tier (real ESP testing is androidTest-tier
 *     responsibility per Story 2-1 T9 D-T9-1 inheritance)
 *   - FakeMwaClient or relaxed-mock test double via `mwaClientFactory`
 *     (per-test-case scenario parameterization)
 *   - no-op `senderProvider`
 *   - `mockkStatic(KeyStore::class)` for AC-1 MasterKey rotation
 *     verification (real Android Keystore is not available in unit-test
 *     JVM; the static mock captures the deleteEntry invocation)
 */
class MwaAndroidPluginForgetAllTest {

    private val fixturesDir: java.io.File by lazy {
        val prop = System.getProperty("mwa.fixtures.dir")
            ?: error("system property 'mwa.fixtures.dir' must be set (wired in build.gradle.kts)")
        java.io.File(prop).also { require(it.isDirectory) { "fixtures dir missing: $it" } }
    }

    private lateinit var nativeBridge: NativeBridge
    private lateinit var senderMock: ActivityResultSender
    private lateinit var storedRecords: MutableMap<CacheKey, CacheRecord>
    private lateinit var store: SecureTokenStore
    private lateinit var keystoreMock: KeyStore

    private val identityUri = "https://game.example"
    private val identityName = "Game"
    private val iconUri = "https://x/i"
    private val walletLabel1 = "Phantom"
    private val walletLabel2 = "Solflare"
    private val walletLabel3 = "Backpack"
    private val walletPackage1 = "app.phantom"
    private val walletPackage2 = "app.solflare"
    private val walletPackage3 = "app.backpack"
    private val walletIconUri = "https://wallet.example/icon.png"
    private val publicKey = "AbcDefGhiJkl1234567890AbcDefGhiJkl1234567890"

    private val authTokenBytes1 = "auth-token-phantom".toByteArray(Charsets.UTF_8)
    private val authTokenBytes2 = "auth-token-solflare".toByteArray(Charsets.UTF_8)
    private val authTokenBytes3 = "auth-token-backpack".toByteArray(Charsets.UTF_8)
    private val salt = ByteArray(32) { 0x01 }

    @BeforeEach
    fun setup() {
        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0

        mockkStatic(Uri::class)
        every { Uri.parse(any()) } returns mockk(relaxed = true)

        // DD-4-2-4 — KeyStore.getInstance("AndroidKeyStore") is the rotation
        // surface. The real Android Keystore is not available in JVM unit
        // tests; the static mock captures deleteEntry / containsAlias /
        // load invocations so AC-1 can assert the rotation path.
        mockkStatic(KeyStore::class)
        keystoreMock = mockk(relaxed = true)
        every { KeyStore.getInstance("AndroidKeyStore") } returns keystoreMock
        every { keystoreMock.containsAlias(SecureTokenStore.MASTER_KEY_ALIAS) } returns true

        nativeBridge = mockk(relaxed = true)
        senderMock = mockk(relaxed = true)

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
            every { listAllKeys() } answers { storedRecords.keys.toList() }
            every { deleteToken(any()) } answers {
                val key: CacheKey = firstArg()
                storedRecords.remove(key)
                Unit
            }
            every { deleteAll() } answers {
                storedRecords.clear()
                Unit
            }
            every { getOrCreatePerInstallSalt() } returns salt
        }

        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @AfterEach
    fun teardown() {
        unmockkStatic(Log::class)
        unmockkStatic(Uri::class)
        unmockkStatic(KeyStore::class)
    }

    private fun buildPlugin(
        clientFactory: () -> MwaClient = {
            error("unexpected mwaClientFactory invocation")
        },
        inflightOverride: InflightMap = InflightMap(),
        storeOverride: SecureTokenStore = store,
    ): GDExtensionAndroidPlugin {
        val godot = mockk<Godot>(relaxed = true)
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.Default)
        return GDExtensionAndroidPlugin(
            godot = godot,
            mwaClientFactory = clientFactory,
            storeProvider = { storeOverride },
            nativeBridge = nativeBridge,
            senderProvider = { senderMock },
            scope = scope,
            clock = { 1_700_000_000_000L },
            inflightMap = inflightOverride,
            diagnostics = MwaDiagnostics(),
            mainDispatcher = Dispatchers.Unconfined,
        )
    }

    private fun awaitCondition(timeoutMs: Long, check: () -> Boolean) {
        val deadline = System.currentTimeMillis() + timeoutMs
        while (System.currentTimeMillis() < deadline) {
            if (check()) return
            Thread.sleep(5)
        }
        error("condition not met within ${timeoutMs}ms")
    }

    private fun seedConnectedSession() {
        GDExtensionAndroidPlugin.sessionState.apply {
            setIdentity(identityUri, iconUri, identityName)
            setCluster(2)
            setClusterName("devnet")
            setAuthToken(SecretString(authTokenBytes1.copyOf()))
            setAuthTokenFingerprint("00000000")
            setWalletIconUri(walletIconUri)
            setPublicKeyBase58(publicKey)
            setWalletLabel(walletLabel1)
            setKey(ByteArray(32) { 0x42 })
        }
    }

    private fun seed3CacheRecords() {
        for ((pkg, label, bytes) in listOf(
            Triple(walletPackage1, walletLabel1, authTokenBytes1),
            Triple(walletPackage2, walletLabel2, authTokenBytes2),
            Triple(walletPackage3, walletLabel3, authTokenBytes3),
        )) {
            val key = CacheKey("devnet", "solana:devnet", pkg, identityUri)
            val record = CacheRecord(
                schema = 1,
                authToken = SecretString(bytes.copyOf()),
                walletPackage = pkg,
                walletLabel = label,
                walletIconUri = walletIconUri,
                publicKey = publicKey,
                cluster = "devnet",
                chainId = "solana:devnet",
                identityUri = identityUri,
                issuedAtMs = 1_700_000_000_000L,
                lastUsedAtMs = 1_700_000_000_000L,
            )
            store.putToken(key, record)
        }
    }

    // ---------------- AC-1 ----------------

    @Test
    fun `AC-1 happy path wipes 3 tokens + rotates MasterKey + clears sessionState within 10s`() {
        // 3 wallets, all happy on deauth. mwaClientFactory returns a relaxed
        // MwaClient mock (deauthorize succeeds via the mock's default Unit
        // return — sealed-class MwaResult requires explicit coEvery setup
        // for Success). Use FakeMwaClient(success) to get real fixture parsing.
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedConnectedSession()
        seed3CacheRecords()
        assertEquals(3, store.listAllKeys().size, "pre-condition: 3 records seeded")

        plugin.mwaForgetAll("fa-ac1")

        // Within 10s: listAllKeys should be empty, MasterKey alias deleted,
        // sessionState cleared, is_connected == false.
        awaitCondition(10_000L) {
            runCatching {
                store.listAllKeys().isEmpty() &&
                    GDExtensionAndroidPlugin.sessionState.getAuthToken() == null
            }.getOrDefault(false)
        }

        assertTrue(store.listAllKeys().isEmpty(), "AC-1: listAllKeys must be empty post-forget_all")
        assertNull(
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
            "AC-1: sessionState.getAuthToken must be null post-clear",
        )
        // DD-4-2-4: KeyStore.deleteEntry(MASTER_KEY_ALIAS) was invoked.
        verify(atLeast = 1) { keystoreMock.deleteEntry(SecureTokenStore.MASTER_KEY_ALIAS) }

        // No terminal error signals fire on the happy path.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postReauthRequired(any()) }
    }

    // ---------------- AC-2 ----------------

    @Test
    fun `AC-2 cancels in-flight sign_messages op via mwa_cancelled_lifecycle BEFORE wipe starts`() {
        val inflight = InflightMap()
        // Pre-seed an in-flight slot with sourceMethod="sign_messages" per
        // DD-4-2-1's 3-arg register signature (default arg is at end).
        inflight.register("inflight-sm-1", 1_700_000_000_000L, sourceMethod = "sign_messages")
        assertEquals(1, inflight.size(), "pre-condition: 1 in-flight slot")

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            inflightOverride = inflight,
        )
        seedConnectedSession()
        seed3CacheRecords()

        val cancelledJsonSlot = slot<String>()
        plugin.mwaForgetAll("fa-ac2")

        // mwa_cancelled_lifecycle must fire for the pre-seeded in-flight slot.
        awaitCondition(5_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaCancelledLifecycle(capture(cancelledJsonSlot)) }
            }.isSuccess
        }

        // AC-2 ordering: postMwaCancelledLifecycle must fire BEFORE
        // store.deleteAll (the wipe) per DD-4-2-3.
        verifyOrder {
            nativeBridge.postMwaCancelledLifecycle(any())
            store.deleteAll()
        }

        val cancelled = JSONObject(cancelledJsonSlot.captured)
        assertEquals("inflight-sm-1", cancelled.getString("request_id"), "AC-2: request_id from in-flight slot")
        assertEquals("sign_messages", cancelled.getString("source_method"), "AC-2: source_method from inflightMap")
        assertEquals("forget_all_invoked", cancelled.getString("reason"), "AC-2: reason literal")

        // Post-wipe: in-flight slot was tryTerminate'd (CAS-removed) by the
        // cancel loop. The MWA snapshot at the cancel-loop time is stable
        // even after the wipe completes.
        assertEquals(0, inflight.size(), "AC-2: in-flight slot must be removed after cancel loop")
    }

    // ---------------- AC-3 ----------------

    @Test
    fun `AC-3 mutex serializes concurrent forget_all and connect — connect observes clean state`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedConnectedSession()
        seed3CacheRecords()

        val identityJson = JSONObject().apply {
            put("name", identityName)
            put("icon_uri", iconUri)
            put("identity_uri", identityUri)
        }.toString()

        // Two concurrent coroutines: forget_all + mwaAuthorize.
        runBlocking {
            val forgetJob = async {
                plugin.mwaForgetAll("fa-ac3-fa")
            }
            val connectJob = async {
                // Small initial delay to give forget_all a chance to grab the
                // mutex first. The serialization invariant should hold either
                // way (forget_all-then-connect OR connect-then-forget_all),
                // but the more interesting case is forget_all-first because
                // it tests connect-after-wipe-sees-clean-state.
                Thread.sleep(50)
                plugin.mwaAuthorize("fa-ac3-conn", identityJson, "devnet", "solana:devnet", 5_000L)
            }
            awaitAll(forgetJob, connectJob)
        }

        // Wait for both ops to settle.
        Thread.sleep(2_000L)

        // AC-3 invariant: NO partial state. Either:
        // (a) forget_all completed first → connect saw clean state, then connect
        //     populated 1 record + sessionState with the post-connect state.
        // (b) connect completed first → forget_all wiped everything, listAllKeys empty.
        // Either way, the FINAL observable state is one of these two clean alternatives.
        val finalKeys = store.listAllKeys()
        val finalAuthToken = GDExtensionAndroidPlugin.sessionState.getAuthToken()
        val cleanAfterForget = finalKeys.isEmpty() && finalAuthToken == null
        val cleanAfterConnect = finalKeys.size == 1 && finalAuthToken != null
        assertTrue(
            cleanAfterForget || cleanAfterConnect,
            "AC-3: final state must be either fully-wiped (forget_all-last) OR fully-connected (connect-last); " +
                "got listAllKeys.size=${finalKeys.size}, authToken=${if (finalAuthToken == null) "null" else "set"}",
        )
    }

    // ---------------- AC-4 ----------------

    @Test
    fun `AC-4 wallet offline does not block local wipe`() {
        // Two-scenario factory: the first call returns success, second returns
        // a slow/erroring client to simulate one offline wallet, third returns
        // success again. FakeMwaClient(network_offline) for the offline wallet.
        var callCount = 0
        val plugin = buildPlugin(
            clientFactory = {
                callCount++
                when (callCount) {
                    2 -> FakeMwaClient(fixturesDir).withScenario("network_offline")
                    else -> FakeMwaClient(fixturesDir).withScenario("success")
                }
            },
        )
        seedConnectedSession()
        seed3CacheRecords()

        plugin.mwaForgetAll("fa-ac4")

        // Within 10s: all 3 records wiped despite the offline wallet.
        awaitCondition(10_000L) {
            store.listAllKeys().isEmpty()
        }

        assertTrue(
            store.listAllKeys().isEmpty(),
            "AC-4: local wipe completes despite one offline wallet",
        )
        verify(atLeast = 1) { keystoreMock.deleteEntry(SecureTokenStore.MASTER_KEY_ALIAS) }
        // The offline wallet's deauth attempt failed but the loop continued —
        // factory was invoked at least 3 times (once per record).
        assertTrue(callCount >= 3, "AC-4: per-wallet deauth loop continued past offline wallet (got $callCount factory calls)")
    }

    // ---------------- DD-4-2-5 per-record StorageCorruptException continue ----------------

    @Test
    fun `runForgetAll loop continues on per-wallet StorageCorruptException for getToken`() {
        seed3CacheRecords()
        val seededKeys = store.listAllKeys()
        // Override getToken to throw StorageCorruptException for the 2nd record
        // ONLY; the 1st and 3rd return their seeded records.
        every { store.getToken(seededKeys[0]) } returns storedRecords[seededKeys[0]]
        every { store.getToken(seededKeys[1]) } throws StorageCorruptException(RuntimeException("simulated corruption on record 2"))
        every { store.getToken(seededKeys[2]) } returns storedRecords[seededKeys[2]]

        var factoryCallCount = 0
        val plugin = buildPlugin(
            clientFactory = {
                factoryCallCount++
                FakeMwaClient(fixturesDir).withScenario("success")
            },
        )
        seedConnectedSession()

        plugin.mwaForgetAll("fa-corrupt-record-2")

        awaitCondition(10_000L) { store.listAllKeys().isEmpty() }

        // Loop continued past the 2nd record's StorageCorruptException — at
        // least 2 deauth attempts were invoked (records 1 and 3); record 2
        // was skipped.
        assertTrue(
            factoryCallCount >= 2,
            "DD-4-2-5: per-wallet deauth loop must continue on per-record StorageCorruptException " +
                "(got $factoryCallCount factory calls; expected ≥2 for records 1 + 3)",
        )
        // Local wipe still proceeds.
        verify(atLeast = 1) { keystoreMock.deleteEntry(SecureTokenStore.MASTER_KEY_ALIAS) }
    }

    // ---------------- DD-4-2-6 listAllKeys StorageCorruptException bypasses wrapper ----------------

    @Test
    fun `forget_all bypasses withStorageOrReauthRequired wrapper on listAllKeys StorageCorruptException`() {
        // Mock listAllKeys to throw — the wrapper would normally fire
        // postReauthRequired here, but DD-4-2-6 LOCKS that forget_all bypasses
        // the wrapper (corruption-during-wipe is irrelevant; the wipe proceeds
        // with the keystore-side rotation which also wipes the corrupt prefs
        // file as a side-effect).
        every { store.listAllKeys() } throws StorageCorruptException(
            RuntimeException("simulated corruption on listAllKeys"),
        )

        val factoryNeverInvoked = mockk<() -> MwaClient>(relaxed = true)
        every { factoryNeverInvoked() } returns FakeMwaClient(fixturesDir).withScenario("success")

        val plugin = buildPlugin(clientFactory = { factoryNeverInvoked() })
        seedConnectedSession()

        plugin.mwaForgetAll("fa-corrupt-listAllKeys")

        // Wait for the keystore rotation to land (proves the wipe proceeded
        // despite listAllKeys throwing).
        awaitCondition(5_000L) {
            runCatching {
                verify(atLeast = 1) { keystoreMock.deleteEntry(SecureTokenStore.MASTER_KEY_ALIAS) }
            }.isSuccess
        }

        // DD-4-2-6: postReauthRequired must NEVER be invoked on the forget_all
        // path even when listAllKeys throws StorageCorruptException.
        verify(exactly = 0) { nativeBridge.postReauthRequired(any()) }
        // No deauth attempts invoked because listAllKeys returned empty list
        // (catch-and-fall-through to keystore-side rotation per DD-4-2-5
        // listAllKeys-empty path).
        verify(exactly = 0) { factoryNeverInvoked() }
    }

    /**
     * CR-67 follow-up #2 closure (2026-05-07) — after the 3-step wipe, two
     * warm probes via `storeProvider(ctx).listAllKeys()` must run so
     * Android's `SharedPreferences` in-process cache is replaced with a
     * Tink-fresh state. Without these probes, the next user-driven
     * authorize emits `reauth_required{keystore_corrupt}` because the
     * cached `SharedPreferencesImpl` returns stale Tink keyset bytes that
     * AEAD-fail with the regenerated MasterKey. This regression test
     * pins the call count: 1 deauth-loop probe + 2 warm probes = 3
     * `listAllKeys()` invocations on the happy path. Probe failures
     * MUST NOT propagate as `postReauthRequired` from forget_all.
     */
    @Test
    fun `forget_all warm-probes the store twice after wipe to flush stale prefs cache (CR-67 #2)`() {
        // FIRST listAllKeys returns 1 record (deauth loop), SECOND throws
        // (simulates the cached-keyset / regenerated-master-key AEAD fail
        // we observed on Android 13 emulator), THIRD returns empty (warm
        // probe #2 lazy-inits cleanly against the now-evicted cache).
        var listAllKeysCalls = 0
        every { store.listAllKeys() } answers {
            listAllKeysCalls += 1
            when (listAllKeysCalls) {
                1 -> listOf(
                    CacheKey("devnet", "solana:devnet", walletPackage1, identityUri),
                )
                2 -> throw StorageCorruptException(RuntimeException("simulated stale prefs cache"))
                else -> emptyList()
            }
        }

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedConnectedSession()

        plugin.mwaForgetAll("fa-warm-probe")

        // 3 invocations: 1 deauth-loop + 2 warm probes (probe #1 throws,
        // probe #2 succeeds). The wipe steps must still complete.
        awaitCondition(5_000L) {
            runCatching {
                verify(exactly = 3) { store.listAllKeys() }
                verify(atLeast = 1) { keystoreMock.deleteEntry(SecureTokenStore.MASTER_KEY_ALIAS) }
            }.isSuccess
        }

        verify(exactly = 3) { store.listAllKeys() }
        // Critical UX invariant: `reauth_required` must NEVER fire from the
        // forget_all path, even when a warm probe trips on stale cache.
        verify(exactly = 0) { nativeBridge.postReauthRequired(any()) }
    }
}
