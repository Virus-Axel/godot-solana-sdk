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

    /**
     * Incremented each time a terminal signal emission is dropped because the
     * requestId already transitioned to TERMINATED via a different path (arch
     * §7.3 terminal-signal invariant, AC-6 load-bearing). Paired with
     * `SdkLog.w { "late_result corrId=..." }`.
     */
    fun incrementLateResult(): Long = _lateResultCount.incrementAndGet()

    val lateResultCount: Long get() = _lateResultCount.get()

    internal fun resetForTest() {
        _lateResultCount.set(0)
    }
}
