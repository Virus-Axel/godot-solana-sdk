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
import kotlinx.coroutines.delay
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
 * Story 3-1 T1 — `GDExtensionAndroidPlugin.mwaSignMessages` + `runSigningOp`
 * coverage (TDD red).
 *
 * 8 scenarios:
 *   1. AC-1 LOC budget — `mwaSignMessages` body ≤ 20 Kotlin LOC (per DD-3-1-9 counter rule)
 *   2. AC-2 happy E2E — FakeMwaClient `sign_messages_success.json` → `signed_payloads.size == 2`
 *   3. AC-3 not-connected preflight — no cached authToken → NOT_CONNECTED within 1 frame; `mwaClientFactory` never invoked (DD-3-1-6)
 *   4. AC-4 user-canceled fixture — `sign_messages_user_canceled.json` → USER_CANCELED with `recoverable=true`
 *   5. `runSigningOp` passes through `MwaResult.Success` unchanged (DD-3-1-2 + DD-3-1-3 evidence)
 *   6. `runSigningOp` watchdog timeout returns `Failure(MwaError.Timeout)` + emits `mwa_timeout` (DD-3-1-3)
 *   7. Duplicate requestId emits PROTOCOL_ERROR on register (carries from 2-1/2-2/2-3)
 *   8. Late-result-after-terminate is dropped (carries from 2-1/2-2)
 *
 * As of Story 3-1 T2, all 8 tests pass against the real impl. Pre-T2 (TDD-red
 * baseline) had 7 tests failing at runtime against stub bodies that threw
 * `kotlin.NotImplementedError`; T2 replaced the stubs with the real impl per
 * DD-3-1-1..11. Test #1 (AC-1 LOC budget) was the only test that passed at T1
 * (stub body was 1 LOC under the ≤20 budget); it now enforces the budget against
 * the real implementation (~13 LOC body in `mwaSignMessages` per DD-3-1-9
 * counter rule).
 *
 * Plugin is built via the `@VisibleForTesting` ctor with injected collaborators
 * (identical to [MwaAndroidPluginReauthorizeTest]):
 *   - recording [NativeBridge] mock
 *   - map-backed fake [SecureTokenStore] via `storeProvider`
 *   - FakeMwaClient or slow-client test double via `mwaClientFactory`
 *   - no-op `senderProvider` (signing path doesn't launch a wallet Activity)
 */
class MwaAndroidPluginSignMessagesTest {

    private val fixturesDir: File by lazy {
        val prop = System.getProperty("mwa.fixtures.dir")
            ?: error("system property 'mwa.fixtures.dir' must be set (wired in build.gradle.kts)")
        File(prop).also { require(it.isDirectory) { "fixtures dir missing: $it" } }
    }

    /** AC-1 source-root system property (Story 3-1 T1 build wiring — `mwa.plugin.source.root`). */
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

    // FakeMwaClient `sign_messages_success.json` fixture's `expected_input.auth_token`
    // is `dGVzdC1hdXRoLXRva2VuLXNpZ25tc2c=` (UTF-8 bytes; Story 1-6 D-2 opaque-string
    // contract). Pre-seed with byte-identical value so `is_connected()` is true AND
    // the FakeMwaClient happy path's auth-token comparison passes if the impl ever
    // adds one (it doesn't currently, but matching the convention is cheaper than
    // diverging).
    private val authTokenBytes = "dGVzdC1hdXRoLXRva2VuLXNpZ25tc2c=".toByteArray(Charsets.UTF_8)
    private val salt = ByteArray(32) { 0x01 }

    @BeforeEach
    fun setup() {
        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0

        // ConnectionIdentity construction goes through Uri.parse(...) per DD-3-1-8 —
        // mock to a relaxed mock so JVM-side construction succeeds.
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
     * Seeds MwaSessionState with a connected-state snapshot per DD-3-1-8 — same
     * field set [mwaSignMessages] reads via `getAuthToken / getConnectedKey /
     * getClusterName / getIdentityUri / getIconUri / getIdentityName`. Note the
     * setter for identity is the COMBINED `setIdentity(identityUri, iconUri,
     * identityName)` (MwaSessionState.kt:136), NOT three separate setters — fix
     * tracked as D-3-1-9 from the Pre-Implementation Audit.
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
    fun `AC-1 mwaSignMessages body is at most 20 LOC`() {
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

        // Locate `fun mwaSignMessages(`. Method-level indent in the existing class is
        // 4 spaces; the regex anchors on that. Optional `@UsedByGodot` annotation line
        // immediately precedes the signature opening line.
        val sigPattern = Regex("""^    fun mwaSignMessages\(""", RegexOption.MULTILINE)
        val match = sigPattern.find(source)
            ?: fail("mwaSignMessages signature not found via regex `^    fun mwaSignMessages\\(`")
        val loc = countMethodLines(source, match.range.first)
        assertTrue(
            loc <= 20,
            "AC-1: mwaSignMessages body must be ≤ 20 non-blank non-comment lines per DD-3-1-9 counter rule; actual=$loc",
        )
    }

    /**
     * LOC counter per DD-3-1-9:
     * - Starts AFTER the signature opening line (excluding signature even if multi-line).
     * - Ends BEFORE the matching closing brace at the method's indentation level.
     * - Excludes blank lines, line-comments (`//`), and block-comment lines (`*` continuations).
     * - Excludes kdoc above the signature (already excluded by start-after-signature).
     */
    private fun countMethodLines(source: String, sigStartIndex: Int): Int {
        // Find end of signature line (first '{' or end of opening line — methods open
        // with `... timeoutMs: Long) {` in this codebase's style).
        var idx = sigStartIndex
        while (idx < source.length && source[idx] != '{') idx++
        require(idx < source.length) { "no opening brace found after signature" }
        // Skip past `{` and following newline.
        idx++
        while (idx < source.length && source[idx] != '\n') idx++
        idx++

        // Walk lines until we hit the closing brace at the method's 4-space indent
        // (line that starts with `    }` and nothing else — accounting for trailing newline).
        val methodIndent = "    "
        var loc = 0
        while (idx < source.length) {
            val lineEnd = source.indexOf('\n', idx).let { if (it < 0) source.length else it }
            val line = source.substring(idx, lineEnd)
            // Match the closing brace: exactly the method indent + `}` + optional trailing whitespace.
            if (line == "$methodIndent}" || line == "$methodIndent} ") break
            val trimmed = line.trim()
            val isBlank = trimmed.isEmpty()
            val isLineComment = trimmed.startsWith("//")
            val isBlockComment = trimmed.startsWith("/*") || trimmed.startsWith("*")
            if (!isBlank && !isLineComment && !isBlockComment) loc++
            idx = lineEnd + 1
        }
        return loc
    }

    // ---------------- AC-2 ----------------

    @Test
    fun `AC-2 happy path emits sign_messages_completed with signed_payloads`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        seedConnectedSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaSignMessages(
            requestId = "sm-ac2",
            messages = listOf("hello".toByteArray(), "world".toByteArray()),
            timeoutMs = 5_000L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postSignMessagesCompleted("sm-ac2", capture(jsonSlot)) }
            }.isSuccess
        }

        val payload = JSONObject(jsonSlot.captured)
        assertEquals("sm-ac2", payload.getString("request_id"))
        assertTrue(payload.has("signed_payloads"), "payload must have signed_payloads")
        val signedPayloads = payload.getJSONArray("signed_payloads")
        assertEquals(2, signedPayloads.length(), "signed_payloads.size must equal input messages.size")

        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }

    // ---------------- AC-3 ----------------

    @Test
    fun `AC-3 not connected preflight emits NOT_CONNECTED within 1 frame`() {
        // No seedConnectedSession() — sessionState.getAuthToken() returns null.
        val clientFactoryMock = mockk<() -> MwaClient>()
        every { clientFactoryMock() } answers { error("factory should NOT be invoked on preflight branch (DD-3-1-6)") }
        val plugin = buildPlugin(clientFactory = { clientFactoryMock() })

        val jsonSlot = slot<String>()
        plugin.mwaSignMessages(
            requestId = "sm-ac3",
            messages = listOf("foo".toByteArray()),
            timeoutMs = 5_000L,
        )

        // DD-3-1-6: preflight is synchronous, no scope.launch — no awaitCondition needed.
        verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }

        val err = JSONObject(jsonSlot.captured)
        assertEquals("sm-ac3", err.getString("request_id"))
        assertEquals("NOT_CONNECTED", err.getString("code"))
        assertEquals("sign_messages", err.getString("source_method"))
        assertEquals("connect", err.getString("retry_hint"))

        // mwaClientFactory NEVER invoked — preflight short-circuits before any client work.
        verify(exactly = 0) { clientFactoryMock() }

        // No other terminal signal raced with the preflight error.
        verify(exactly = 0) { nativeBridge.postSignMessagesCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }

    // ---------------- AC-4 ----------------

    @Test
    fun `AC-4 user_canceled fixture emits USER_CANCELED with recoverable=true`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("user_canceled") },
        )
        seedConnectedSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaSignMessages(
            requestId = "sm-ac4",
            messages = listOf("foo".toByteArray()),
            timeoutMs = 5_000L,
        )

        awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
            }.isSuccess
        }

        val err = JSONObject(jsonSlot.captured)
        assertEquals("sm-ac4", err.getString("request_id"))
        assertEquals("USER_CANCELED", err.getString("code"))
        assertEquals("sign_messages", err.getString("source_method"))
        assertTrue(err.getBoolean("recoverable"), "USER_CANCELED is recoverable per ErrorTaxonomy")

        verify(exactly = 0) { nativeBridge.postSignMessagesCompleted(any(), any()) }
    }

    // ---------------- runSigningOp direct tests (#5, #6) ----------------

    @Test
    fun `runSigningOp passes through MwaResult Success unchanged`() {
        // DD-3-1-2 + DD-3-1-3: helper has block receiver = MwaClient AND timeoutMs param.
        // Direct call (helper is internal + @VisibleForTesting per T1 stub).
        //
        // C-3-1-V fix: provide a non-crashing MwaClient factory. The default factory in
        // buildPlugin throws "unexpected mwaClientFactory invocation" — but per the v2
        // helper pseudocode, mwaClientFactory() is called BEFORE the watchdog wraps the
        // block, so the throw aborts the helper before our lambda ever runs. A relaxed
        // mock satisfies the call site without driving any actual wallet behavior.
        val plugin = buildPlugin(
            clientFactory = { mockk<MwaClient>(relaxed = true) },
        )
        seedConnectedSession()

        // Lambda body returns a fake MwaResult.Success directly (no FakeMwaClient invocation).
        // The block receiver is MwaClient (DD-3-1-2 LOCK); the compile-time `val _: MwaClient = this`
        // assertion below makes that lock testable at the type-check level (C-3-1-CC polish).
        val expectedSignResult = SignResult(listOf(byteArrayOf(0x01, 0x02, 0x03)))
        val result = runBlocking {
            plugin.runSigningOp<SignResult>(
                op = SigningOp.SIGN_MESSAGES,
                requestId = "rso-1",
                timeoutMs = 5_000L,
            ) {
                // C-3-1-CC: compile-time receiver-type lock — fails to compile if DD-3-1-2 ever drifts.
                @Suppress("UNUSED_VARIABLE")
                val receiverIsMwaClient: MwaClient = this
                MwaResult.Success(expectedSignResult, null)
            }
        }

        // C-3-1-W single-wrap shape (Revision 2): helper returns MwaResult<X> directly,
        // NOT MwaResult<MwaResult<X>>. The lambda's `MwaResult.Success` is forwarded;
        // callers do NOT need to unwrap an outer/inner pair.
        assertTrue(result is MwaResult.Success, "runSigningOp must return Success when block returns Success")
        @Suppress("UNCHECKED_CAST")
        val success = result as MwaResult.Success<SignResult>
        assertEquals(expectedSignResult, success.value, "SignResult value preserved byte-for-byte")
    }

    @Test
    fun `runSigningOp watchdog timeout returns Failure with Timeout`() {
        // DD-3-1-3: timeoutMs=100L → effectiveWatchdog clamps → withTimeoutOrNull cancels
        // a slow lambda (delay(5_000)). Wallclock duration ~100-200ms.
        //
        // C-3-1-V fix: same non-crashing factory as test #5 — the helper invokes
        // mwaClientFactory() before delegating to the block, so the default throwing
        // factory would abort the helper before withTimeoutOrNull ever wraps the lambda.
        val plugin = buildPlugin(
            clientFactory = { mockk<MwaClient>(relaxed = true) },
        )
        seedConnectedSession()

        val result = runBlocking {
            plugin.runSigningOp<SignResult>(
                op = SigningOp.SIGN_MESSAGES,
                requestId = "rso-to",
                timeoutMs = 100L,
            ) {
                delay(5_000L)
                error("unreachable — withTimeoutOrNull should cancel before this")
            }
        }

        // Single-wrap shape (C-3-1-W): helper returns MwaResult<SignResult> directly.
        assertTrue(result is MwaResult.Failure, "watchdog timeout must yield Failure")
        val failure = result as MwaResult.Failure
        assertEquals(MwaError.Timeout, failure.error, "Failure carries MwaError.Timeout")

        // Side effect: emitTimeoutSign should have fired with source_method=sign_messages.
        val jsonSlot = slot<String>()
        verify(exactly = 1) { nativeBridge.postMwaTimeout(capture(jsonSlot)) }
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("rso-to", payload.getString("request_id"))
        assertEquals("sign_messages", payload.getString("source_method"))
    }

    // ---------------- duplicate requestId + late-result ----------------

    @Test
    fun `duplicate requestId emits PROTOCOL_ERROR on register`() {
        val inflight = InflightMap()
        // Pre-register the requestId — simulates a caller that re-uses the same id for a retry.
        inflight.register("sm-dup", 1_700_000_000_000L)

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            inflightOverride = inflight,
        )
        seedConnectedSession()
        seedCacheRecord()

        val jsonSlot = slot<String>()
        plugin.mwaSignMessages(
            requestId = "sm-dup",
            messages = listOf("foo".toByteArray()),
            timeoutMs = 5_000L,
        )

        // Duplicate-id path SHOULD emit PROTOCOL_ERROR. Per DD-3-1-6 the preflight passes
        // (sessionState is connected); the PROTOCOL_ERROR comes from runSigningOp's
        // InflightMap.register failure branch.
        awaitCondition(1_000L) {
            runCatching {
                verify(atLeast = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
            }.isSuccess
        }

        val err = JSONObject(jsonSlot.captured)
        assertEquals("sm-dup", err.getString("request_id"))
        assertEquals("PROTOCOL_ERROR", err.getString("code"))
        assertEquals("sign_messages", err.getString("source_method"))
        assertEquals("duplicate_request_id", err.getString("cause"))

        // Original Success signal MUST NOT fire (FakeMwaClient never invoked).
        verify(exactly = 0) { nativeBridge.postSignMessagesCompleted(any(), any()) }
    }

    @Test
    fun `late result after terminate is dropped`() {
        // Mock InflightMap so tryTerminate returns false — simulates a coroutine that
        // completes AFTER the terminal-signal slot was already taken (e.g., by a watchdog
        // or a competing emit). The success path MUST detect the late_result and drop
        // the signal (carries from 2-1/2-2 terminal-signal invariant).
        val inflight = mockk<InflightMap>(relaxed = true)
        every { inflight.register(any(), any()) } returns true
        every { inflight.tryTerminate(any()) } returns false

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            inflightOverride = inflight,
        )
        seedConnectedSession()
        seedCacheRecord()

        plugin.mwaSignMessages(
            requestId = "sm-late",
            messages = listOf("foo".toByteArray()),
            timeoutMs = 5_000L,
        )

        // Wait briefly to let the FakeMwaClient coroutine finish (it would normally
        // emit a Success signal, but tryTerminate=false should drop it).
        Thread.sleep(500)

        // No terminal signal of any kind — Success was dropped due to tryTerminate=false.
        verify(exactly = 0) { nativeBridge.postSignMessagesCompleted(any(), any()) }
        // The PROTOCOL_ERROR / NOT_CONNECTED / TIMEOUT paths also must NOT fire on
        // a happy-path that lost the CAS race — the impl must observe the failed
        // CAS and silently drop, NOT emit a different terminal signal.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
