package com.godotengine.godot_solana_sdk.mwa.client

import com.godotengine.godot_solana_sdk.mwa.client.dto.AuthResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignAndSendResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignResult
import com.godotengine.godot_solana_sdk.mwa.generated.MwaError
import com.godotengine.godot_solana_sdk.mwa.util.SdkLog
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.Blockchain
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import com.solana.mobilewalletadapter.clientlib.MobileWalletAdapter
import com.solana.mobilewalletadapter.clientlib.Solana
import com.solana.mobilewalletadapter.clientlib.TransactionParams
import com.solana.mobilewalletadapter.clientlib.TransactionResult
import com.solana.mobilewalletadapter.clientlib.protocol.JsonRpc20Client
import com.solana.mobilewalletadapter.clientlib.protocol.MobileWalletAdapterClient
import com.solana.mobilewalletadapter.common.ProtocolContract
import java.util.UUID

/**
 * Production [MwaClient] wrapping `com.solana.mobilewalletadapter.clientlib.*`
 * (clientlib-ktx **2.0.3 exact** — see `build.gradle.kts:92`; the version is
 * pinned precisely rather than via a `+` suffix because
 * [TransactionResult.Success.authResult] is 2.0.x-specific API shape that a
 * minor-version dep bump could remove).
 *
 * Honors the 6 LOCKED Story 1-6 design decisions — see top-of-file comments in
 * [FakeMwaClient] for parity requirements.
 *
 * Op-name strings passed to [runOp] use **snake_case** identical to
 * [FakeMwaClient] so a log aggregator filtering on `op=sign_messages` catches
 * both impls' lines.
 *
 * Never logs `authToken` or `publicKey` bytes (CI grep in `ci/grep_bans.sh`).
 * Only `corrId` and op-name appear in log lines.
 */
class MwaClientImpl : MwaClient {

    override suspend fun authorize(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        cluster: String,
        chainId: String,
    ): MwaResult<AuthResult> = runOp("authorize") { corrId ->
        val adapter = configureAdapter(identity, cluster, chainId, corrId)
            ?: return@runOp MwaResult.Failure(MwaError.ProtocolError, corrId)
        mapAuthTransactionResult(adapter.connect(sender), "authorize", cluster, chainId, corrId)
    }

    override suspend fun reauthorize(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        cluster: String,
        chainId: String,
    ): MwaResult<AuthResult> = runOp("reauthorize") { corrId ->
        val adapter = configureAdapter(identity, cluster, chainId, corrId)
            ?: return@runOp MwaResult.Failure(MwaError.ProtocolError, corrId)
        adapter.authToken = authToken.toOpaqueString()
        mapAuthTransactionResult(adapter.connect(sender), "reauthorize", cluster, chainId, corrId)
    }

    override suspend fun deauthorize(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
    ): MwaResult<Unit> = runOp("deauthorize") { corrId ->
        // No blockchain set: deauthorize is a wallet-only op — no RPC endpoint
        // routing required. Same asymmetry in signMessages / signTransactions
        // below (they're wallet-local signature ops).
        val adapter = MobileWalletAdapter(identity)
        val tokenStr = authToken.toOpaqueString()
        adapter.authToken = tokenStr
        mapUnitTransactionResult(
            adapter.transact(sender) { _ -> deauthorize(tokenStr) },
            op = "deauthorize",
            corrId = corrId,
        )
    }

    override suspend fun disconnect(): MwaResult<Unit> = runOp("disconnect") { corrId ->
        // Client-layer no-op per MwaClient.disconnect KDoc. Nothing to release
        // or close at this layer; plugin handles local cache teardown (Story 2-3).
        MwaResult.Success(Unit, corrId)
    }

    override suspend fun signMessages(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        messages: List<ByteArray>,
        addresses: List<ByteArray>,
    ): MwaResult<SignResult> = runOp("sign_messages") { corrId ->
        // Story 1-6 explicitly rejects multi-signer sign_messages — SignResult
        // only carries one ByteArray per message, which is incompatible with
        // N-sigs-per-message. If/when multi-sig becomes a requirement,
        // SignResult shape changes and this guard moves.
        if (addresses.size != 1) {
            return@runOp MwaResult.Failure(MwaError.ProtocolError, corrId)
        }
        val adapter = MobileWalletAdapter(identity)
        adapter.authToken = authToken.toOpaqueString()
        val result = adapter.transact(sender) { _ ->
            signMessagesDetached(messages.toTypedArray(), addresses.toTypedArray())
        }
        when (result) {
            is TransactionResult.Success -> {
                val signed = ArrayList<ByteArray>(result.payload.messages.size)
                for ((index, sm) in result.payload.messages.withIndex()) {
                    val sig = sm.signatures.firstOrNull()
                        ?: return@runOp MwaResult.Failure(MwaError.ProtocolError, corrId).also {
                            SdkLog.w(TAG, corrId) {
                                "sign_messages.result: empty signatures for message index=$index"
                            }
                        }
                    signed.add(sig)
                }
                MwaResult.Success(SignResult(signed), corrId)
            }
            is TransactionResult.NoWalletFound ->
                MwaResult.Failure(MwaError.NoMwaWalletInstalled, corrId)
            is TransactionResult.Failure ->
                MwaResult.Failure(classifyException(result.e, "sign_messages"), corrId)
        }
    }

