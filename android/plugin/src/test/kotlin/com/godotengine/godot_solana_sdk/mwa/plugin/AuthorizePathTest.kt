package com.godotengine.godot_solana_sdk.mwa.plugin

import android.net.Uri
import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.AuthResult
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
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
import org.godotengine.godot.Godot
import org.json.JSONObject
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 2-1 T4 — `GDExtensionAndroidPlugin.mwaAuthorize` coverage.
 *
 * 5 scenarios:
 *   1. happy path (FakeMwaClient + authorize_success.json)
 *   2. user-canceled (authorize_user_canceled.json)
 *   3. protocol-error (authorize_protocol_error.json)
 *   4. watchdog timeout (slow-client test double, short effective watchdog)
 *   5. late result dropped (mock InflightMap forces tryTerminate → false)
 *
 * Plugin is constructed via the `@VisibleForTesting` ctor overload with
 * injected collaborators — no Godot runtime / no Android Keystore / no JNI.
 */
class AuthorizePathTest {

    private val fixturesDir: File by lazy {
        val prop = System.getProperty("mwa.fixtures.dir")
            ?: error("system property 'mwa.fixtures.dir' must be set (wired in build.gradle.kts)")
        File(prop).also { require(it.isDirectory) { "fixtures dir missing: $it" } }
    }

    private lateinit var nativeBridge: NativeBridge
    private lateinit var senderMock: ActivityResultSender

    private val identityJson = JSONObject().apply {
        put("name", "Test Game")
        put("icon_uri", "https://example.com/icon.png")
        put("identity_uri", "https://example.com/godot-mwa-sample")
    }.toString()

