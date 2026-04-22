package com.godotengine.godot_solana_sdk.mwa.client

import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.generated.MwaError
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import io.mockk.every
import io.mockk.mockk
import io.mockk.mockkStatic
import io.mockk.unmockkStatic
import kotlinx.coroutines.runBlocking
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertFalse
import org.junit.jupiter.api.Assertions.assertSame
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import org.junit.jupiter.api.assertThrows
import org.junit.jupiter.api.io.TempDir
import org.junit.jupiter.api.parallel.Execution
import org.junit.jupiter.api.parallel.ExecutionMode
import java.io.File

/**
 * Tests for [FakeMwaClient] (Story 1-6 Task 6 / AC-2, AC-4).
 *
 * Fixtures come from the canonical `testdata/mwa-fixtures/` directory via the
 * `mwa.fixtures.dir` system property that `android/plugin/build.gradle.kts`
 * sets on every unit-test invocation (Story 1-6 Design Decision 6). The
 * directory is expected to exist before any test runs — an `error()` in setup
 * short-circuits the suite with a clear message if the wiring breaks.
 *
 * `android.util.Log` is mocked via `mockkStatic(Log::class)` so that the
 * corrId-format test can inspect the per-op log lines emitted by `SdkLog.d`.
 * `Log::class` is JVM-global static state — [Execution] with
 * [ExecutionMode.SAME_THREAD] serializes this class even if the project
 * enables JUnit 5 parallel execution (`junit.jupiter.execution.parallel.enabled`).
 *
 * ### Fixture-value provenance
 *
 * Several tests assert on values baked into canonical fixtures:
 *   - `testdata/mwa-fixtures/authorize_success.json` — cluster=devnet,
 *     chain_id=solana:devnet, account_label="Test Wallet",
 *     auth_token="dGVzdC1hdXRoLXRva2VuLXN1Y2Nlc3M=", public_key=32-byte base64.
 *   - `testdata/mwa-fixtures/reauthorize_success.json` — auth_token
 *     "dGVzdC1hdXRoLXRva2VuLXJlYXV0aA==" (distinct from authorize_success
 *     on purpose — proves parseAuthResponse reads the reauth fixture).
 *   - Signed-payload counts come from the concrete arrays in the fixtures.
 *
 * If you edit any of the above fixtures, update the matching assertion here
 * in the same PR — the two sources of truth must stay in lockstep.
 */
@Execution(ExecutionMode.SAME_THREAD)
class FakeMwaClientTest {

    private lateinit var fixtureDir: File
    private lateinit var client: FakeMwaClient
    private val sender: ActivityResultSender = mockk(relaxed = true)
    private val identity: ConnectionIdentity = mockk(relaxed = true)
    private val anyToken = SecretString("opaque-token".toByteArray(Charsets.UTF_8))

    @BeforeEach
    fun setup() {
        val path = System.getProperty("mwa.fixtures.dir")
            ?: error(
                "mwa.fixtures.dir system property is required; see android/plugin/build.gradle.kts " +
                    "testOptions.unitTests.all (Story 1-6 Design Decision 6).",
            )
        fixtureDir = File(path)
        assertTrue(fixtureDir.isDirectory, "fixture dir must exist: $fixtureDir")
        client = FakeMwaClient(fixtureDir)

        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0
    }

    @AfterEach
    fun teardown() {
        unmockkStatic(Log::class)
    }

    // ----------------------------------------------------------------------
    // AC-2 happy paths (canonical fixtures)
    // ----------------------------------------------------------------------

