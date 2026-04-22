package com.godotengine.godot_solana_sdk.mwa.client

import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.generated.MwaError
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import com.solana.mobilewalletadapter.clientlib.MobileWalletAdapter
import com.solana.mobilewalletadapter.clientlib.Solana
import com.solana.mobilewalletadapter.clientlib.TransactionResult
import com.solana.mobilewalletadapter.clientlib.protocol.JsonRpc20Client
import com.solana.mobilewalletadapter.clientlib.protocol.MobileWalletAdapterClient
import com.solana.mobilewalletadapter.common.ProtocolContract
import io.mockk.coEvery
import io.mockk.coVerify
import io.mockk.every
import io.mockk.mockk
import io.mockk.mockkStatic
import io.mockk.unmockkStatic
import io.mockk.verify
import kotlinx.coroutines.runBlocking
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotEquals
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Assertions.assertSame
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import org.junit.jupiter.api.parallel.Execution
import org.junit.jupiter.api.parallel.ExecutionMode

/**
 * Tests for [MwaClientImpl] (Story 1-6 Task 7 / AC-1).
 *
 * Strategy: inject a mock [MobileWalletAdapter] via the `adapterFactory`
 * constructor parameter rather than `mockkConstructor` — avoids byte-code
 * manipulation and keeps the per-call construction contract (DD-1) visible
 * to tests (they count factory invocations).
 *
 * `android.util.Log` is mocked via `mockkStatic(Log::class)` for SdkLog
 * capture. `@Execution(ExecutionMode.SAME_THREAD)` serializes this class
 * against any future parallel-execution flip (mirrors [FakeMwaClientTest]).
 *
 * Coverage matrix:
 *   - Pure-function tests (classifyException branches, encodeBase58 vectors)
 *   - Validation-before-side-effects (unknown cluster, chainId mismatch,
 *     multi-signer sign_messages) verifying the adapter is never touched
 *   - Adapter-interaction tests (blockchain set, authToken set, connect
 *     invoked with the right sender, transact invoked once)
 *   - Result-mapping tests (Success → MwaResult.Success with AuthResult
 *     fields populated from AuthorizationResult; NoWalletFound →
 *     NoMwaWalletInstalled; Failure → classified MwaError)
 *   - Per-call adapter construction (two sequential calls → factory invoked
 *     twice with fresh instance each time)
 *   - corrId embedded in MwaResult variants + matching the 8-hex-char log
 *     format
 *   - close() no-op
 */
@Execution(ExecutionMode.SAME_THREAD)
class MwaClientImplTest {

    private lateinit var mockAdapter: MobileWalletAdapter
    private var factoryInvocations: Int = 0
    private lateinit var client: MwaClientImpl

    private val sender: ActivityResultSender = mockk(relaxed = true)
    private val identity: ConnectionIdentity = mockk(relaxed = true)
    private val opaqueAuthToken = SecretString("wire-token-abc".toByteArray(Charsets.UTF_8))

    @BeforeEach
    fun setup() {
        mockAdapter = mockk(relaxed = true)
        factoryInvocations = 0
        client = MwaClientImpl(adapterFactory = { _ ->
            factoryInvocations++
            mockAdapter
        })

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
    // Validation BEFORE side effects — no adapter should be constructed
    // ----------------------------------------------------------------------

    @Test
    fun `authorize returns Failure(ProtocolError) without invoking factory when chainId mismatches cluster`() = runBlocking {
        val result = client.authorize(sender, identity, "devnet", "solana:mainnet")
        assertTrue(result is MwaResult.Failure, "expected Failure, got $result")
        assertSame(MwaError.ProtocolError, (result as MwaResult.Failure).error)
        assertEquals(0, factoryInvocations, "factory must NOT run when chainId/cluster mismatch")
    }

    @Test
    fun `authorize returns Failure(ProtocolError) without invoking factory for unknown cluster`() = runBlocking {
        val result = client.authorize(sender, identity, "bogus-net", "solana:bogus")
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.ProtocolError, (result as MwaResult.Failure).error)
        assertEquals(0, factoryInvocations)
    }

