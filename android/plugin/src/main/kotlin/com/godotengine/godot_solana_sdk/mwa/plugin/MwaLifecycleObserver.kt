package com.godotengine.godot_solana_sdk.mwa.plugin

import androidx.lifecycle.DefaultLifecycleObserver
import androidx.lifecycle.LifecycleOwner
import org.json.JSONObject

/**
 * Story 5-3 — `DefaultLifecycleObserver` that cancels in-flight MWA ops on
 * Activity `ON_DESTROY` (e.g., device rotation, app teardown), emitting
 * `mwa_cancelled_lifecycle{request_id, source_method, reason:"activity_destroyed"}`
 * per snapshotted slot per AC-1.
 *
 * Story 5-3 is the SECOND emitter of `mwa_cancelled_lifecycle` (Story 4-2's
 * `mwaForgetAll` cancel-in-flight loop is the FIRST). The emit shape and
 * CAS-on-`tryTerminate` discipline are inherited verbatim from Story 4-2 per
 * DD-5-3-3 LOCKED — `tryTerminate` returns `false` on already-terminated
 * requestIds, satisfying AC-3 ("no double terminal") automatically. The
 * vocabulary of `reason` literals: `{forget_all_invoked, activity_destroyed}`
 * (per A-12 fixed-vocabulary contract; future literals require an amendment).
 *
 * **Wiring (per DD-5-3-2 LOCKED):** the observer is registered at plugin
 * construction via `(godot.getActivity() as? LifecycleOwner)?.lifecycle
 * ?.addObserver(observer)`. `DefaultLifecycleObserver` auto-detaches on
 * `ON_DESTROY` per the `androidx.lifecycle:lifecycle-runtime` contract — no
 * explicit `removeObserver` is required in plugin teardown. Cross-Activity-
 * recreation continuity (rotation case from AC-2) is OUT OF SCOPE for
 * Story 5-3 and tracked under CR-5-3-A.
 *
 * **Cancellation-spillover symmetry to CR-5-3-A.** `cancelInFlight` invokes
 * `scope.coroutineContext[Job]?.cancelChildren()` on the plugin's coroutine
 * scope. If the plugin instance is reused across Activity recreation cycles
 * (the open registration gap that CR-5-3-A documents), an OLD-Activity
 * `ON_DESTROY` can spuriously cancel ops started against the NEW post-
 * rotation Activity on the same scope — failure mode is silent (the new
 * op's `CancellationException` propagates through the plugin's catch-CE
 * branch without emitting a terminal signal, leaving the user's call
 * hanging). CR-5-3-A's mitigation (per-Activity registration) closes both
 * the registration gap AND this cancellation-spillover gap on the same
 * axis; until that lands, the spillover is a documented narrowing rather
 * than a bug.
 *
 * **Lambda-injection ctor design (per DD-5-3-1 + T1 deviation from DD-5-3-1's
 * inline class shape):** the observer takes `cleanupBreadcrumb`, `payloadBuilder`,
 * and `cancelInFlight` as `() -> Unit`-style lambdas instead of a hard reference
 * to `GDExtensionAndroidPlugin`. This (a) keeps the plugin's `private fun
 * buildCancelledLifecycleJson` + `private fun cleanupBreadcrumb` discipline
 * intact (no visibility widening), (b) makes the observer fully unit-testable
 * without engaging the plugin's surface, (c) lets the plugin's init-block
 * inject `::buildCancelledLifecycleJson` + `::cleanupBreadcrumb` as method
 * references at registration time.
 *
 * **NOT `@UsedByGodot`** — the observer is invoked by Android's lifecycle
 * dispatcher (`androidx.lifecycle:lifecycle-runtime`), NOT Godot's GDScript-
 * side method-name resolution. A-10's `@UsedByGodot`-keep rule does NOT apply;
 * T2 lands an explicit `-keepclassmembers` rule in `consumer-rules.pro` for
 * `MwaLifecycleObserver.onDestroy(LifecycleOwner)` to defend against R8
 * inlining (the JNI bridge between androidx.lifecycle and the JVM-loaded
 * observer goes through reflection-style invocation; an inlined-away override
 * would never fire).
 */
internal class MwaLifecycleObserver(
    private val inflightMap: InflightMap,
    private val nativeBridge: NativeBridge,
    private val diagnostics: MwaDiagnostics,
    private val cleanupBreadcrumb: (requestId: String) -> Unit,
    private val payloadBuilder: (requestId: String, sourceMethod: String, reason: String) -> JSONObject,
    private val cancelInFlight: () -> Unit,
) : DefaultLifecycleObserver {

    /**
     * On Activity `ON_DESTROY`, cancel every in-flight op by:
     *   1. Snapshotting `inflightMap` for `(requestId → sourceMethod)` slots.
     *   2. CAS-claiming each slot via `tryTerminate` (atomic
     *      `ConcurrentHashMap.remove`); if claimed, emit `mwa_cancelled_lifecycle`
     *      with `reason:"activity_destroyed"` (DD-5-3-4 LOCKED literal) and
     *      invoke `cleanupBreadcrumb(requestId)` (DD-5-3-5 unconditional —
     *      idempotent for non-sign_and_send via existing helper's
     *      no-op-on-absent-key behavior).
     *   3. If `tryTerminate` returns `false` (already-terminated requestId,
     *      e.g., a `connect_completed` already fired before rotation),
     *      increment `diagnostics.lateResultCount` and skip emission —
     *      AC-3 invariant satisfied.
     *   4. After the loop, invoke `cancelInFlight()` — the plugin-provided
     *      lambda cancels the plugin's coroutine scope's children so any
     *      suspending op bodies see `CancellationException` and abort.
     */
    override fun onDestroy(owner: LifecycleOwner) {
        val snapshot = inflightMap.snapshot()
        for ((reqId, srcMethod) in snapshot) {
            if (inflightMap.tryTerminate(reqId)) {
                nativeBridge.postMwaCancelledLifecycle(
                    payloadBuilder(reqId, srcMethod, "activity_destroyed").toString(),
                )
                cleanupBreadcrumb(reqId)
            } else {
                diagnostics.incrementLateResult()
            }
        }
        cancelInFlight()
    }
}
