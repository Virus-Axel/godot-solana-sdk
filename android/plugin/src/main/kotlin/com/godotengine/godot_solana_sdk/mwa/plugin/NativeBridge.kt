package com.godotengine.godot_solana_sdk.mwa.plugin

import plugin.walletadapterandroid.GDExtensionAndroidPlugin

/**
 * Kotlin → C++ callback seam for the MWA authorize/sign pipeline. The plugin
 * layer builds a typed payload (serialized to JSON) and hands it here; the
 * production impl invokes the `external fun` JNI declarations on the plugin
 * companion so the C++ dispatcher can emit to Godot main via
 * `Callable::call_deferred` (D-5 / D-6).
 *
 * **A-12 arity discipline** — the seam mirrors [docs/amendments.md#A-12]'s signal
 * shape directly:
 *   - `connect_completed` (and the other `*_completed` signals) are 2-param on
 *     the GDScript side: `(request_id: String, result: Dictionary)`. The seam
 *     method keeps `requestId` as a separate arg so the C++ side can route it
 *     into the dispatcher's arity-2 Array (`Array::make(request_id, result)`).
 *   - `mwa_error`, `mwa_timeout`, `mwa_cancelled_lifecycle` are 1-param on the
 *     GDScript side: `(payload: Dictionary)` with `request_id` embedded as a
 *     field. The seam methods take ONLY the JSON blob — do NOT add a separate
 *     `requestId` arg; the C++ side would have no place to put it without
 *     violating A-12's arity-1 contract for error/lifecycle signals.
 *
 * **Token-leak hazard — read before modifying** — every `*DictJson` parameter
 * on every method here is a serialized Dictionary that includes the wallet's
 * `auth_token` (connect path) or enough correlation metadata to reconstruct
 * a session (error/timeout/cancellation paths). **NEVER** log, interpolate,
 * include in exception messages, or otherwise materialize these values in
 * output. `ci/grep_bans.sh` pattern-8 bans the variable names
 * (`resultDictJson`, `errorDictJson`, `timeoutDictJson`,
 * `cancelledDictJson`, `reauthDictJson`) from any `Log.*` or `SdkLog.*`
 * call — that catch enforces the rule at CI-time.
 *
 * Two implementations:
 *  - [DefaultNativeBridge] — Story 2-1 T4 stub that logs method NAME only (no
 *    payload content). The `external fun` wiring + C++ JNI entry points land
 *    in T5. Keeping T4 green without a JNI runtime lets tests inject mocks
 *    and the plugin code compiles standalone.
 *  - Test doubles — inject a recording impl via the `@VisibleForTesting` ctor
 *    overload on the plugin.
 */
internal interface NativeBridge {

    /**
     * 2-param `connect_completed` per A-12 — `requestId` becomes the first
     * signal argument, `resultDictJson` becomes the second (Dictionary).
     *
     * **WARNING — `resultDictJson` contains the wallet's auth_token.** Do NOT
     * log, interpolate, or include in exception messages.
     */
    fun postConnectCompleted(requestId: String, resultDictJson: String)

    /**
     * 2-param `disconnect_completed` per A-12 — Story 2-3. `requestId` is the
     * first signal argument; `resultDictJson` is the second (Dictionary
     * carrying `{request_id, source_method: "disconnect"}` only — no secret
     * material).
     *
     * Even though disconnect's payload is not secret, the 2-param
     * `*_completed` family uniformly warns against payload logging to keep the
     * seam convention uniform and the grep-ban surface consistent.
     *
     * **WARNING — do NOT log or interpolate `resultDictJson`.**
     */
    fun postDisconnectCompleted(requestId: String, resultDictJson: String)

    /**
     * 2-param `reauthorize_completed` per A-12 — Story 2-2. `requestId` is the
     * first signal argument; `resultDictJson` is the second (Dictionary carrying
     * `{request_id, auth_token_fingerprint, public_key, wallet_label,
     * wallet_icon_uri, cluster}` — same 6-key shape as `connect_completed`
     * per DD-2-2-4).
     *
     * **WARNING — `resultDictJson` contains `auth_token_fingerprint` (a
     * derivative of the wallet's auth_token). Do NOT log, interpolate, or
     * include in exception messages.** `ci/grep_bans.sh` pattern-8 bans
     * `resultDictJson` from any `Log.*` / `SdkLog.*` call.
     */
    fun postReauthorizeCompleted(requestId: String, resultDictJson: String)