    @Test
    fun `signMessages returns Failure(ProtocolError) without invoking factory when addresses size is not 1`() = runBlocking {
        val result = client.signMessages(
            sender,
            identity,
            opaqueAuthToken,
            messages = listOf(ByteArray(4)),
            // 2 addresses — multi-signer rejected per Story 1-6 Task 2 contract.
            addresses = listOf(ByteArray(32), ByteArray(32)),
        )
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.ProtocolError, (result as MwaResult.Failure).error)
        assertEquals(0, factoryInvocations)
    }

    @Test
    fun `signAndSendTransactions returns Failure(ProtocolError) without invoking factory for unknown cluster`() = runBlocking {
        val result = client.signAndSendTransactions(
            sender,
            identity,
            opaqueAuthToken,
            listOf(ByteArray(8)),
            cluster = "imaginarynet",
        )
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.ProtocolError, (result as MwaResult.Failure).error)
        assertEquals(0, factoryInvocations)
    }

    // ----------------------------------------------------------------------
    // Adapter interaction — blockchain + authToken set before wire call
    // ----------------------------------------------------------------------

    @Test
    fun `authorize sets blockchain to Solana Devnet for devnet cluster before invoking connect`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Success(
            Unit,
            successAuthResult(),
        )
        client.authorize(sender, identity, "devnet", "solana:devnet")
        verify { mockAdapter.blockchain = Solana.Devnet }
        coVerify(exactly = 1) { mockAdapter.connect(sender) }
    }

    @Test
    fun `reauthorize sets adapter authToken via identity transform before connect`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Success(
            Unit,
            successAuthResult(),
        )
        client.reauthorize(sender, identity, opaqueAuthToken, "devnet", "solana:devnet")
        // The opaque-token identity transform: String(reveal(), UTF_8) of
        // SecretString("wire-token-abc".toByteArray(UTF_8)) is "wire-token-abc".
        verify { mockAdapter.authToken = "wire-token-abc" }
        coVerify(exactly = 1) { mockAdapter.connect(sender) }
    }

    // ----------------------------------------------------------------------
    // Result mapping — Success / NoWalletFound / Failure
    // ----------------------------------------------------------------------

    @Test
    fun `authorize success returns MwaResult Success with AuthResult populated from AuthorizationResult`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Success(
            Unit,
            successAuthResult(authToken = "opaque-wire-token", publicKey = ByteArray(32) { it.toByte() }),
        )
        val result = client.authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Success)
        val auth = (result as MwaResult.Success).value
        assertEquals("opaque-wire-token", String(auth.authToken.reveal(), Charsets.UTF_8))
        assertEquals(32, auth.publicKey.size)
        assertEquals("Test Wallet", auth.accountLabel)
        assertEquals("devnet", auth.cluster)
        assertEquals("solana:devnet", auth.chainId)
        // clientlib-ktx 2.0.3 does not surface walletPackage at this layer — null is expected.
        assertEquals(null, auth.walletPackage)
    }

    @Test
    fun `authorize maps TransactionResult NoWalletFound to Failure(NoMwaWalletInstalled)`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.NoWalletFound("no wallet")
        val result = client.authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.NoMwaWalletInstalled, (result as MwaResult.Failure).error)
    }

    @Test
    fun `authorize maps TransactionResult Failure(unknown exception) to Failure(ProtocolError)`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Failure(
            "unknown wire error",
            RuntimeException("something went wrong"),
        )
        val result = client.authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.ProtocolError, (result as MwaResult.Failure).error)
    }

    @Test
    fun `disconnect always returns Success without touching adapter`() = runBlocking {
        // Pins the client-layer no-op contract documented on MwaClient.disconnect()'s
        // KDoc: at this layer there is no persistent session to tear down;
        // local cache teardown (is_connected / get_public_key getters,
        // SecureTokenStore handle release) lands in Story 2-3 at the plugin
        // layer. If a future refactor introduces client-layer session state,
        // it MUST change this contract deliberately and update both the
        // interface KDoc and this assertion.
        val result = client.disconnect()
        assertTrue(result is MwaResult.Success)
        assertEquals(0, factoryInvocations, "disconnect must NOT construct an adapter")
    }

    // ----------------------------------------------------------------------
    // Exception classification — every ProtocolContract code branch
    // ----------------------------------------------------------------------

    @Test
    fun `classifyException maps ERROR_NOT_SIGNED to UserCanceled regardless of op`() {
        val ex = remoteException(ProtocolContract.ERROR_NOT_SIGNED)
        assertSame(MwaError.UserCanceled, client.classifyException(ex, "authorize"))
        assertSame(MwaError.UserCanceled, client.classifyException(ex, "sign_messages"))
        assertSame(MwaError.UserCanceled, client.classifyException(ex, "sign_and_send"))
    }

    @Test
    fun `classifyException maps ERROR_AUTHORIZATION_FAILED to TokenExpired on reauthorize and NotConnected on other ops`() {
        val ex = remoteException(ProtocolContract.ERROR_AUTHORIZATION_FAILED)
        assertSame(MwaError.TokenExpired, client.classifyException(ex, "reauthorize"))
        assertSame(MwaError.NotConnected, client.classifyException(ex, "authorize"))
        assertSame(MwaError.NotConnected, client.classifyException(ex, "deauthorize"))
    }

    @Test
    fun `classifyException maps ERROR_NOT_SUBMITTED to RpcFailed`() {
        val ex = remoteException(ProtocolContract.ERROR_NOT_SUBMITTED)
        assertSame(MwaError.RpcFailed, client.classifyException(ex, "sign_and_send"))
    }

    @Test
    fun `classifyException maps unknown remote codes and non-remote exceptions to ProtocolError`() {
        val unknownRemote = remoteException(-9999)
        assertSame(MwaError.ProtocolError, client.classifyException(unknownRemote, "authorize"))
        assertSame(MwaError.ProtocolError, client.classifyException(RuntimeException(), "authorize"))
        assertSame(MwaError.ProtocolError, client.classifyException(null, "authorize"))
    }

    // ----------------------------------------------------------------------
    // End-to-end classification — asserts op methods pass the right op-name
    // string to classifyException. Guards against snake_case/camelCase drift
    // at the runOp call sites.
    // ----------------------------------------------------------------------

    @Test
    fun `authorize with ERROR_AUTHORIZATION_FAILED maps end-to-end to NotConnected`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Failure(
            "auth failed",
            remoteException(ProtocolContract.ERROR_AUTHORIZATION_FAILED),
        )
        val result = client.authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.NotConnected, (result as MwaResult.Failure).error)
    }

    @Test
    fun `reauthorize with ERROR_AUTHORIZATION_FAILED maps end-to-end to TokenExpired`() = runBlocking {
        // This is the LOAD-BEARING op-name test. classifyException produces
        // TokenExpired ONLY when op == "reauthorize". If MwaClientImpl.reauthorize
        // ever drifts to runOp("reAuthorize") or similar, this test fails.
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Failure(
            "auth failed",
            remoteException(ProtocolContract.ERROR_AUTHORIZATION_FAILED),
        )
        val result = client.reauthorize(sender, identity, opaqueAuthToken, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.TokenExpired, (result as MwaResult.Failure).error)
    }

    @Test
    fun `authorize with ERROR_NOT_SIGNED maps end-to-end to UserCanceled`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Failure(
            "user canceled",
            remoteException(ProtocolContract.ERROR_NOT_SIGNED),
        )
        val result = client.authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Failure)
        assertSame(MwaError.UserCanceled, (result as MwaResult.Failure).error)
    }

    // ----------------------------------------------------------------------
    // Base58 encoder — Bitcoin alphabet test vectors
    // ----------------------------------------------------------------------

    @Test
    fun `encodeBase58 returns empty string for empty input`() {
        assertEquals("", client.encodeBase58(ByteArray(0)))
    }

    @Test
    fun `encodeBase58 preserves leading zeros as '1' characters`() {
        // Per base58 convention, each leading 0x00 byte becomes one leading '1'
        // in the encoded output.
        assertEquals("1", client.encodeBase58(byteArrayOf(0)))
        assertEquals("11", client.encodeBase58(byteArrayOf(0, 0)))
        assertEquals("1111", client.encodeBase58(byteArrayOf(0, 0, 0, 0)))
    }

    @Test
    fun `encodeBase58 produces expected output for known test vectors`() {
        // Canonical Bitcoin base58 vector (ASCII bytes).
        assertEquals("2NEpo7TZRRrLZSi2U", client.encodeBase58("Hello World!".toByteArray(Charsets.UTF_8)))
        // Single 0x00 byte → leading-zero preservation.
        assertEquals("1", client.encodeBase58(byteArrayOf(0x00)))
        // 64 all-zero bytes → exactly 64 '1' characters (strengthened from
        // length-only to value-exact assertion — catches any character drift).
        val allZero64 = ByteArray(64)
        assertEquals("1".repeat(64), client.encodeBase58(allZero64))
    }

    @Test
    fun `encodeBase58 matches reference output for 64-byte signature vectors`() {
        // Vectors independently computed via Python's base58 library (PyPI
        // `base58` package). These are Solana-signature-sized inputs — the
        // real production use case.
        //
        //   bytes(range(64))           → 1GMkH3brNXiNNs1t… (first-byte 0x00
        //                                yields a leading '1' per leading-zero rule)
        //   bytes([0xFF] * 64)         → 67rpwLCuS5DGA8KG…
        val vector1 = ByteArray(64) { it.toByte() }
        assertEquals(
            "1GMkH3brNXiNNs1tiFZHu4yZSRrzJwxi5wB9bHFtMinfCXNnR1adh8Vo8NTheK4evneedH4qmvjeqcBBNAefgS",
            client.encodeBase58(vector1),
        )
        val vector2 = ByteArray(64) { 0xFF.toByte() }
        assertEquals(
            "67rpwLCuS5DGA8KGZXKsVQ7dnPb9goRLoKfgGbLfQg9WoLUgNY77E2jT11fem3coV9nAkguBACzrU1iyZM4B8roQ",
            client.encodeBase58(vector2),
        )
    }

    @Test
    fun `signAndSendTransactions success base58-encodes signatures end-to-end`() = runBlocking {
        // Drives a 64-byte wire signature through the full signAndSend path and
        // asserts the final SignAndSendResult.signatures[0] is the base58 form.
        // If someone regresses encodeBase58(it) back to a hex stub or drops
        // the call entirely, this test surfaces the regression at the DTO.
        val sigBytes = ByteArray(64) { it.toByte() }
        val expectedBase58 =
            "1GMkH3brNXiNNs1tiFZHu4yZSRrzJwxi5wB9bHFtMinfCXNnR1adh8Vo8NTheK4evneedH4qmvjeqcBBNAefgS"
        val sigResult = MobileWalletAdapterClient.SignAndSendTransactionsResult(arrayOf(sigBytes))
        coEvery {
            mockAdapter.transact<MobileWalletAdapterClient.SignAndSendTransactionsResult>(
                any(),
                any(),
                any(),
            )
        } returns TransactionResult.Success(sigResult, successAuthResult())

        val result = client.signAndSendTransactions(
            sender,
            identity,
            opaqueAuthToken,
            listOf(ByteArray(8)),
            "devnet",
        )
        assertTrue(result is MwaResult.Success)
        val signatures = (result as MwaResult.Success).value.signatures
        assertEquals(1, signatures.size)
        assertEquals(expectedBase58, signatures[0])
    }

    @Test
    fun `reauthorize preserves base64 special characters in authToken round-trip`() = runBlocking {
        // Catches accidental URL-safe-base64 re-encoding: standard base64
        // uses +/= ; URL-safe base64 uses -/_ with no padding. A library
        // that silently URL-escapes would corrupt the opaque token.
        val tokenWithBase64Chars = SecretString("abc+def/ghi==".toByteArray(Charsets.UTF_8))
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Success(Unit, successAuthResult())

        client.reauthorize(sender, identity, tokenWithBase64Chars, "devnet", "solana:devnet")

        verify { mockAdapter.authToken = "abc+def/ghi==" }
    }

    // ----------------------------------------------------------------------
    // Design Decision 1 — per-call adapter construction
    // ----------------------------------------------------------------------

    @Test
    fun `per-call adapter construction — two sequential authorize calls invoke factory twice`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Success(Unit, successAuthResult())
        client.authorize(sender, identity, "devnet", "solana:devnet")
        client.authorize(sender, identity, "devnet", "solana:devnet")
        assertEquals(2, factoryInvocations, "each authorize must request a fresh adapter from the factory")
    }

    // ----------------------------------------------------------------------
    // Design Decision 3 — corrId embedded in results
    // ----------------------------------------------------------------------

    @Test
    fun `corrId is embedded in MwaResult Success returned from authorize`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Success(Unit, successAuthResult())
        val result = client.authorize(sender, identity, "devnet", "solana:devnet")
        assertNotNull(result.corrId, "Success.corrId must be set so callers can join log traces")
        assertTrue(
            result.corrId!!.matches(Regex("^[0-9a-f]{8}$")),
            "corrId '${result.corrId}' must be 8 hex chars",
        )
    }

    @Test
    fun `corrId is embedded in MwaResult Failure returned from authorize`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.NoWalletFound("nope")
        val result = client.authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Failure)
        val corrId = (result as MwaResult.Failure).corrId
        assertNotNull(corrId, "Failure.corrId must be set for error-path correlation")
        assertTrue(corrId!!.matches(Regex("^[0-9a-f]{8}$")))
    }

    @Test
    fun `corrId differs across sequential calls`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Success(Unit, successAuthResult())
        val a = client.authorize(sender, identity, "devnet", "solana:devnet").corrId
        val b = client.authorize(sender, identity, "devnet", "solana:devnet").corrId
        assertNotNull(a)
        assertNotNull(b)
        assertNotEquals(a, b, "corrId must be unique per-call (UUID-derived)")
    }

    @Test
    fun `enter and exit log lines carry matching 8-hex-char corrId for authorize success`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Success(Unit, successAuthResult())
        val captured = mutableListOf<String>()
        every { Log.d("MWA", capture(captured)) } returns 0

        client.authorize(sender, identity, "devnet", "solana:devnet")

        val corrIdRegex = Regex("""\[corrId=([0-9a-f]{8})]""")
        val corrIds = captured.mapNotNull { corrIdRegex.find(it)?.groupValues?.get(1) }
        assertTrue(corrIds.size >= 2, "expected enter + exit lines; captured=$captured")
        assertEquals(corrIds[0], corrIds[1], "enter/exit corrIds must match")
    }

    // ----------------------------------------------------------------------
    // close() — no-op, idempotent, client remains usable
    // ----------------------------------------------------------------------

    @Test
    fun `close is no-op idempotent and does not invalidate the client`() = runBlocking {
        coEvery { mockAdapter.connect(sender) } returns TransactionResult.Success(Unit, successAuthResult())
        client.close()
        client.close()

        val result = client.authorize(sender, identity, "devnet", "solana:devnet")
        assertTrue(result is MwaResult.Success, "client must remain functional after close()")
    }

    // ----------------------------------------------------------------------
    // Helpers
    // ----------------------------------------------------------------------

    /**
     * Constructs a synthetic AuthorizationResult with minimal test values.
     * Uses the public `.create(authToken, publicKey, accountLabel, walletUriBase)`
     * factory introduced by clientlib 2.0.x (see javap introspection in T2).
     */
    private fun successAuthResult(
        authToken: String = "wire-token-default",
        publicKey: ByteArray = ByteArray(32),
        accountLabel: String = "Test Wallet",
    ): MobileWalletAdapterClient.AuthorizationResult = MobileWalletAdapterClient.AuthorizationResult.create(
        authToken,
        publicKey,
        accountLabel,
        // walletUriBase — null avoids Android Uri instantiation in JVM tests.
        null,
    )

    private fun remoteException(code: Int): JsonRpc20Client.JsonRpc20RemoteException =
        JsonRpc20Client.JsonRpc20RemoteException(code, "synthetic error for tests", null)
}
