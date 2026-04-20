package com.godotengine.godot_solana_sdk.mwa.session

class MwaSessionState {

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

    // FORWARD-CONTRACT (Story 2-1): `authToken` is intentionally a raw
    // `String?` here, NOT a `SecretString?`. Story 1-2 only extracts the
    // shape out of the scaffold globals; Story 2-1 migrates the field to
    // `SecretString?` and wires `SecureTokenStore` for on-disk encryption
    // (see DD-14 + concerns.md CR-5). Until that migration lands:
    //   * NEVER pass `authToken` (or the String returned by `getAuthToken()`)
    //     to any `Log.*` call -- enforced by `ci/grep_bans.sh` pattern #1
    //     (`Log.(v|d|i|w|e)(.*?authToken`) which fails CI on any leak.
    //   * NEVER concatenate the token into a log message string OUTSIDE a
    //     lambda -- enforced by ci/grep_bans.sh pattern #4 (`SdkLog lambda
    //     + interpolation outside lambda`).
    //   * Callers that must transmit the token across IPC / network must
    //     treat the `String?` as sensitive for the duration of the
    //     reference and clear references promptly; Story 2-1 replaces this
    //     manual discipline with the `SecretString` wrapper API.
    private var authToken: String? = null

    fun setResult(result: Any?) {
        this.lastResult = result
    }
    fun getLastResult(): Any? = lastResult

    fun setAction(action: Int) {
        this.pendingAction = action
    }
    fun getPendingAction(): Int = pendingAction

    fun setTransaction(tx: ByteArray?) {
        this.pendingTransaction = tx
    }
    fun getPendingTransaction(): ByteArray? = pendingTransaction

    fun setTextMessage(msg: String) {
        this.pendingTextMessage = msg
    }
    fun getPendingTextMessage(): String = pendingTextMessage

    fun setSignature(sig: ByteArray?) {
        this.lastSignature = sig
    }
    fun getLastSignature(): ByteArray? = lastSignature

    fun setStatus(status: Int) {
        this.signingStatus = status
    }
    fun getSigningStatus(): Int = signingStatus
    fun clearStatus() {
        this.signingStatus = 0
    }

    fun setKey(key: ByteArray?) {
        this.connectedKey = key
    }
    fun getConnectedKey(): ByteArray? = connectedKey

    fun setCluster(cluster: Int) {
        this.cluster = cluster
    }
    fun getCluster(): Int = cluster

    fun setIdentity(identityUri: String, iconUri: String, identityName: String) {
        this.identityUri = identityUri
        this.iconUri = iconUri
        this.identityName = identityName
    }
    fun getIdentityUri(): String = identityUri
    fun getIconUri(): String = iconUri
    fun getIdentityName(): String = identityName

    fun setAuthToken(token: String?) {
        this.authToken = token
    }
    fun getAuthToken(): String? = authToken

    fun clear() {
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
    }
}
