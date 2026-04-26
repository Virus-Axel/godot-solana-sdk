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

        // Story 2-2 T1 stub-first scaffolding: delegates to the instance method whose
        // body is `TODO("Story 2-2 T2 fills in")`. T2 replaces the TODO with real impl.
        @JvmStatic
        fun mwaReauthorizeFromJni(reqId: String, identityJson: String, cluster: String, chainId: String, timeoutMs: Long) {
            instance?.mwaReauthorize(reqId, identityJson, cluster, chainId, timeoutMs)
                ?: emitInstanceNullError(reqId, "reauthorize")
        }

        @JvmStatic
        fun mwaDisconnectFromJni(reqId: String) {
            instance?.mwaDisconnect(reqId) ?: emitInstanceNullError(reqId, "disconnect")
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

        /**
         * Story 2-1 T6 — synchronous state-snapshot pull from the C++
         * `MwaJniContext::query_session_state`. Returns a JSON object with the
         * 5 keys `MobileWalletAdapter`'s state getters need:
         * `is_connected`, `public_key`, `cluster`, `wallet_label`,
         * `auth_token_fingerprint`. Atomic snapshot via
         * [MwaSessionState.snapshotSessionStateJson] (single `@Synchronized`
         * critical section); avoids 5 separate JNI `CallStatic` calls that
         * could interleave with a concurrent `handleSuccess` / `clear` write.
         *
         * Called on the Godot main thread; must be cheap (the critical section
         * copies a handful of strings — no network, no disk, no coroutine).
         * Empty JSON (`{}`-equivalent defaults) when [instance] is null.
         */
        @JvmStatic
        fun mwaQuerySessionStateFromJni(): String {
            // Unconditional read from the companion-object `sessionState` —
            // even if the plugin `instance` is null (class loaded pre-ctor),
            // the state is default-initialized to "not connected" and the
            // serializer returns the empty-defaults shape. Matches the NoOp
            // shape on the C++ side.
            return sessionState.snapshotSessionStateJson()
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

        /**
         * Story 2-3 T2 — 2-param `disconnect_completed` JNI callback per A-12.
         * Kotlin → C++ `Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_postDisconnectCompletedNative`
         * (Story 2-3 T3) → dispatcher.post("disconnect_completed", Array::make(reqId, result_dict)).
         *
         * `resultDictJson` is `{request_id, source_method: "disconnect"}` — NO
         * secret material on the disconnect path. The warning is convention
         * uniformity (same seam shape as connect_completed).
         */
        @JvmStatic
        external fun postDisconnectCompletedNative(reqId: String, resultDictJson: String)

        /**
         * Story 2-2 T1 stub — 2-param `reauthorize_completed` JNI callback per A-12.
         * Kotlin → C++ `Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_postReauthorizeCompletedNative`
         * (Story 2-2 T3) → dispatcher.post("reauthorize_completed", Array::make(reqId, result_dict)).
         *
         * `resultDictJson` carries `{request_id, auth_token_fingerprint, public_key,
         * wallet_label, wallet_icon_uri, cluster}` per DD-2-2-4. WARNING — do NOT log
         * or interpolate `resultDictJson` (contains auth_token_fingerprint).
         */
        @JvmStatic
        external fun postReauthorizeCompletedNative(reqId: String, resultDictJson: String)

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

    /**
     * Story 2-3 T2 — `disconnect` entry point. Called from the JNI
     * [MwaAndroidBridgeJni] on a worker thread. Pure client-side wipe
     * (DD-2-3-1): wipes [MwaSessionState] auth-only fields via
     * [MwaSessionState.clearOnLogout] but leaves [SecureTokenStore] intact
     * so a subsequent `reauthorize` can restore the session silently (Story
     * 2-2).
     *
     * Dispatcher-marshalled to [mainDispatcher] via `scope.launch +
     * withContext` for DD-22 parity with [mwaAuthorize]. No watchdog —
     * the body has no suspension points after the dispatcher hop.
     *
     * Ordering (DD-2-3-2): `register → tryTerminate CAS → clearOnLogout →
     * post`. The CAS gates both state mutation AND emission. Any
     * implementation that wipes before the CAS is a Rule 2 deviation
     * requiring an amendment (breaks state-machine discipline uniformity).
     *
     * Idempotence (AC-3): double-disconnect with DIFFERENT `requestId`
     * produces two independent register/CAS/wipe/emit cycles → two
     * `disconnect_completed` emissions, one per call. The wipe is a no-op
     * on already-cleared state.
     *
     * **AC-3 caller contract:** each `MWA.disconnect()` GDScript call
     * produces a FRESH `requestId` via C++ `generate_request_id()` (D-4 at
     * `src/mwa/mobile_wallet_adapter.cpp`). A caller that re-uses the same
     * `requestId` for a retry hits the `InflightMap.register` CAS below
     * (duplicate_request_id branch) and gets `mwa_error{PROTOCOL_ERROR,
     * cause=duplicate_request_id}` — NOT a second `disconnect_completed`.
     * AC-3's "disconnect_completed still fires" semantics therefore apply
     * to the GDScript-observable flow (distinct request_ids per call), NOT
     * to same-id programmatic retry within Kotlin.
     */
    @UsedByGodot
    fun mwaDisconnect(requestId: String) {
        scope.launch {
            withContext(mainDispatcher) {
                // Step 1: reserve the slot. Same-requestId duplicate is a caller contract
                // violation — emit PROTOCOL_ERROR and abort without touching state.
                if (!inflightMap.register(requestId, clock())) {
                    nativeBridge.postMwaError(
                        buildErrorJson(
                            requestId = requestId,
                            error = MwaError.ProtocolError,
                            developerDetails = "duplicate requestId on disconnect — caller must use fresh UUIDs per op",
                            layer = "kotlin",
                            cause = "duplicate_request_id",
                            sourceMethod = "disconnect",
                        ).toString(),
                    )
                    return@withContext
                }

                // Step 2 (DD-2-3-2 CAS-first): win the terminal-signal CAS BEFORE
                // mutating state. If we lose the CAS (not expected on the disconnect
                // path — no competing timeout/failure path — but defense in depth
                // preserves the state-machine discipline), do NOT wipe and do NOT
                // emit; just log the late_result.
                if (!inflightMap.tryTerminate(requestId)) {
                    diagnostics.incrementLateResult()
                    SdkLog.w(TAG, requestId) { "late_result outcome=disconnect (CAS lost)" }
                    return@withContext
                }

                // Step 3 (DD-2-3-1): wipe auth-only MwaSessionState. Caller identity +
                // `cluster: Int` are preserved for a subsequent reauthorize. The
                // SecureTokenStore CacheRecord is NOT touched — Story 4-1 deauthorize
                // is the path that deletes the token.
                try {
                    sessionState.clearOnLogout()
                } catch (ex: Throwable) {
                    // Not realistically reachable — clearOnLogout is pure @Synchronized
                    // field assignment — but defense in depth preserves the
                    // terminal-signal invariant. We already won the CAS so we MUST emit
                    // exactly one terminal signal; since wipe failed, it's mwa_error.
                    SdkLog.e(TAG, requestId) { "mwaDisconnect wipe crashed: ${ex.javaClass.simpleName}" }
                    nativeBridge.postMwaError(
                        buildErrorJson(
                            requestId = requestId,
                            error = MwaError.ProtocolError,
                            developerDetails = "clearOnLogout crashed: ${ex.javaClass.simpleName}",
                            layer = "kotlin",
                            cause = ex.javaClass.simpleName,
                            sourceMethod = "disconnect",
                        ).toString(),
                    )
                    return@withContext
                }

                // Step 4: emit the terminal signal. CAS won in Step 2, wipe succeeded
                // in Step 3, so exactly one disconnect_completed fires per requestId.
                val resultJson = JSONObject().apply {
                    put("request_id", requestId)
                    put("source_method", "disconnect")
                }.toString()
                nativeBridge.postDisconnectCompleted(requestId, resultJson)
            }
        }
    }

    /**
     * Story 2-2 T2 — reauthorize-path entry point. Called from the JNI
     * [mwaReauthorizeFromJni] on a worker thread and from the
     * `@VisibleForTesting` ctor in unit tests. Implements UC-3 silent
     * reauthorize per arch §4.2 + DD-2-2-1 through DD-2-2-7:
     *
     * 1. Register the request in [InflightMap] (PROTOCOL_ERROR on duplicate).
     * 2. `scope.launch { withContext(mainDispatcher) { ... } }` — all state
     *    mutations and signal emissions run on the main dispatcher (DD-22 parity).
     * 3. Parse identity JSON (PROTOCOL_ERROR on malformed).
     * 4. Discover CacheKey via `store.listAllKeys()` 3-tuple filter
     *    `(cluster, chainId, identityUri)` — DD-2-2-7. NO walletPackage in
     *    caller args; iteration is O(N) where N ≤ small (typically 1-3).
     *    If the filter returns empty: NOT_CONNECTED (AC-2 + AC-4 via DD-2-2-1).
     *    Tie-break on multi-match: most-recently-used (DD-2-2-7).
     * 5. Load CacheRecord; if vanished between listAllKeys and getToken: NOT_CONNECTED.
     * 6. Call [MwaClient.reauthorize] inside `withTimeoutOrNull(effectiveMs)`
     *    (DD-2-2-3). On null: [emitTimeoutReauth].
     * 7. On [MwaResult.Success]: [handleReauthSuccess] (putToken + fingerprint +
     *    sessionState + CAS + postReauthorizeCompleted).
     * 8. On [MwaResult.Failure] with [MwaError.TokenExpired]: [handleTokenExpired]
     *    (CAS → clearOnLogout → deleteToken → postMwaError, DD-2-2-2).
     * 9. On other failures: [emitFailureReauth].
     *
     * **DD-2-2-1 [LOCKED]:** NO separate cluster-comparison branch.
     * Cluster mismatch is detected implicitly by the 3-tuple filter returning
     * empty (DD-27 + DD-2-2-7). `ci/grep_bans.sh` pattern-9 enforces statically.
     */
    @UsedByGodot
    fun mwaReauthorize(requestId: String, identityJson: String, cluster: String, chainId: String, timeoutMs: Long) {
        val effectiveMs = effectiveWatchdog(timeoutMs)
        if (!inflightMap.register(requestId, clock())) {
            SdkLog.w(TAG, requestId) { "duplicate requestId at mwaReauthorize; refusing re-register" }
            nativeBridge.postMwaError(
                buildErrorJson(
                    requestId = requestId,
                    error = MwaError.ProtocolError,
                    developerDetails = "duplicate requestId — caller must use fresh UUIDs per op",
                    layer = "kotlin",
                    cause = "duplicate_request_id",
                    sourceMethod = "reauthorize",
                ).toString(),
            )
            return
        }
        scope.launch {
            try {
                withContext(mainDispatcher) {
                    // Step A: parse identity — PROTOCOL_ERROR on malformed input.
                    val parsed = parseIdentity(identityJson) ?: run {
                        emitFailureReauth(
                            requestId,
                            MwaError.ProtocolError,
                            developerDetails = "identity.name required; identityJson=<redacted>",
                        )
                        return@withContext
                    }

                    // Step B: discover CacheKey via DD-2-2-7 3-tuple filter.
                    // walletPackage is NOT in caller args — iterate listAllKeys + filter.
                    // DD-2-2-1: NO separate cluster comparison branch; the filter itself
                    // is the detection mechanism (empty result → NOT_CONNECTED).
                    val store = storeProvider(requireContext())
                    val candidates = store.listAllKeys().filter {
                        it.cluster == cluster &&
                            it.chainId == chainId &&
                            it.identityUri == parsed.identityUri
                    }
                    // Materialize once: load each candidate's record now (single decrypt per
                    // candidate). Folds the race-guard ("vanished between listAllKeys and
                    // getToken") into the same empty-result path — same NOT_CONNECTED
                    // outcome on the wire either way (AC-2 / AC-4 / race all collapse here).
                    val candidatesWithRecords = candidates.mapNotNull { k ->
                        store.getToken(k)?.let { k to it }
                    }
                    if (candidatesWithRecords.isEmpty()) {
                        emitFailureReauth(
                            requestId,
                            MwaError.NotConnected,
                            developerDetails = "no readable cached token under (cluster=$cluster, " +
                                "chainId=$chainId, identityUri=${parsed.identityUri})",
                        )
                        return@withContext
                    }

                    // Tie-break on multi-match (DD-2-2-7): most-recently-used wallet.
                    val (key, cached) = candidatesWithRecords.maxBy { it.second.lastUsedAtMs }

                    // Step D: call MwaClient.reauthorize inside watchdog (DD-2-2-3).
                    val client = mwaClientFactory()
                    try {
                        val result = withTimeoutOrNull(effectiveMs) {
                            client.reauthorize(
                                senderProvider(),
                                parsed.connectionIdentity,
                                cached.authToken,
                                cluster,
                                chainId,
                            )
                        }
                        when (result) {
                            null -> emitTimeoutReauth(requestId, effectiveMs)
                            is MwaResult.Success -> handleReauthSuccess(requestId, key, result.value, store)
                            is MwaResult.Failure -> {
                                if (result.error is MwaError.TokenExpired) {
                                    // DD-2-2-2 graceful wipe: CAS → clearOnLogout → deleteToken → postMwaError.
                                    handleTokenExpired(requestId, key, store)
                                } else {
                                    emitFailureReauth(requestId, result.error, developerDetails = null)
                                }
                            }
                        }
                    } finally {
                        runCatching { client.close() }
                    }
                }
            } catch (ce: CancellationException) {
                // Preserve cancellation propagation — graceful scope teardown, not a crash.
                throw ce
            } catch (ex: Throwable) {
                SdkLog.e(TAG, requestId) {
                    "mwaReauthorize crashed: ${ex.javaClass.simpleName}: ${ex.message}"
                }
                if (inflightMap.tryTerminate(requestId)) {
                    nativeBridge.postMwaError(
                        buildErrorJson(
                            requestId = requestId,
                            error = MwaError.ProtocolError,
                            developerDetails = "mwaReauthorize coroutine crashed: ${ex.javaClass.simpleName}",
                            layer = "kotlin",
                            cause = ex.javaClass.simpleName,
                            sourceMethod = "reauthorize",
                        ).toString(),
                    )
                } else {
                    diagnostics.incrementLateResult()
                    SdkLog.w(TAG, requestId) { "late_result outcome=crash" }
                }
            }
        }
    }

    /**
     * Story 2-2 T2 — success path for reauthorize.
     *
     * Updates [SecureTokenStore] `lastUsedAtMs`, handles wallet-side token rotation
     * (DD-2-2-5), updates [MwaSessionState], wins the terminal-signal CAS, and emits
     * `reauthorize_completed` via [NativeBridge.postReauthorizeCompleted] using
     * [buildSuccessJson] (DD-2-2-4 — NO sibling builder).
     *
     * **DD-2-2-5 rotation-detection (LOCKED):** compare `auth.authToken.reveal()` vs
     * `cached.authToken.reveal()` byte-for-byte. The fingerprint, stored [CacheRecord],
     * and [MwaSessionState] writes ALL use the RETURNED bytes (`auth.authToken`).
     * - When NOT rotated (FakeMwaClient happy path + clientlib-ktx 2.0.3 + Phantom
     *   production per story line 124), `auth.authToken == cached.authToken` byte-for-byte
     *   → fingerprint/store/sessionState writes observably no-op on token bytes.
     * - When rotated (defensive contract), we log a non-secret event marker
     *   `auth_token_rotated_by_wallet` (bytes never logged), store the new bytes,
     *   and let the recomputed fingerprint flow through.
     *
     * AC-6 "fingerprint identical pre/post" holds in test scope because the happy-path
     * fixture returns byte-identical bytes (per the LOCKED contract).
     *
     * **`publicKey` is NOT in DD-2-2-5 scope.** Public key represents the canonical
     * session identity and AC-1 "public_key retained" requires the cached base58 value.
     * The wallet confirms the same identity via reauthorize; we do not re-encode
     * `auth.publicKey` (raw bytes) on this path.
     */
    private suspend fun handleReauthSuccess(requestId: String, key: CacheKey, auth: AuthResult, store: SecureTokenStore) {
        val cached = store.getToken(key)
        if (cached == null) {
            // Extremely unlikely race: record disappeared during the reauth round-trip.
            SdkLog.w(TAG, requestId) { "cached record vanished during reauth success path; emitting NOT_CONNECTED" }
            emitFailureReauth(
                requestId,
                MwaError.NotConnected,
                developerDetails = "cached record vanished during reauth success path",
            )
            return
        }

        // DD-2-2-5 (LOCKED) — rotation-detection. Compare returned vs cached bytes
        // byte-for-byte. When rotated, log a non-secret event marker (bytes never
        // logged). Fingerprint, stored CacheRecord, and sessionState writes ALL use
        // the RETURNED bytes (auth.authToken) — when not rotated, returned == cached
        // by construction so the writes are observable no-ops on token bytes; when
        // rotated (defensive contract per story line 126-131), the new bytes flow
        // through. Per-install salt is never rotated by reauth (only forget_all per
        // Story 4-2). Public key is NOT in DD-2-2-5 scope — AC-1 "public_key retained"
        // uses cached.publicKey (base58 already).
        val isRotated = !auth.authToken.reveal().contentEquals(cached.authToken.reveal())
        if (isRotated) {
            SdkLog.w(TAG, requestId) { "auth_token_rotated_by_wallet" } // event marker, NO bytes
        }

        // DD-2-2-5(a) + (b): persist returned authToken bytes + fresh lastUsedAtMs;
        // recompute fingerprint from RETURNED bytes (when not rotated, this equals
        // AuthTokenFingerprint.compute(cached.authToken.reveal(), salt)).
        // Wrap store ops in StorageCorruptException catch to mirror handleSuccess —
        // gives callers a consistent MwaError.StorageCorrupt code across authorize +
        // reauthorize on disk corruption (rather than reauth surfacing PROTOCOL_ERROR
        // via the outer Throwable handler).
        val activeFingerprint: String = try {
            val salt = store.getOrCreatePerInstallSalt()
            store.putToken(
                key,
                cached.copy(authToken = auth.authToken, lastUsedAtMs = clock()),
            )
            AuthTokenFingerprint.compute(auth.authToken.reveal(), salt)
        } catch (ex: StorageCorruptException) {
            SdkLog.w(TAG, requestId) {
                "storage corrupt on reauthorize success: ${ex.cause?.javaClass?.simpleName}"
            }
            emitFailureReauth(
                requestId,
                MwaError.StorageCorrupt,
                developerDetails = "EncryptedSharedPreferences corrupt: ${ex.cause?.javaClass?.simpleName}",
            )
            return
        }

        // AC-1 "public_key retained" — use the cached base58 string (DD-2-2-5 covers
        // authToken rotation only, not publicKey).
        val publicKeyBase58 = cached.publicKey

        // DD-2-2-5(d) + DD-22: update MwaSessionState on mainDispatcher (T2 is already
        // inside withContext(mainDispatcher) in the caller). Restore the FULL surface
        // that handleSuccess sets — on cold-start, sessionState is freshly empty and
        // mwaReauthorize is the only path that re-populates it. Identity metadata
        // (walletLabel, walletIconUri) comes from the cached record (wallet doesn't
        // rotate identity on reauth, only authToken per DD-2-2-5). connectedKey raw
        // bytes come from auth.publicKey (parallel to handleSuccess).
        sessionState.setAuthToken(auth.authToken)
        sessionState.setAuthTokenFingerprint(activeFingerprint)
        sessionState.setWalletIconUri(cached.walletIconUri)
        sessionState.setKey(auth.publicKey)
        sessionState.setPublicKeyBase58(publicKeyBase58)
        sessionState.setClusterName(auth.cluster)
        sessionState.setWalletLabel(cached.walletLabel)

        // CAS — win the terminal-signal slot exactly once per requestId.
        if (!inflightMap.tryTerminate(requestId)) {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=reauthorize" }
            return
        }

        // Emit reauthorize_completed — 2-param A-12 shape, 6-key DD-2-2-4 result payload.
        // DO NOT introduce a sibling builder — reuse buildSuccessJson (DD-2-2-4 LOCKED).
        nativeBridge.postReauthorizeCompleted(
            requestId,
            buildSuccessJson(
                requestId = requestId,
                fingerprint = activeFingerprint,
                publicKeyBase58 = publicKeyBase58,
                walletLabel = cached.walletLabel,
                walletIconUri = cached.walletIconUri,
                cluster = auth.cluster,
            ).toString(),
        )
    }

    /**
     * Story 2-2 T2 — TOKEN_EXPIRED graceful-wipe path (AC-3, DD-2-2-2).
     *
     * **DD-2-2-2 ordering [LOCKED]:** CAS → wipe in-memory state → wipe on-disk
     * record → emit terminal error. This exact lexical order must be preserved;
     * any reordering is a Rule 2 deviation requiring an amendment.
     *
     *  1. `inflightMap.tryTerminate(requestId)` — win the CAS.
     *  2. `sessionState.clearOnLogout()` — wipe auth-only in-memory fields.
     *  3. `store.deleteToken(key)` — **first production caller of deleteToken**.
     *  4. `nativeBridge.postMwaError(...)` — emit TOKEN_EXPIRED terminal error.
     */
    private fun handleTokenExpired(requestId: String, key: CacheKey, store: SecureTokenStore) {
        if (!inflightMap.tryTerminate(requestId)) {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=token_expired" }
            return
        }
        // DD-2-2-2 ordering: Step 2 in-memory wipe → Step 3 disk wipe → Step 4 emit error.
        // First production caller of SecureTokenStore.deleteToken (story T1 spec line 76).
        sessionState.clearOnLogout()
        store.deleteToken(key)
        nativeBridge.postMwaError(
            buildErrorJson(
                requestId = requestId,
                error = MwaError.TokenExpired,
                developerDetails = "wallet reported auth token expired; cached record deleted (graceful wipe)",
                layer = "kotlin",
                cause = null,
                sourceMethod = "reauthorize",
            ).toString(),
        )
    }

    /**
     * Story 2-2 T2 — emits a typed failure for the reauthorize path.
     * Parallel to [emitFailure] on the authorize path; uses `sourceMethod =
     * "reauthorize"` for A-14 payload consistency.
     */
    private fun emitFailureReauth(requestId: String, error: MwaError, developerDetails: String?) {
        if (inflightMap.tryTerminate(requestId)) {
            nativeBridge.postMwaError(
                buildErrorJson(
                    requestId = requestId,
                    error = error,
                    developerDetails = developerDetails,
                    layer = "kotlin",
                    cause = null,
                    sourceMethod = "reauthorize",
                ).toString(),
            )
        } else {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=failure code=${error.code}" }
        }
    }

    /**
     * Story 2-2 T2 — emits `mwa_timeout` for the reauthorize path (DD-2-2-3).
     * Parallel to [emitTimeout] on the authorize path; uses `sourceMethod =
     * "reauthorize"`.
     */
    private fun emitTimeoutReauth(requestId: String, effectiveMs: Long) {
        if (inflightMap.tryTerminate(requestId)) {
            nativeBridge.postMwaTimeout(buildTimeoutJson(requestId, effectiveMs, "reauthorize").toString())
        } else {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=timeout" }
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
            // Story 2-1 T6 — fields read via MwaJniContext::query_session_state.
            sessionState.setPublicKeyBase58(publicKeyBase58)
            sessionState.setClusterName(auth.cluster)
            sessionState.setWalletLabel(auth.accountLabel.orEmpty())
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
