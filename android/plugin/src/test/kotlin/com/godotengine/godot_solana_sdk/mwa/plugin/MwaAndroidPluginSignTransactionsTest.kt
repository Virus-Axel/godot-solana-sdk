package com.godotengine.godot_solana_sdk.mwa.plugin

import android.net.Uri
import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignResult
import com.godotengine.godot_solana_sdk.mwa.generated.MwaError
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
import kotlinx.coroutines.runBlocking
import org.godotengine.godot.Godot
import org.json.JSONObject
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Assertions.fail
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import plugin.walletadapterandroid.SigningOp
import java.io.File

/**
 * Story 3-2 T1 — `GDExtensionAndroidPlugin.mwaSignTransactions` coverage (TDD red).
 *
 * 7 scenarios:
 *   1. AC-1 happy path — FakeMwaClient `sign_transactions_success.json` →
 *      `signed_transactions.size == 2` (renamed payload key per DD-3-2-3 + D-3-2-1)
 *   2. AC-2 LOC budget — `mwaSignTransactions` body ≤ 20 Kotlin LOC (per shared
 *      [countMethodLines] helper in `LocCountUtil.kt`, DD-3-2-2 + D-3-2-3)
 *   3. AC-3 not-connected preflight — no cached authToken → NOT_CONNECTED within 1
 *      frame; `mwaClientFactory` never invoked (DD-3-1-6 inherited via DD-3-2-5)
 *   4. AC-4 wallet_rejected fixture — `sign_transactions_wallet_rejected.json` →
 *      WALLET_REJECTED (distinct error code from sign_messages's USER_CANCELED;
 *      same emission path through `runSigningOp`'s Failure branch)
 *   5. `runSigningOp` routes Failure through mwa_error with
 *      `source_method=sign_transactions` (DD-3-2-5 inheritance evidence — confirms
 *      `SigningOp.SIGN_TRANSACTIONS.sourceMethod == "sign_transactions"` flows
 *      correctly through the helper's Failure branch)
 *   6. Duplicate requestId emits PROTOCOL_ERROR on register (carries from 2-1/2-2/2-3/3-1)
 *   7. Late-result-after-terminate is dropped (carries from 2-1/2-2/3-1)
 *
 * **History:** T1 landed this file with stub bodies in
 * [GDExtensionAndroidPlugin.mwaSignTransactions] +
 * [GDExtensionAndroidPlugin.handleSignTransactionsSuccess], producing a TDD-red
 * baseline of 5 failing + 2 passing (AC-2 LOC budget passed at 1-LOC stub;
 * runSigningOp direct test bypassed the stub). T2 (commit 54798086) replaced
 * the stub bodies with the real impl per DD-3-2-1..5; all 7 turn GREEN.
 *
 * Plugin is built via the `@VisibleForTesting` ctor with injected collaborators
 * (identical to [MwaAndroidPluginSignMessagesTest] / [MwaAndroidPluginReauthorizeTest]):
 *   - recording [NativeBridge] mock
 *   - map-backed fake [SecureTokenStore] via `storeProvider`
 *   - FakeMwaClient or relaxed-mock test double via `mwaClientFactory`
 *     (relaxed-mock is needed for runSigningOp direct tests #5 + the AC-3 preflight
 *     branch's "factory must NOT invoke" assertion — see C-3-2-B note in story §168)
 *   - no-op `senderProvider` (signing path doesn't launch a wallet Activity)
 */
class MwaAndroidPluginSignTransactionsTest {

    private val fixturesDir: File by lazy {
        val prop = System.getProperty("mwa.fixtures.dir")
            ?: error("system property 'mwa.fixtures.dir' must be set (wired in build.gradle.kts)")
        File(prop).also { require(it.isDirectory) { "fixtures dir missing: $it" } }
    }

    /** AC-2 source-root system property (Story 3-1 T1 build wiring — `mwa.plugin.source.root`). */
    private val pluginSourceRoot: File by lazy {
        val prop = System.getProperty("mwa.plugin.source.root")
            ?: error("system property 'mwa.plugin.source.root' must be set (wired in build.gradle.kts)")
        File(prop).also { require(it.isDirectory) { "plugin source root missing: $it" } }
    }

