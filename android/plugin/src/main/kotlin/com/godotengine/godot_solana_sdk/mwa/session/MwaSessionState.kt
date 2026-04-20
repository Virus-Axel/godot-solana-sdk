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
