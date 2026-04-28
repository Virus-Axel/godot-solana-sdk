package com.godotengine.godot_solana_sdk.mwa.plugin

import java.util.concurrent.atomic.AtomicLong

/**
 * Plugin-layer diagnostic counters. Surfaced by `get_diagnostics()` in Story 5-2;
 * populated incrementally by the MWA code paths as they land.
 *
 * Thread-safe via [AtomicLong]. All counters are monotonic (never decrement);
 * [resetForTest] wipes them for unit-test isolation only.
 */
class MwaDiagnostics {

    private val _lateResultCount = AtomicLong(0)
    private val _cleanupFailedCount = AtomicLong(0)

    /**
     * Incremented each time a terminal signal emission is dropped because the
     * requestId already transitioned to TERMINATED via a different path (arch
     * §7.3 terminal-signal invariant, AC-6 load-bearing). Paired with
     * `SdkLog.w { "late_result corrId=..." }`.
     */
    fun incrementLateResult(): Long = _lateResultCount.incrementAndGet()

    val lateResultCount: Long get() = _lateResultCount.get()

    /**
     * Story 3-3 (DD-3-3-G + DD-4-1-3 wipe-crashed-flag pattern) — incremented
     * each time a pending-submission breadcrumb cleanup attempt fails because
     * `SecureTokenStore.removePendingSubmission` raised `StorageCorruptException`.
     * The cleanup site is intentionally NOT wrapped with `withStorageOrReauthRequired`
     * (a cleanup failure must NOT abandon the user's already-emitted terminal
     * signal); the counter + a non-secret SdkLog event marker are the only
     * observability for this failure mode. The breadcrumb survives to be cleaned
     * up on next launch via `scanPendingSubmissions`.
     */
    fun incrementCleanupFailedCount(): Long = _cleanupFailedCount.incrementAndGet()

    val cleanupFailedCount: Long get() = _cleanupFailedCount.get()

    internal fun resetForTest() {
        _lateResultCount.set(0)
        _cleanupFailedCount.set(0)
    }
}
