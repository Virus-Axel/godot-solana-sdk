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
import com.godotengine.godot_solana_sdk.mwa.util.Base58
import com.godotengine.godot_solana_sdk.mwa.util.SdkLog
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import com.solana.mobilewalletadapter.clientlib.TransactionResult
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.CoroutineDispatcher
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
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
    private val mainDispatcher: CoroutineDispatcher = Dispatchers.Main,
) : GodotPlugin(godot) {

    init {
        // Story 2-1 T5 — JNI entry points on the companion (`mwaAuthorizeFromJni` etc.)
        // forward through this instance. Last-writer wins on re-init (uncommon but
        // possible under test runners that reload the plugin class).
        @Suppress("LeakingThis")
        instance = this
    }

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
        mainDispatcher = Dispatchers.Main,
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

        /**
         * Story 2-1 T5 — active plugin instance, set in [init]. JNI forward-wrapper
         * methods on this companion (`mwaAuthorizeFromJni` etc.) need to find the
         * live instance to delegate. `@Volatile` for cross-thread visibility (JNI
         * calls arrive on worker threads).
         */
        @Volatile
        @JvmStatic
        internal var instance: GDExtensionAndroidPlugin? = null

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

        // ========== Story 2-1 T5 — JNI forward wrappers (C++ → Kotlin) ==========
        //
        // `@JvmStatic` so C++ can invoke via `CallStaticVoidMethod`. Each wrapper
        // forwards to the active [instance]; if null (plugin class loaded but no
        // instance created yet), we log at ERROR and return — production should
        // never hit this path because `GDExtensionAndroidPlugin` is a singleton
        // plugin Godot instantiates at load time.
        //
        // JNI symbol lookup keys on these NAMES — do NOT rename without updating
        // `GetStaticMethodID("mwaAuthorizeFromJni", ...)` calls in
        // `src/mwa/mwa_android_bridge_jni.cpp::JNI_OnLoad`.

        @JvmStatic
        fun mwaAuthorizeFromJni(reqId: String, identityJson: String, cluster: String, chainId: String, timeoutMs: Long) {
            val plugin = instance
            if (plugin == null) {
                Log.e(TAG, "mwaAuthorizeFromJni: instance is null (plugin class loaded but not instantiated)")
                // Preserve terminal-signal invariant — emit directly via DefaultNativeBridge
                // since there's no plugin instance to route through nativeBridge.
                DefaultNativeBridge.postMwaError(buildInstanceNullErrorJson(reqId, "connect").toString())
                return
            }
            plugin.mwaAuthorize(reqId, identityJson, cluster, chainId, timeoutMs)
        }

        /** Emit a terminal mwa_error for ops whose wrapper hits null instance (AC-6). */
        private fun emitInstanceNullError(reqId: String, sourceMethod: String) {
            DefaultNativeBridge.postMwaError(buildInstanceNullErrorJson(reqId, sourceMethod).toString())
        }

        // Story 2-2 wires the real reauthorize path; T5 logs at I-level and
        // emits NOT_CONNECTED via the NativeBridge so GDScript consumers
        // observe a typed error instead of a silent drop. Remaining params
        // match the mwaAuthorizeFromJni JNI signature for symmetry (same
        // 5-arg authorize_sig in JNI_OnLoad).
        @JvmStatic
        @Suppress("UNUSED_PARAMETER")
        fun mwaReauthorizeFromJni(reqId: String, identityJson: String, cluster: String, chainId: String, timeoutMs: Long) {
            Log.i(TAG, "mwaReauthorizeFromJni: Story 2-2 scope; reqId=$reqId")
            instance?.emitNotImplemented(reqId, "reauthorize") ?: emitInstanceNullError(reqId, "reauthorize")
        }

        @JvmStatic
        fun mwaDisconnectFromJni(reqId: String) {
            Log.i(TAG, "mwaDisconnectFromJni: Story 2-3 scope; reqId=$reqId")
            instance?.emitNotImplemented(reqId, "disconnect") ?: emitInstanceNullError(reqId, "disconnect")
        }

        @JvmStatic
        fun mwaDeauthorizeFromJni(reqId: String) {
            Log.i(TAG, "mwaDeauthorizeFromJni: Story 4-1 scope; reqId=$reqId")
            instance?.emitNotImplemented(reqId, "deauthorize") ?: emitInstanceNullError(reqId, "deauthorize")
        }

        @JvmStatic
        fun mwaSignMessagesFromJni(reqId: String) {
            Log.i(TAG, "mwaSignMessagesFromJni: Story 3-1 scope; reqId=$reqId")
            instance?.emitNotImplemented(reqId, "sign_messages") ?: emitInstanceNullError(reqId, "sign_messages")
        }

        @JvmStatic
        fun mwaSignTransactionsFromJni(reqId: String) {
            Log.i(TAG, "mwaSignTransactionsFromJni: Story 3-2 scope; reqId=$reqId")
            instance?.emitNotImplemented(reqId, "sign_transactions") ?: emitInstanceNullError(reqId, "sign_transactions")
        }

        @JvmStatic
        fun mwaSignAndSendFromJni(reqId: String) {
            Log.i(TAG, "mwaSignAndSendFromJni: Story 3-3 scope; reqId=$reqId")
            instance?.emitNotImplemented(reqId, "sign_and_send") ?: emitInstanceNullError(reqId, "sign_and_send")
        }

        @JvmStatic
        fun mwaForgetAllFromJni(reqId: String) {
            Log.i(TAG, "mwaForgetAllFromJni: Story 4-2 scope; reqId=$reqId")
            instance?.emitNotImplemented(reqId, "forget_all") ?: emitInstanceNullError(reqId, "forget_all")
        }

        @JvmStatic
        fun mwaGetDiagnosticsFromJni(reqId: String) {
            Log.i(TAG, "mwaGetDiagnosticsFromJni: Story 5-2 scope; reqId=$reqId")
            instance?.emitNotImplemented(reqId, "get_diagnostics") ?: emitInstanceNullError(reqId, "get_diagnostics")
        }

        // ========== Story 2-1 T5 — JNI callback declarations (Kotlin → C++) ==========
        //
        // `external fun` declarations resolved at `System.loadLibrary(...)` (above)
        // to the `Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_*`
        // JNIEXPORT functions in `src/mwa/mwa_android_bridge_jni.cpp`.
        //
        // Invoked by [DefaultNativeBridge] to post signals through the C++
        // dispatcher. Arity matches A-12: `postConnectCompletedNative` is 2-arg
        // (request_id + result dict JSON); the rest are 1-arg (request_id embedded
        // in the payload dict).
        //
        // WARNING per NativeBridge kdoc: every `*DictJson` string parameter carries
        // secret material (auth_token on the connect path, correlation metadata on
        // error/timeout/cancel/reauth paths). Callers MUST NOT log or interpolate
        // these values. `ci/grep_bans.sh` pattern 8 enforces at CI-time.

        @JvmStatic
        external fun postConnectCompletedNative(reqId: String, resultDictJson: String)

        @JvmStatic
        external fun postMwaErrorNative(errorDictJson: String)

        @JvmStatic
        external fun postMwaTimeoutNative(timeoutDictJson: String)

        @JvmStatic
        external fun postMwaCancelledLifecycleNative(cancelledDictJson: String)

        @JvmStatic
        external fun postReauthRequiredNative(reauthDictJson: String)
    }

    /**
     * Story 2-1 T5 — emits a typed NOT_CONNECTED mwa_error for ops whose Kotlin
     * wiring lands in a later story (2-2 / 2-3 / 3-x / 4-x / 5-2). Prevents
     * silent drops when a gamedev invokes a yet-to-ship op from GDScript via
     * the MWA facade.
     */
    internal fun emitNotImplemented(reqId: String, sourceMethod: String) {
        nativeBridge.postMwaError(
            buildErrorJson(
                requestId = reqId,
                error = MwaError.NotConnected,
                developerDetails = "Op '$sourceMethod' Kotlin wiring lands in a later Story 2-1 sibling; " +
                    "this JNI entry point is reachable but returns an immediate error.",
                layer = "kotlin",
                cause = null,
                sourceMethod = sourceMethod,
            ).toString(),
        )
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
        if (!inflightMap.register(requestId, clock())) {
            // Caller contract violation: requestId must be unique per call. Rather
            // than silently launching a second coroutine racing for the terminal
            // signal, log + emit a typed PROTOCOL_ERROR so the caller's bug
            // surfaces instead of hanging on a signal that may never come.
            SdkLog.w(TAG, requestId) { "duplicate requestId at mwaAuthorize; refusing re-register" }
            nativeBridge.postMwaError(
                buildErrorJson(
                    requestId = requestId,
                    error = MwaError.ProtocolError,
                    developerDetails = "duplicate requestId — caller must use fresh UUIDs per op",
                    layer = "kotlin",
                    cause = "duplicate_request_id",
                    sourceMethod = "connect",
                ).toString(),
            )
            return
        }
        scope.launch {
            try {
                runAuthorize(requestId, identityJson, cluster, chainId, effectiveMs)
            } catch (ce: CancellationException) {
                // Preserve cancellation propagation — graceful scope teardown, not a crash.
                // Do NOT emit a terminal signal; the caller's scope cancel expects
                // everything to unwind cleanly.
                throw ce
            } catch (ex: Throwable) {
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
                            sourceMethod = "connect",
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

    private suspend fun handleSuccess(requestId: String, identityUri: String, auth: AuthResult) {
        val walletPackage = auth.walletPackage.orEmpty()
        val nowMs = clock()
        // Arch §3.1 + §3.2: public_key is base58-encoded in both the CacheRecord AND the
        // connect_completed signal payload. Previous D-T4-1 (hex) was off-spec — corrected
        // here with a minimal in-module Base58 encoder (no new transitive dep).
        val publicKeyBase58 = Base58.encode(auth.publicKey)
        val record = CacheRecord(
            schema = CacheRecord.SCHEMA_V1,
            authToken = auth.authToken,
            walletPackage = walletPackage,
            walletLabel = auth.accountLabel.orEmpty(),
            walletIconUri = auth.walletUriBase.orEmpty(),
            publicKey = publicKeyBase58,
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

        // T5 thread-marshalling fix: sessionState writes happen on Dispatchers.Main so
        // scaffold @UsedByGodot getters (called on Godot's main thread) observe the
        // compound (authToken + fingerprint + walletIconUri + connectedKey) state
        // atomically — no torn reads between the 4 writes. Individual setters remain
        // @Synchronized for reference visibility; the withContext binds the sequence
        // to the main thread so a scaffold reader can't interleave.
        withContext(mainDispatcher) {
            sessionState.setAuthToken(auth.authToken)
            sessionState.setAuthTokenFingerprint(fingerprint)
            sessionState.setWalletIconUri(auth.walletUriBase.orEmpty())
            sessionState.setKey(auth.publicKey)
        }

        if (inflightMap.tryTerminate(requestId)) {
            nativeBridge.postConnectCompleted(
                requestId,
                buildSuccessJson(
                    requestId = requestId,
                    fingerprint = fingerprint,
                    publicKeyBase58 = publicKeyBase58,
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
            nativeBridge.postMwaTimeout(buildTimeoutJson(requestId, effectiveMs, "connect").toString())
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
                    sourceMethod = "connect",
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
private fun buildErrorJson(
    requestId: String,
    error: MwaError,
    developerDetails: String?,
    layer: String,
    cause: String?,
    sourceMethod: String,
): JSONObject = JSONObject().apply {
    put("request_id", requestId)
    put("code", error.code)
    // CR-40: `message` + `user_message` currently carry identical content because
    // MwaError (codegen from Story 1-1) only exposes one default-message field.
    put("message", error.defaultUserMessage)
    put("user_message", error.defaultUserMessage)
    put("developer_details", developerDetails ?: "")
    put("recoverable", error.recoverable)
    put("retry_hint", error.retryHint)
    put("layer", layer)
    put("cause", cause ?: JSONObject.NULL)
    put("source_method", sourceMethod)
}

private fun buildTimeoutJson(requestId: String, watchdogMs: Long, sourceMethod: String): JSONObject {
    val json = JSONObject()
    json.put("request_id", requestId)
    json.put("source_method", sourceMethod)
    json.put("elapsed_ms", watchdogMs)
    json.put("watchdog_ms", watchdogMs)
    return json
}

/** Arch §3.2 specifies `public_key` as base58-encoded in `connect_completed`. */
private fun buildSuccessJson(
    requestId: String,
    fingerprint: String,
    publicKeyBase58: String,
    walletLabel: String,
    walletIconUri: String,
    cluster: String,
): JSONObject = JSONObject().apply {
    put("request_id", requestId)
    put("auth_token_fingerprint", fingerprint)
    put("public_key", publicKeyBase58)
    put("wallet_label", walletLabel)
    put("wallet_icon_uri", walletIconUri)
    put("cluster", cluster)
}

/**
 * Builds an mwa_error payload for the JNI-forward-wrapper null-instance case
 * — when `GDExtensionAndroidPlugin.Companion.instance` is null but the C++ side
 * has already dispatched a call. Used by the companion's @JvmStatic wrappers
 * to emit a terminal signal via [DefaultNativeBridge] directly (no plugin
 * instance exists to route through the injected NativeBridge). Preserves the
 * terminal-signal invariant (AC-6) for the degenerate "class loaded but
 * instance null" state.
 */
internal fun buildInstanceNullErrorJson(requestId: String, sourceMethod: String): JSONObject {
    val json = JSONObject()
    json.put("request_id", requestId)
    json.put("code", MwaError.NotConnected.code)
    json.put("message", "MWA plugin not yet initialized")
    json.put("user_message", MwaError.NotConnected.defaultUserMessage)
    json.put("developer_details", "$sourceMethod called before GDExtensionAndroidPlugin instance was set")
    json.put("recoverable", MwaError.NotConnected.recoverable)
    json.put("retry_hint", MwaError.NotConnected.retryHint)
    json.put("layer", "kotlin")
    json.put("cause", "instance_null")
    json.put("source_method", sourceMethod)
    return json
}
