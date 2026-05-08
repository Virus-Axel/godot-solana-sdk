package com.godotengine.godot_solana_sdk.mwa.plugin

import android.net.Uri
import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.AuthResult
import com.godotengine.godot_solana_sdk.mwa.store.AuthTokenFingerprint
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import io.mockk.every
import io.mockk.mockk
import io.mockk.mockkObject
import io.mockk.mockkStatic
import io.mockk.slot
import io.mockk.unmockkObject
import io.mockk.unmockkStatic
import io.mockk.verify
import io.mockk.verifyOrder
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.delay
import org.godotengine.godot.Godot
import org.json.JSONObject
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Assertions.assertNull
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 2-2 T1 — `GDExtensionAndroidPlugin.mwaReauthorize` coverage (TDD red).
 *
 * 8 scenarios:
 *   1. AC-1 silent happy — FakeMwaClient `reauthorize_success.json`
 *   2. AC-2 no cached token — cache-miss → NOT_CONNECTED within 1 frame
 *   3. AC-3 token expired — graceful wipe + TOKEN_EXPIRED + DD-2-2-2 ordering
 *   4. AC-4 cluster mismatch — collapses to NOT_CONNECTED via CacheKey miss (DD-2-2-1)
 *   5. AC-6 fingerprint identical pre and post reauthorize
 *   6. Watchdog timeout — slow client, short effective watchdog
 *   7. Late-result-after-terminate dropped — terminal-signal invariant
 *   8. Duplicate requestId — PROTOCOL_ERROR
 *
 * Plugin is built via the `@VisibleForTesting` ctor with injected collaborators
 * (identical to [AuthorizePathTest] / [MwaAndroidPluginDisconnectTest]):
 *   - recording [NativeBridge] mock
 *   - map-backed fake [SecureTokenStore] via `storeProvider`
 *   - FakeMwaClient or slow-client test double via `mwaClientFactory`
 *   - no-op `senderProvider` (reauth path doesn't launch a wallet Activity)
 *
 * Tests compile but fail at runtime against `TODO("Story 2-2 T2 fills in")` stub
 * bodies — that is the intended RED state for T1. T2 replaces stubs with real impl.
 */
class MwaAndroidPluginReauthorizeTest {

    private val fixturesDir: File by lazy {
        val prop = System.getProperty("mwa.fixtures.dir")
            ?: error("system property 'mwa.fixtures.dir' must be set (wired in build.gradle.kts)")
        File(prop).also { require(it.isDirectory) { "fixtures dir missing: $it" } }
    }

    private lateinit var nativeBridge: NativeBridge
    private lateinit var senderMock: ActivityResultSender
    private lateinit var storedRecords: MutableMap<CacheKey, CacheRecord>
    private lateinit var store: SecureTokenStore

    // Stable identity / cluster values mirroring Story 2-1 / 2-3 fixtures.
    // walletPackage is stored in CacheRecord but is NOT passed as a caller arg —
    // the 3-tuple filter (DD-2-2-7) discovers it from listAllKeys().
    private val identityUri = "https://game.example"
    private val identityName = "Game"
    private val iconUri = "https://x/i"
    private val walletPackage = "com.wallet.app"
    private val walletLabel = "TestWallet"
    private val walletIconUri = "https://wallet.example/icon.png"
    private val publicKey = "AbcDefGhiJkl1234567890AbcDefGhiJkl1234567890"

    // FakeMwaClient treats `auth_token` as an OPAQUE STRING — UTF-8 bytes of the raw
    // string from the fixture, NOT base64-decoded (see FakeMwaClient.kt:52-57 and L169:
    // `SecretString(payload.getString("auth_token").toByteArray(Charsets.UTF_8))`).
    // Seed value MUST be the byte sequence FakeMwaClient will return so the impl's
    // DD-2-2-5 rotation-detection (`auth.authToken.reveal().contentEquals(cached.authToken.reveal())`)
    // observes byte-identity in the happy path — matching the LOCKED contract at story line 122-124
    // ("expected_input.auth_token and response.payload.auth_token are byte-identical:
    //   `dGVzdC1hdXRoLXRva2VuLXJlYXV0aA==`").
    private val authTokenBytes = "dGVzdC1hdXRoLXRva2VuLXJlYXV0aA==".toByteArray(Charsets.UTF_8)
    private val salt = ByteArray(32) { 0x01 }

    // AC-6 fingerprint identity is mathematical (DD-2-2-5): HKDF-SHA256(authTokenBytes, salt).
    // Compute once at class-init so seeded session-state value matches the value the plugin
    // recomputes post-reauthorize against the byte-identical FakeMwaClient fixture token.
    private val preFingerprint: String = AuthTokenFingerprint.compute(authTokenBytes, salt)

    // I-T1-B (DD-2-2-2 ordering capture): records `sessionState.getAuthToken()` AT THE MOMENT
    // `store.deleteToken(...)` is invoked. If `clearOnLogout()` ran first, this list contains
    // `null`. If `deleteToken` ran before `clearOnLogout`, this list contains a non-null token.
    // Lower-friction substitute for `verifyOrder { sessionState.clearOnLogout(); ... }` since
    // sessionState is the real singleton (not mockable here without invasive surgery).
    private lateinit var authTokenAtDeleteCall: MutableList<SecretString?>

    private val identityJson = JSONObject().apply {
        put("name", identityName)
        put("icon_uri", iconUri)
        put("identity_uri", identityUri)
    }.toString()

    @BeforeEach
    fun setup() {
        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0

        // `parseIdentity` constructs `ConnectionIdentity` via `Uri.parse(...)` —
        // mock to a relaxed mock so ConnectionIdentity construction succeeds on JVM.
        mockkStatic(Uri::class)
        every { Uri.parse(any()) } returns mockk(relaxed = true)

        nativeBridge = mockk(relaxed = true)
        senderMock = mockk(relaxed = true)

        // Map-backed fake store with listAllKeys() support for DD-2-2-7 3-tuple filter.
        storedRecords = mutableMapOf()
        authTokenAtDeleteCall = mutableListOf()
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
            every { deleteToken(any()) } answers {
                val key: CacheKey = firstArg()
                // I-T1-B: snapshot sessionState BEFORE removing the record so AC-3 can
                // assert clearOnLogout() ran first (DD-2-2-2 step 2 precedes step 3).
                authTokenAtDeleteCall.add(GDExtensionAndroidPlugin.sessionState.getAuthToken())
                storedRecords.remove(key)
                Unit
            }
            every { listAllKeys() } answers { storedRecords.keys.toList() }
            every { getOrCreatePerInstallSalt() } returns salt
        }

        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @AfterEach
    fun teardown() {
        unmockkStatic(Log::class)
        unmockkStatic(Uri::class)
    }

    private fun buildPlugin(
        clientFactory: () -> MwaClient = {
            error("unexpected mwaClientFactory invocation")
        },
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
            // Tests run without an Android main looper — use Unconfined so
            // `withContext(mainDispatcher)` runs in the caller's thread.
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
     * Seeds MwaSessionState with a post-connect snapshot — same values the
     * pre-seeded CacheRecord carries — so AC-6 can capture a pre-call fingerprint.
     */
    private fun seedPostConnectSession() {
        GDExtensionAndroidPlugin.sessionState.apply {
            setIdentity(identityUri, iconUri, identityName)
            setCluster(2)
            setClusterName("devnet")
            setAuthToken(SecretString(authTokenBytes.copyOf()))
            setAuthTokenFingerprint(preFingerprint)
            setWalletIconUri(walletIconUri)
            setPublicKeyBase58(publicKey)
            setWalletLabel(walletLabel)
            setKey(ByteArray(32) { 0x42 })
        }
    }

    /**
     * Seeds the fake store with a CacheRecord using the test's canonical
     * (cluster="devnet", chainId="solana:devnet", walletPackage, identityUri) 4-tuple.
     * The mwaReauthorize call under test MUST NOT receive walletPackage — it
     * discovers the key via the 3-tuple filter (DD-2-2-7).
     */
    private fun seedCacheRecord(cluster: String = "devnet", chainId: String = "solana:devnet"): Pair<CacheKey, CacheRecord> {
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
        store.putToken(key, record)
        return key to record
    }

    @Test
    fun `AC-1 silent reauth emits reauthorize_completed with retained public_key`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedPostConnectSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaReauthorize("rea-ac1", identityJson, "devnet", "solana:devnet", 5000L)

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postReauthorizeCompleted("rea-ac1", capture(jsonSlot)) }
            }.isSuccess
        }

        // (a) Result payload shape — 6-key per DD-2-2-4 + A-12.
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("rea-ac1", payload.getString("request_id"))
        assertTrue(payload.has("auth_token_fingerprint"), "payload must have auth_token_fingerprint")
        assertEquals(publicKey, payload.getString("public_key"), "public_key retained (AC-1)")
        assertTrue(payload.has("wallet_label"), "payload must have wallet_label")
        assertTrue(payload.has("wallet_icon_uri"), "payload must have wallet_icon_uri")
        assertEquals("devnet", payload.getString("cluster"), "cluster field present")

        // (b) Session state updated — AC-1 silent success.
        assertNotNull(GDExtensionAndroidPlugin.sessionState.getAuthToken(), "authToken must be set")
        assertEquals(publicKey, GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(), "publicKeyBase58 retained")

        // (c) Terminal-signal invariant — no competing error or timeout signals.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }

    @Test
    fun `AC-2 no cached token emits NOT_CONNECTED within 1 frame`() {
        // DO NOT call seedCacheRecord() — empty store simulates first-launch / post-deauthorize.
        val clientFactory = mockk<() -> MwaClient>()
        val plugin = buildPlugin(clientFactory = { clientFactory() })

        val jsonSlot = slot<String>()
        plugin.mwaReauthorize("rea-ac2", identityJson, "devnet", "solana:devnet", 5000L)

        awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
            }.isSuccess
        }

        // (a) Error payload — A-14 10-key shape with NOT_CONNECTED + source_method + retry_hint.
        val err = JSONObject(jsonSlot.captured)
        assertEquals("rea-ac2", err.getString("request_id"))
        assertEquals("NOT_CONNECTED", err.getString("code"))
        assertEquals("reauthorize", err.getString("source_method"))
        assertEquals("connect", err.getString("retry_hint"))
        assertTrue(err.has("message"), "A-14 key: message")
        assertTrue(err.has("user_message"), "A-14 key: user_message")
        assertTrue(err.has("developer_details"), "A-14 key: developer_details")
        assertTrue(err.has("recoverable"), "A-14 key: recoverable")
        assertTrue(err.has("layer"), "A-14 key: layer")

        // (b) No reauthorize_completed emitted — cache-miss never calls MwaClient.
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }

        // (c) mwaClientFactory NEVER invoked — cache-miss short-circuits before MwaClient call.
        verify(exactly = 0) { clientFactory() }
    }

    @Test
    fun `AC-3 token expired triggers graceful wipe and TOKEN_EXPIRED`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("token_expired") },
        )
        seedPostConnectSession()
        val (cacheKey, _) = seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaReauthorize("rea-ac3", identityJson, "devnet", "solana:devnet", 5000L)

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
            }.isSuccess
        }

        // (a) Error payload — TOKEN_EXPIRED + source_method=reauthorize + retry_hint=connect.
        val err = JSONObject(jsonSlot.captured)
        assertEquals("rea-ac3", err.getString("request_id"))
        assertEquals("TOKEN_EXPIRED", err.getString("code"))
        assertEquals("reauthorize", err.getString("source_method"))
        assertEquals("connect", err.getString("retry_hint"))

        // (b) Graceful wipe — CacheRecord deleted from store (first production caller of deleteToken).
        assertNull(store.getToken(cacheKey), "CacheRecord must be deleted (graceful wipe AC-3)")

        // (c) Session state wiped — auth-only fields cleared.
        assertNull(GDExtensionAndroidPlugin.sessionState.getAuthToken(), "authToken wiped")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(), "publicKeyBase58 wiped")

        // (d) No reauthorize_completed emitted alongside.
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }

        // (e) DD-2-2-2 ordering assertion — mock-call-order verifies the back half:
        //     ... → deleteToken (disk) → postMwaError (emit).
        verifyOrder {
            store.deleteToken(cacheKey)
            nativeBridge.postMwaError(any())
        }

        // (f) DD-2-2-2 step-2-before-step-3 — `clearOnLogout()` precedes `deleteToken()`.
        // The deleteToken mock answer captured `sessionState.getAuthToken()` at call time;
        // null here means clearOnLogout already ran (sessionState is the real singleton, so
        // direct verifyOrder against it is not possible without invasive companion surgery).
        assertEquals(1, authTokenAtDeleteCall.size, "deleteToken should be invoked exactly once")
        assertNull(
            authTokenAtDeleteCall[0],
            "DD-2-2-2: sessionState must be wiped (clearOnLogout) BEFORE deleteToken",
        )
    }

    @Test
    fun `AC-4 cluster mismatch collapses to NOT_CONNECTED via CacheKey miss`() {
        // Seed a devnet record; caller passes mainnet — 3-tuple filter finds zero matches.
        seedPostConnectSession()
        val (devnetKey, _) = seedCacheRecord(cluster = "devnet", chainId = "solana:devnet")

        val clientFactory = mockk<() -> MwaClient>()
        val plugin = buildPlugin(clientFactory = { clientFactory() })

        val jsonSlot = slot<String>()
        // Call with MAINNET — cluster mismatch causes filter to return empty.
        plugin.mwaReauthorize("rea-ac4", identityJson, "mainnet", "solana:mainnet", 5000L)

        awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
            }.isSuccess
        }

        // (a) Outcome: NOT_CONNECTED (not a distinct cluster_mismatch code — DD-2-2-1).
        val err = JSONObject(jsonSlot.captured)
        assertEquals("rea-ac4", err.getString("request_id"))
        assertEquals("NOT_CONNECTED", err.getString("code"))
        assertEquals("reauthorize", err.getString("source_method"))

        // (b) mwaClientFactory NEVER invoked — cache-miss (empty 3-tuple filter) exits before MwaClient call.
        verify(exactly = 0) { clientFactory() }

        // (c) Devnet CacheRecord is NOT deleted — only AC-3 deletes (DD-2-2-1 / graceful wipe scope).
        assertNotNull(store.getToken(devnetKey), "devnet CacheRecord must NOT be deleted on cluster mismatch")

        // (d) No reauthorize_completed emitted.
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }
    }

    @Test
    fun `AC-6 fingerprint identical pre and post reauthorize`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedPostConnectSession()
        val (cacheKey, _) = seedCacheRecord()

        // Capture pre-call fingerprint from session state.
        val prePrint = GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint()

        plugin.mwaReauthorize("rea-ac6", identityJson, "devnet", "solana:devnet", 5000L)

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postReauthorizeCompleted("rea-ac6", any()) }
            }.isSuccess
        }

        // (a) Session-state fingerprint is byte-identical pre and post reauthorize (AC-6).
        val postPrint = GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint()
        assertEquals(prePrint, postPrint, "AC-6: auth_token_fingerprint must be identical pre and post reauthorize")

        // (b) Function-level invariant — AuthTokenFingerprint.compute is deterministic.
        //     FakeMwaClient reauthorize_success.json returns byte-identical authToken bytes
        //     (dGVzdC1hdXRoLXRva2VuLXJlYXV0aA== both input and response per DD-2-2-5).
        val recordAfter = store.getToken(cacheKey)
        assertNotNull(recordAfter, "CacheRecord must still exist post successful reauth")
        val recomputedPrint = AuthTokenFingerprint.compute(recordAfter!!.authToken.reveal(), salt)
        assertEquals(prePrint, recomputedPrint, "AuthTokenFingerprint.compute returns same value for same token bytes + salt")

        // (c) No competing error/timeout.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }

    @Test
    fun `watchdog timeout emits mwa_timeout source_method=reauthorize`() {
        // Inline slow client — delays past the effective watchdog so withTimeoutOrNull fires.
        val slowClient = object : MwaClient {
            override suspend fun reauthorize(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                cluster: String,
                chainId: String,
            ): MwaResult<AuthResult> {
                delay(5_000)
                error("unreachable — should be cancelled by withTimeoutOrNull")
            }
            override suspend fun authorize(sender: ActivityResultSender, identity: ConnectionIdentity, cluster: String, chainId: String) =
                error("nyi: authorize")
            override suspend fun deauthorize(sender: ActivityResultSender, identity: ConnectionIdentity, authToken: SecretString) =
                error("nyi: deauthorize")
            override suspend fun disconnect() = error("nyi: disconnect")
            override suspend fun signMessages(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                messages: List<ByteArray>,
                addresses: List<ByteArray>,
            ) = error("nyi: signMessages")
            override suspend fun signTransactions(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                transactions: List<ByteArray>,
            ) = error("nyi: signTransactions")
            override suspend fun signAndSendTransactions(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                transactions: List<ByteArray>,
                cluster: String,
            ) = error("nyi: signAndSend")
            override fun close() {}
        }

        val plugin = buildPlugin(clientFactory = { slowClient })
        seedPostConnectSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        // effectiveMs = min(60_000, 100) = 100ms — watchdog fires before slow client completes.
        plugin.mwaReauthorize("rea-to", identityJson, "devnet", "solana:devnet", 100L)

        awaitCondition(1_500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaTimeout(capture(jsonSlot)) }
            }.isSuccess
        }

        // (a) Timeout payload fields — DD-2-2-3.
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("rea-to", payload.getString("request_id"))
        assertEquals("reauthorize", payload.getString("source_method"))
        assertEquals(100L, payload.getLong("watchdog_ms"))

        // (b) Terminal-signal invariant — no competing signals.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }
    }

    @Test
    fun `late-result-after-terminate is dropped (terminal-signal invariant)`() {
        val fakeInflight = mockk<InflightMap>(relaxed = true)
        every { fakeInflight.register(any(), any(), any()) } returns true
        // Force CAS loss on tryTerminate — simulates a late result after watchdog already fired.
        every { fakeInflight.tryTerminate(any()) } returns false

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            inflightOverride = fakeInflight,
        )
        seedPostConnectSession()
        seedCacheRecord()

        plugin.mwaReauthorize("rea-late", identityJson, "devnet", "solana:devnet", 5000L)

        awaitCondition(3_000L) {
            runCatching {
                // Diagnostics incremented means late_result was observed.
                assertTrue(plugin.diagnosticsForTest().lateResultCount >= 1)
            }.isSuccess
        }

        // (a) ZERO terminal signals emitted (CAS lost → drop).
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }

        // (b) Diagnostics incremented.
        assertTrue(plugin.diagnosticsForTest().lateResultCount >= 1, "late result count must be ≥1")
    }

    @Test
    fun `duplicate requestId emits PROTOCOL_ERROR on register`() {
        val fakeInflight = mockk<InflightMap>(relaxed = true)
        // Simulate duplicate: register returns false immediately.
        every { fakeInflight.register(any(), any(), any()) } returns false

        val plugin = buildPlugin(inflightOverride = fakeInflight)
        seedPostConnectSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaReauthorize("req-dup", identityJson, "devnet", "solana:devnet", 5000L)

        awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
            }.isSuccess
        }

        // (a) Error payload — PROTOCOL_ERROR + duplicate_request_id cause.
        val err = JSONObject(jsonSlot.captured)
        assertEquals("req-dup", err.getString("request_id"))
        assertEquals("PROTOCOL_ERROR", err.getString("code"))
        assertEquals("reauthorize", err.getString("source_method"))
        assertEquals("duplicate_request_id", err.getString("cause"))
        assertTrue(err.has("message"), "A-14 key: message")
        assertTrue(err.has("user_message"), "A-14 key: user_message")
        assertTrue(err.has("developer_details"), "A-14 key: developer_details")
        assertTrue(err.has("recoverable"), "A-14 key: recoverable")
        assertTrue(err.has("retry_hint"), "A-14 key: retry_hint")

        // (b) No reauthorize_completed — PROTOCOL_ERROR is the sole terminal signal.
        verify(exactly = 0) { nativeBridge.postReauthorizeCompleted(any(), any()) }
    }

    /**
     * CR-67 follow-up #3 closure (2026-05-07) — JNI shim must reconstruct
     * identity / cluster / chainId from MwaSessionState when the C++ stub at
     * `MwaAndroidBridgeJni::reauthorize` passes empty strings (Story 2-2 stub
     * never completed). With a populated session, the shim synthesizes
     * identityJson + cluster + chainId and forwards to mwaReauthorize, which
     * proceeds to the AC-1 happy path.
     */
    @Test
    fun `JNI shim with empty args falls back to sessionState (CR-67 #3)`() {
        buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedPostConnectSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        // Static JNI shim called with empty identityJson/cluster/chainId — same
        // shape as the C++ stub at mwa_android_bridge_jni.cpp:481-484 emits.
        GDExtensionAndroidPlugin.mwaReauthorizeFromJni("rea-jni-empty", "", "", "", 5000L)

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postReauthorizeCompleted("rea-jni-empty", capture(jsonSlot)) }
            }.isSuccess
        }

        val payload = JSONObject(jsonSlot.captured)
        assertEquals("rea-jni-empty", payload.getString("request_id"))
        assertEquals(publicKey, payload.getString("public_key"), "public_key retained via sessionState fallback")
        assertEquals("devnet", payload.getString("cluster"), "cluster reconstructed from sessionState")

        // Terminal-signal invariant — no PROTOCOL_ERROR despite empty C++ args.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
    }

    /**
     * CR-67 follow-up #3 closure (2026-05-07) — JNI shim with empty args AND
     * empty MwaSessionState (caller invokes Reauthorize without a prior
     * Connect, or after a wipe that cleared sessionState) emits NOT_CONNECTED
     * with `cause="no_prior_session"` rather than the misleading
     * `PROTOCOL_ERROR{cause=identity.name required}` that the pre-fix path
     * produced.
     */
    @Test
    fun `JNI shim with empty args and empty session emits NOT_CONNECTED no_prior_session (CR-67 #3)`() {
        buildPlugin()
        // Deliberately do NOT call seedPostConnectSession — sessionState is empty.
        // Belt-and-suspenders: also explicitly clear in case a prior test polluted
        // the companion-object singleton (sessionState is process-wide).
        GDExtensionAndroidPlugin.sessionState.clear()

        mockkObject(DefaultNativeBridge)
        try {
            val jsonSlot = slot<String>()
            every { DefaultNativeBridge.postMwaError(capture(jsonSlot)) } returns Unit

            GDExtensionAndroidPlugin.mwaReauthorizeFromJni("rea-jni-empty-sess", "", "", "", 5000L)

            verify(exactly = 1) { DefaultNativeBridge.postMwaError(any()) }

            val err = JSONObject(jsonSlot.captured)
            assertEquals("rea-jni-empty-sess", err.getString("request_id"))
            assertEquals("NOT_CONNECTED", err.getString("code"))
            assertEquals("reauthorize", err.getString("source_method"))
            assertEquals("no_prior_session", err.getString("cause"))
            assertTrue(
                err.getString("developer_details").contains("Story 2-2 stub"),
                "developer_details must reference the C++ stub for triage",
            )
        } finally {
            unmockkObject(DefaultNativeBridge)
        }
    }
}
