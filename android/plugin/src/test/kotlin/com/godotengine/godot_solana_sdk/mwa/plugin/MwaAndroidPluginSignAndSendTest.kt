package com.godotengine.godot_solana_sdk.mwa.plugin

import android.net.Uri
import android.util.Log
import androidx.lifecycle.LifecycleOwner
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignAndSendResult
import com.godotengine.godot_solana_sdk.mwa.generated.MwaError
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
import kotlinx.coroutines.runBlocking
import org.godotengine.godot.Godot
import org.json.JSONObject
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import plugin.walletadapterandroid.SigningOp
import java.io.File

/**
 * Story 3-3 T1 — `GDExtensionAndroidPlugin.mwaSignAndSendTransactions` coverage (TDD red).
 *
 * 10 scenarios:
 *   1. AC-1 happy path — `sign_and_send_completed` 4-key payload
 *      `{request_id, signatures: [base58 strings], submitted_at: Long, confirmation_status: "submitted"}`
 *      per DD-3-3-E
 *   2. AC-2 breadcrumb write-ahead — 6-key dict written via
 *      `secureTokenStore.putPendingSubmission` BEFORE `mwaClientFactory()`
 *      invocation per DD-3-3-B (write-then-call ordering); tx_preview_hashes
 *      are full-64-char lowercase-hex SHA-256 per DD-3-3-F
 *   3. AC-3 cleanup-on-success ordering — `removePendingSubmission(requestId)`
 *      called BEFORE `postSignAndSendCompleted` per DD-3-3-C handleSignAndSendSuccess
 *   4. AC-4a cleanup-on-error — `removePendingSubmission` called once on
 *      WALLET_REJECTED via fixture
 *   5. AC-4b cleanup-on-timeout — `removePendingSubmission` called once on
 *      watchdog timeout
 *   6. AC-4c cleanup-on-cancellation — [MwaLifecycleObserver.onDestroy]
 *      invokes `cleanupBreadcrumb(requestId)` per snapshotted slot
 *      (Story 5-3 closed CR-3-3-B)
 *   7. AC-5 stale-breadcrumb-on-next-connect — pre-seed breadcrumb;
 *      mwaAuthorize success fires `postPendingSubmissionFound` AFTER
 *      `postConnectCompleted` and breadcrumb is removed (one-shot); reconnect
 *      does NOT re-fire
 *   8. AC-6 cluster-bleed refusal — sessionState.cluster swapped post-connect;
 *      mwaSignAndSendTransactions emits NOT_CONNECTED preflight
 *   9. runSigningOp(SIGN_AND_SEND) Failure routing — direct test on the helper;
 *      DD-3-2-5 inheritance evidence (SigningOp.SIGN_AND_SEND.sourceMethod →
 *      "sign_and_send")
 *  10. Breadcrumb-write StorageCorruptException → reauth_required, NO wallet
 *      round-trip — DD-3-3-G fail-closed write site
 *
 * Test #9 (runSigningOp direct) bypasses `mwaSignAndSendTransactions` and
 * calls the shared [runSigningOp] helper from Story 3-1 directly. Test #6
 * (AC-4c) exercises the lifecycle-cancellation cleanup path via
 * [MwaLifecycleObserver] (Story 5-3 closed CR-3-3-B).
 *
 * Plugin is built via the `@VisibleForTesting` ctor with injected collaborators
 * (identical to [MwaAndroidPluginSignTransactionsTest] / [MwaAndroidPluginSignMessagesTest]):
 *   - recording [NativeBridge] mock
 *   - map-backed fake [SecureTokenStore] via `storeProvider` — tests that
 *     exercise the breadcrumb storage API mock the 4 NEW methods directly
 *     (putPendingSubmission, getPendingSubmission, listAllPendingSubmissions,
 *     removePendingSubmission) per DD-3-3-A
 *   - FakeMwaClient or relaxed-mock test double via `mwaClientFactory`
 *   - no-op `senderProvider`
 */
class MwaAndroidPluginSignAndSendTest {

    private val fixturesDir: File by lazy {
        val prop = System.getProperty("mwa.fixtures.dir")
            ?: error("system property 'mwa.fixtures.dir' must be set (wired in build.gradle.kts)")
        File(prop).also { require(it.isDirectory) { "fixtures dir missing: $it" } }
    }

