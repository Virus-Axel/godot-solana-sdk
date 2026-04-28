package plugin.walletadapterandroid

import android.content.Context
import android.content.Intent
import android.util.Log
import androidx.annotation.VisibleForTesting
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClientImpl
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.AuthResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignAndSendResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignResult
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
import org.json.JSONArray
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

        // Story 2-2 T2 — JNI shim forwards to the `mwaReauthorize(...)` instance
        // method with the full reauth-args set (identityJson, cluster, chainId,
        // timeoutMs); the C++ side at `MobileWalletAdapter::reauthorize` populates
        // these from sessionState before crossing JNI. Null-instance branch routes
        // to mwa_error{NOT_CONNECTED, source_method="reauthorize"} via the AC-6
        // emitInstanceNullError helper.
        @JvmStatic
        fun mwaReauthorizeFromJni(reqId: String, identityJson: String, cluster: String, chainId: String, timeoutMs: Long) {
            instance?.mwaReauthorize(reqId, identityJson, cluster, chainId, timeoutMs)
                ?: emitInstanceNullError(reqId, "reauthorize")
        }

        @JvmStatic
        fun mwaDisconnectFromJni(reqId: String) {
            instance?.mwaDisconnect(reqId) ?: emitInstanceNullError(reqId, "disconnect")
        }

        // Story 4-1 — JNI shim forwards to the `mwaDeauthorize(...)` instance
        // method (try/catch/finally + multi-key wipe per DD-4-1-3/DD-4-1-6).
        @JvmStatic
        fun mwaDeauthorizeFromJni(reqId: String) {
            Log.i(TAG, "mwaDeauthorizeFromJni: Story 4-1 scope; reqId=$reqId")
            instance?.mwaDeauthorize(reqId) ?: emitInstanceNullError(reqId, "deauthorize")
        }

        @JvmStatic
        fun mwaSignMessagesFromJni(reqId: String) {
            // Story 3-1 T2 — JNI shim signature stays at `(reqId)` for T1+T2 (the
            // C++ side at `MobileWalletAdapter::sign_messages` still calls the 1-arg
            // form). Story 3-1 T3 evolves BOTH sides together: C++ passes through
            // real `messages` + `timeoutMs`, and this shim's signature evolves to
            // `(reqId, messagesJson, timeoutMs)` (or equivalent) at that point. For
            // T1+T2 the shim delegates to `mwaSignMessages(...)` with empty messages
            // so the JNI surface lights up at link time — production callers that
            // route through C++ currently hit a no-op signing path (empty payload
            // never produces a wallet round-trip) until T3 wires real messages
            // through. Kotlin unit tests bypass this shim and call `mwaSignMessages`
            // directly with real values per DD-3-1-9 responsibility split.
            instance?.mwaSignMessages(reqId, emptyList(), 0L) ?: emitInstanceNullError(reqId, "sign_messages")
        }

        @JvmStatic
        fun mwaSignTransactionsFromJni(reqId: String) {
            // Story 3-2 T1 — JNI shim signature stays at `(reqId)` for T1+T2 (the
            // C++ side at `MobileWalletAdapter::sign_transactions` still calls the
            // 1-arg form). Story 3-2 T3 evolves BOTH sides together: C++ passes
            // through real `transactions` + `timeoutMs`, and this shim's signature
            // evolves to `(reqId, transactionsJson, timeoutMs)` (or equivalent) at
            // that point. For T1+T2 the shim delegates to `mwaSignTransactions(...)`
            // with empty transactions so the JNI surface lights up at link time —
            // production callers that route through C++ currently hit a no-op
            // signing path (empty payload never produces a wallet round-trip) until
            // T3 wires real transactions through. Kotlin unit tests bypass this
            // shim and call `mwaSignTransactions` directly with real values per
            // DD-3-1-9 responsibility split inherited by Story 3-2.
            instance?.mwaSignTransactions(reqId, emptyList(), 0L) ?: emitInstanceNullError(reqId, "sign_transactions")
        }

        @JvmStatic
        fun mwaSignAndSendFromJni(reqId: String) {
            // Story 3-3 T1 — JNI shim signature stays at `(reqId)` for T1+T2 (the
            // C++ side at `MobileWalletAdapter::sign_and_send` still calls the
            // 1-arg form). Story 3-3 T3 evolves BOTH sides together: C++ passes
            // through real `transactions` + `timeoutMs`, and this shim's signature
            // evolves to `(reqId, transactionsJson, timeoutMs)` (or equivalent) at
            // that point. For T1+T2 the shim delegates to `mwaSignAndSendTransactions(...)`
            // with empty transactions so the JNI surface lights up at link time —
            // production callers that route through C++ currently hit a no-op
            // signing path (empty payload never produces a wallet round-trip) until
            // T3 wires real transactions through. Kotlin unit tests bypass this
            // shim and call `mwaSignAndSendTransactions` directly with real values per
            // DD-3-1-9 responsibility split inherited via DD-3-2-5.
            instance?.mwaSignAndSendTransactions(reqId, emptyList(), 0L) ?: emitInstanceNullError(reqId, "sign_and_send")
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

        // Story 3-1 T1 — parallel external fun for sign_messages_completed's
        // Kotlin→C++ callback path. JNI symbol linked at runtime via the
        // matching JNIEXPORT in src/mwa/mwa_android_bridge_jni.cpp (T3).
        @JvmStatic
        external fun postSignMessagesCompletedNative(reqId: String, resultDictJson: String)

        // Story 3-2 T1 — parallel external fun for sign_transactions_completed's
        // Kotlin→C++ callback path. JNI symbol linked at runtime via the
        // matching JNIEXPORT in src/mwa/mwa_android_bridge_jni.cpp (Story 3-2 T3).
        // Inert at JVM level until the JNI symbol arrives in T3.
        @JvmStatic
        external fun postSignTransactionsCompletedNative(reqId: String, resultDictJson: String)

        // Story 3-3 T1 — parallel external fun for sign_and_send_completed's
        // Kotlin→C++ callback path. JNI symbol linked at runtime via the
        // matching JNIEXPORT in src/mwa/mwa_android_bridge_jni.cpp (Story 3-3 T3 —
        // post_arity2_completed("sign_and_send_completed", ...)). The
        // resultDictJson carries the 4-key shape {request_id, signatures: [base58
        // strings], submitted_at: int, confirmation_status: "submitted"} per AC-1
        // + DD-3-3-E. Inert at JVM level until the JNI symbol arrives in T3.
        @JvmStatic
        external fun postSignAndSendCompletedNative(reqId: String, resultDictJson: String)

        // Story 4-1 T1 — parallel external fun for deauthorize_completed's
        // Kotlin→C++ callback path. JNI symbol linked at runtime via the
        // matching JNIEXPORT in src/mwa/mwa_android_bridge_jni.cpp (T3 of 4-1).
        // Result Dictionary is the 4-key shape `{request_id,
        // remote_revoke_succeeded, local_cache_cleared, warning}` per arch.md:669.
        @JvmStatic
        external fun postDeauthorizeCompletedNative(reqId: String, resultDictJson: String)

        @JvmStatic
        external fun postMwaErrorNative(errorDictJson: String)

        @JvmStatic
        external fun postMwaTimeoutNative(timeoutDictJson: String)

        @JvmStatic
        external fun postMwaCancelledLifecycleNative(cancelledDictJson: String)

        @JvmStatic
        external fun postReauthRequiredNative(reauthDictJson: String)

        // Story 3-3 T1 — parallel external fun for pending_submission_found's
        // Kotlin→C++ callback path. JNI symbol linked at runtime via the
        // matching JNIEXPORT in src/mwa/mwa_android_bridge_jni.cpp (Story 3-3 T3 —
        // post_arity1("pending_submission_found", ...)). 1-PARAM signal per A-12
        // (parallel to postReauthRequiredNative); the pendingDictJson carries the
        // 6-key shape {request_id, op_type, started_at_ms, tx_count,
        // tx_preview_hashes, recommendation} per DD-3-3-E. Inert at JVM level
        // until the JNI symbol arrives in T3.
        @JvmStatic
        external fun postPendingSubmissionFoundNative(pendingDictJson: String)
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
                    //
                    // Story 4-3 DD-4-3-1.a — fail-closed plugin-boundary wrapper. Wrap
                    // the cache-LOOKUP touchpoints (listAllKeys + per-candidate getToken)
                    // so a Tink corruption event during cache READ surfaces as
                    // `reauth_required{reason:"keystore_corrupt"}` rather than propagating
                    // as an uncaught Throwable → `mwa_error{PROTOCOL_ERROR}`. Null-return
                    // aborts the op (the wrapper has already emitted the terminal signal).
                    val store = storeProvider(requireContext())
                    val candidatesWithRecords = withStorageOrReauthRequired(requestId, "reauthorize") {
                        val candidates = store.listAllKeys().filter {
                            it.cluster == cluster &&
                                it.chainId == chainId &&
                                it.identityUri == parsed.identityUri
                        }
                        // Materialize once: load each candidate's record now (single decrypt per
                        // candidate). Folds the race-guard ("vanished between listAllKeys and
                        // getToken") into the same empty-result path — same NOT_CONNECTED
                        // outcome on the wire either way (AC-2 / AC-4 / race all collapse here).
                        candidates.mapNotNull { k ->
                            store.getToken(k)?.let { k to it }
                        }
                    } ?: return@withContext
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
        // Story 4-3 DD-4-3-1 PIVOT — StorageCorruptException now surfaces as
        // `reauth_required{reason:"keystore_corrupt"}` via emitReauthRequiredKeystoreCorrupt
        // (NOT `mwa_error{StorageCorrupt}` — AC-1 contract; legacy emitFailureReauth
        // call deleted to preserve the terminal-signal CAS invariant — no dual-emit).
        val activeFingerprint: String = try {
            val salt = store.getOrCreatePerInstallSalt()
            store.putToken(
                key,
                cached.copy(authToken = auth.authToken, lastUsedAtMs = clock()),
            )
            AuthTokenFingerprint.compute(auth.authToken.reveal(), salt)
        } catch (ex: StorageCorruptException) {
            emitReauthRequiredKeystoreCorrupt(requestId, "reauthorize", ex)
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

    // ---------------- Story 3-1 — sign_messages + runSigningOp ----------------
    //
    // Single-wrap shared signing pipeline implementing the locked design:
    //   DD-3-1-1: runSigningOp is a private (`@VisibleForTesting internal`)
    //             member function on GDExtensionAndroidPlugin.
    //   DD-3-1-2: block receiver = MwaClient (the SDK seam — preserves
    //             FakeMwaClient test injection).
    //   DD-3-1-3: signature carries `timeoutMs: Long`; helper passes through
    //             `effectiveWatchdog(timeoutMs)` for watchdog parity with the
    //             auth path.
    //   DD-3-1-4: SigningOp enum + `sourceMethod` extension property.
    //   DD-3-1-6: NOT_CONNECTED preflight runs BEFORE InflightMap.register
    //             (synchronous, no scope.launch on disconnected branch).
    //   DD-3-1-8: ConnectionIdentity reconstructed from sessionState fields.
    //   DD-3-1-9: responsibility split — helper does CAS+watchdog+errors;
    //             call site does sessionState reads.
    //   D-3-1-11: single-wrap return MwaResult<X> — block returns MwaResult<X>
    //             directly, helper inspects Failure and routes via
    //             nativeBridge.postMwaError + op.sourceMethod, caller only
    //             handles Success. Stories 3-2 / 3-3 inherit the same shape.
    // See `docs/stories/3-1.md` for full detail.

    /**
     * Story 3-1 — `sign_messages` entry point. Called from the JNI
     * [mwaSignMessagesFromJni] shim on a worker thread (T3 evolves the JNI
     * shim signature to pass `messages` + `timeoutMs` through), or directly
     * by Kotlin unit tests with real values per DD-3-1-9.
     *
     * Body shape (T2 fills in):
     *  1. DD-3-1-6 preflight — synchronous `is_connected()` check BEFORE
     *     `InflightMap.register`; emit `mwa_error{NOT_CONNECTED}` and return
     *     if disconnected. NO `scope.launch` on this branch (AC-3 "within 1
     *     frame, no wallet UI").
     *  2. DD-3-1-8 reconstruction — read `sessionState.getAuthToken()`,
     *     `getConnectedKey()` (NOT `getKey()` — asymmetric naming inherited
     *     from Story 2-1 T4), `getIdentityUri()` / `getIconUri()` /
     *     `getIdentityName()`, build `ConnectionIdentity`.
     *  3. `scope.launch { runSigningOp(SIGN_MESSAGES, requestId, timeoutMs) {
     *     signMessages(senderProvider(), identity, authToken, messages,
     *     listOf(publicKey)) } }` — DD-3-1-2 receiver is `MwaClient`; the
     *     lambda calls the 5-arg `signMessages(...)` (NOT the clientlib-ktx
     *     `signMessagesDetached(...)`).
     *  4. On `MwaResult.Success`, route to `handleSignMessagesSuccess`
     *     (T2 lands this helper) which CAS-terminates and emits
     *     `sign_messages_completed` via `postSignMessagesCompleted`.
     *
     * AC-1 enforces a ≤20-LOC budget on this body — see
     * `MwaAndroidPluginSignMessagesTest."AC-1 mwaSignMessages body is at most
     * 20 LOC"` (T1) for the source-line counter rule.
     */
    @UsedByGodot
    fun mwaSignMessages(requestId: String, messages: List<ByteArray>, timeoutMs: Long) {
        val authToken = sessionState.getAuthToken() ?: return emitNotConnectedSign(requestId)
        val publicKey = sessionState.getConnectedKey() ?: error("setKey was never called — see Story 2-2 C-T2-A")
        val identity = ConnectionIdentity(
            identityUri = android.net.Uri.parse(sessionState.getIdentityUri()),
            iconUri = android.net.Uri.parse(sessionState.getIconUri()),
            identityName = sessionState.getIdentityName(),
        )
        scope.launch {
            val result = runSigningOp(SigningOp.SIGN_MESSAGES, requestId, timeoutMs) {
                signMessages(senderProvider(), identity, authToken, messages, listOf(publicKey))
            }
            if (result is MwaResult.Success) handleSignMessagesSuccess(requestId, result.value)
        }
    }

    /**
     * Story 3-2 — `sign_transactions` entry point. Called from the JNI
     * [mwaSignTransactionsFromJni] shim on a worker thread (T3 evolves the JNI
     * shim signature to pass `transactions` + `timeoutMs` through), or directly
     * by Kotlin unit tests with real values per DD-3-1-9 (inherited).
     *
     * Body shape (T2 fills in — mirrors [mwaSignMessages] with two textual deltas:
     *   (a) lambda calls `signTransactions(sender, identity, authToken, transactions)`
     *       (4-arg, no `addresses`) instead of `signMessages(... messages, addresses)`
     *       (5-arg);
     *   (b) success path emits `sign_transactions_completed` via
     *       [handleSignTransactionsSuccess] instead of `sign_messages_completed`):
     *  1. DD-3-1-6 preflight (inherited via 3-2 DD-3-2-5) — synchronous
     *     `is_connected()` check via `sessionState.getAuthToken()`. Emit
     *     `mwa_error{NOT_CONNECTED, source_method="sign_transactions"}` and return
     *     if disconnected. NO `scope.launch` on this branch (AC-3 "within 1 frame").
     *  2. Identity reconstruction per DD-3-1-8 (inherited).
     *  3. `scope.launch { runSigningOp(SIGN_TRANSACTIONS, requestId, timeoutMs) {
     *     signTransactions(senderProvider(), identity, authToken, transactions) } }`
     *     — DD-3-1-2 receiver is `MwaClient`; the lambda calls the 4-arg
     *     `signTransactions(...)` (no `addresses`).
     *  4. On `MwaResult.Success`, route to [handleSignTransactionsSuccess]
     *     (T2 lands this helper) which CAS-terminates and emits
     *     `sign_transactions_completed` via `postSignTransactionsCompleted`
     *     using `buildSignSuccessJson(requestId, signedPayloads,
     *     payloadKey = "signed_transactions")` per DD-3-2-3.
     *
     * AC-2 enforces a ≤20-LOC budget on this body — see
     * `MwaAndroidPluginSignTransactionsTest."AC-2 mwaSignTransactions body is at most
     * 20 LOC"` (T1) for the source-line counter rule (shared `countMethodLines`
     * helper in `LocCountUtil.kt` per DD-3-2-2 + D-3-2-3).
     */
    @UsedByGodot
    fun mwaSignTransactions(requestId: String, transactions: List<ByteArray>, timeoutMs: Long) {
        val authToken = sessionState.getAuthToken() ?: return emitNotConnectedSign(requestId, "sign_transactions")
        // Defensive guard mirroring [mwaSignMessages] (Story 3-1 Dev Notes C-T2-A inheritance) —
        // signTransactions does NOT take an `addresses` arg so the key isn't passed downstream,
        // but the null-check still surfaces the same setKey-was-never-called failure mode loudly
        // (instead of letting later code fail with a less-actionable ConnectionIdentity error).
        sessionState.getConnectedKey() ?: error("setKey was never called — see Story 3-1 Dev Notes C-T2-A inheritance")
        val identity = ConnectionIdentity(
            identityUri = android.net.Uri.parse(sessionState.getIdentityUri()),
            iconUri = android.net.Uri.parse(sessionState.getIconUri()),
            identityName = sessionState.getIdentityName(),
        )
        scope.launch {
            val result = runSigningOp(SigningOp.SIGN_TRANSACTIONS, requestId, timeoutMs) {
                signTransactions(senderProvider(), identity, authToken, transactions)
            }
            if (result is MwaResult.Success) handleSignTransactionsSuccess(requestId, result.value)
        }
    }

    /**
     * Story 3-3 — `sign_and_send` entry point. Called from the JNI
     * [mwaSignAndSendFromJni] shim on a worker thread (T3 evolves the JNI
     * shim signature to pass `transactions` + `timeoutMs` through), or directly
     * by Kotlin unit tests with real values per DD-3-1-9 inheritance via
     * DD-3-2-5.
     *
     * Body shape (T2 fills in — mirrors [mwaSignTransactions] with three
     * structural deltas locked at story-creation per DD-3-3-A..G):
     *   (a) breadcrumb-write-ahead step BEFORE `runSigningOp` per DD-3-3-B
     *       (write-then-call ordering); the write is wrapped with
     *       `withStorageOrReauthRequired` per DD-3-3-G (StorageCorruptException
     *       surfaces as `reauth_required` and abandons the op with no wallet
     *       round-trip);
     *   (b) lambda calls 5-arg `signAndSendTransactions(sender, identity,
     *       authToken, transactions, cluster)` instead of 4-arg
     *       `signTransactions(...)` — the cluster arg flows from
     *       `sessionState.getClusterName()` and is required by the wallet to
     *       select the RPC endpoint for submission;
     *   (c) success path routes to [handleSignAndSendSuccess] which
     *       CAS-terminates, removes the breadcrumb (AC-3 ordering: BEFORE
     *       the terminal signal fires), and emits `sign_and_send_completed`
     *       via `postSignAndSendCompleted` with the 4-key payload built by
     *       [buildSignAndSendSuccessJson] per DD-3-3-E.
     *
     * Cleanup on error/timeout/cancellation (AC-4) per DD-3-3-C:
     *   - Error/timeout: [cleanupBreadcrumb] invoked from the runSigningOp
     *     Failure branch (after the terminal-signal emission). Wraps
     *     [SecureTokenStore.removePendingSubmission] in a try/catch — on
     *     StorageCorruptException, increment diagnostics + log + continue
     *     (DD-4-1-3 wipe-crashed-flag pattern).
     *   - Cancellation (mwa_cancelled_lifecycle): DEFERRED to Story 5-3
     *     lifecycle observer. Story 3-3 lands the [cleanupBreadcrumb]
     *     helper signature; Story 5-3 wires the lifecycle invocation.
     *
     * Cluster-bleed refusal (AC-6) per DD-27 / AC-NFR-SEC-4: the
     * `sessionState.getClusterName()` value at call time MUST match the
     * cluster bound at connect/reauth time; if it has been swapped, refuse
     * with `mwa_error{NOT_CONNECTED, source_method="sign_and_send"}` per the
     * preflight check (no wallet round-trip on the disconnected branch).
     */
    @UsedByGodot
    fun mwaSignAndSendTransactions(requestId: String, transactions: List<ByteArray>, timeoutMs: Long) {
        TODO("Story 3-3 T2 fills in")
    }

    /**
     * Story 4-1 — `deauthorize` revoke + unconditional local-cache wipe (FR-5 /
     * AC-NFR-SEC-5). Best-effort remote `clientlib.revoke(authToken)` wrapped in
     * `try { ... } catch { ... } finally { wipe }` per DD-4-1-3; on remote
     * failure the path emits `deauthorize_completed{remote_revoke_succeeded:
     * false, warning: "remote_unreachable"}` (NOT `mwa_error`) per DD-4-1-1.
     * The `finally`-block multi-key wipe (DD-4-1-6: `listAllKeys` filter on
     * `identityUri`) deletes ALL CacheRecord entries scoped to this caller's
     * identity, then `MwaSessionState.clear()` (full clear, NOT
     * `clearOnLogout()` — DD-4-1-2). Idempotence (DD-4-1-4): if no
     * `authToken` or no `identityUri` in session state, the remote-revoke is
     * skipped (vacuous success); the wipe-loop is naturally a no-op.
     *
     * **CAS-first ordering** is inherited from Story 2-3 — register +
     * tryTerminate must precede any state mutation. **No watchdog** on the
     * deauthorize path (story Dev Notes "Watchdog scoping decision"): AC-2
     * requires remote failure to surface as `remote_unreachable`, NOT
     * `mwa_timeout`.
     *
     * **Flag-based post-finally branch** (DD-4-1-3 / C-4-1-F): the
     * `wipeCrashed` flag is set inside the inner finally-catch and read by an
     * `if (!wipeCrashed) emit deauthorize_completed` guard AFTER the outer
     * try/catch/finally completes. Returning from inside the inner finally
     * would swallow any in-flight exception from the outer catch (e.g. a
     * defensive `SdkLog.w` throw). T1 case 5 enforces the skip path.
     */
    @UsedByGodot
    fun mwaDeauthorize(requestId: String) {
        scope.launch {
            withContext(mainDispatcher) {
                // Step 1 (CAS-first per Story 2-3 inheritance): reserve the slot.
                if (!inflightMap.register(requestId, clock())) {
                    nativeBridge.postMwaError(
                        buildErrorJson(
                            requestId = requestId,
                            error = MwaError.ProtocolError,
                            developerDetails =
                            "duplicate requestId on deauthorize — caller must use fresh UUIDs per op",
                            layer = "kotlin",
                            cause = "duplicate_request_id",
                            sourceMethod = "deauthorize",
                        ).toString(),
                    )
                    return@withContext
                }

                // Step 2: win the terminal-signal CAS BEFORE mutating state.
                // Defense-in-depth: deauthorize has no competing watchdog, but
                // preserving the CAS discipline keeps the state-machine uniform.
                if (!inflightMap.tryTerminate(requestId)) {
                    diagnostics.incrementLateResult()
                    SdkLog.w(TAG, requestId) { "late_result outcome=deauthorize (CAS lost)" }
                    return@withContext
                }

                // Step 3: snapshot inputs. Empty/null snapshots feed DD-4-1-4
                // idempotent path (skip remote, listAllKeys filter is naturally
                // a no-op).
                var remoteSucceeded = false
                var wipeCrashed = false
                val authTokenForRevoke = sessionState.getAuthToken()
                val identityUriSnapshot = sessionState.getIdentityUri()
                val store = storeProvider(requireContext())

                if (authTokenForRevoke == null || identityUriSnapshot.isEmpty()) {
                    // DD-4-1-4 idempotent path — vacuous success. No remote attempted.
                    remoteSucceeded = true
                    try {
                        // listAllKeys filter on empty identityUri returns empty list;
                        // the forEach is a no-op. Defensive call kept for symmetry
                        // with the else branch — single source of wipe semantics.
                        store.listAllKeys()
                            .filter { it.identityUri == identityUriSnapshot }
                            .forEach { store.deleteToken(it) }
                        sessionState.clear()
                    } catch (ex: Throwable) {
                        wipeCrashed = true
                        SdkLog.e(TAG, requestId) {
                            "mwaDeauthorize wipe crashed (idempotent path): ${ex.javaClass.simpleName}"
                        }
                        nativeBridge.postMwaError(
                            buildErrorJson(
                                requestId = requestId,
                                error = MwaError.ProtocolError,
                                developerDetails = "wipe crashed: ${ex.javaClass.simpleName}",
                                layer = "kotlin",
                                cause = ex.javaClass.simpleName,
                                sourceMethod = "deauthorize",
                            ).toString(),
                        )
                    }
                } else {
                    // Reconstruct ConnectionIdentity from MwaSessionState's
                    // preserved-across-disconnect identity fields per DD-3-1-8
                    // (Story 3-1 inheritance).
                    val identity = ConnectionIdentity(
                        identityUri = android.net.Uri.parse(identityUriSnapshot),
                        iconUri = android.net.Uri.parse(sessionState.getIconUri()),
                        identityName = sessionState.getIdentityName(),
                    )
                    try {
                        when (
                            val result = mwaClientFactory().deauthorize(
                                senderProvider(),
                                identity,
                                authTokenForRevoke,
                            )
                        ) {
                            is MwaResult.Success -> remoteSucceeded = true
                            is MwaResult.Failure -> {
                                remoteSucceeded = false
                                SdkLog.w(TAG, requestId) {
                                    "remote revoke failed: ${result.error.javaClass.simpleName}"
                                }
                            }
                        }
                    } catch (e: Throwable) {
                        // DD-4-1-1: any non-Success outcome (Failure result OR
                        // thrown exception) routes to `remote_unreachable` —
                        // never to `mwa_error`. Catch is defensive for
                        // unexpected throws (clientlib bugs, coroutine
                        // cancellation surfacing as throw, etc.).
                        remoteSucceeded = false
                        SdkLog.w(TAG, requestId) {
                            "remote revoke threw: ${e.javaClass.simpleName}"
                        }
                    } finally {
                        try {
                            // DD-4-1-6 multi-key wipe: filter listAllKeys on
                            // identityUri, delete all matches. Aligns with the
                            // security threat model: "this device is no longer
                            // trusted with this wallet."
                            store.listAllKeys()
                                .filter { it.identityUri == identityUriSnapshot }
                                .forEach { store.deleteToken(it) }
                            // DD-4-1-2: full clear (NOT clearOnLogout — that's
                            // the disconnect-path wipe that preserves identity).
                            sessionState.clear()
                        } catch (ex: Throwable) {
                            wipeCrashed = true
                            SdkLog.e(TAG, requestId) {
                                "mwaDeauthorize wipe crashed: ${ex.javaClass.simpleName}"
                            }
                            nativeBridge.postMwaError(
                                buildErrorJson(
                                    requestId = requestId,
                                    error = MwaError.ProtocolError,
                                    developerDetails =
                                    "wipe crashed: ${ex.javaClass.simpleName}",
                                    layer = "kotlin",
                                    cause = ex.javaClass.simpleName,
                                    sourceMethod = "deauthorize",
                                ).toString(),
                            )
                        }
                    }
                }

                // DD-4-1-3 / C-4-1-F: flag-based post-finally branch. NEVER
                // `return@withContext` from inside the inner finally — that
                // would swallow in-flight exceptions from the outer catch.
                if (!wipeCrashed) {
                    val resultJson = JSONObject().apply {
                        put("request_id", requestId)
                        put("remote_revoke_succeeded", remoteSucceeded)
                        put("local_cache_cleared", true)
                        put("warning", if (!remoteSucceeded) "remote_unreachable" else "")
                    }.toString()
                    nativeBridge.postDeauthorizeCompleted(requestId, resultJson)
                }
            }
        }
    }

    /**
     * Story 3-1 T2 — DD-3-1-6 NOT_CONNECTED preflight emission helper. Extracted
     * from `mwaSignMessages` to keep that method's body under the AC-1 ≤20-LOC
     * budget (per DD-3-1-9 counter rule). Synchronous; emits one terminal
     * `mwa_error{code=NOT_CONNECTED, source_method=<sourceMethod>}` signal and
     * returns Unit. NO scope.launch and NO inflightMap.register on this branch
     * (DD-3-1-6 LOCK — preflight runs entirely on the calling thread).
     *
     * **Story 3-2 T2 (D-3-2-2 Rule 1):** parameterized [sourceMethod] with a
     * `"sign_messages"` default so [mwaSignMessages] (the original 3-1 caller)
     * keeps its single-arg call shape unchanged while [mwaSignTransactions]
     * (the 3-2 caller) supplies `"sign_transactions"`. Story 3-3 will add the
     * third caller with `"sign_and_send"`.
     */
    private fun emitNotConnectedSign(requestId: String, sourceMethod: String = "sign_messages") {
        nativeBridge.postMwaError(
            buildErrorJson(
                requestId,
                MwaError.NotConnected,
                "Not connected to a wallet. Call MWA.connect() first.",
                "kotlin",
                null,
                sourceMethod,
            ).toString(),
        )
    }

    /**
     * Story 3-1 T2 — shared signing pipeline. Consumed by `mwaSignMessages`
     * (this story), `mwaSignTransactions` (Story 3-2), and
     * `mwaSignAndSendTransactions` (Story 3-3).
     *
     * Per DD-3-1-9 responsibility split: this helper handles CAS register,
     * watchdog, client invoke, error translation. The CALL SITE handles
     * sessionState reads (identity / cluster / authToken) — those flow into
     * the lambda closure.
     *
     * Per DD-3-1-2 the block receiver is `MwaClient` (the SDK seam), NOT
     * `MobileWalletAdapterClient` (clientlib-ktx — would break FakeMwaClient
     * test injection). Per DD-3-1-3 the `timeoutMs: Long` parameter
     * propagates through `effectiveWatchdog(timeoutMs)` → `withTimeoutOrNull`.
     *
     * Returns `MwaResult<T>`. Note: when callers pass `MwaClient.signMessages`
     * (which itself returns `MwaResult<SignResult>`), the resulting
     * `MwaResult<MwaResult<SignResult>>` is the documented double-wrap shape
     * (helper-level success/failure distinguished from wallet-level
     * success/failure). Callers pattern-match on the outer `Success`/`Failure`
     * and unwrap the inner result. Stories 3-2 / 3-3 inherit this shape.
     *
     * T2 fills in the body per the pseudocode in `docs/stories/3-1.md`
     * "Private `runSigningOp` helper" section.
     */
    // `@VisibleForTesting internal` (rather than the DD-3-1-1 nominal `private`) to
    // permit T1 tests #5 ("runSigningOp passes through MwaResult.Success unchanged")
    // and #6 ("runSigningOp watchdog timeout returns Failure(MwaError.Timeout)")
    // to invoke the helper directly per the story's 8-test contract. Semantically
    // still file-private — production code outside `GDExtensionAndroidPlugin` has no
    // legitimate caller. Stories 3-2 / 3-3 are in this same file and use it as a
    // sibling member. Same idiom as `internal val sessionState` (line 95) +
    // `@VisibleForTesting`-style overloads on the ctor. Logged as D-3-1-N (Rule 1
    // — visibility relaxed for cross-package test access; DD-3-1-1 narrative
    // updated in Revision 2 of the story file).
    //
    // **C-3-1-W single-wrap signature (Revision 2):** the block returns
    // `MwaResult<X>` (NOT a generic `T`); the helper inspects `Failure` cases
    // internally and routes them through `nativeBridge.postMwaError(...)` with
    // `op.sourceMethod`. Callers receive a flat `MwaResult<X>` (single wrap),
    // NOT `MwaResult<MwaResult<X>>`. This unifies wallet-level Failures
    // (USER_CANCELED, etc.) with helper-level Failures (Timeout, ProtocolError)
    // into one signal-emission path; the CALL SITE only needs to pattern-match
    // on `Success(payload)` and route to `handleSignMessagesSuccess` — all error
    // paths are already terminal-emitted by the helper. Stories 3-2 / 3-3
    // inherit the single-wrap shape unchanged.
    @VisibleForTesting
    internal suspend fun <X> runSigningOp(
        op: SigningOp,
        requestId: String,
        timeoutMs: Long,
        block: suspend MwaClient.() -> MwaResult<X>,
    ): MwaResult<X> = withContext(mainDispatcher) {
        // CAS register — duplicate request id is a caller contract violation.
        if (!inflightMap.register(requestId, clock())) {
            nativeBridge.postMwaError(
                buildErrorJson(
                    requestId,
                    MwaError.ProtocolError,
                    "duplicate requestId on ${op.sourceMethod} — caller must use fresh UUIDs per op",
                    "kotlin",
                    "duplicate_request_id",
                    op.sourceMethod,
                ).toString(),
            )
            return@withContext MwaResult.Failure(MwaError.ProtocolError, null)
        }
        val effectiveMs = effectiveWatchdog(timeoutMs)
        try {
            val client = mwaClientFactory()
            try {
                val rawResult: MwaResult<X>? = withTimeoutOrNull(effectiveMs) {
                    client.block()
                }
                when (rawResult) {
                    null -> {
                        // Watchdog cancelled the lambda; emitTimeoutSign CAS-gates the post.
                        emitTimeoutSign(requestId, effectiveMs, op)
                        MwaResult.Failure(MwaError.Timeout, null)
                    }
                    is MwaResult.Failure -> {
                        // D-3-1-11 single-wrap: route wallet-level Failure (USER_CANCELED, etc.)
                        // through nativeBridge.postMwaError with op.sourceMethod. Caller (e.g.,
                        // mwaSignMessages) only handles Success; all error paths terminate here.
                        if (inflightMap.tryTerminate(requestId)) {
                            nativeBridge.postMwaError(
                                buildErrorJson(
                                    requestId,
                                    rawResult.error,
                                    developerDetails = null,
                                    layer = "kotlin",
                                    cause = null,
                                    sourceMethod = op.sourceMethod,
                                ).toString(),
                            )
                        } else {
                            diagnostics.incrementLateResult()
                            SdkLog.w(TAG, requestId) { "late_result outcome=${rawResult.error.code}" }
                        }
                        rawResult
                    }
                    is MwaResult.Success -> rawResult
                }
            } finally {
                runCatching { client.close() }
            }
        } catch (ce: CancellationException) {
            throw ce
        } catch (ex: Throwable) {
            SdkLog.e(TAG, requestId) {
                "${op.sourceMethod} crashed: ${ex.javaClass.simpleName}: ${ex.message}"
            }
            if (inflightMap.tryTerminate(requestId)) {
                nativeBridge.postMwaError(
                    buildErrorJson(
                        requestId,
                        MwaError.ProtocolError,
                        developerDetails = "${op.sourceMethod} coroutine crashed: ${ex.javaClass.simpleName}",
                        layer = "kotlin",
                        cause = ex.javaClass.simpleName,
                        sourceMethod = op.sourceMethod,
                    ).toString(),
                )
            } else {
                diagnostics.incrementLateResult()
                SdkLog.w(TAG, requestId) { "late_result outcome=crash" }
            }
            MwaResult.Failure(MwaError.ProtocolError, null)
        }
    }

    /**
     * Story 3-1 T2 — success path for sign_messages.
     *
     * CAS-terminates the inflight request and emits `sign_messages_completed` via
     * [NativeBridge.postSignMessagesCompleted] with the 2-key payload shape per
     * arch §3 line 236-242 + DD-3-1-5 (`{request_id, signed_payloads}`). On
     * `tryTerminate` failure (lost CAS race), increments `lateResult` diagnostics
     * and silently drops — terminal-signal invariant per arch §7.3.
     *
     * Note: signing does NOT mutate `MwaSessionState` — the auth_token stays
     * unchanged after a sign call (DD-2-2-5 token rotation contract is
     * reauth-only; CR-48 N/A for sign_messages confirms the invariant).
     */
    private fun handleSignMessagesSuccess(requestId: String, result: SignResult) {
        if (!inflightMap.tryTerminate(requestId)) {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=sign_messages" }
            return
        }
        nativeBridge.postSignMessagesCompleted(
            requestId,
            buildSignSuccessJson(requestId, result.signedPayloads).toString(),
        )
    }

    /**
     * Story 3-2 T2 — success path for sign_transactions. Mirrors
     * [handleSignMessagesSuccess] with two textual deltas: (a) emits
     * `sign_transactions_completed` via [NativeBridge.postSignTransactionsCompleted];
     * (b) calls [buildSignSuccessJson] with `payloadKey = "signed_transactions"`
     * per DD-3-2-3 + D-3-2-1 (parameterized helper, default arg keeps Story 3-1
     * call site unchanged).
     *
     * CAS-terminates the inflight request and emits the 2-key
     * `{request_id, signed_transactions}` payload per DD-3-2-3. On `tryTerminate`
     * failure (lost CAS race), increments `lateResult` diagnostics and silently
     * drops — terminal-signal invariant per arch §7.3 (inherited).
     *
     * Note: signing does NOT mutate `MwaSessionState` — the auth_token stays
     * unchanged after a sign call (DD-2-2-5 reauth-only; CR-48 N/A for
     * sign_transactions confirms the invariant).
     */
    private fun handleSignTransactionsSuccess(requestId: String, result: SignResult) {
        if (!inflightMap.tryTerminate(requestId)) {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=sign_transactions" }
            return
        }
        nativeBridge.postSignTransactionsCompleted(
            requestId,
            buildSignSuccessJson(requestId, result.signedPayloads, payloadKey = "signed_transactions").toString(),
        )
    }

    /**
     * Story 3-3 T2 — success path for sign_and_send. Distinct from
     * [handleSignMessagesSuccess] / [handleSignTransactionsSuccess] in three
     * ways per DD-3-3-E:
     *   (a) emits `sign_and_send_completed` via
     *       [NativeBridge.postSignAndSendCompleted];
     *   (b) uses [buildSignAndSendSuccessJson] (4-key payload —
     *       `{request_id, signatures, submitted_at, confirmation_status}`),
     *       NOT [buildSignSuccessJson] (2-key payload);
     *   (c) removes the breadcrumb via
     *       [SecureTokenStore.removePendingSubmission] BEFORE emitting the
     *       terminal signal (AC-3 ordering invariant).
     *
     * Per DD-3-3-G the cleanup site here is NOT wrapped with
     * `withStorageOrReauthRequired`: a cleanup failure on disk corruption
     * MUST NOT abandon the user's wallet-completed op. Instead the breadcrumb
     * survives to be cleaned up on next launch via [scanPendingSubmissions]
     * (AC-5 one-shot path). The cleanup is invoked through [cleanupBreadcrumb]
     * which centralizes the try/catch + diagnostics-counter pattern.
     *
     * On `tryTerminate` CAS loss, increments `lateResult` diagnostics and
     * silently drops — terminal-signal invariant per arch §7.3 (inherited
     * from 2-1/2-2/3-1/3-2). The breadcrumb is also left for next-launch
     * scan to clean up in this case.
     *
     * T2 fills in the body.
     */
    private fun handleSignAndSendSuccess(requestId: String, result: SignAndSendResult) {
        TODO("Story 3-3 T2 fills in")
    }

    /**
     * Story 3-3 T2 (DD-3-3-C error/timeout cleanup helper) — removes the
     * pending-submission breadcrumb for [requestId]. Invoked from the
     * `runSigningOp` Failure branch (mwa_error / mwa_timeout terminal signals)
     * after the terminal signal has already been emitted. Per DD-3-3-G the
     * cleanup site is NOT wrapped with `withStorageOrReauthRequired`: a
     * cleanup failure must NOT abandon the user's already-emitted error
     * signal.
     *
     * On [com.godotengine.godot_solana_sdk.mwa.store.StorageCorruptException]
     * during the remove, increment `diagnostics.cleanupFailedCount` + log
     * via SdkLog.w + return (DD-4-1-3 wipe-crashed-flag pattern: the
     * breadcrumb survives for next-launch scan to clean up; the user's
     * terminal signal has already fired).
     *
     * Story 5-3's lifecycle observer will invoke this same helper on
     * mwa_cancelled_lifecycle paths once that story lands; T1 stubs an
     * @Disabled placeholder test for AC-4c to make the gap explicit.
     *
     * T2 fills in the body.
     */
    private fun cleanupBreadcrumb(requestId: String) {
        TODO("Story 3-3 T2 fills in")
    }

    /**
     * Story 3-3 T2 (DD-3-3-D scan + AC-5 one-shot semantics) — invoked from
     * [handleConnectSuccess] / [handleReauthSuccess] AFTER the success
     * signal fires. Identity-scoped: filters
     * [SecureTokenStore.listAllPendingSubmissions] by matching
     * `breadcrumbDictJson.identity_uri == identityUri`. For each matching
     * entry: emits `pending_submission_found` via
     * [NativeBridge.postPendingSubmissionFound] with the 6-key payload built
     * by [buildPendingSubmissionFoundJson], THEN calls
     * [SecureTokenStore.removePendingSubmission] (one-shot — prevents
     * duplicate notifications across reconnects).
     *
     * Per DD-3-3-G the [SecureTokenStore.listAllPendingSubmissions] read
     * call IS wrapped with `withStorageOrReauthRequired`: if the prefs file
     * is corrupt at scan time, the wrapper emits `reauth_required` and
     * abandons the scan (the connect/reauth success signal HAS ALREADY
     * fired per DD-3-3-D ordering). Per-entry `removePendingSubmission`
     * calls go through [cleanupBreadcrumb] (NOT wrapped — cleanup failures
     * survive to the next launch).
     *
     * T2 fills in the body.
     */
    private fun scanPendingSubmissions(identityUri: String) {
        TODO("Story 3-3 T2 fills in")
    }

    /**
     * Story 3-3 T2 (DD-3-3-E + AC-1) — builds the `sign_and_send_completed`
     * 4-key payload `{request_id, signatures: JSONArray<base58 strings>,
     * submitted_at: long, confirmation_status: String}`. Distinct from
     * [buildSignSuccessJson] (which produces a 2-key payload for
     * sign_messages / sign_transactions) — DD-3-3-E LOCKS no reuse because
     * the structural shape diverges (3 keys + the request_id, vs 2 keys +
     * the request_id; signatures are base58 strings, NOT base64 byte arrays).
     *
     * `submittedAtMs` is provided by the caller (typically `clock()` at
     * `handleSignAndSendSuccess` time — clock injection is testable;
     * fixture-only would be brittle per the Pre-Impl Audit ambiguity
     * resolution in story §Dev Notes). `confirmationStatus` is the literal
     * "submitted" today; future Story 5-x may extend to "confirmed" /
     * "finalized" if a confirmation-tracking surface lands.
     *
     * T2 fills in the body.
     */
    private fun buildSignAndSendSuccessJson(
        requestId: String,
        signatures: List<String>,
        submittedAtMs: Long,
        confirmationStatus: String,
    ): JSONObject {
        TODO("Story 3-3 T2 fills in")
    }

    /**
     * Story 3-3 T2 (DD-3-3-E + AC-5) — builds the `pending_submission_found`
     * 6-key payload `{request_id, op_type, started_at_ms, tx_count,
     * tx_preview_hashes, recommendation}` from the breadcrumb's stored
     * dict. The input is the JSON object that was originally written by
     * [buildBreadcrumb] + [SecureTokenStore.putPendingSubmission]; this
     * helper extracts the relevant fields and adds the
     * `recommendation: "check_chain_for_signatures"` literal per AC-5.
     *
     * NOTE: the input breadcrumb dict has 7 keys per AC-2 (`request_id`,
     * `op_type`, `cluster`, `identity_uri`, `started_at_ms`, `tx_count`,
     * `tx_preview_hashes`); the output payload has 6 keys (drops `cluster`
     * and `identity_uri` — those are correlation metadata for the scan but
     * not user-facing on the signal — and adds `recommendation`).
     *
     * T2 fills in the body.
     */
    private fun buildPendingSubmissionFoundJson(breadcrumb: JSONObject): JSONObject {
        TODO("Story 3-3 T2 fills in")
    }

    /**
     * Story 3-3 T2 (AC-2 + DD-3-3-F) — builds the breadcrumb 7-key dict for
     * disk persistence (the dict that goes through
     * [SecureTokenStore.putPendingSubmission]). Shape:
     * `{request_id, op_type:"sign_and_send", cluster, identity_uri,
     * started_at_ms, tx_count, tx_preview_hashes:[sha256(tx)[:32]]}`.
     *
     * `tx_preview_hashes` per DD-3-3-F: lowercase hex string per transaction,
     * full 64-char (32-byte / 256-bit) SHA-256 digest. NOT base64; NOT
     * base58; NOT truncated below 64 chars. The plan-spec language
     * `sha256(tx)[:32]` is Python-slice notation over a `bytes(32)` value
     * — equivalent to the full digest.
     *
     * `started_at_ms` is provided by the caller (typically `clock()` at
     * `mwaSignAndSendTransactions` entry time — clock injection per the
     * existing pattern).
     *
     * T2 fills in the body.
     */
    private fun buildBreadcrumb(requestId: String, cluster: String, transactions: List<ByteArray>, startedAtMs: Long): JSONObject {
        TODO("Story 3-3 T2 fills in")
    }

    /**
     * Story 3-1 T2 — emits `mwa_timeout` for the signing path family.
     * Parallel to [emitTimeoutReauth]; uses `sourceMethod = op.sourceMethod`
     * so Stories 3-1 / 3-2 / 3-3 share the helper.
     */
    private fun emitTimeoutSign(requestId: String, watchdogMs: Long, op: SigningOp) {
        if (inflightMap.tryTerminate(requestId)) {
            nativeBridge.postMwaTimeout(buildTimeoutJson(requestId, watchdogMs, op.sourceMethod).toString())
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

        // Story 4-3 DD-4-3-1.a — fail-closed plugin-boundary wrapper. Touch the
        // SecureTokenStore BEFORE the wallet round-trip so a Tink corruption
        // event during cache LOOKUP surfaces as `reauth_required{reason:"keystore_corrupt"}`
        // rather than propagating as an uncaught Throwable → `mwa_error{PROTOCOL_ERROR}`.
        // The actual `listAllKeys()` result is not consumed here (authorize always
        // performs a fresh wallet handshake) — the call exists purely to give the
        // fail-closed wrapper a chance to detect corruption before mwaClientFactory()
        // is invoked. Null-return aborts the op (the wrapper has already emitted
        // the terminal `reauth_required` signal).
        withStorageOrReauthRequired(requestId, "authorize") {
            storeProvider(requireContext()).listAllKeys()
        } ?: return

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
        // Story 4-3 DD-4-3-1 PIVOT — StorageCorruptException now surfaces as
        // `reauth_required{reason:"keystore_corrupt"}` via emitReauthRequiredKeystoreCorrupt
        // (NOT `mwa_error{StorageCorrupt}` — AC-1 contract; legacy emitFailure
        // call deleted to preserve the terminal-signal CAS invariant — no dual-emit).
        val fingerprint: String = try {
            val store = storeProvider(requireContext())
            store.putToken(key, record)
            val salt = store.getOrCreatePerInstallSalt()
            AuthTokenFingerprint.compute(auth.authToken.reveal(), salt)
        } catch (ex: StorageCorruptException) {
            emitReauthRequiredKeystoreCorrupt(requestId, "authorize", ex)
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

    // ---------------- Story 4-3 T1 — STUB-FIRST scaffolding (C-4-3-A) ----------------
    //
    // The 3 helpers below are declared with `TODO("Story 4-3 T2: ...")` bodies so:
    //   1. The new RED test file `MwaAndroidPluginKeystoreCorruptRecoveryTest.kt`
    //      compiles (MockK + verify call sites need the symbols to resolve).
    //   2. Test cases that exercise these helpers fail at runtime with
    //      NotImplementedError — distinct-RED contract per C-4-3-A; each TODO
    //      message names which T2 deliverable still needs implementation.
    //
    // T2 replaces these bodies with the real impl:
    //   - buildReauthRequiredKeystoreCorruptJson — DD-4-3-1.b 5-key Dictionary shape
    //   - emitReauthRequiredKeystoreCorrupt — InflightMap CAS-first + nativeBridge.postReauthRequired
    //   - withStorageOrReauthRequired — fail-closed plugin-boundary wrapper (DD-4-3-1.a)
    //
    // T2 ALSO pivots the existing catch sites at lines ~720 (reauthorize success)
    // and ~1328 (authorize success) from `emitFailure(...MwaError.StorageCorrupt...)`
    // / `emitFailureReauth(...)` to call `emitReauthRequiredKeystoreCorrupt(...)` —
    // those call sites are intentionally LEFT UNCHANGED in T1 so cases 1, 2 of the
    // RED test fail via "verify { postReauthRequired } never invoked" (the cleanest
    // RED signal for the catch-site-pivot half of T2).

    // `inline` form planned by the story spec for `withStorageOrReauthRequired`
    // was downgraded to a non-inline `suspend` helper because
    // `emitReauthRequiredKeystoreCorrupt` is `suspend` and an inline function
    // may not call suspend functions without matching `crossinline` + `suspend`
    // constraints. T2 may refactor if a clean inline form lands; logged as
    // D-T1-RULE1-1 (Rule 1 — minor signature deviation from spec, no
    // behavioral change).

    /**
     * Story 4-3 T2 — DD-4-3-1.b 5-key `reauth_required` Dictionary builder for
     * the `reason=keystore_corrupt` branch. The `cause` field uses
     * [MwaError.StorageCorrupt].`code` (per C-4-3-G) so a future codegen change
     * to the literal string flows through without touching this file.
     *
     * Empty-string defaults for `request_id` / `source_method` (per DD-4-3-1.b)
     * are NOT used by current callers — both PIVOT sites + cache-LOOKUP wrappers
     * always supply both. The empty-string contract is reserved for hypothetical
     * future "corruption fired outside an op" paths.
     */
    private fun buildReauthRequiredKeystoreCorruptJson(requestId: String, sourceMethod: String, ex: StorageCorruptException): String =
        JSONObject()
            .put("reason", "keystore_corrupt")
            .put("request_id", requestId)
            .put("source_method", sourceMethod)
            .put("developer_details", "EncryptedSharedPreferences corrupt: ${ex.cause?.javaClass?.simpleName}")
            .put("cause", MwaError.StorageCorrupt.code)
            .toString()

    /**
     * Story 4-3 T2 — terminal `reauth_required` emitter for the keystore-corrupt
     * branch. Mirrors [emitFailure] / [emitTimeout] structure: CAS-first via
     * [InflightMap.tryTerminate]; on lost-CAS, increments late-result diagnostic
     * and returns silently. On won-CAS, emits via [NativeBridge.postReauthRequired].
     *
     * **Token-leak discipline (`ci/grep_bans.sh` pattern-8):** the SdkLog.w call
     * MUST NOT interpolate the JSON payload — only the human-readable breadcrumb
     * (reason / source_method / exception class name). The `reauthDictJson`
     * variable name is banned from `Log.*` / `SdkLog.*` calls per pattern-8.
     */
    private suspend fun emitReauthRequiredKeystoreCorrupt(requestId: String, sourceMethod: String, ex: StorageCorruptException) {
        if (!inflightMap.tryTerminate(requestId)) {
            diagnostics.incrementLateResult()
            SdkLog.w(TAG, requestId) { "late_result outcome=reauth_required source=$sourceMethod" }
            return
        }
        SdkLog.w(TAG, requestId) {
            "reauth_required reason=keystore_corrupt source=$sourceMethod cause=${ex.cause?.javaClass?.simpleName}"
        }
        nativeBridge.postReauthRequired(buildReauthRequiredKeystoreCorruptJson(requestId, sourceMethod, ex))
    }

    /**
     * Story 4-3 T2 — fail-closed plugin-boundary wrapper (DD-4-3-1.a). Runs
     * [block] and converts any [StorageCorruptException] thrown inside it into
     * a terminal `reauth_required` signal via [emitReauthRequiredKeystoreCorrupt],
     * returning `null` so the caller treats it as the abort signal (mirrors
     * the existing `emitX(...) ; return` early-return pattern across this file).
     *
     * Non-`StorageCorruptException` throwables propagate uncaught — the outer
     * `mwaAuthorize` / `mwaReauthorize` `catch (ex: Throwable)` handler still
     * fires `mwa_error{PROTOCOL_ERROR}` per existing convention.
     *
     * **D-T1-RULE1-1:** kept `suspend` (not `inline`) because
     * [emitReauthRequiredKeystoreCorrupt] is `suspend` — an `inline` function
     * cannot call suspend functions without `crossinline` + `suspend`
     * constraints that would force the lambda parameter to also be suspend.
     * The non-inline form imposes only a small lambda allocation per call,
     * which happens at most once per op.
     */
    private suspend fun <R> withStorageOrReauthRequired(requestId: String, sourceMethod: String, block: () -> R): R? = try {
        block()
    } catch (ex: StorageCorruptException) {
        emitReauthRequiredKeystoreCorrupt(requestId, sourceMethod, ex)
        null
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

// ---------------- Story 3-1 T1 stubs — SigningOp enum + buildSignSuccessJson ----------------

/**
 * Story 3-1 — closed enumeration of signing op-types. Used by [runSigningOp]'s
 * `op` parameter to (a) tag the CAS-register entry for diagnostics and (b)
 * select the `source_method` string for `mwa_error` envelopes. Per DD-3-1-4
 * an enum is sufficient (the three values are closed; the helper does not
 * carry op-specific payload data — that flows via the `block` lambda).
 *
 * Stories 3-2 (`signTransactions`) and 3-3 (`signAndSendTransactions`)
 * inherit `SIGN_TRANSACTIONS` and `SIGN_AND_SEND` respectively without
 * adding new values.
 */
internal enum class SigningOp {
    SIGN_MESSAGES,
    SIGN_TRANSACTIONS,
    SIGN_AND_SEND,
}

/**
 * Maps each [SigningOp] value to its `source_method` string per arch §3
 * line 288 (the enumeration of valid `source_method` values for `mwa_error`
 * envelopes). Used inside [runSigningOp] when emitting timeouts / failures.
 *
 * Snake-case wire format matches the GDScript signal naming convention
 * (Story 1-6 D-4 op-name parity contract).
 */
internal val SigningOp.sourceMethod: String
    get() = when (this) {
        SigningOp.SIGN_MESSAGES -> "sign_messages"
        SigningOp.SIGN_TRANSACTIONS -> "sign_transactions"
        SigningOp.SIGN_AND_SEND -> "sign_and_send"
    }

/**
 * Story 3-1 T2 — success-payload builder for `sign_messages_completed`.
 * Per arch §3 line 236-242 the schema is 2-key:
 * `{request_id, signed_payloads: Array[PackedByteArray]}`. Each
 * `signed_payload` is base64-encoded on the wire (FakeMwaClient /
 * clientlib-ktx convention; the C++/GDScript side decodes back to
 * `PackedByteArray` per the signal schema).
 *
 * Per DD-3-1-5 (Revision-1 renumbering — was DD-3-1-4 in v1) this helper
 * is parallel to [buildSuccessJson] but with a SMALLER, op-specific shape
 * (no auth_token_fingerprint / public_key / wallet metadata since those
 * don't change on a sign call). DO NOT reuse [buildSuccessJson] — its
 * 6-key auth shape is wrong for sign-completed signals.
 *
 * **Story 3-2 T2 (D-3-2-1 Rule 1 + DD-3-2-3):** the helper is shared across
 * the signing-op family rather than spawning siblings. [payloadKey] selects
 * the array's JSON key — `"signed_payloads"` (default — Story 3-1
 * `sign_messages_completed`) or `"signed_transactions"` (Story 3-2
 * `sign_transactions_completed`). The default arg preserves the 3-1 call
 * site unchanged. Story 3-3's `sign_and_send` keeps `"signed_payloads"`
 * since the wire format and the GDScript signal both reuse that name when
 * the array carries pre-submission signed transactions; the post-submission
 * `signature_*` fields ride a separate top-level key, not on this helper.
 */
internal fun buildSignSuccessJson(requestId: String, signedPayloads: List<ByteArray>, payloadKey: String = "signed_payloads"): JSONObject {
    val payloads = JSONArray()
    signedPayloads.forEach { bytes ->
        // base64 encoding matches the FakeMwaClient / clientlib-ktx wire format
        // (sign_messages_success.json fixture stores signed_payloads as base64
        // strings; the C++/GDScript side decodes back to PackedByteArray per the
        // arch §3 line 236-242 signal schema). java.util.Base64 (JVM built-in)
        // is preferred over android.util.Base64 to keep the helper testable on
        // the JVM unit-test classpath without Robolectric.
        payloads.put(java.util.Base64.getEncoder().encodeToString(bytes))
    }
    return JSONObject().apply {
        put("request_id", requestId)
        put(payloadKey, payloads)
    }
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
