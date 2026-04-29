package com.godotengine.godot_solana_sdk.mwa.plugin

import org.junit.jupiter.api.Assertions.assertDoesNotThrow
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Assertions.assertThrows
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test

/**
 * Story 5-2 T1 — `MwaDiagnostics` ring-buffer surface coverage (TDD red).
 *
 * 5 scenarios exercising AC-2 + DD-5-2-2 LOCKED:
 *   1. AC-2 ring buffer FIFO eviction — 25 entries → size 20, oldest 5 evicted,
 *      newest 20 retained in insertion order.
 *   2. AC-2 [CorrelationTraceEntry] has the 5-key shape `{request_id,
 *      source_method, terminal_signal, elapsed_ms, timestamp_ms}` with the
 *      correct types via `data class` generated equals/hashCode/toString.
 *   3. AC-2 ring buffer is thread-safe under concurrent
 *      [MwaDiagnostics.recordCorrelationTrace] from 4 threads × 50 inserts each
 *      = 200 inserts; final `lastNCorrelationTrace.size == 20`; no exceptions.
 *   4. AC-2 [MwaDiagnostics.recordCorrelationTrace] with empty `requestId` —
 *      no defensive throw at this tier (D-5-2-T1-2 Rule 1: defer to caller-
 *      side validation; the recorder is a leaf trust boundary). Asserts
 *      empty-requestId entry IS recorded (vs. silently dropped) — surfaces a
 *      regression where a future change adds defensive filtering without
 *      updating the contract.
 *   5. AC-2 [MwaDiagnostics.lastNCorrelationTrace] returns a defensive copy
 *      (or immutable List); modifying the returned list does NOT affect the
 *      internal buffer.
 *
 * Story 5-2 ALSO surfaces `late_result_count` (CR-3-3-A closure) and
 * `cleanup_failed_count` is excluded per DD-5-2-5 — those counters' increment
 * behavior is exercised transitively via `MwaLifecycleObserverTest`,
 * `MwaAndroidPluginSignAndSendTest`, and `MwaAndroidPluginForgetAllTest`. No
 * direct counter tests in this file (D-5-2-T1-1 Rule 1: story said "extends
 * existing 4 tests" but no `MwaDiagnosticsTest.kt` existed; legacy counter
 * coverage is implicit via plugin-tier integration tests).
 *
 * 5 scenarios cover the AC-2 ring-buffer surface; all GREEN at T2 (commit
 * f15673d2) which filled in [MwaDiagnostics.recordCorrelationTrace] +
 * [MwaDiagnostics.lastNCorrelationTrace].
 */
class MwaDiagnosticsTest {

    private lateinit var diagnostics: MwaDiagnostics

    @BeforeEach
    fun setup() {
        diagnostics = MwaDiagnostics()
    }

    private fun makeEntry(reqId: String, terminal: String = "connect_completed"): CorrelationTraceEntry = CorrelationTraceEntry(
        requestId = reqId,
        sourceMethod = "connect",
        terminalSignal = terminal,
        elapsedMs = 100L,
        timestampMs = 1_700_000_000_000L,
    )

    @Test
    fun `AC-2 ring buffer FIFO eviction — 25 entries shrink to 20 with newest retained`() {
        // Insert 25 entries; expect oldest 5 evicted, newest 20 retained
        // in insertion order (FIFO).
        for (i in 0 until 25) {
            diagnostics.recordCorrelationTrace(makeEntry("req-$i"))
        }
        val snapshot = diagnostics.lastNCorrelationTrace
        assertEquals(20, snapshot.size, "AC-2: ring buffer capped at 20")
        // Oldest 5 (req-0..req-4) evicted; newest 20 (req-5..req-24) retained.
        assertEquals("req-5", snapshot.first().requestId, "AC-2: oldest retained = req-5 (req-0..4 evicted)")
        assertEquals("req-24", snapshot.last().requestId, "AC-2: newest = req-24")
    }

