package plugin.walletadapterandroid

import android.content.Context
import android.content.Intent
import android.util.Log
import androidx.annotation.VisibleForTesting
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClientImpl
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.AuthResult
import com.godotengine.godot_solana_sdk.mwa.generated.MwaError
import com.godotengine.godot_solana_sdk.mwa.plugin.DefaultNativeBridge
import com.godotengine.godot_solana_sdk.mwa.plugin.InflightMap
import com.godotengine.godot_solana_sdk.mwa.plugin.MwaDiagnostics
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.session.MwaSessionState
import com.godotengine.godot_solana_sdk.mwa.store.AuthTokenFingerprint
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.store.StorageCorruptException
import com.godotengine.godot_solana_sdk.mwa.store.bytesToLowerHex
import com.godotengine.godot_solana_sdk.mwa.util.SdkLog
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import com.solana.mobilewalletadapter.clientlib.TransactionResult
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.launch
import kotlinx.coroutines.withTimeoutOrNull
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.UsedByGodot
import org.json.JSONObject

class GDExtensionAndroidPlugin @VisibleForTesting internal constructor(
    godot: Godot,
    private val mwaClientFactory: () -> MwaClient,
    private val storeProvider: (Context) -> SecureTokenStore,
    private val nativeBridge: NativeBridge,
    private val senderProvider: () -> ActivityResultSender,
    private val scope: CoroutineScope,
    private val clock: () -> Long,
    private val inflightMap: InflightMap,
    private val diagnostics: MwaDiagnostics,
) : GodotPlugin(godot) {

    /** Production constructor — Godot instantiates the plugin via this reflection target. */
    constructor(godot: Godot) : this(
        godot = godot,
        mwaClientFactory = { MwaClientImpl() },
        storeProvider = { context -> SecureTokenStore(context) },
        nativeBridge = DefaultNativeBridge,
        senderProvider = {
            // Production wiring lands with T5/T6 (C++ node + GDScript facade); the
            // mwaAuthorize @UsedByGodot path is not yet reachable from Godot in T4.
            throw NotImplementedError(
                "ActivityResultSender wiring lands in Story 2-1 T5 (real JNI bridge) + T6 (node op body)",
            )
        },
        scope = CoroutineScope(SupervisorJob() + Dispatchers.Default),
        clock = System::currentTimeMillis,
        inflightMap = InflightMap(),
        diagnostics = MwaDiagnostics(),
    )

    companion object {
        val TAG: String = GDExtensionAndroidPlugin::class.java.simpleName

        /**
         * Watchdog caps per DD-23: effective = `min(internal_default, opts.timeout_ms)`
         * with both clamped to a sensible floor. Exposed so [mwaAuthorize] and tests
         * agree on the same math.
         */
        internal const val INTERNAL_WATCHDOG_DEFAULT_MS: Long = 60_000L

        // Single session-state instance shared between the plugin (Godot-facing API) and
        // ComposeWalletActivity (which reads it to dispatch the right @Composable). A formal DI
        // solution is out of scope for Story 1-2; Story 2-1 may introduce one. `internal` per
        // T4 visibility narrowing — game code should never touch plugin session state directly.
        internal val sessionState: MwaSessionState = MwaSessionState()

        init {
            try {
                Log.v(TAG, "Loading ${BuildConfig.GODOT_PLUGIN_NAME} library")
                System.loadLibrary(BuildConfig.GODOT_PLUGIN_NAME)
            } catch (e: UnsatisfiedLinkError) {
                Log.e(TAG, "Unable to load ${BuildConfig.GODOT_PLUGIN_NAME} shared library")
            }
        }

        /** DD-23 effective-watchdog clamp, pure-function for tests + [mwaAuthorize]. */
        internal fun effectiveWatchdog(callerTimeoutMs: Long): Long = if (callerTimeoutMs > 0L) {
            minOf(INTERNAL_WATCHDOG_DEFAULT_MS, callerTimeoutMs)
        } else {
            INTERNAL_WATCHDOG_DEFAULT_MS
        }
    }

    @VisibleForTesting
    internal fun diagnosticsForTest(): MwaDiagnostics = diagnostics

    @VisibleForTesting
    internal fun inflightMapForTest(): InflightMap = inflightMap

    override fun getPluginName(): String = BuildConfig.GODOT_PLUGIN_NAME

    override fun getPluginGDExtensionLibrariesPaths(): Set<String> =
        setOf("res://addons/${BuildConfig.GODOT_PLUGIN_NAME}/plugin.gdextension")

    /**
     * Story 2-1 T4 — authorize-path entry point. Called from the JNI
     * [MwaAndroidBridgeJni] on a worker thread (T5 lands the JNI impl; T6
     * wires the C++ node op body). Launches a coroutine that (a) parses the
     * identity dict, (b) calls [MwaClient.authorize] inside
     * [withTimeoutOrNull] at the DD-23 effective watchdog, (c) on success
     * persists the [CacheRecord] + fingerprint + session state, (d) emits a
     * single terminal signal via [nativeBridge] gated by
     * [InflightMap.tryTerminate] so AT MOST ONE terminal signal fires per
     * `requestId` (arch §7.3, AC-6 load-bearing).
     *
     * `identityJson` is `{"name": "...", "icon_uri": "...", "identity_uri": "..."}`.
     * `timeoutMs <= 0` selects [INTERNAL_WATCHDOG_DEFAULT_MS]; any positive
     * value is clamped down to the internal default.
     */
    @UsedByGodot
    fun mwaAuthorize(requestId: String, identityJson: String, cluster: String, chainId: String, timeoutMs: Long) {
        val effectiveMs = effectiveWatchdog(timeoutMs)
        inflightMap.register(requestId, clock())
        scope.launch {
            runCatching {
                runAuthorize(requestId, identityJson, cluster, chainId, effectiveMs)
            }.onFailure { ex ->
                SdkLog.e(TAG, requestId) {
                    "mwaAuthorize crashed: ${ex.javaClass.simpleName}: ${ex.message}"
                }
                if (inflightMap.tryTerminate(requestId)) {
                    nativeBridge.postMwaError(
                        buildErrorJson(
                            requestId = requestId,
                            error = MwaError.ProtocolError,
                            developerDetails = "mwaAuthorize coroutine crashed: ${ex.javaClass.simpleName}",
                            layer = "kotlin",
                            cause = ex.javaClass.simpleName,
                        ).toString(),
                    )
                } else {
                    diagnostics.incrementLateResult()
                    SdkLog.w(TAG, requestId) { "late_result outcome=crash" }
                }
            }
        }
    }

    private suspend fun runAuthorize(requestId: String, identityJson: String, cluster: String, chainId: String, effectiveMs: Long) {
        val parsed = parseIdentity(identityJson)
        if (parsed == null) {
            emitFailure(
                requestId,
                MwaError.ProtocolError,
                developerDetails = "identity.name required; identityJson=<redacted>",
            )
            return
        }

        val client = mwaClientFactory()
        try {
            val result = withTimeoutOrNull(effectiveMs) {
                client.authorize(senderProvider(), parsed.connectionIdentity, cluster, chainId)
            }
            when (result) {
                null -> emitTimeout(requestId, effectiveMs)
                is MwaResult.Success -> handleSuccess(requestId, parsed.identityUri, result.value)
                is MwaResult.Failure -> emitFailure(requestId, result.error, developerDetails = null)
            }
        } finally {
            runCatching { client.close() }
        }
    }

    private fun handleSuccess(requestId: String, identityUri: String, auth: AuthResult) {
        val walletPackage = auth.walletPackage.orEmpty()
        val nowMs = clock()
        val record = CacheRecord(
            schema = CacheRecord.SCHEMA_V1,
            authToken = auth.authToken,
            walletPackage = walletPackage,
            walletLabel = auth.accountLabel.orEmpty(),
            walletIconUri = auth.walletUriBase.orEmpty(),
            // Hex over base58 (D-T4-1): arch §3.1 says "base58 pubkey" but base58 would need a
            // new transitive dep; hex is consistent with our existing helpers and the field
            // is internal — GDScript consumers read from `MwaSessionState` getters, not the record.
            publicKey = bytesToLowerHex(auth.publicKey),
            cluster = auth.cluster,
            chainId = auth.chainId,
            identityUri = identityUri,
            issuedAtMs = nowMs,
            lastUsedAtMs = nowMs,
        )
        val key = CacheKey(auth.cluster, auth.chainId, walletPackage, identityUri)
        val fingerprint: String = try {
            val store = storeProvider(requireContext())
            store.putToken(key, record)
            val salt = store.getOrCreatePerInstallSalt()
            AuthTokenFingerprint.compute(auth.authToken.reveal(), salt)
        } catch (ex: StorageCorruptException) {
            SdkLog.w(TAG, requestId) {
                "storage corrupt on authorize success: ${ex.cause?.javaClass?.simpleName}"
            }
            emitFailure(
                requestId,
                MwaError.StorageCorrupt,
                developerDetails = "EncryptedSharedPreferences corrupt: ${ex.cause?.javaClass?.simpleName}",
            )
            return
        }

        sessionState.setAuthToken(auth.authToken)
        sessionState.setAuthTokenFingerprint(fingerprint)
        sessionState.setWalletIconUri(auth.walletUriBase.orEmpty())
        sessionState.setKey(auth.publicKey)

        if (inflightMap.tryTerminate(requestId)) {
            nativeBridge.postConnectCompleted(
                requestId,
                buildSuccessJson(
                    requestId = requestId,
                    fingerprint = fingerprint,
                    publicKeyHex = bytesToLowerHex(auth.publicKey),
                    walletLabel = auth.accountLabel.orEmpty(),
                    walletIconUri = auth.walletUriBase.orEmpty(),
                    cluster = auth.cluster,
                ).toString(),
            )
        } else {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=success" }
        }
    }

    private fun emitTimeout(requestId: String, effectiveMs: Long) {
        if (inflightMap.tryTerminate(requestId)) {
            // A-12 1-param: `request_id` is embedded inside the payload, not a separate seam arg.
            nativeBridge.postMwaTimeout(buildTimeoutJson(requestId, effectiveMs).toString())
        } else {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=timeout" }
        }
    }

    private fun emitFailure(requestId: String, error: MwaError, developerDetails: String?) {
        if (inflightMap.tryTerminate(requestId)) {
            // A-12 1-param: `request_id` is embedded inside the payload (A-14 10-key shape).
            nativeBridge.postMwaError(
                buildErrorJson(
                    requestId = requestId,
                    error = error,
                    developerDetails = developerDetails,
                    layer = "kotlin",
                    cause = null,
                ).toString(),
            )
        } else {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=failure code=${error.code}" }
        }
    }

    /**
     * `requireContext()` defers to the Godot activity; tests override via
     * [storeProvider]. If the activity is unavailable (plugin lifecycle
     * teardown) we propagate as a typed error so the coroutine's
     * `runCatching` catches it cleanly.
     */
    private fun requireContext(): Context = godot.getActivity()
        ?: throw IllegalStateException("Godot activity not available; plugin likely torn down")

    // ---------------- scaffold-era @UsedByGodot surface (STAY AS-IS per 2-1 Dev Notes) ----------------

    @UsedByGodot
    fun connectWallet(cluster: Int, uri: String, icon: String, name: String) {
        if (sessionState.getLastResult() is TransactionResult.Success<*>) {
            return
        }
        sessionState.setIdentity(uri, icon, name)
        sessionState.setCluster(cluster)
        sessionState.setAction(0)
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getConnectionStatus(): Int {
        val last = sessionState.getLastResult()
        return when {
            last == null -> 0
            last is TransactionResult.Success<*> -> 1
            else -> 2
        }
    }

    @UsedByGodot
    fun getSigningStatus(): Int = sessionState.getSigningStatus()

    @UsedByGodot
    fun getConnectedKey(): ByteArray {
        sessionState.setAction(0)
        return sessionState.getConnectedKey() ?: ByteArray(0)
    }

    @UsedByGodot
    fun signTransaction(serializedTransaction: ByteArray) {
        sessionState.setAction(1)
        sessionState.setTransaction(serializedTransaction)
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun signTextMessage(textMessage: String) {
        sessionState.setAction(2)
        sessionState.setTextMessage(textMessage)
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getMessageSignature(): ByteArray = sessionState.getLastSignature() ?: ByteArray(0)

    @UsedByGodot
    fun getLatestAction(): Int = sessionState.getPendingAction()

    @UsedByGodot
    fun clearState() {
        sessionState.clearStatus()
    }
}

// ---------------- JSON payload builders (file-private helpers) ----------------

private data class ParsedIdentity(
    val connectionIdentity: ConnectionIdentity,
    val identityUri: String,
)

private fun parseIdentity(identityJson: String): ParsedIdentity? {
    return try {
        val obj = JSONObject(identityJson)
        val name = obj.optString("name", "")
        val iconUri = obj.optString("icon_uri", "")
        val identityUri = obj.optString("identity_uri", "")
        if (name.isEmpty() || identityUri.isEmpty()) return null
        ParsedIdentity(
            connectionIdentity = ConnectionIdentity(
                identityUri = android.net.Uri.parse(identityUri),
                iconUri = android.net.Uri.parse(iconUri),
                identityName = name,
            ),
            identityUri = identityUri,
        )
    } catch (_: org.json.JSONException) {
        null
    }
}

/**
 * `mwa_error` 10-key shape per Amendment A-14 (CR-26 resolution). `message`
 * is the AC-3 breadcrumb; `developer_details` is the canonical field consumers
 * should read.
 */
private fun buildErrorJson(requestId: String, error: MwaError, developerDetails: String?, layer: String, cause: String?): JSONObject =
    JSONObject().apply {
        put("request_id", requestId)
        put("code", error.code)
        put("message", error.defaultUserMessage)
        put("user_message", error.defaultUserMessage)
        put("developer_details", developerDetails ?: "")
        put("recoverable", error.recoverable)
        put("retry_hint", error.retryHint)
        put("layer", layer)
        put("cause", cause ?: JSONObject.NULL)
        put("source_method", "connect")
    }

private fun buildTimeoutJson(requestId: String, watchdogMs: Long): JSONObject = JSONObject().apply {
    put("request_id", requestId)
    put("source_method", "connect")
    put("elapsed_ms", watchdogMs)
    put("watchdog_ms", watchdogMs)
}

private fun buildSuccessJson(
    requestId: String,
    fingerprint: String,
    publicKeyHex: String,
    walletLabel: String,
    walletIconUri: String,
    cluster: String,
): JSONObject = JSONObject().apply {
    put("request_id", requestId)
    put("auth_token_fingerprint", fingerprint)
    put("public_key", publicKeyHex)
    put("wallet_label", walletLabel)
    put("wallet_icon_uri", walletIconUri)
    put("cluster", cluster)
}