    private lateinit var nativeBridge: NativeBridge
    private lateinit var senderMock: ActivityResultSender
    private lateinit var storedRecords: MutableMap<CacheKey, CacheRecord>
    private lateinit var storedPending: MutableMap<String, String>
    private lateinit var store: SecureTokenStore

    private val identityUri = "https://game.example"
    private val identityName = "Game"
    private val iconUri = "https://x/i"
    private val walletPackage = "com.wallet.app"
    private val walletLabel = "TestWallet"
    private val walletIconUri = "https://wallet.example/icon.png"
    private val publicKey = "AbcDefGhiJkl1234567890AbcDefGhiJkl1234567890"

    // FakeMwaClient `sign_and_send_success.json` fixture's `expected_input.auth_token`.
    private val authTokenBytes = "dGVzdC1hdXRoLXRva2VuLXNpZ25zZW5k".toByteArray(Charsets.UTF_8)
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

        nativeBridge = mockk(relaxed = true)
        senderMock = mockk(relaxed = true)

        storedRecords = mutableMapOf()
        storedPending = mutableMapOf()
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
            every { getOrCreatePerInstallSalt() } returns salt
            // DD-3-3-A breadcrumb storage API — the 4 NEW methods, mocked on the
            // shared map-backed fake store. T1 tests exercise these via verify(...)
            // + the storedPending map's reflective read.
            every { putPendingSubmission(any(), any()) } answers {
                val rid: String = firstArg()
                val json: String = secondArg()
                storedPending[rid] = json
            }
            every { getPendingSubmission(any()) } answers {
                val rid: String = firstArg()
                storedPending[rid]
            }
            every { listAllPendingSubmissions() } answers {
                storedPending.entries.map { it.key to it.value }
            }
            every { removePendingSubmission(any()) } answers {
                val rid: String = firstArg()
                storedPending.remove(rid)
                Unit
            }
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

    private fun seedConnectedSession(clusterName: String = "devnet") {
        GDExtensionAndroidPlugin.sessionState.apply {
            setIdentity(identityUri, iconUri, identityName)
            setCluster(2)
            setClusterName(clusterName)
            setAuthToken(SecretString(authTokenBytes.copyOf()))
            setAuthTokenFingerprint("00000000")
            setWalletIconUri(walletIconUri)
            setPublicKeyBase58(publicKey)
            setWalletLabel(walletLabel)
            setKey(ByteArray(32) { 0x42 })
        }
    }

    private fun seedCacheRecord(clusterName: String = "devnet", chainId: String = "solana:devnet") {
        val key = CacheKey(clusterName, chainId, walletPackage, identityUri)
        val record = CacheRecord(
            schema = 1,
            authToken = SecretString(authTokenBytes.copyOf()),
            walletPackage = walletPackage,
            walletLabel = walletLabel,
            walletIconUri = walletIconUri,
            publicKey = publicKey,
            cluster = clusterName,
            chainId = chainId,
            identityUri = identityUri,
            issuedAtMs = 1_700_000_000_000L,
            lastUsedAtMs = 1_700_000_000_000L,
        )
        store.putToken(key, record)
    }

    // ---------------- AC-1 ----------------