    private lateinit var nativeBridge: NativeBridge
    private lateinit var senderMock: ActivityResultSender
    private lateinit var storedRecords: MutableMap<CacheKey, CacheRecord>
    private lateinit var store: SecureTokenStore

    private val identityUri = "https://game.example"
    private val identityName = "Game"
    private val iconUri = "https://x/i"
    private val walletPackage = "com.wallet.app"
    private val walletLabel = "TestWallet"
    private val walletIconUri = "https://wallet.example/icon.png"
    private val publicKey = "AbcDefGhiJkl1234567890AbcDefGhiJkl1234567890"

    // FakeMwaClient `sign_transactions_success.json` fixture's `expected_input.auth_token`
    // (Story 1-6 D-2 opaque-string contract). Pre-seed with byte-identical value so
    // `is_connected()` is true AND the FakeMwaClient happy path's auth-token comparison
    // passes if the impl ever adds one (it doesn't currently, but matching the convention
    // is cheaper than diverging). Mirrors the SignMessages test convention with the
    // `-signtxn` suffix to identify which fixture's bytes these came from.
    private val authTokenBytes = "dGVzdC1hdXRoLXRva2VuLXNpZ250eA==".toByteArray(Charsets.UTF_8)
    private val salt = ByteArray(32) { 0x01 }