    override suspend fun signTransactions(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        transactions: List<ByteArray>,
    ): MwaResult<SignResult> = runOp("sign_transactions") { corrId ->
        val adapter = MobileWalletAdapter(identity)
        adapter.authToken = authToken.toOpaqueString()
        val result = adapter.transact(sender) { _ ->
            signTransactions(transactions.toTypedArray())
        }
        when (result) {
            is TransactionResult.Success ->
                MwaResult.Success(SignResult(result.payload.signedPayloads.toList()), corrId)
            is TransactionResult.NoWalletFound ->
                MwaResult.Failure(MwaError.NoMwaWalletInstalled, corrId)
            is TransactionResult.Failure ->
                MwaResult.Failure(classifyException(result.e, "sign_transactions"), corrId)
        }
    }

    override suspend fun signAndSendTransactions(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        transactions: List<ByteArray>,
        cluster: String,
    ): MwaResult<SignAndSendResult> = runOp("sign_and_send") { corrId ->
        // signAndSend needs `blockchain` on the adapter so clientlib can pick
        // the RPC endpoint; unknown cluster → ProtocolError before any wire call.
        val blockchain = clusterToBlockchain(cluster)
            ?: return@runOp MwaResult.Failure(MwaError.ProtocolError, corrId)
        val adapter = MobileWalletAdapter(identity).also { it.blockchain = blockchain }
        adapter.authToken = authToken.toOpaqueString()
        val result = adapter.transact(sender) { _ ->
            signAndSendTransactions(
                transactions.toTypedArray(),
                // Wallet defaults — v1 of the SDK does not surface commitment/
                // skipPreflight/maxRetries to the Godot caller. Named args make
                // the "take the wallet default" intent unambiguous.
                TransactionParams(
                    /* minContextSlot = */
                    null,
                    /* commitment = */
                    null,
                    /* skipPreflight = */
                    null,
                    /* maxRetries = */
                    null,
                    /* waitForCommitmentToSendNextTransaction = */
                    null,
                ),
            )
        }
        when (result) {
            is TransactionResult.Success ->
                MwaResult.Success(
                    SignAndSendResult(result.payload.signatures.map { encodeBase58(it) }),
                    corrId,
                )
            is TransactionResult.NoWalletFound ->
                MwaResult.Failure(MwaError.NoMwaWalletInstalled, corrId)
            is TransactionResult.Failure ->
                MwaResult.Failure(classifyException(result.e, "sign_and_send"), corrId)
        }
    }

    /** No-op per Design Decision 1. Idempotent. */
    override fun close() {
        // intentionally empty
    }

    // ----------------------------------------------------------------------
    // Internals
    // ----------------------------------------------------------------------

    private suspend fun <T> runOp(op: String, body: suspend (corrId: String) -> MwaResult<T>): MwaResult<T> {
        val corrId = UUID.randomUUID().toString().take(CORR_ID_LEN)
        SdkLog.d(TAG, corrId) { "$op.enter" }
        var outcome = "exception"
        try {
            val result = body(corrId)
            outcome = if (result is MwaResult.Success<*>) "success" else "failure"
            return result
        } finally {
            SdkLog.d(TAG, corrId) { "$op.exit outcome=$outcome" }
        }
    }

