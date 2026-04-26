package com.godotengine.godot_solana_sdk.mwa.session

import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import org.json.JSONObject

/**
 * In-memory session state for the MWA plugin. Single instance held as a
 * companion-object field on `GDExtensionAndroidPlugin` (scaffold DI pattern
 * from Story 1-2; Story 2-3 hardens this into per-instance state).
 *
 * **Thread-safety contract (Story 2-1 T4).** Every accessor here is
 * `@Synchronized` on the instance lock. This is load-bearing because MWA
 * services multi-threaded callers: Godot main thread (reads for
 * `get_auth_token_fingerprint` etc.), clientlib-ktx coroutine dispatcher
 * threads (writes on authorize result), and future T5 JNI callback workers.
 * Without the lock, a compound op like [setIdentity] (3 fields) or [clear]
 * (all fields) would be observed half-applied by a racing reader — including
 * internally inconsistent states like `authToken=null` but
 * `authTokenFingerprint` still populated. The lock turns every public method
 * into an atomic unit; contention is negligible because MWA calls are slow
 * human-driven operations.
 *
 * **Caller-race caveat for `authToken` wipe.** [clear] / [clearOnLogout]
 * wipe the underlying [SecretString.clear] bytes BEFORE nulling the
 * reference, and they do so under the instance lock. But a caller that has
 * already obtained a reference via [getAuthToken] and is calling
 * `token.reveal()` outside the lock will see the zeroed bytes. Callers MUST
 * minimize the scope between [getAuthToken] and [SecretString.reveal] (or
 * copy the bytes immediately under the lock-holding getter).
 *
 * Story 2-1 T4 migrated `authToken` from raw `String?` to `SecretString?`
 * (closes CR-5). The scaffold-era `@UsedByGodot` methods and their
 * `MyComposable` consumers adapt at the seam: wrap inbound `String` from
 * clientlib's `walletAdapter.authToken` in a `SecretString`, and unwrap via
 * `String(token.reveal(), Charsets.UTF_8)` when handing the token back to
 * clientlib. The raw `String` is never held as a class field.
 */
internal class MwaSessionState {

    private var lastResult: Any? = null
    private var pendingAction: Int = 0
    private var pendingTransaction: ByteArray? = null
    private var pendingTextMessage: String = ""
    private var lastSignature: ByteArray? = null
    private var signingStatus: Int = 0
    private var connectedKey: ByteArray? = null
    private var cluster: Int = 0
    private var identityUri: String = ""
    private var iconUri: String = ""
    private var identityName: String = ""
    private var authToken: SecretString? = null
    private var authTokenFingerprint: String = ""
    private var walletIconUri: String = ""

    // Story 2-1 T6 — fields needed by `MobileWalletAdapter`'s C++ state
    // getters via `MwaJniContext::query_session_state`. T4 stored publicKey
    // bytes in [connectedKey] and cluster as an Int on the scaffold surface,
    // neither of which matches the String-typed GDScript getters. Adding
    // String-typed shadows so the compound [snapshotSessionStateJson] returns
    // the shape consumed by arch §7.1 state-getter contract without decoding
    // per call.
    private var publicKeyBase58: String = ""
    private var clusterName: String = ""
    private var walletLabel: String = ""

    @Synchronized
    fun setResult(result: Any?) {
        this.lastResult = result
    }

    @Synchronized
    fun getLastResult(): Any? = lastResult

    @Synchronized
    fun setAction(action: Int) {
        this.pendingAction = action
    }

    @Synchronized
    fun getPendingAction(): Int = pendingAction

    @Synchronized
    fun setTransaction(tx: ByteArray?) {
        this.pendingTransaction = tx
    }

    @Synchronized
    fun getPendingTransaction(): ByteArray? = pendingTransaction

    @Synchronized
    fun setTextMessage(msg: String) {
        this.pendingTextMessage = msg
    }

    @Synchronized
    fun getPendingTextMessage(): String = pendingTextMessage

    @Synchronized
    fun setSignature(sig: ByteArray?) {
        this.lastSignature = sig
    }

    @Synchronized
    fun getLastSignature(): ByteArray? = lastSignature

    @Synchronized
    fun setStatus(status: Int) {
        this.signingStatus = status
    }

    @Synchronized
    fun getSigningStatus(): Int = signingStatus

    @Synchronized
    fun clearStatus() {
        this.signingStatus = 0
    }

    @Synchronized
    fun setKey(key: ByteArray?) {
        this.connectedKey = key
    }

    @Synchronized
    fun getConnectedKey(): ByteArray? = connectedKey

    @Synchronized
    fun setCluster(cluster: Int) {
        this.cluster = cluster
    }