    @Test
    fun `authorize returns Success with AuthResult for authorize_success scenario`() = runBlocking {
        val result = client.withScenario("success").authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Success)
        val auth = (result as MwaResult.Success).value
        assertEquals("devnet", auth.cluster)
        assertEquals("solana:devnet", auth.chainId)
        assertEquals("Test Wallet", auth.accountLabel)
        // auth_token is an opaque STRING; SecretString wraps its UTF-8 bytes so
        // round-trip via String(reveal(), UTF_8) recovers the original string.
        assertEquals("dGVzdC1hdXRoLXRva2VuLXN1Y2Nlc3M=", String(auth.authToken.reveal(), Charsets.UTF_8))
        // public_key is REAL base64 → 32-byte ed25519 key.
        assertEquals(32, auth.publicKey.size)
    }

    @Test
    fun `reauthorize returns Success with AuthResult for reauthorize_success scenario`() = runBlocking {
        // Takes authToken: SecretString as a parameter. The fake IGNORES the input
        // token (it's a test double) but the canonical fixture's auth_token is
        // intentionally distinct from authorize_success.json — proves we read the
        // reauth fixture not the auth one. MwaClientImpl will pass the token to the
        // real adapter via adapter.authToken = String(reveal(), UTF_8) (DD-2).
        val result = client.withScenario("success").reauthorize(
            sender,
            identity,
            anyToken,
            "devnet",
            "solana:devnet",
        )
        assertTrue(result is MwaResult.Success)
        val auth = (result as MwaResult.Success).value
        assertEquals(
            "dGVzdC1hdXRoLXRva2VuLXJlYXV0aA==",
            String(auth.authToken.reveal(), Charsets.UTF_8),
            "reauthorize must load reauthorize_success.json (its own auth_token), not authorize_success.json",
        )
        assertEquals("devnet", auth.cluster)
        assertEquals(32, auth.publicKey.size)
    }

    @Test
    fun `signMessages returns Success with signed_payloads list for sign_messages_success scenario`() = runBlocking {
        val result = client.withScenario("success").signMessages(
            sender,
            identity,
            anyToken,
            listOf(ByteArray(4), ByteArray(4)),
            listOf(ByteArray(32)),
        )
        assertTrue(result is MwaResult.Success)
        val signed = (result as MwaResult.Success).value.signedPayloads
        // Fixture has 2 signed_payloads entries — proves parseSignResponse decodes the array.
        assertEquals(2, signed.size)
        assertTrue(signed.all { it.isNotEmpty() })
    }

    @Test
    fun `signMessages returns Failure(WalletRejected) for sign_messages_wallet_rejected scenario`() = runBlocking {
        val result = client.withScenario("wallet_rejected").signMessages(
            sender,
            identity,
            anyToken,
            listOf(ByteArray(4)),
            listOf(ByteArray(32)),
        )
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.WalletRejected, (result as MwaResult.Failure).error)
    }

    @Test
    fun `signTransactions returns Success with signed_payloads for sign_transactions_success scenario`() = runBlocking {
        // Exercises the sign_transactions op dispatch path specifically — a typo in
        // runOp("sign_transaction") vs runOp("sign_transactions") would slip past
        // the signMessages tests since they share parseSignResponse internally.
        val result = client.withScenario("success").signTransactions(
            sender,
            identity,
            anyToken,
            listOf(ByteArray(8), ByteArray(8)),
        )
        assertTrue(result is MwaResult.Success)
        val signed = (result as MwaResult.Success).value.signedPayloads
        assertEquals(2, signed.size)
    }

    @Test
    fun `signAndSendTransactions returns Success with signatures for sign_and_send_success scenario`() = runBlocking {
        val result = client.withScenario("success").signAndSendTransactions(
            sender,
            identity,
            anyToken,
            listOf(ByteArray(8)),
            "devnet",
        )
        assertTrue(result is MwaResult.Success)
        val sigs = (result as MwaResult.Success).value.signatures
        assertEquals(1, sigs.size)
        assertTrue(sigs[0].isNotEmpty())
    }

    @Test
    fun `signAndSendTransactions returns Failure(NetworkOffline) for sign_and_send_network_offline scenario`() = runBlocking {
        val result = client.withScenario("network_offline").signAndSendTransactions(
            sender,
            identity,
            anyToken,
            listOf(ByteArray(8)),
            "devnet",
        )
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.NetworkOffline, (result as MwaResult.Failure).error)
    }

    @Test
    fun `authorize returns Failure(UserCanceled) for authorize_user_canceled scenario`() = runBlocking {
        val result = client.withScenario("user_canceled").authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.UserCanceled, (result as MwaResult.Failure).error)
    }

    @Test
    fun `deauthorize returns Success for deauthorize_success scenario`() = runBlocking {
        val result = client.withScenario("success").deauthorize(sender, identity, anyToken)
        assertTrue(result is MwaResult.Success)
    }

    @Test
    fun `disconnect returns Success for disconnect_success scenario`() = runBlocking {
        val result = client.withScenario("success").disconnect()
        assertTrue(result is MwaResult.Success)
    }

    // ----------------------------------------------------------------------
    // AC-4 missing-fixture contract
    // ----------------------------------------------------------------------

    @Test
    fun `missing scenario throws IllegalStateException with verbatim message`() {
        val ex = assertThrows<IllegalStateException> {
            runBlocking {
                client.withScenario("definitely_does_not_exist").authorize(
                    sender,
                    identity,
                    "devnet",
                    "solana:devnet",
                )
            }
        }
        assertEquals("no fixture for scenario 'definitely_does_not_exist'", ex.message)
    }

    @Test
    fun `scenario resets to null after each op call`() {
        // Non-runBlocking outer — avoids nested-coroutine blocking, which would
        // deadlock under runTest-style single-threaded test dispatchers.
        val first = runBlocking {
            client.withScenario("success").authorize(sender, identity, "devnet", "solana:devnet")
        }
        assertTrue(first is MwaResult.Success)

        val ex = assertThrows<IllegalStateException> {
            runBlocking { client.authorize(sender, identity, "devnet", "solana:devnet") }
        }
        assertEquals("no fixture for scenario '<unset>'", ex.message)
    }

    @Test
    fun `next op without withScenario throws no-fixture IllegalStateException`() {
        val ex = assertThrows<IllegalStateException> {
            runBlocking { client.authorize(sender, identity, "devnet", "solana:devnet") }
        }
        assertEquals("no fixture for scenario '<unset>'", ex.message)
    }

    // ----------------------------------------------------------------------
    // DD-2: authToken opaque-string invariant
    // ----------------------------------------------------------------------

    @Test
    fun `AuthResult authToken reveal returns UTF-8 bytes of opaque string and toString is redacted`() = runBlocking {
        val result = client.withScenario("success").authorize(sender, identity, "devnet", "solana:devnet")
        val auth = (result as MwaResult.Success).value

        val revealed = String(auth.authToken.reveal(), Charsets.UTF_8)
        assertEquals("dGVzdC1hdXRoLXRva2VuLXN1Y2Nlc3M=", revealed)

        assertTrue(
            auth.toString().contains("<redacted>"),
            "AuthResult.toString() must propagate SecretString's redaction",
        )
        assertFalse(
            auth.toString().contains("dGVzdC1hdXRo"),
            "AuthResult.toString() must NOT leak the auth_token prefix",
        )
    }

    // ----------------------------------------------------------------------
    // DD-3: per-call corrId + exception-path exit logging
    // ----------------------------------------------------------------------

    @Test
    fun `corrId emitted per call matches 8-hex-char format in both enter and exit logs`() = runBlocking {
        val capturedMsgs = mutableListOf<String>()
        every { Log.d("MWA", capture(capturedMsgs)) } returns 0

        client.withScenario("success").authorize(sender, identity, "devnet", "solana:devnet")

        // The `[corrId=xxxxxxxx]` framing is pinned by `SdkLog.d` in Story 1-2 DD-30:
        //   `Log.d(tag, "[corrId=$corrId] ${msg()}")` (SdkLog.kt:28-29). This regex
        // depends on that exact format; if SdkLog's format changes, update here AND
        // add a matching SdkLogTest assertion so both contracts move together.
        val corrIdRegex = Regex("""\[corrId=([0-9a-f]{8})]""")
        val corrIds = capturedMsgs.mapNotNull { corrIdRegex.find(it)?.groupValues?.get(1) }

        assertTrue(
            corrIds.size >= 2,
            "expected at least 2 log lines with corrId (enter + exit); got ${capturedMsgs.size} msgs: $capturedMsgs",
        )
        assertEquals(corrIds[0], corrIds[1], "enter and exit log lines must share the same corrId")
        assertTrue(
            corrIds[0].matches(Regex("^[0-9a-f]{8}$")),
            "corrId '${corrIds[0]}' must match 8-hex-char format",
        )
    }

    @Test
    fun `corrId emitted on exception path so enter and exit pair even on throw`() {
        val capturedMsgs = mutableListOf<String>()
        every { Log.d("MWA", capture(capturedMsgs)) } returns 0

        assertThrows<IllegalStateException> {
            runBlocking { client.authorize(sender, identity, "devnet", "solana:devnet") }
        }

        val enterLines = capturedMsgs.filter { it.contains("authorize.enter") }
        val exitLines = capturedMsgs.filter { it.contains("authorize.exit") }
        assertEquals(1, enterLines.size, "must emit authorize.enter once even when op throws")
        assertEquals(1, exitLines.size, "must emit authorize.exit once on exception path")
        assertTrue(
            exitLines[0].contains("outcome=exception"),
            "exit on throw path must log outcome=exception; got: ${exitLines[0]}",
        )
    }

    // ----------------------------------------------------------------------
    // close() idempotency + post-close usability
    // ----------------------------------------------------------------------

    @Test
    fun `close is no-op idempotent and does not invalidate the client`() = runBlocking {
        client.close()
        client.close() // second call MUST NOT throw

        // close() is a no-op per Design Decision 1 — FakeMwaClient has no state to
        // release. Verify the client is still functional post-close so future
        // refactors that introduce real close() semantics don't silently drop this
        // invariant.
        val result = client.withScenario("success").authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Success, "client must remain functional after close()")
    }

    // ----------------------------------------------------------------------
    // Cross-impl parity: ERROR_BY_CODE covers all 13 generated MwaError codes
    // ----------------------------------------------------------------------

    @Test
    fun `ERROR_BY_CODE resolves each of the 13 generated MwaError codes to the correct singleton`(@TempDir tempDir: File) = runBlocking {
        val allErrors = listOf(
            MwaError.UserCanceled, MwaError.WalletRejected, MwaError.NoMwaWalletInstalled,
            MwaError.WalletUninstalled, MwaError.TokenExpired, MwaError.RpcFailed,
            MwaError.NetworkOffline, MwaError.Timeout, MwaError.NotConnected,
            MwaError.UnsupportedPlatform, MwaError.ProtocolError, MwaError.StorageCorrupt,
            MwaError.ReauthRequired,
        )
        assertEquals(13, allErrors.size, "expected 13 MwaError singletons (Story 1-1 manifest)")

        val adHoc = FakeMwaClient(tempDir)
        for (error in allErrors) {
            val scenarioName = "probe_${error.code.lowercase()}"
            File(tempDir, "authorize_$scenarioName.json").writeText(
                """
                {
                  "schema_version": 1,
                  "op": "authorize",
                  "scenario": "$scenarioName",
                  "expected_input": {},
                  "response": {"type": "error", "code": "${error.code}"}
                }
                """.trimIndent(),
            )
        }

        for (error in allErrors) {
            val scenarioName = "probe_${error.code.lowercase()}"
            val result = adHoc.withScenario(scenarioName).authorize(
                sender,
                identity,
                "devnet",
                "solana:devnet",
            )
            assertTrue(
                result is MwaResult.Failure,
                "expected Failure for code '${error.code}' but got $result",
            )
            assertSame(
                error,
                (result as MwaResult.Failure).error,
                "ERROR_BY_CODE must map '${error.code}' to ${error::class.simpleName}; " +
                    "if a new MwaError code was added to the manifest, update FakeMwaClient.ERROR_BY_CODE",
            )
        }
    }

    // ----------------------------------------------------------------------
    // Malformed-fixture error wrapping
    // ----------------------------------------------------------------------

    @Test
    fun `malformed fixture triggers IllegalStateException naming the file`(@TempDir tempDir: File) {
        File(tempDir, "authorize_broken.json").writeText(
            // Missing required payload fields (public_key / cluster / chain_id) —
            // triggers JSONException in parseAuthResponse, which FakeMwaClient wraps
            // with the fixture file name.
            """{"schema_version":1,"op":"authorize","scenario":"broken","expected_input":{},
               "response":{"type":"success","payload":{"auth_token":"abc"}}}
            """.trimIndent(),
        )
        val adHoc = FakeMwaClient(tempDir)
        val ex = assertThrows<IllegalStateException> {
            runBlocking {
                adHoc.withScenario("broken").authorize(sender, identity, "devnet", "solana:devnet")
            }
        }
        assertTrue(
            ex.message!!.contains("authorize_broken.json"),
            "error message must name the malformed file; got '${ex.message}'",
        )
    }
}