    @Test
    fun `AC-1 happy path emits sign_and_send_completed with 4-key payload and base58 signatures`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedConnectedSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaSignAndSendTransactions(
            requestId = "sas-ac1",
            transactions = listOf(byteArrayOf(0x01, 0x02)),
            timeoutMs = 5_000L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postSignAndSendCompleted("sas-ac1", capture(jsonSlot)) }
            }.isSuccess
        }

        val payload = JSONObject(jsonSlot.captured)
        assertEquals("sas-ac1", payload.getString("request_id"))
        assertTrue(payload.has("signatures"), "DD-3-3-E: payload must have signatures")
        assertTrue(payload.has("submitted_at"), "DD-3-3-E: payload must have submitted_at")
        assertTrue(payload.has("confirmation_status"), "DD-3-3-E: payload must have confirmation_status")
        // DD-3-3-E negative-key guards (regression catches if buildSignSuccessJson were
        // accidentally wired into postSignAndSendCompleted instead of buildSignAndSendSuccessJson).
        assertTrue(!payload.has("signed_payloads"), "DD-3-3-E: must NOT have signed_payloads (sign_messages key)")
        assertTrue(!payload.has("signed_transactions"), "DD-3-3-E: must NOT have signed_transactions (sign_transactions key)")

        val signatures = payload.getJSONArray("signatures")
        assertEquals(1, signatures.length(), "AC-1: signatures.size must equal fixture's signatures count")
        // Fixture's response.payload.signatures[0] is "5CrhasMLvBzCo8bfv6WpPzGTVrrAENZrwUtsSENXkpBJcsaQyBFbLDxLK2wpTtQhMwUS2UvuyJrqN2D2jqV5gn1T".
        assertEquals(
            "5CrhasMLvBzCo8bfv6WpPzGTVrrAENZrwUtsSENXkpBJcsaQyBFbLDxLK2wpTtQhMwUS2UvuyJrqN2D2jqV5gn1T",
            signatures.getString(0),
            "AC-1 / fixture-byte-fidelity: signature must match sign_and_send_success.json fixture verbatim",
        )

        val submittedAt = payload.getLong("submitted_at")
        assertEquals(1_700_000_000_000L, submittedAt, "AC-1: submitted_at must equal injected clock() value")

        assertEquals(
            "submitted",
            payload.getString("confirmation_status"),
            "AC-1: confirmation_status must be the literal \"submitted\"",
        )

        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }

    // ---------------- AC-2 ----------------

    @Test
    fun `AC-2 breadcrumb written before wallet round-trip with 6-key payload and full-hex preview hashes`() {
        val factoryInvoked = mockk<() -> Unit>(relaxed = true)
        val plugin = buildPlugin(
            clientFactory = {
                factoryInvoked()
                FakeMwaClient(fixturesDir).withScenario("success")
            },
        )
        seedConnectedSession()
        seedCacheRecord()

        // T2 fixup (D-3-3-T2-1, Rule 1) — capture the breadcrumb JSON at
        // putPendingSubmission time. AC-3 cleanup runs BEFORE the terminal
        // success signal, so reading storedPending[rid] AFTER the awaitCondition
        // for postSignAndSendCompleted would observe a removed entry. Slot
        // capture preserves the breadcrumb body without ordering coupling.
        val breadcrumbSlot = slot<String>()

        plugin.mwaSignAndSendTransactions(
            requestId = "sas-ac2",
            transactions = listOf(byteArrayOf(0x01, 0x02), byteArrayOf(0x03, 0x04)),
            timeoutMs = 5_000L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postSignAndSendCompleted("sas-ac2", any()) }
            }.isSuccess
        }

        // AC-2 / DD-3-3-B write-then-call ordering: putPendingSubmission must
        // have been called BEFORE the mwaClientFactory() invocation.
        verifyOrder {
            store.putPendingSubmission("sas-ac2", any())
            factoryInvoked()
        }

        // Inspect the breadcrumb JSON captured at put time (immune to AC-3 cleanup).
        verify(exactly = 1) { store.putPendingSubmission("sas-ac2", capture(breadcrumbSlot)) }
        val breadcrumbJson = breadcrumbSlot.captured
        val breadcrumb = JSONObject(breadcrumbJson)
        // 6 keys per AC-2 (the dict has 7 — request_id + 6 — but the spec lists the 6 carried-forward keys + the request_id).
        assertEquals("sas-ac2", breadcrumb.getString("request_id"), "AC-2: request_id")
        assertEquals("sign_and_send", breadcrumb.getString("op_type"), "AC-2: op_type literal")
        assertEquals("devnet", breadcrumb.getString("cluster"), "AC-2: cluster from sessionState.getClusterName()")
        assertEquals(identityUri, breadcrumb.getString("identity_uri"), "AC-2: identity_uri from sessionState")
        assertEquals(1_700_000_000_000L, breadcrumb.getLong("started_at_ms"), "AC-2: started_at_ms from clock()")
        assertEquals(2, breadcrumb.getInt("tx_count"), "AC-2: tx_count must equal transactions.size")

        val previewHashes = breadcrumb.getJSONArray("tx_preview_hashes")
        assertEquals(2, previewHashes.length(), "AC-2: tx_preview_hashes.size must equal tx_count")
        // DD-3-3-F: each entry is a 64-char lowercase hex SHA-256 digest.
        for (i in 0 until previewHashes.length()) {
            val hash = previewHashes.getString(i)
            assertEquals(64, hash.length, "DD-3-3-F: preview hash $i must be 64 chars")
            assertTrue(
                hash.all { it.isDigit() || it in 'a'..'f' },
                "DD-3-3-F: preview hash $i must be lowercase hex",
            )
        }
    }

    // ---------------- AC-3 ----------------

    @Test
    fun `AC-3 breadcrumb cleared on success before terminal signal`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedConnectedSession()
        seedCacheRecord()

        plugin.mwaSignAndSendTransactions(
            requestId = "sas-ac3",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 5_000L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postSignAndSendCompleted("sas-ac3", any()) }
            }.isSuccess
        }

        // AC-3 ordering invariant: removePendingSubmission MUST be called BEFORE
        // postSignAndSendCompleted fires per DD-3-3-C.
        verifyOrder {
            store.removePendingSubmission("sas-ac3")
            nativeBridge.postSignAndSendCompleted("sas-ac3", any())
        }
        // Breadcrumb is gone after the op terminates.
        assertEquals(null, storedPending["sas-ac3"], "AC-3: breadcrumb must be removed post-success")
    }

    // ---------------- AC-4 ----------------

    @Test
    fun `AC-4a breadcrumb cleared on error WALLET_REJECTED`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("wallet_rejected") },
        )
        seedConnectedSession()
        seedCacheRecord()

        plugin.mwaSignAndSendTransactions(
            requestId = "sas-ac4a",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 5_000L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(any()) }
            }.isSuccess
        }

        verify(exactly = 1) { store.removePendingSubmission("sas-ac4a") }
        verify(exactly = 0) { nativeBridge.postSignAndSendCompleted(any(), any()) }
        assertEquals(null, storedPending["sas-ac4a"], "AC-4a: breadcrumb must be removed post-error")
    }

    @Test
    fun `AC-4b breadcrumb cleared on watchdog timeout`() {
        // T2 fixup (D-3-3-T2-2, Rule 1) — original T1 comment claimed the
        // relaxed mock's signAndSendTransactions "returns the default
        // MwaResult.Success with default field values, but the watchdog fires
        // before the lambda's body completes." MockK actually returns a
        // synthetic MwaResult subclass instantly (no delay), so withTimeoutOrNull
        // resolves to a non-null synthetic before the watchdog's 100ms wakeup.
        // Canonical fix mirroring Story 3-1's `runSigningOp watchdog timeout`
        // test: stub a slow `coAnswers { delay(5_000L); error("unreachable") }`
        // so withTimeoutOrNull MUST cancel the lambda — same behavior the test
        // intent calls for.
        val slowClient = mockk<MwaClient>(relaxed = true)
        io.mockk.coEvery {
            slowClient.signAndSendTransactions(any(), any(), any(), any(), any())
        } coAnswers {
            kotlinx.coroutines.delay(5_000L)
            error("unreachable — withTimeoutOrNull should cancel before this")
        }
        val plugin = buildPlugin(clientFactory = { slowClient })
        seedConnectedSession()
        seedCacheRecord()

        plugin.mwaSignAndSendTransactions(
            requestId = "sas-ac4b",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 100L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaTimeout(any()) }
            }.isSuccess
        }

        verify(exactly = 1) { store.removePendingSubmission("sas-ac4b") }
        assertEquals(null, storedPending["sas-ac4b"], "AC-4b: breadcrumb must be removed post-timeout")
    }

    @Test
    fun `AC-4c breadcrumb cleared on lifecycle cancellation`() {
        // Story 5-3 closure of CR-3-3-B — exercise the production wiring of
        // MwaLifecycleObserver.onDestroy → cleanupBreadcrumb(requestId) →
        // store.removePendingSubmission(requestId). The observer is built via
        // plugin.buildLifecycleObserver() so the plugin's actual method
        // references (::cleanupBreadcrumb + ::buildCancelledLifecycleJson) are
        // the path under test (NOT a test-side replica).

        // Pre-seed a stale breadcrumb under the cancelled requestId; mirrors
        // the AC-2 6-key shape from Story 3-3 DD-3-3-A.
        val breadcrumb = JSONObject().apply {
            put("request_id", "sas-ac4c")
            put("op_type", "sign_and_send")
            put("cluster", "devnet")
            put("identity_uri", identityUri)
            put("started_at_ms", 1_700_000_000_000L)
            put("tx_count", 1)
            val previewArr = org.json.JSONArray()
            previewArr.put("a".repeat(64))
            put("tx_preview_hashes", previewArr)
        }.toString()
        storedPending["sas-ac4c"] = breadcrumb

        // Shared InflightMap so we can pre-register an in-flight slot the
        // observer will see in its snapshot.
        val sharedInflight = InflightMap()
        val plugin = buildPlugin(inflightOverride = sharedInflight)
        assertTrue(sharedInflight.register("sas-ac4c", 1_700_000_000_000L, "sign_and_send"))
        assertEquals(1, sharedInflight.size())

        val observer = plugin.buildLifecycleObserver()
        observer.onDestroy(mockk<LifecycleOwner>(relaxed = true))

        // DD-5-3-3 + DD-5-3-5: observer emits mwa_cancelled_lifecycle for the
        // claimed slot then invokes cleanupBreadcrumb (same call ordering as
        // Story 3-3 AC-3 cleanup-on-success — emit-before-cleanup is invariant).
        val payloadSlot = slot<String>()
        verifyOrder {
            nativeBridge.postMwaCancelledLifecycle(capture(payloadSlot))
            store.removePendingSubmission("sas-ac4c")
        }
        // Code-review fix #5 — payload contents verification: DD-5-3-3 3-key
        // dict via ::buildCancelledLifecycleJson + DD-5-3-4 reason literal.
        // Without these asserts a regression emitting wrong request_id /
        // source_method / reason would still pass the call-ordering verifyOrder.
        val payload = JSONObject(payloadSlot.captured)
        assertEquals("sas-ac4c", payload.getString("request_id"), "AC-4c: request_id from in-flight slot")
        assertEquals(
            "sign_and_send",
            payload.getString("source_method"),
            "AC-4c: source_method from inflightMap",
        )
        assertEquals(
            "activity_destroyed",
            payload.getString("reason"),
            "AC-4c / DD-5-3-4 LOCKED: reason literal",
        )
        assertEquals(
            null,
            storedPending["sas-ac4c"],
            "AC-4c: breadcrumb must be removed post-lifecycle-cancellation",
        )
        // Slot was tryTerminate'd (CAS-removed); inflight map is empty.
        assertEquals(0, sharedInflight.size(), "AC-4c: in-flight slot removed via tryTerminate")
    }

    // ---------------- AC-5 ----------------

    @Test
    fun `AC-5 stale breadcrumb surfaces pending_submission_found on next connect AND clears one-shot`() {
        // Pre-seed a stale breadcrumb under the SAME identity_uri as the upcoming
        // connect — DD-3-3-D identity-scoped scan filter must match.
        val staleBreadcrumb = JSONObject().apply {
            put("request_id", "sas-stale")
            put("op_type", "sign_and_send")
            put("cluster", "devnet")
            put("identity_uri", identityUri)
            put("started_at_ms", 1_699_000_000_000L)
            put("tx_count", 1)
            val previewArr = org.json.JSONArray()
            previewArr.put("a".repeat(64))
            put("tx_preview_hashes", previewArr)
        }.toString()
        storedPending["sas-stale"] = staleBreadcrumb

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        // No seedConnectedSession — the connect flow is what we're testing.
        val identityJson = JSONObject().apply {
            put("name", identityName)
            put("icon_uri", iconUri)
            put("identity_uri", identityUri)
        }.toString()

        val pendingJsonSlot = slot<String>()
        plugin.mwaAuthorize(
            requestId = "req-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postPendingSubmissionFound(capture(pendingJsonSlot)) }
            }.isSuccess
        }

        // DD-3-3-D + AC-5 ordering: postConnectCompleted MUST fire BEFORE
        // postPendingSubmissionFound; removePendingSubmission MUST be called
        // AFTER postPendingSubmissionFound (one-shot).
        verifyOrder {
            nativeBridge.postConnectCompleted("req-connect", any())
            nativeBridge.postPendingSubmissionFound(any())
            store.removePendingSubmission("sas-stale")
        }

        val pending = JSONObject(pendingJsonSlot.captured)
        assertEquals("sas-stale", pending.getString("request_id"), "AC-5: request_id from breadcrumb")
        assertEquals("sign_and_send", pending.getString("op_type"), "AC-5: op_type")
        assertEquals(1_699_000_000_000L, pending.getLong("started_at_ms"), "AC-5: started_at_ms from breadcrumb")
        assertEquals(1, pending.getInt("tx_count"), "AC-5: tx_count from breadcrumb")
        assertEquals(1, pending.getJSONArray("tx_preview_hashes").length(), "AC-5: tx_preview_hashes from breadcrumb")
        assertEquals(
            "check_chain_for_signatures",
            pending.getString("recommendation"),
            "AC-5: recommendation must be the literal \"check_chain_for_signatures\"",
        )
        // DD-3-3-E payload-shape negative guards (cluster + identity_uri are
        // breadcrumb-internal, NOT signaled — see buildPendingSubmissionFoundJson kdoc).
        assertTrue(!pending.has("cluster"), "AC-5 / DD-3-3-E: pending payload must NOT carry cluster")
        assertTrue(!pending.has("identity_uri"), "AC-5 / DD-3-3-E: pending payload must NOT carry identity_uri")

        // One-shot semantics: breadcrumb is removed after the emit.
        assertEquals(null, storedPending["sas-stale"], "AC-5: breadcrumb must be removed post-emit (one-shot)")
    }

    // ---------------- AC-6 ----------------

    @Test
    fun `AC-6 cluster-bleed refuses with NOT_CONNECTED`() {
        val factoryNeverInvoked = mockk<() -> MwaClient>()
        every { factoryNeverInvoked() } answers {
            error("factory should NOT be invoked when cluster-bleed preflight refuses (DD-3-3-A + AC-6)")
        }
        val plugin = buildPlugin(clientFactory = { factoryNeverInvoked() })
        seedConnectedSession(clusterName = "devnet")
        seedCacheRecord(clusterName = "devnet", chainId = "solana:devnet")
        // Swap the active sessionState's cluster AFTER connect, BEFORE sign — the
        // 4-tuple lookup against the cached record will fail because the sessionState's
        // (cluster, chainId, walletPackage, identityUri) no longer matches the cached
        // record's tuple. This is the AC-6 cluster-bleed defensive guard.
        GDExtensionAndroidPlugin.sessionState.setClusterName("mainnet-beta")

        val errSlot = slot<String>()
        plugin.mwaSignAndSendTransactions(
            requestId = "sas-ac6",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 5_000L,
        )

        // DD-3-1-6 inheritance: NOT_CONNECTED preflight is synchronous, no scope.launch.
        verify(exactly = 1) { nativeBridge.postMwaError(capture(errSlot)) }
        val err = JSONObject(errSlot.captured)
        assertEquals("sas-ac6", err.getString("request_id"))
        assertEquals("NOT_CONNECTED", err.getString("code"))
        assertEquals("sign_and_send", err.getString("source_method"))

        verify(exactly = 0) { factoryNeverInvoked() }
        verify(exactly = 0) { nativeBridge.postSignAndSendCompleted(any(), any()) }
    }

    // ---------------- runSigningOp(SIGN_AND_SEND) Failure routing ----------------

    @Test
    fun `runSigningOp SIGN_AND_SEND routes Failure through mwa_error with source_method sign_and_send`() {
        val plugin = buildPlugin(
            clientFactory = { mockk<MwaClient>(relaxed = true) },
        )
        seedConnectedSession()

        // Direct call on runSigningOp — bypasses mwaSignAndSendTransactions.
        // Acts as a regression guard that DD-3-2-5 inheritance
        // (SigningOp.SIGN_AND_SEND → "sign_and_send" sourceMethod mapping) flows
        // correctly through the shared helper's Failure branch.
        val result = runBlocking {
            plugin.runSigningOp<SignAndSendResult>(
                op = SigningOp.SIGN_AND_SEND,
                requestId = "rso-sas-fail",
                timeoutMs = 5_000L,
            ) {
                MwaResult.Failure(MwaError.WalletRejected, null)
            }
        }

        assertTrue(result is MwaResult.Failure, "runSigningOp must propagate Failure")
        val failure = result as MwaResult.Failure
        assertEquals(MwaError.WalletRejected, failure.error)

        val jsonSlot = slot<String>()
        verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("rso-sas-fail", payload.getString("request_id"))
        assertEquals(
            "sign_and_send",
            payload.getString("source_method"),
            "DD-3-2-5 inheritance — SigningOp.SIGN_AND_SEND.sourceMethod must map to \"sign_and_send\"",
        )
    }

    // ---------------- breadcrumb-write StorageCorruptException → reauth_required ----------------

    @Test
    fun `cluster-bleed listAllKeys StorageCorruptException emits reauth_required synchronously and aborts`() {
        // Code-review post-T7 finding #1 (HIGH): the AC-6 cluster-bleed sync
        // preflight calls listAllKeys() which triggers SecureTokenStore's lazy
        // prefs init — that init throws StorageCorruptException on Tink
        // corruption. The original T2 impl let this exception propagate
        // uncaught to the JNI thread, violating DD-3-3-G's fail-closed contract
        // and the terminal-signal invariant per arch §7.3. The fix wraps the
        // sync preflight in a try/catch and inlines the register+CAS+emit
        // sequence (the suspend wrapper is unreachable from this preflight
        // because mwaSignAndSendTransactions is called from the JNI thread
        // without a coroutine scope).
        val factoryNeverInvoked = mockk<() -> MwaClient>()
        every { factoryNeverInvoked() } answers {
            error("factory should NOT be invoked when cluster-bleed lookup hits Tink corruption (DD-3-3-G)")
        }
        // Override the storage mock to throw on listAllKeys (the AC-6
        // preflight call). NO seedCacheRecord — listAllKeys would throw
        // before reaching the filter step.
        every { store.listAllKeys() } throws StorageCorruptException(
            RuntimeException("simulated Tink corruption during cluster-bleed lookup"),
        )

        val plugin = buildPlugin(clientFactory = { factoryNeverInvoked() })
        seedConnectedSession()

        plugin.mwaSignAndSendTransactions(
            requestId = "sas-cluster-bleed-corrupt",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 5_000L,
        )

        // DD-3-3-G fail-closed (sync preflight branch): postReauthRequired
        // must fire SYNCHRONOUSLY (preflight is sync per DD-3-1-6 inheritance
        // + the AC-6 contract; no awaitCondition needed). mwaClientFactory
        // must NOT be invoked. postSignAndSendCompleted / postMwaError /
        // postMwaTimeout / postPendingSubmissionFound must NOT fire.
        verify(exactly = 1) { nativeBridge.postReauthRequired(any()) }
        verify(exactly = 0) { factoryNeverInvoked() }
        verify(exactly = 0) { nativeBridge.postSignAndSendCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postPendingSubmissionFound(any()) }
    }

    @Test
    fun `breadcrumb-write StorageCorruptException emits reauth_required and aborts wallet round-trip`() {
        val factoryNeverInvoked = mockk<() -> MwaClient>()
        every { factoryNeverInvoked() } answers {
            error("factory should NOT be invoked when breadcrumb-write fail-closed wrapper triggers (DD-3-3-G)")
        }
        // Override the storage mock to throw on putPendingSubmission.
        every { store.putPendingSubmission(any(), any()) } throws StorageCorruptException(RuntimeException("simulated Tink corruption"))

        val plugin = buildPlugin(clientFactory = { factoryNeverInvoked() })
        seedConnectedSession()
        seedCacheRecord()

        plugin.mwaSignAndSendTransactions(
            requestId = "sas-corrupt",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 5_000L,
        )

        // DD-3-3-G fail-closed: postReauthRequired must fire; mwaClientFactory must
        // NOT be invoked; postSignAndSendCompleted and postMwaError must NOT fire.
        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postReauthRequired(any()) }
            }.isSuccess
        }
        verify(exactly = 0) { factoryNeverInvoked() }
        verify(exactly = 0) { nativeBridge.postSignAndSendCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