    /**
     * Validates `(cluster, chainId)` consistency and constructs the
     * [MobileWalletAdapter] with blockchain set. Returns null when either the
     * cluster is unknown OR the chainId does not match the cluster's canonical
     * form. Caller must map null to `MwaResult.Failure(ProtocolError, corrId)`.
     * Consolidates the previous `validateClusterAndChainId` + `newAdapter`
     * pair which produced a dead `?:` branch on the fresh-auth paths.
     */
    private fun configureAdapter(identity: ConnectionIdentity, cluster: String, chainId: String, corrId: String): MobileWalletAdapter? {
        val blockchain = clusterToBlockchain(cluster) ?: run {
            SdkLog.w(TAG, corrId) { "unknown cluster '$cluster'" }
            return null
        }
        val expectedChainId = canonicalChainIdFor(cluster) ?: run {
            SdkLog.w(TAG, corrId) { "no canonical chainId for cluster '$cluster'" }
            return null
        }
        if (expectedChainId != chainId) {
            SdkLog.w(TAG, corrId) {
                "chainId mismatch: expected='$expectedChainId' for cluster='$cluster', got='$chainId'"
            }
            return null
        }
        return MobileWalletAdapter(identity).also { it.blockchain = blockchain }
    }

    private fun clusterToBlockchain(cluster: String): Blockchain? = when (cluster) {
        "devnet" -> Solana.Devnet
        "mainnet-beta" -> Solana.Mainnet
        "testnet" -> Solana.Testnet
        else -> null
    }

    private fun canonicalChainIdFor(cluster: String): String? = when (cluster) {
        "devnet" -> "solana:devnet"
        "mainnet-beta" -> "solana:mainnet"
        "testnet" -> "solana:testnet"
        else -> null
    }

    private fun mapAuthTransactionResult(
        result: TransactionResult<Unit>,
        op: String,
        cluster: String,
        chainId: String,
        corrId: String,
    ): MwaResult<AuthResult> = when (result) {
        is TransactionResult.Success -> {
            val ar: MobileWalletAdapterClient.AuthorizationResult = result.authResult
            MwaResult.Success(
                AuthResult(
                    authToken = SecretString(ar.authToken.toByteArray(Charsets.UTF_8)),
                    publicKey = ar.publicKey,
                    accountLabel = ar.accountLabel,
                    walletUriBase = ar.walletUriBase?.toString(),
                    // clientlib-ktx 2.0.3's AuthorizationResult does not surface walletPackage;
                    // Story 2-1 derives it via Android Intent resolution at the plugin layer.
                    walletPackage = null,
                    cluster = cluster,
                    chainId = chainId,
                ),
                corrId,
            )
        }
        is TransactionResult.NoWalletFound ->
            MwaResult.Failure(MwaError.NoMwaWalletInstalled, corrId)
        is TransactionResult.Failure ->
            MwaResult.Failure(classifyException(result.e, op), corrId)
    }

    private fun mapUnitTransactionResult(result: TransactionResult<Unit>, op: String, corrId: String): MwaResult<Unit> = when (result) {
        is TransactionResult.Success -> MwaResult.Success(Unit, corrId)
        is TransactionResult.NoWalletFound -> MwaResult.Failure(MwaError.NoMwaWalletInstalled, corrId)
        is TransactionResult.Failure -> MwaResult.Failure(classifyException(result.e, op), corrId)
    }

    /**
     * Maps a clientlib-ktx [TransactionResult.Failure] cause to a typed
     * [MwaError]. Classification is primarily driven by
     * [JsonRpc20Client.JsonRpc20RemoteException.code] matched against
     * [ProtocolContract] error constants:
     *
     * | `ProtocolContract` constant | MwaError (context-dependent) |
     * |---|---|
     * | `ERROR_AUTHORIZATION_FAILED` (-1) | `TokenExpired` on `reauthorize`; `NotConnected` elsewhere |
     * | `ERROR_NOT_SIGNED` (-3) | `UserCanceled` — MWA protocol does NOT distinguish user-cancel from wallet-reject at the wire level; see note below |
     * | `ERROR_NOT_SUBMITTED` (-4) | `RpcFailed` |
     * | `ERROR_CLUSTER_NOT_SUPPORTED` (-7) | `ProtocolError` |
     * | Other remote codes / non-remote exceptions | `ProtocolError` |
     *
     * **Fake/real parity asymmetry (DD-28 caveat):** `FakeMwaClient` CAN
     * return `MwaError.WalletRejected` when the fixture's `response.code` is
     * `"WALLET_REJECTED"`. `MwaClientImpl` CANNOT — ERROR_NOT_SIGNED (-3)
     * always maps to `UserCanceled` because the MWA protocol frames user
     * cancel and wallet auto-reject as the same wire-level event. Plugin-layer
     * code (Story 2-1+) should treat `UserCanceled` and `WalletRejected` as
     * semantically interchangeable for retry/recovery decisions.
     *
     * Timeouts (`Timeout`), network failures (`NetworkOffline`), and storage
     * errors (`StorageCorrupt`) are classified by the plugin layer in
     * Stories 2-1 (watchdog) and 4-3 (storage recovery) — those don't come
     * from clientlib-ktx exceptions, they come from timers and Android
     * framework signals.
     */
    internal fun classifyException(e: Exception?, op: String): MwaError = when (e) {
        is MobileWalletAdapterClient.InvalidPayloadsException -> MwaError.ProtocolError
        is MobileWalletAdapterClient.NotSubmittedException -> MwaError.RpcFailed
        is JsonRpc20Client.JsonRpc20RemoteException -> when (e.code) {
            ProtocolContract.ERROR_AUTHORIZATION_FAILED ->
                if (op == "reauthorize") MwaError.TokenExpired else MwaError.NotConnected
            ProtocolContract.ERROR_NOT_SIGNED -> MwaError.UserCanceled
            ProtocolContract.ERROR_NOT_SUBMITTED -> MwaError.RpcFailed
            ProtocolContract.ERROR_CLUSTER_NOT_SUPPORTED,
            ProtocolContract.ERROR_INVALID_PAYLOADS,
            ProtocolContract.ERROR_NOT_CLONED,
            ProtocolContract.ERROR_TOO_MANY_PAYLOADS,
            -> MwaError.ProtocolError
            else -> MwaError.ProtocolError
        }
        else -> MwaError.ProtocolError
    }