    /**
     * 2-param `sign_messages_completed` per A-12 — Story 3-1. `requestId` is the
     * first signal argument; `resultDictJson` is the second (Dictionary carrying
     * `{request_id, signed_payloads: Array[PackedByteArray]}` per arch §3
     * line 236-242 — 2-key shape, NOT the 6-key auth-success shape).
     *
     * Unlike `postConnectCompleted` / `postReauthorizeCompleted`, the
     * `resultDictJson` here does NOT carry secret material — `signed_payloads`
     * are detached signatures (public bytes by definition; the wallet has
     * already returned them and the caller intends to forward them to the
     * blockchain). The 2-param `*_completed` family uniformly warns against
     * payload logging to keep the seam convention uniform and the grep-ban
     * surface consistent.
     *
     * **WARNING — do NOT log or interpolate `resultDictJson`.** Even though
     * signing payloads are not secret, uniform payload-log discipline avoids
     * accidental leaks if a future maintainer copy-pastes this method into a
     * branch that DOES carry secrets (e.g., a hypothetical `sign_personal`
     * variant that round-trips the auth_token).
     */
    fun postSignMessagesCompleted(requestId: String, resultDictJson: String)

    /**
     * 2-param `sign_transactions_completed` per A-12 — Story 3-2. `requestId` is
     * the first signal argument; `resultDictJson` is the second (Dictionary
     * carrying `{request_id, signed_transactions: Array[PackedByteArray]}` per
     * arch §3 — 2-key shape, mirrors `sign_messages_completed` with the
     * payload-key renamed to `signed_transactions` per DD-3-2-3 + D-3-2-1).
     *
     * Like `postSignMessagesCompleted`, the `resultDictJson` here does NOT
     * carry secret material — `signed_transactions` are wallet-signed serialized
     * Solana transactions (public bytes by definition). The 2-param `*_completed`
     * family uniformly warns against payload logging to keep the seam convention
     * uniform and the grep-ban surface consistent.
     *
     * **WARNING — do NOT log or interpolate `resultDictJson`.** `ci/grep_bans.sh`
     * pattern-8 bans `resultDictJson` from any `Log.*` / `SdkLog.*` call.
     */
    fun postSignTransactionsCompleted(requestId: String, resultDictJson: String)

    /**
     * 2-param `sign_and_send_completed` per A-12 — Story 3-3. `requestId` is
     * the first signal argument; `resultDictJson` is the second (Dictionary
     * carrying the 4-key shape `{request_id, signatures: Array[String],
     * submitted_at: int, confirmation_status: String}` per AC-1 + DD-3-3-E).
     *
     * Distinct from the 2-key `sign_messages_completed` /
     * `sign_transactions_completed` shapes — `sign_and_send` MUST surface the
     * cluster-submission timestamp + status because the wallet has already
     * forwarded the transaction(s) to the chosen RPC endpoint by the time
     * the signal fires. `signatures` are base58-encoded transaction
     * signatures (NOT base64-encoded byte arrays; the Solana convention for
     * tx signatures is base58, distinct from the base64 wire format used
     * for signed payloads in sign_messages / sign_transactions).
     * `confirmation_status` is the literal "submitted" today; future Story
     * 5-x may extend with "confirmed" / "finalized" if a confirmation-
     * tracking surface is added.
     *
     * Like the rest of the 2-param `*_completed` family, the `resultDictJson`
     * here does NOT carry secret material — `signatures` are public on-chain
     * artifacts; `submitted_at` is a wall-clock timestamp; `confirmation_status`
     * is a fixed-vocabulary enum string. The 2-param family uniformly warns
     * against payload logging to keep the seam convention uniform.
     *
     * **WARNING — do NOT log or interpolate `resultDictJson`.** `ci/grep_bans.sh`
     * pattern-8 bans `resultDictJson` from any `Log.*` / `SdkLog.*` call.
     */
    fun postSignAndSendCompleted(requestId: String, resultDictJson: String)