    @Synchronized
    fun getCluster(): Int = cluster

    @Synchronized
    fun setIdentity(identityUri: String, iconUri: String, identityName: String) {
        this.identityUri = identityUri
        this.iconUri = iconUri
        this.identityName = identityName
    }

    @Synchronized
    fun getIdentityUri(): String = identityUri

    @Synchronized
    fun getIconUri(): String = iconUri

    @Synchronized
    fun getIdentityName(): String = identityName

    @Synchronized
    fun setAuthToken(token: SecretString?) {
        this.authToken = token
    }

    @Synchronized
    fun getAuthToken(): SecretString? = authToken

    /** Story 2-1 T4 — AC-7 fingerprint surfaced for `MWA.get_auth_token_fingerprint()`. */
    @Synchronized
    fun setAuthTokenFingerprint(fingerprint: String) {
        this.authTokenFingerprint = fingerprint
    }

    @Synchronized
    fun getAuthTokenFingerprint(): String = authTokenFingerprint

    /** Story 2-1 T4 — wallet-provided icon URI for the UI (distinct from the game's [iconUri]). */
    @Synchronized
    fun setWalletIconUri(uri: String) {
        this.walletIconUri = uri
    }

    @Synchronized
    fun getWalletIconUri(): String = walletIconUri

    @Synchronized
    fun setPublicKeyBase58(encoded: String) {
        this.publicKeyBase58 = encoded
    }

    @Synchronized
    fun getPublicKeyBase58(): String = publicKeyBase58

    @Synchronized
    fun setClusterName(name: String) {
        this.clusterName = name
    }

    @Synchronized
    fun getClusterName(): String = clusterName

    @Synchronized
    fun setWalletLabel(label: String) {
        this.walletLabel = label
    }

    @Synchronized
    fun getWalletLabel(): String = walletLabel

    /**
     * Story 2-1 T6 — atomic JSON snapshot of the 5 state-getter values the
     * C++ node reads via `MwaJniContext::query_session_state`. The
     * `@Synchronized` keyword ensures callers see a consistent post-connect
     * tuple (no torn read between `authToken` going non-null and
     * `publicKeyBase58` populating).
     *
     * Shape (all keys present, strings empty when not connected):
     *   ```json
     *   {
     *     "is_connected": Bool,
     *     "public_key": String,
     *     "cluster": String,
     *     "wallet_label": String,
     *     "auth_token_fingerprint": String
     *   }
     *   ```
     * `is_connected` is true iff we hold a non-null `authToken` — the field
     * is wiped to null by [clear] / [clearOnLogout] so it's the authoritative
     * "session live?" signal.
     */
    @Synchronized
    fun snapshotSessionStateJson(): String = JSONObject().apply {
        put("is_connected", authToken != null)
        put("public_key", publicKeyBase58)
        put("cluster", clusterName)
        put("wallet_label", walletLabel)
        put("auth_token_fingerprint", authTokenFingerprint)
    }.toString()

    /**
     * Full wipe. Used by the scaffold-era plugin shutdown path and test
     * teardown. Wipes the [authToken]'s underlying bytes via
     * [SecretString.clear] before nulling the reference — so even if a
     * caller has already pulled the reference out via [getAuthToken], they
     * read zeros the moment this returns (modulo the caller-race caveat in
     * class kdoc). For an authorize/re-authorize retry that should preserve
     * the caller-provided identity, prefer [clearOnLogout].
     */
    @Synchronized
    fun clear() {
        authToken?.clear()
        lastResult = null
        pendingAction = 0
        pendingTransaction = null
        pendingTextMessage = ""
        lastSignature = null
        signingStatus = 0
        connectedKey = null
        cluster = 0
        identityUri = ""
        iconUri = ""
        identityName = ""
        authToken = null
        authTokenFingerprint = ""
        walletIconUri = ""
        publicKeyBase58 = ""
        clusterName = ""
        walletLabel = ""
    }

    /**
     * Clears only auth-related state — preserves [identityUri] / [iconUri] /
     * [identityName] / [cluster] so a subsequent `connect` can reuse the
     * caller's context. Wipes [authToken] bytes before nulling (same wipe
     * semantics as [clear]). Invoked on `mwaDisconnect` (Story 2-3) and
     * after `forget_all` rotation (Story 4-2).
     */
    @Synchronized
    fun clearOnLogout() {
        authToken?.clear()
        authToken = null
        authTokenFingerprint = ""
        walletIconUri = ""
        connectedKey = null
        lastResult = null
        signingStatus = 0
        publicKeyBase58 = ""
        clusterName = ""
        walletLabel = ""
    }
}