    /**
     * Reveals the auth token as a String for clientlib-ktx consumption.
     *
     * **Security acknowledgment (DD-2):** Once revealed, the token lives in
     * the JVM's String heap until GC reclaims it. clientlib-ktx's
     * [MobileWalletAdapter.authToken] is typed `String?`, so this escape is
     * unavoidable at the library boundary. SDK redaction hygiene ends here.
     *
     * Round-trip identity: opaque String → UTF-8 bytes → [SecretString] →
     * [SecretString.reveal] → UTF-8 decode → original String. Lossless because
     * every valid Kotlin String round-trips through UTF-8 cleanly.
     */
    private fun SecretString.toOpaqueString(): String = String(reveal(), Charsets.UTF_8)

    /**
     * Base58 encoder using the Bitcoin alphabet. Solana RPC submits, explorers
     * (`https://explorer.solana.com/tx/<sig>`), and tooling across the
     * ecosystem expect signatures and public keys in this form.
     *
     * Algorithm: standard big-endian base-256→base-58 conversion with
     * leading-zero preservation. Based on the Bitcoin reference
     * implementation (public-domain idiom; same algorithm as bitcoinj).
     *
     * Implementing inline rather than pulling a dedicated base58 dep: the
     * plugin's existing `rpc-core` dep (pinned in `build.gradle.kts:93`) ships
     * a base58 codec transitively, but its package path is unstable across
     * minor versions and introducing that surface here for one caller costs
     * more than the ~20 lines of well-known algorithm.
     */
    private fun encodeBase58(bytes: ByteArray): String {
        if (bytes.isEmpty()) return ""
        var leadingZeros = 0
        while (leadingZeros < bytes.size && bytes[leadingZeros] == 0.toByte()) leadingZeros++
        val input = bytes.copyOf()
        val encoded = CharArray(bytes.size * 2) // upper bound: log(256)/log(58) ≈ 1.37
        var outputStart = encoded.size
        var inputStart = leadingZeros
        while (inputStart < input.size) {
            encoded[--outputStart] = BASE58_ALPHABET[divmod(input, inputStart, 256, 58)]
            if (input[inputStart] == 0.toByte()) inputStart++
        }
        // Strip any spurious leading '1's that the conversion produced beyond
        // the actual leading-zero byte count.
        while (outputStart < encoded.size && encoded[outputStart] == BASE58_ALPHABET[0]) outputStart++
        // Preserve one '1' per leading zero byte (base58 convention).
        repeat(leadingZeros) { encoded[--outputStart] = BASE58_ALPHABET[0] }
        return String(encoded, outputStart, encoded.size - outputStart)
    }

    private fun divmod(number: ByteArray, firstDigit: Int, base: Int, divisor: Int): Int {
        var remainder = 0
        for (i in firstDigit until number.size) {
            val digit = number[i].toInt() and 0xff
            val temp = remainder * base + digit
            number[i] = (temp / divisor).toByte()
            remainder = temp % divisor
        }
        return remainder
    }

    private companion object {
        const val TAG = "MWA"
        const val CORR_ID_LEN = 8

        val BASE58_ALPHABET: CharArray =
            "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz".toCharArray()
    }
}