    @BeforeEach
    fun setup() {
        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0

        // ConnectionIdentity construction goes through Uri.parse(...) per DD-3-1-8
        // (inherited) — mock to a relaxed mock so JVM-side construction succeeds.
        mockkStatic(Uri::class)
        every { Uri.parse(any()) } returns mockk(relaxed = true)

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
     * Seeds MwaSessionState with a connected-state snapshot per DD-3-1-8 (inherited
     * via DD-3-2-5) — same field set [mwaSignTransactions] reads via `getAuthToken /
     * getConnectedKey / getClusterName / getIdentityUri / getIconUri / getIdentityName`.
     * Setter for identity is the COMBINED `setIdentity(identityUri, iconUri,
     * identityName)` form (D-3-1-9 inheritance).
     */
    private fun seedConnectedSession() {
        GDExtensionAndroidPlugin.sessionState.apply {
            setIdentity(identityUri, iconUri, identityName)
            setCluster(2)
            setClusterName("devnet")
            setAuthToken(SecretString(authTokenBytes.copyOf()))
            setAuthTokenFingerprint("00000000")
            setWalletIconUri(walletIconUri)
            setPublicKeyBase58(publicKey)
            setWalletLabel(walletLabel)
            setKey(ByteArray(32) { 0x42 }) // 32 raw bytes per C-3-1-F fix; getter is getConnectedKey()
        }
    }

    /** Seeds the fake store with a CacheRecord for the canonical 4-tuple. */
    private fun seedCacheRecord(): Pair<CacheKey, CacheRecord> {
        val key = CacheKey("devnet", "solana:devnet", walletPackage, identityUri)
        val record = CacheRecord(
            schema = 1,
            authToken = SecretString(authTokenBytes.copyOf()),
            walletPackage = walletPackage,
            walletLabel = walletLabel,
            walletIconUri = walletIconUri,
            publicKey = publicKey,
            cluster = "devnet",
            chainId = "solana:devnet",
            identityUri = identityUri,
            issuedAtMs = 1_700_000_000_000L,
            lastUsedAtMs = 1_700_000_000_000L,
        )
        store.putToken(key, record)
        return key to record
    }

    // ---------------- AC-1 ----------------

    @Test
    fun `AC-1 happy path emits sign_transactions_completed with signed_transactions`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedConnectedSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaSignTransactions(
            requestId = "st-ac1",
            transactions = listOf(byteArrayOf(0x01, 0x02), byteArrayOf(0x03, 0x04)),
            timeoutMs = 5_000L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postSignTransactionsCompleted("st-ac1", capture(jsonSlot)) }
            }.isSuccess
        }

        val payload = JSONObject(jsonSlot.captured)
        assertEquals("st-ac1", payload.getString("request_id"))
        assertTrue(
            payload.has("signed_transactions"),
            "payload must have signed_transactions (DD-3-2-3 + D-3-2-1 — payloadKey rename)",
        )
        val signedTransactions = payload.getJSONArray("signed_transactions")
        assertEquals(
            2,
            signedTransactions.length(),
            "signed_transactions.size must equal input transactions.size (fixture has 2 tx)",
        )

        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }

    // ---------------- AC-2 ----------------

    @Test
    fun `AC-2 mwaSignTransactions body is at most 20 LOC`() {
        // Read source via build.gradle.kts-wired system property `mwa.plugin.source.root`.
        // System property points at `android/plugin/src/main`; navigate to the .kt file.
        val pluginSourceFile = File(
            pluginSourceRoot,
            "java/plugin/walletadapterandroid/GDExtensionAndroidPlugin.kt",
        )
        require(pluginSourceFile.isFile) {
            "plugin source file missing: ${pluginSourceFile.absolutePath} — " +
                "verify build.gradle.kts wiring of mwa.plugin.source.root"
        }
        val source = pluginSourceFile.readText()

        // Locate `fun mwaSignTransactions(`. Method-level indent in the existing class is
        // 4 spaces; the regex anchors on that. The optional `@UsedByGodot` line precedes
        // the signature opening line but does NOT need to be matched by the anchor —
        // [countMethodLines] starts AFTER the signature opening line (`{`) regardless.
        val sigPattern = Regex("""^    fun mwaSignTransactions\(""", RegexOption.MULTILINE)
        val match = sigPattern.find(source)
            ?: fail("mwaSignTransactions signature not found via regex `^    fun mwaSignTransactions\\(`")
        // [countMethodLines] is the shared top-level `internal fun` from `LocCountUtil.kt`
        // (Story 3-2 T1 PRE-STEP, D-3-2-3 Rule 1) — DD-3-2-2 + DD-3-1-9 counter rule.
        val loc = countMethodLines(source, match.range.first)
        assertTrue(
            loc <= 20,
            "AC-2: mwaSignTransactions body must be ≤ 20 non-blank non-comment lines per DD-3-1-9 counter rule (inherited); actual=$loc",
        )
    }

    // ---------------- AC-3 ----------------

    @Test
    fun `AC-3 not connected preflight emits NOT_CONNECTED within 1 frame`() {
        // No seedConnectedSession() — sessionState.getAuthToken() returns null.
        val clientFactoryMock = mockk<() -> MwaClient>()
        every { clientFactoryMock() } answers {
            error("factory should NOT be invoked on preflight branch (DD-3-1-6 inherited)")
        }
        val plugin = buildPlugin(clientFactory = { clientFactoryMock() })

        val jsonSlot = slot<String>()
        plugin.mwaSignTransactions(
            requestId = "st-ac3",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 5_000L,
        )

        // DD-3-1-6: preflight is synchronous, no scope.launch — no awaitCondition needed.
        verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }

        val err = JSONObject(jsonSlot.captured)
        assertEquals("st-ac3", err.getString("request_id"))
        assertEquals("NOT_CONNECTED", err.getString("code"))
        assertEquals("sign_transactions", err.getString("source_method"))
        assertEquals("connect", err.getString("retry_hint"))

        // mwaClientFactory NEVER invoked — preflight short-circuits before any client work.
        verify(exactly = 0) { clientFactoryMock() }

        // No other terminal signal raced with the preflight error.
        verify(exactly = 0) { nativeBridge.postSignTransactionsCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }

    // ---------------- AC-4 ----------------

    @Test
    fun `AC-4 wallet_rejected fixture emits WALLET_REJECTED`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("wallet_rejected") },
        )
        seedConnectedSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaSignTransactions(
            requestId = "st-ac4",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 5_000L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
            }.isSuccess
        }

        val err = JSONObject(jsonSlot.captured)
        assertEquals("st-ac4", err.getString("request_id"))
        assertEquals("WALLET_REJECTED", err.getString("code"))
        assertEquals("sign_transactions", err.getString("source_method"))

        verify(exactly = 0) { nativeBridge.postSignTransactionsCompleted(any(), any()) }
    }

    // ---------------- runSigningOp(SIGN_TRANSACTIONS) Failure routing ----------------

    @Test
    fun `runSigningOp SIGN_TRANSACTIONS routes Failure through mwa_error with source_method sign_transactions`() {
        // C-3-2-B fix (mirrors Story 3-1 C-3-1-V): runSigningOp calls mwaClientFactory()
        // BEFORE invoking the block. The default throwing factory in buildPlugin would
        // abort the helper before our lambda's MwaResult.Failure return is ever reached.
        // A relaxed mock satisfies the call site without driving any actual wallet behavior.
        val plugin = buildPlugin(
            clientFactory = { mockk<MwaClient>(relaxed = true) },
        )
        seedConnectedSession()

        // Direct call on runSigningOp — bypasses mwaSignTransactions entirely.
        // Acts as a regression guard that DD-3-2-5 inheritance
        // (SigningOp.SIGN_TRANSACTIONS → "sign_transactions" sourceMethod
        // mapping) flows correctly through the shared helper's Failure branch.
        val result = runBlocking {
            plugin.runSigningOp<SignResult>(
                op = SigningOp.SIGN_TRANSACTIONS,
                requestId = "rso-st-fail",
                timeoutMs = 5_000L,
            ) {
                MwaResult.Failure(MwaError.WalletRejected, null)
            }
        }

        // C-3-1-W single-wrap shape: helper returns MwaResult<SignResult> directly.
        assertTrue(result is MwaResult.Failure, "runSigningOp must propagate Failure")
        val failure = result as MwaResult.Failure
        assertEquals(MwaError.WalletRejected, failure.error, "Failure carries MwaError.WalletRejected")

        // Side effect: postMwaError fires exactly once with source_method=sign_transactions.
        val jsonSlot = slot<String>()
        verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("rso-st-fail", payload.getString("request_id"))
        assertEquals(
            "sign_transactions",
            payload.getString("source_method"),
            "DD-3-2-5 inheritance — SigningOp.SIGN_TRANSACTIONS.sourceMethod must map to \"sign_transactions\"",
        )
    }

    // ---------------- duplicate requestId + late-result ----------------

    @Test
    fun `duplicate requestId on sign_transactions emits PROTOCOL_ERROR`() {
        val inflight = InflightMap()
        // Pre-register the requestId — simulates a caller that re-uses the same id for a retry.
        inflight.register("st-dup", 1_700_000_000_000L)

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            inflightOverride = inflight,
        )
        seedConnectedSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaSignTransactions(
            requestId = "st-dup",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 5_000L,
        )

        // Duplicate-id path SHOULD emit PROTOCOL_ERROR. Per DD-3-1-6 (inherited) the
        // preflight passes (sessionState is connected); the PROTOCOL_ERROR comes from
        // runSigningOp's InflightMap.register failure branch.
        awaitCondition(1_000L) {
            runCatching {
                verify(atLeast = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
            }.isSuccess
        }

        val err = JSONObject(jsonSlot.captured)
        assertEquals("st-dup", err.getString("request_id"))
        assertEquals("PROTOCOL_ERROR", err.getString("code"))
        assertEquals("sign_transactions", err.getString("source_method"))
        assertEquals("duplicate_request_id", err.getString("cause"))

        // Original Success signal MUST NOT fire (FakeMwaClient never invoked).
        verify(exactly = 0) { nativeBridge.postSignTransactionsCompleted(any(), any()) }
    }

    @Test
    fun `late result after terminate is dropped`() {
        // Mock InflightMap so tryTerminate returns false — simulates a coroutine that
        // completes AFTER the terminal-signal slot was already taken (e.g., by a watchdog
        // or a competing emit). The success path MUST detect the late_result and drop
        // the signal (carries from 2-1/2-2/3-1 terminal-signal invariant per arch §7.3).
        val inflight = mockk<InflightMap>(relaxed = true)
        every { inflight.register(any(), any(), any()) } returns true
        every { inflight.tryTerminate(any()) } returns false

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            inflightOverride = inflight,
        )
        seedConnectedSession()
        seedCacheRecord()

        plugin.mwaSignTransactions(
            requestId = "st-late",
            transactions = listOf(byteArrayOf(0x01)),
            timeoutMs = 5_000L,
        )

        // Wait briefly to let the FakeMwaClient coroutine finish (it would normally
        // emit a Success signal, but tryTerminate=false should drop it).
        Thread.sleep(500)

        // No terminal signal of any kind — Success was dropped due to tryTerminate=false.
        verify(exactly = 0) { nativeBridge.postSignTransactionsCompleted(any(), any()) }
        // The PROTOCOL_ERROR / NOT_CONNECTED / TIMEOUT paths also must NOT fire on
        // a happy-path that lost the CAS race — the impl must observe the failed
        // CAS and silently drop, NOT emit a different terminal signal.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
