package com.godotengine.godot_solana_sdk.mwa.plugin

import java.util.concurrent.atomic.AtomicLong

/**
 * Story 5-2 — `last_n_correlation_trace` ring buffer entry per AC-2's 5-key
 * shape. Recorded by the plugin's `recordOnEmit` helper at every terminal-
 * signal emission site (DD-5-2-2 LOCKED — caller-side centralization of the
 * recorder cross-cut). Surfaced via [MwaDiagnostics.lastNCorrelationTrace]
 * → `MwaDiagnosticsBuilder.buildDiagnosticsJson` → `MWA.get_diagnostics()`.
 */
data class CorrelationTraceEntry(
    val requestId: String,
    val sourceMethod: String,
    val terminalSignal: String,
    val elapsedMs: Long,
    val timestampMs: Long,
)

/**
 * Plugin-layer diagnostic counters. Surfaced by `get_diagnostics()` in Story 5-2;
 * populated incrementally by the MWA code paths as they land.
 *
 * Thread-safe via [AtomicLong] for counters. All counters are monotonic (never
 * decrement); [resetForTest] wipes them for unit-test isolation only.
 *
 * Story 5-2 ADDS: ring buffer `lastNCorrelationTrace` (capacity 20, FIFO eviction)
 * + recorder [recordCorrelationTrace] populated by the plugin's `recordOnEmit`
 * cross-cut helper. Thread-safe via `synchronized(correlationLock)` for both
 * insert and snapshot — recorder may be called from `Dispatchers.Default` (op-
 * method coroutines) AND main thread (lifecycle observer onDestroy runs on
 * main); reader (get_diagnostics) is called from main thread.
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

    /**
     * Story 5-2 (DD-5-2-2 LOCKED) — ring buffer of the last 20 terminal-signal
     * emissions. Population is the responsibility of the plugin's `recordOnEmit`
     * cross-cut helper (invoked at every `nativeBridge.post*Native` site). Capacity
     * 20; on insert when full, the oldest entry is evicted. Thread-safe via
     * [correlationLock] for both insert and snapshot.
     */
    private val correlationBuffer: ArrayDeque<CorrelationTraceEntry> = ArrayDeque(RING_BUFFER_CAPACITY)
    private val correlationLock: Any = Any()

    fun recordCorrelationTrace(entry: CorrelationTraceEntry) {
        synchronized(correlationLock) {
            if (correlationBuffer.size == RING_BUFFER_CAPACITY) {
                correlationBuffer.removeFirst()
            }
            correlationBuffer.addLast(entry)
        }
    }

    val lastNCorrelationTrace: List<CorrelationTraceEntry>
        // Kotlin's `toList()` short-circuits to `Collections.singletonList()` when
        // size==1 — a JDK SingletonList throws UnsupportedOperationException on
        // mutation, breaking the AC-2 defensive-copy contract for the size-1 path.
        // `toMutableList()` always allocates a fresh ArrayList so callers can mutate
        // the returned reference without touching the internal buffer.
        get() = synchronized(correlationLock) { correlationBuffer.toMutableList() }

    internal fun resetForTest() {
        _lateResultCount.set(0)
        _cleanupFailedCount.set(0)
        synchronized(correlationLock) { correlationBuffer.clear() }
    }

    companion object {
        /**
         * Story 5-2 AC-2 — ring buffer capacity. 20 chosen per DD-32 LOCKED
         * "ring buffer N=20 correlation traces". Hardcoded; not configurable.
         */
        const val RING_BUFFER_CAPACITY: Int = 20
    }
}