    @BeforeEach
    fun setup() {
        // GDExtensionAndroidPlugin's companion `init` block calls android.util.Log —
        // mock before the first companion touch (matches Story 1-2 SdkLogTest pattern).
        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0

        // `parseIdentity` constructs `ConnectionIdentity` via `Uri.parse(...)` —
        // also Android-framework-only. Mock to a relaxed mock so ConnectionIdentity
        // construction succeeds on JVM.
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
        clientFactory: () -> MwaClient,
        storeOverride: SecureTokenStore = mockk(relaxed = true) {
            io.mockk.every { getOrCreatePerInstallSalt() } returns ByteArray(32) { 0x01 }
            io.mockk.every { putToken(any(), any()) } returns Unit
        },
        inflightOverride: InflightMap = InflightMap(),
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
            // Tests run without an Android main looper — use Unconfined so withContext
            // runs in place instead of dispatching to a non-existent Main dispatcher.
            mainDispatcher = Dispatchers.Unconfined,
        )
    }

    /** Wait up to [timeoutMs] for [check] to return true — polls every 5ms. */
    private fun awaitCondition(timeoutMs: Long, check: () -> Boolean) {
        val deadline = System.currentTimeMillis() + timeoutMs
        while (System.currentTimeMillis() < deadline) {
            if (check()) return
            Thread.sleep(5)
        }
        error("condition not met within ${timeoutMs}ms")
    }

    @Test
    fun `happy path - authorize_success_json results in postConnectCompleted with fingerprint`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )
        val jsonSlot = slot<String>()

        plugin.mwaAuthorize("req-happy", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postConnectCompleted("req-happy", capture(jsonSlot)) } }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-happy", payload.getString("request_id"))
        assertEquals(8, payload.getString("auth_token_fingerprint").length)
        assertEquals("devnet", payload.getString("cluster"))
        assertTrue(payload.getString("public_key").isNotEmpty())
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }

    @Test
    fun `AC-4 no wallet - authorize_no_wallet_json emits mwa_error with NO_MWA_WALLET_INSTALLED`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("no_wallet") },
        )
        val jsonSlot = slot<String>()

        plugin.mwaAuthorize("req-nowallet", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) } }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-nowallet", payload.getString("request_id"))
        assertEquals("NO_MWA_WALLET_INSTALLED", payload.getString("code"))
        assertEquals("kotlin", payload.getString("layer"))
        assertEquals("connect", payload.getString("source_method"))
        // AC-4 shape: 10-key A-14 envelope present regardless of code.
        assertTrue(payload.has("message"))
        assertTrue(payload.has("user_message"))
        assertTrue(payload.has("developer_details"))
        assertTrue(payload.has("recoverable"))
        assertTrue(payload.has("retry_hint"))
        assertTrue(payload.has("cause"))
        // Terminal-signal invariant — no other terminal signal alongside.
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }

    @Test
    fun `user canceled - authorize_user_canceled_json emits mwa_error with USER_CANCELED`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("user_canceled") },
        )
        val jsonSlot = slot<String>()

        plugin.mwaAuthorize("req-cancel", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) } }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-cancel", payload.getString("request_id"))
        assertEquals("USER_CANCELED", payload.getString("code"))
        assertTrue(payload.getBoolean("recoverable"))
        assertEquals("retry_now", payload.getString("retry_hint"))
        assertEquals("kotlin", payload.getString("layer"))
        assertEquals("connect", payload.getString("source_method"))
        // 10-key A-14 shape
        assertTrue(payload.has("message"))
        assertTrue(payload.has("user_message"))
        assertTrue(payload.has("developer_details"))
        assertTrue(payload.has("cause"))
    }

    @Test
    fun `protocol error - authorize_protocol_error_json emits mwa_error with PROTOCOL_ERROR`() {
        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("protocol_error") },
        )
        val jsonSlot = slot<String>()

        plugin.mwaAuthorize("req-proto", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        awaitCondition(2_000L) {
            runCatching { verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) } }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-proto", payload.getString("request_id"))
        assertEquals("PROTOCOL_ERROR", payload.getString("code"))
    }

    @Test
    fun `watchdog timeout - slow client triggers postMwaTimeout at effective watchdog`() {
        val slowClient = object : MwaClient {
            override suspend fun authorize(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                cluster: String,
                chainId: String,
            ): MwaResult<AuthResult> {
                delay(5_000)
                error("unreachable — should be cancelled by withTimeoutOrNull")
            }
            override suspend fun reauthorize(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                cluster: String,
                chainId: String,
            ) = error("nyi")
            override suspend fun deauthorize(sender: ActivityResultSender, identity: ConnectionIdentity, authToken: SecretString) =
                error("nyi")
            override suspend fun disconnect() = error("nyi")
            override suspend fun signMessages(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                messages: List<ByteArray>,
                addresses: List<ByteArray>,
            ) = error("nyi")
            override suspend fun signTransactions(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                transactions: List<ByteArray>,
            ) = error("nyi")
            override suspend fun signAndSendTransactions(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                transactions: List<ByteArray>,
                cluster: String,
            ) = error("nyi")
            override fun close() {}
        }
        val plugin = buildPlugin(clientFactory = { slowClient })
        val jsonSlot = slot<String>()

        plugin.mwaAuthorize("req-timeout", identityJson, "devnet", "solana:devnet", timeoutMs = 100L)

        awaitCondition(1_500L) {
            runCatching { verify(exactly = 1) { nativeBridge.postMwaTimeout(capture(jsonSlot)) } }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-timeout", payload.getString("request_id"))
        assertEquals("connect", payload.getString("source_method"))
        assertEquals(100L, payload.getLong("watchdog_ms"))
        // Terminal-signal invariant: mwa_error must NOT fire alongside mwa_timeout.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }
    }

    @Test
    fun `late result dropped - tryTerminate false on success path increments diagnostics and suppresses emission`() {
        // Mock InflightMap: register reports success (plugin doesn't check the return value, but
        // relaxed default `false` could mislead a reader); tryTerminate returns false to simulate a
        // JNI callback arriving after a prior terminal signal has already fired (the entry is gone).
        // MockK 1.13.13's agent mocks final Kotlin classes; matches the mockkConstructor pattern
        // used for `MasterKey.Builder` in SecureTokenStoreTest.
        val fakeInflight = mockk<InflightMap>(relaxed = true)
        every { fakeInflight.register(any(), any(), any()) } returns true
        every { fakeInflight.tryTerminate(any()) } returns false

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
            inflightOverride = fakeInflight,
        )

        plugin.mwaAuthorize("req-late", identityJson, "devnet", "solana:devnet", timeoutMs = 2_000L)

        awaitCondition(2_000L) {
            plugin.diagnosticsForTest().lateResultCount == 1L
        }
        // No terminal signal must reach the native bridge — all emissions were gated by tryTerminate.
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