    /**
     * 2-param `deauthorize_completed` per A-12 — Story 4-1. `requestId` is the
     * first signal argument; `resultDictJson` is the second (Dictionary
     * carrying the 4-key shape `{request_id, remote_revoke_succeeded,
     * local_cache_cleared, warning}` per arch.md:669 — distinct from the
     * 6-key auth-success shape and the 2-key sign-success shape).
     *
     * The `resultDictJson` does NOT carry secret material (no auth_token, no
     * fingerprint) — `remote_revoke_succeeded` and `local_cache_cleared` are
     * Boolean state flags; `warning` is the literal `"remote_unreachable"` or
     * empty. The 2-param `*_completed` family uniformly warns against payload
     * logging anyway to keep the seam convention uniform and the grep-ban
     * surface consistent.
     *
     * **WARNING — do NOT log or interpolate `resultDictJson`.**
     */
    fun postDeauthorizeCompleted(requestId: String, resultDictJson: String)

    /**
     * 1-param `mwa_error` per A-12. `request_id` is embedded inside
     * `errorDictJson` at the `request_id` field (A-14 10-key shape).
     *
     * **WARNING — `errorDictJson` carries developer-details + correlation data.**
     * Do NOT log or interpolate into messages.
     */
    fun postMwaError(errorDictJson: String)

    /**
     * 1-param `mwa_timeout` per A-12. `request_id` is embedded inside
     * `timeoutDictJson`.
     *
     * **WARNING — do NOT log or interpolate `timeoutDictJson`.**
     */
    fun postMwaTimeout(timeoutDictJson: String)

    /**
     * 1-param `mwa_cancelled_lifecycle` per A-12. Fires when an in-flight op
     * is cancelled by Android lifecycle teardown (Story 5-3 lifecycle
     * observer). Included here so the 4-signal terminal-signal invariant
     * ([InflightMap] kdoc) is backed by a full seam surface; the authorize
     * path does not invoke this method directly.
     *
     * **WARNING — do NOT log or interpolate `cancelledDictJson`.**
     */
    fun postMwaCancelledLifecycle(cancelledDictJson: String)

    /**
     * 1-param `reauth_required` per A-12. Story 4-3 — emits a lifecycle signal
     * carrying a corrupt-recovery or session-renewal payload. The argument is
     * the FULL JSON Dictionary blob built by
     * [GDExtensionAndroidPlugin]'s `buildReauthRequiredKeystoreCorruptJson`
     * helper (DD-4-3-1.b 5-key shape: `reason`, `request_id`, `source_method`,
     * `developer_details`, `cause`).
     *
     * Used by the plugin-boundary fail-closed wrapper
     * `withStorageOrReauthRequired` so [com.godotengine.godot_solana_sdk.mwa.store.StorageCorruptException]
     * surfaces as a typed lifecycle signal to GDScript instead of an opaque
     * `mwa_error{PROTOCOL_ERROR}` (AC-1).
     *
     * **WARNING — `reauthDictJson` MAY contain `developer_details` with
     * exception class names but never authToken bytes per DD-4-3-1.b.** Do
     * NOT log, interpolate, or include in exception messages. Story 4-3 T2
     * extends `ci/grep_bans.sh` pattern-8 to ban this variable name from any
     * `Log.*` / `SdkLog.*` call.
     */
    fun postReauthRequired(reauthDictJson: String)

