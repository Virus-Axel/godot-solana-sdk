package com.godotengine.godot_solana_sdk.mwa.plugin

import java.util.concurrent.ConcurrentHashMap

/**
 * Tracks the terminal-signal lifecycle state for in-flight MWA operations
 * keyed by `requestId` (arch §7.3 / DD-23). Callers [register] on op start,
 * then on every would-be terminal emission (success / error / timeout) call
 * [tryTerminate] to CAS-remove the entry. First winner gets `true` and may
 * emit the terminal signal; late callers see `false` and must drop
 * (invariant: AT MOST ONE terminal signal per requestId across
 * `{connect_completed, mwa_error, mwa_timeout, mwa_cancelled_lifecycle}`).
 *
 * **Memory bound:** entries are REMOVED on successful termination — so the
 * map size is bounded by the number of CONCURRENTLY in-flight operations,
 * not by the cumulative op count over the session. A 1000-op session that
 * serializes cleanly retains zero entries at rest; only pending ops are
 * tracked. Late callbacks against removed entries correctly return
 * `false` (map lookup returns `null`), same observable behavior as a
 * TERMINATED marker but without the zombie-entry growth.
 *
 * Thread-safe: [ConcurrentHashMap.putIfAbsent] and [ConcurrentHashMap.remove]
 * are atomic; CAS at both seams.
 */
internal data class InflightEntry(val sourceMethod: String, val startedAtMs: Long)

internal class InflightMap {

    private val map = ConcurrentHashMap<String, InflightEntry>()

    /**
     * Record a new PENDING op. Caller contract (arch §7.2 correlation IDs):
     * `requestId` is unique per authorize/sign call — the architecture
     * generates fresh 8-hex IDs per `connect()` from GDScript, so duplicates
     * should never reach this function. If a duplicate DOES arrive, we leave
     * the existing entry untouched and return `false`. Retries use a fresh
     * `requestId`, not re-registration of the same one.
     *
     * **Story 4-2 DD-4-2-1 (LOCKED) — extended to 3-arg with `sourceMethod`
     * placed AT END for positional-arg compile-compat with the 6 existing
     * 2-arg callers** (D-4-2-T1-1 Rule 1: story-spec placed `sourceMethod`
     * in the middle, but Kotlin positional args make that signature break
     * existing callers; placing the default last preserves
     * `register(requestId, clock())` call-shape and lets T2 back-fill
     * explicit `sourceMethod` values per-call-site). Without
     * `sourceMethod`, [snapshot] reports "unknown" — Story 5-3's
     * lifecycle observer + Story 4-2's `mwa_cancelled_lifecycle` cancel
     * loop both consume `sourceMethod` for the cancellation payload; T2
     * back-fills.
     *
     * @return `true` if this call created a new entry, `false` if an entry
     *         already existed for `requestId`.
     */
    fun register(requestId: String, startedAtMs: Long, sourceMethod: String = "unknown"): Boolean {
        return map.putIfAbsent(requestId, InflightEntry(sourceMethod, startedAtMs)) == null
    }

    /**
     * Atomically remove `requestId`. Returns `true` if the entry was present
     * (the caller is the FIRST terminal signal and may emit); `false` if
     * the entry was absent (late callback, OR never-registered — either way,
     * drop the signal). Callers with `false` MUST increment
     * [MwaDiagnostics.lateResultCount] and log via
     * `SdkLog.w { "late_result corrId=$requestId" }`.
     */
    fun tryTerminate(requestId: String): Boolean {
        return map.remove(requestId) != null
    }

    /**
     * Story 4-2 DD-4-2-3 — read-only snapshot of currently-pending ops as
     * `(requestId → sourceMethod)`. Used by `mwaForgetAll` to iterate
     * in-flight slots and emit `mwa_cancelled_lifecycle{request_id,
     * source_method, reason:"forget_all_invoked"}` per slot via
     * [tryTerminate] CAS BEFORE the wipe starts (AC-2 ordering). The
     * snapshot is a defensive copy — concurrent mutation of the
     * underlying map (other ops registering / terminating) does NOT
     * affect iteration.
     */
    fun snapshot(): Map<String, String> = map.mapValues { it.value.sourceMethod }

    /** Number of entries currently PENDING. Intended for tests / diagnostics. */
    fun size(): Int = map.size

    /** Drop all entries. Used on `forget_all` (Story 4-2) and in test teardown. */
    fun clear() {
        map.clear()
    }
}
