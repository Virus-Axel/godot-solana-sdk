package com.godotengine.godot_solana_sdk.mwa.client

import com.godotengine.godot_solana_sdk.mwa.client.dto.AuthResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignAndSendResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignResult
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity

/**
 * Kotlin abstraction over the Mobile Wallet Adapter protocol. Consumed by the
 * Godot plugin; has two implementations:
 *  - `MwaClientImpl` — production, wraps `com.solana.mobilewalletadapter.clientlib.MobileWalletAdapter`.
 *  - `FakeMwaClient` — test double, reads canned responses from `testdata/mwa-fixtures/`.
 *
 * Every op returns an [MwaResult] rather than throwing for expected failures
 * (user canceled, wallet rejected, token expired, etc.). [MwaResult.Failure] carries
 * a typed [com.godotengine.godot_solana_sdk.mwa.generated.MwaError]; only unexpected
 * programming errors propagate as exceptions.
 *
 * All ops take [ActivityResultSender] per call — it is ephemeral and bound to the
 * calling Activity. The MwaClient itself holds no Activity reference.
 *
 * [authToken] parameters are opaque: `SecretString` wraps the UTF-8 bytes of the
 * base64 string the wallet returned. Impls must not decode / re-encode — see
 * `AuthResult` KDoc for the identity-transform invariant.
 */
interface MwaClient {

    /**
     * First-time authorization. Surfaces the wallet picker UI; after user approval the
     * wallet returns an opaque [AuthResult.authToken] (base64 bytes wrapped in a
     * [SecretString]) plus the player's [AuthResult.publicKey] (32-byte ed25519 key).
     *
     * Returns `Failure(NoMwaWalletInstalled)` when no MWA-compatible wallet is installed,
     * `Failure(UserCanceled)` when the user dismisses the chooser, or
     * `Failure(ProtocolError)` if `(cluster, chainId)` are inconsistent (see
     * `MwaClientImpl`'s `canonicalChainIdFor`).
     */
    suspend fun authorize(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        cluster: String,
        chainId: String,
    ): MwaResult<AuthResult>

    /**
     * Silent re-authorization using a previously cached [authToken]. No wallet UI is
     * surfaced (per NFR-5). Callers must pass the **same** [SecretString] obtained from
     * a prior `authorize` / `reauthorize` result; passing a different token returns
     * `Failure(TokenExpired)`. `(cluster, chainId)` must match the auth-time values;
     * mismatch returns `Failure(ProtocolError)` without touching clientlib.
     */
    suspend fun reauthorize(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        cluster: String,
        chainId: String,
    ): MwaResult<AuthResult>

    /**
     * Revoke [authToken] with the wallet. Wallet-side the token is marked expired;
     * caller is expected to also clear it locally (the plugin's SecureTokenStore owns
     * that — see Story 4-1). Returns `Failure(WalletUninstalled)` if the wallet app is
     * gone (local wipe still proceeds per 4-1 AC-2).
     */
    @Suppress("ktlint:standard:function-signature")
    suspend fun deauthorize(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
    ): MwaResult<Unit>

    /**
     * Close the current MWA session on the MwaClient side without revoking the token.
     * The cached [authToken] stays valid for a subsequent `reauthorize`. Does not
     * surface wallet UI and does not touch the wallet process (local-only drop).
     */
    suspend fun disconnect(): MwaResult<Unit>

    /**
     * Request detached ed25519 signatures over [messages] under the given [addresses]
     * (32-byte ed25519 public keys). Returns [SignResult.signedPayloads] in the same
     * order as input — each element is the 64-byte detached signature for the
     * corresponding message. `messages.size == addresses.size` is caller's invariant.
     */
    suspend fun signMessages(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        messages: List<ByteArray>,
        addresses: List<ByteArray>,
    ): MwaResult<SignResult>

    /**
     * Request wallet-signed Solana transactions. Wire format is the canonical
     * serialized-transaction bytes (legacy or v0). Signed payloads returned are
     * re-serialized transactions with the user's signature attached; the caller is
     * responsible for RPC submission. Use [signAndSendTransactions] for the fused
     * sign-plus-submit variant.
     */
    suspend fun signTransactions(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        transactions: List<ByteArray>,
    ): MwaResult<SignResult>

    /**
     * Sign and submit [transactions] to the Solana RPC endpoint for the given
     * [cluster]. Wallet co-submits; returns the per-transaction base58 signatures.
     *
     * Note on parameter asymmetry: this op takes [cluster] (used by the wallet to
     * pick the RPC endpoint it submits to) but does NOT take `chainId`. Rationale:
     * the `chainId` is an MWA-protocol identity used to bind the auth flow to a
     * specific chain — it is established by the preceding `authorize`/`reauthorize`
     * call and then implicit for subsequent ops under the same token. RPC submission
     * only needs the cluster name to route; any drift between the cluster used here
     * and the one bound at auth time is a protocol-level concern the wallet surfaces
     * via `TokenExpired` or `ReauthRequired`.
     */
    suspend fun signAndSendTransactions(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        transactions: List<ByteArray>,
        cluster: String,
    ): MwaResult<SignAndSendResult>

    /**
     * Cancel any in-flight operations held by the impl. No-op on [FakeMwaClient].
     * On [MwaClientImpl], cancels the per-call coroutine scope if one is still live.
     *
     * **Idempotent:** calling `close()` twice (e.g. from overlapping lifecycle callbacks
     * on Android — `onStop` then `onDestroy`) MUST NOT throw. Impls that hold a
     * `Closeable` resource should null it out after the first call and short-circuit
     * on subsequent invocations.
     */
    fun close()
}
