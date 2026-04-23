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
internal class InflightMap {

    private val map = ConcurrentHashMap<String, Long>()

    /**
     * Record a new PENDING op. Caller contract (arch §7.2 correlation IDs):
     * `requestId` is unique per authorize/sign call — the architecture
     * generates fresh 8-hex IDs per `connect()` from GDScript, so duplicates
     * should never reach this function. If a duplicate DOES arrive, we leave
     * the existing entry untouched and return `false`. Retries use a fresh
     * `requestId`, not re-registration of the same one.
     *
     * @return `true` if this call created a new entry, `false` if an entry
     *         already existed for `requestId`.
     */
    fun register(requestId: String, startedAtMs: Long): Boolean {
        return map.putIfAbsent(requestId, startedAtMs) == null
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

    /** Number of entries currently PENDING. Intended for tests / diagnostics. */
    fun size(): Int = map.size

    /** Drop all entries. Used on `forget_all` (Story 4-2) and in test teardown. */
    fun clear() {
        map.clear()
    }
}