    @Test
    fun `AC-2 CorrelationTraceEntry has 5-key shape with correct types`() {
        val entry = CorrelationTraceEntry(
            requestId = "req-shape",
            sourceMethod = "sign_messages",
            terminalSignal = "sign_messages_completed",
            elapsedMs = 2_340L,
            timestampMs = 1_700_000_000_000L,
        )
        // 5 properties with exact types (data class compile-time check + runtime field reads).
        assertEquals("req-shape", entry.requestId)
        assertEquals("sign_messages", entry.sourceMethod)
        assertEquals("sign_messages_completed", entry.terminalSignal)
        assertEquals(2_340L, entry.elapsedMs)
        assertEquals(1_700_000_000_000L, entry.timestampMs)
        // data class generated equals/hashCode/toString surface — proves declaration.
        val same = CorrelationTraceEntry(
            "req-shape",
            "sign_messages",
            "sign_messages_completed",
            2_340L,
            1_700_000_000_000L,
        )
        assertEquals(entry, same, "AC-2: data class equals by value")
        assertEquals(entry.hashCode(), same.hashCode(), "AC-2: data class hashCode by value")
        assertNotNull(entry.toString(), "AC-2: data class toString non-null")
    }

    @Test
    fun `AC-2 ring buffer is thread-safe under 4 threads x 50 concurrent inserts`() {
        // 4 threads × 50 inserts = 200 total; capacity 20 → final size == 20;
        // ZERO exceptions (no ConcurrentModificationException, no IndexOutOfBounds).
        val threads = (0 until 4).map { tid ->
            Thread {
                repeat(50) { i ->
                    diagnostics.recordCorrelationTrace(makeEntry("t$tid-i$i"))
                }
            }
        }
        assertDoesNotThrow {
            threads.forEach { it.start() }
            threads.forEach { it.join() }
        }
        assertEquals(
            20,
            diagnostics.lastNCorrelationTrace.size,
            "AC-2: 200 concurrent inserts → final size capped at 20 capacity",
        )
    }

    @Test
    fun `AC-2 recordCorrelationTrace with empty requestId is recorded (no defensive throw at recorder tier)`() {
        // D-5-2-T1-2 Rule 1: the recorder is a leaf trust boundary; caller-side
        // validation is the responsibility of `recordOnEmit`. Empty-requestId
        // is recorded as-is so a regression introducing silent drops surfaces.
        assertDoesNotThrow {
            diagnostics.recordCorrelationTrace(makeEntry(""))
        }
        assertEquals(1, diagnostics.lastNCorrelationTrace.size, "empty-requestId entry IS recorded")
        assertEquals("", diagnostics.lastNCorrelationTrace.first().requestId)
    }

    @Test
    fun `AC-2 lastNCorrelationTrace returns immutable or defensive copy — caller mutation does not leak`() {
        diagnostics.recordCorrelationTrace(makeEntry("req-immut"))
        val snapshot = diagnostics.lastNCorrelationTrace
        // If the returned List is a `MutableList` or backed by the internal buffer,
        // a cast + mutation could leak into subsequent reads. Either: (a) the impl
        // returns `List` (immutable view via .toList()); the cast throws at
        // runtime; OR (b) the returned list IS mutable but is a defensive copy
        // (independent of the internal buffer). Both paths satisfy the AC-2 invariant
        // "modifying returned list does NOT affect internal buffer".

        @Suppress("UNCHECKED_CAST")
        val asMutable = snapshot as? MutableList<CorrelationTraceEntry>
        if (asMutable != null) {
            // Path (b): the returned reference is mutable; mutating it must NOT
            // alter the internal buffer.
            asMutable.clear()
            assertEquals(
                1,
                diagnostics.lastNCorrelationTrace.size,
                "AC-2: mutating the returned list MUST NOT clear internal buffer (defensive copy required)",
            )
        } else {
            // Path (a): the returned reference is immutable. AssertionError on cast
            // → satisfied by `assertThrows` of `ClassCastException` if we tried to
            // cast then mutate. Since we already returned null from the safe cast,
            // this branch is satisfied automatically.
            // Defensive secondary check: ensure mutating an in-test copy doesn't break.
            assertThrows(UnsupportedOperationException::class.java) {
                @Suppress("UNCHECKED_CAST")
                (snapshot as MutableList<CorrelationTraceEntry>).clear()
            }
        }
    }
}