    /**
     * 1-param `pending_submission_found` per A-12 — Story 3-3 (DD-3-3-E). Fires
     * on the next successful `connect` or `reauthorize` AFTER the success signal
     * if a stale sign_and_send breadcrumb survives a process death (AC-5). The
     * argument is the FULL JSON Dictionary blob built by
     * [GDExtensionAndroidPlugin]'s `buildPendingSubmissionFoundJson` helper
     * (DD-3-3-E 6-key shape: `request_id`, `op_type`, `started_at_ms`,
     * `tx_count`, `tx_preview_hashes`, `recommendation`).
     *
     * One-shot semantics per AC-5: each pending breadcrumb produces ONE
     * `pending_submission_found` emission; the breadcrumb is then removed from
     * `SecureTokenStore` so reconnects don't double-fire.
     *
     * The payload does NOT carry secret material — `request_id` is the
     * caller-side correlation token; `tx_preview_hashes` are SHA-256 hex
     * digests of the transaction bytes (NOT the signed bytes; transactions
     * may not even have signatures yet at the breadcrumb-write moment per
     * DD-3-3-B write-then-call ordering). `recommendation` is a
     * fixed-vocabulary enum (today: "check_chain_for_signatures").
     *
     * **WARNING — `pendingDictJson` MAY contain `tx_preview_hashes`** which,
     * while not secret, can correlate transactions across observability
     * channels. Do NOT log, interpolate, or include in exception messages.
     * `ci/grep_bans.sh` pattern-8 bans `pendingDictJson` from any `Log.*` /
     * `SdkLog.*` call (DD-3-3-E.a — extended at Story 3-3 T2).
     */
    fun postPendingSubmissionFound(pendingDictJson: String)
}

/**
 * Default (production) impl. Story 2-1 T5 wires to the `external fun` JNI
 * declarations on [GDExtensionAndroidPlugin]'s companion —
 * `postConnectCompletedNative` etc. — which the JVM resolves to the
 * `Java_plugin_walletadapterandroid_GDExtensionAndroidPlugin_00024Companion_*`
 * JNIEXPORT functions in `src/mwa/mwa_android_bridge_jni.cpp` via the
 * `System.loadLibrary(...)` in the plugin's class init.
 *
 * Do NOT log payload content in this class — pattern-8 in
 * `ci/grep_bans.sh` catches accidental leaks (banned variable names:
 * `resultDictJson`, `errorDictJson`, `timeoutDictJson`, `cancelledDictJson`,
 * `reauthDictJson`).
 */
internal object DefaultNativeBridge : NativeBridge {
    override fun postConnectCompleted(requestId: String, resultDictJson: String) {
        GDExtensionAndroidPlugin.postConnectCompletedNative(requestId, resultDictJson)
    }

    override fun postDisconnectCompleted(requestId: String, resultDictJson: String) {
        GDExtensionAndroidPlugin.postDisconnectCompletedNative(requestId, resultDictJson)
    }

    override fun postReauthorizeCompleted(requestId: String, resultDictJson: String) {
        GDExtensionAndroidPlugin.postReauthorizeCompletedNative(requestId, resultDictJson)
    }

    override fun postSignMessagesCompleted(requestId: String, resultDictJson: String) {
        GDExtensionAndroidPlugin.postSignMessagesCompletedNative(requestId, resultDictJson)
    }

    override fun postSignTransactionsCompleted(requestId: String, resultDictJson: String) {
        GDExtensionAndroidPlugin.postSignTransactionsCompletedNative(requestId, resultDictJson)
    }

    override fun postSignAndSendCompleted(requestId: String, resultDictJson: String) {
        GDExtensionAndroidPlugin.postSignAndSendCompletedNative(requestId, resultDictJson)
    }

    override fun postDeauthorizeCompleted(requestId: String, resultDictJson: String) {
        GDExtensionAndroidPlugin.postDeauthorizeCompletedNative(requestId, resultDictJson)
    }

    override fun postMwaError(errorDictJson: String) {
        GDExtensionAndroidPlugin.postMwaErrorNative(errorDictJson)
    }

    override fun postMwaTimeout(timeoutDictJson: String) {
        GDExtensionAndroidPlugin.postMwaTimeoutNative(timeoutDictJson)
    }

    override fun postMwaCancelledLifecycle(cancelledDictJson: String) {
        GDExtensionAndroidPlugin.postMwaCancelledLifecycleNative(cancelledDictJson)
    }

    override fun postReauthRequired(reauthDictJson: String) {
        GDExtensionAndroidPlugin.postReauthRequiredNative(reauthDictJson)
    }

    override fun postPendingSubmissionFound(pendingDictJson: String) {
        GDExtensionAndroidPlugin.postPendingSubmissionFoundNative(pendingDictJson)
    }
}
