package com.godotengine.godot_solana_sdk.mwa.plugin

import androidx.lifecycle.LifecycleOwner
import io.mockk.mockk
import io.mockk.slot
import io.mockk.verify
import org.json.JSONObject
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test

/**
 * Story 5-3 T1 — `MwaLifecycleObserver.onDestroy` coverage (TDD red).
 *
 * 3 scenarios exercising the LOCKED design decisions DD-5-3-1..-5:
 *   1. AC-1 happy path — pre-register a `connect` slot then trigger
 *      `onDestroy(mockLifecycleOwner)`; assert `nativeBridge
 *      .postMwaCancelledLifecycle` invoked once with the expected 3-key
 *      payload (`request_id`, `source_method`, `reason:"activity_destroyed"`),
 *      `cleanupBreadcrumb` invoked once with the requestId,
 *      `cancelInFlight` invoked once.
 *   2. AC-3 already-terminated — register slot then immediately
 *      `tryTerminate` it (simulates a `connect_completed` already fired
 *      before rotation); trigger `onDestroy`; assert
 *      `postMwaCancelledLifecycle` invoked ZERO times,
 *      `cleanupBreadcrumb` invoked ZERO times, `cancelInFlight`
 *      invoked once (always, regardless of inflight state).
 *   3. Multi-slot — pre-register 3 distinct slots with different
 *      `source_method` values (`connect`, `sign_messages`,
 *      `sign_and_send`); trigger `onDestroy`; assert exactly 3
 *      `postMwaCancelledLifecycle` invocations, each carrying the
 *      correct `request_id` + `source_method` + `reason:"activity_destroyed"`;
 *      `cleanupBreadcrumb` invoked 3 times (DD-5-3-5 unconditional).
 *
 * **TDD red baseline (T1):** all 3 tests fail at runtime against the
 * `TODO("Story 5-3 T2 fills in")` body in
 * [MwaLifecycleObserver.onDestroy]. T2 turns them GREEN.
 *
 * Observer is constructed via the lambda-injection ctor pattern (per
 * T1 deviation from DD-5-3-1's inline class shape): mock collaborators
 * for `nativeBridge` + `diagnostics`; pure-function lambdas for
 * `cleanupBreadcrumb` (test-side capture), `payloadBuilder` (real
 * `JSONObject` build mirroring `GDExtensionAndroidPlugin
 * .buildCancelledLifecycleJson`), and `cancelInFlight` (test-side
 * counter).
 */
class MwaLifecycleObserverTest {

    private lateinit var inflightMap: InflightMap
    private lateinit var nativeBridge: NativeBridge
    private lateinit var diagnostics: MwaDiagnostics
    private lateinit var cleanupCalls: MutableList<String>
    private var cancelInFlightCalls: Int = 0

    private val clock: () -> Long = { 1_700_000_000_000L }

    @BeforeEach
    fun setup() {
        inflightMap = InflightMap()
        nativeBridge = mockk(relaxed = true)
        diagnostics = MwaDiagnostics()
        cleanupCalls = mutableListOf()
        cancelInFlightCalls = 0
    }

    /** Builds an observer with test-side capture lambdas. Mirrors the
     * plugin's init-block wiring per T2 (which passes
     * `::buildCancelledLifecycleJson` + `::cleanupBreadcrumb` method
     * references); here we use pure-function lambdas for unit isolation. */
    private fun buildObserver(): MwaLifecycleObserver = MwaLifecycleObserver(
        inflightMap = inflightMap,
        nativeBridge = nativeBridge,
        diagnostics = diagnostics,
        cleanupBreadcrumb = { requestId -> cleanupCalls.add(requestId) },
        payloadBuilder = { requestId, sourceMethod, reason ->
            JSONObject().apply {
                put("request_id", requestId)
                put("source_method", sourceMethod)
                put("reason", reason)
            }
        },
        cancelInFlight = { cancelInFlightCalls += 1 },
    )

    // ---------------- AC-1 ----------------

    @Test
    fun `AC-1 ON_DESTROY with in-flight connect emits mwa_cancelled_lifecycle{reason=activity_destroyed}`() {
        // Pre-register an in-flight slot mirroring a real connect-in-flight
        // state (Story 4-2 DD-4-2-1 3-arg register signature).
        assertTrue(inflightMap.register("conn-ac1", clock(), "connect"))
        assertEquals(1, inflightMap.size(), "pre-condition: 1 in-flight slot")

        val observer = buildObserver()
        val mockOwner: LifecycleOwner = mockk(relaxed = true)

        observer.onDestroy(mockOwner)

        // AC-1 emit assertion — exactly one mwa_cancelled_lifecycle fired
        // with the 3-key payload shape per DD-5-3-4 (reason literal).
        val payloadSlot = slot<String>()
        verify(exactly = 1) { nativeBridge.postMwaCancelledLifecycle(capture(payloadSlot)) }
        val payload = JSONObject(payloadSlot.captured)
        assertEquals("conn-ac1", payload.getString("request_id"), "AC-1: request_id from in-flight slot")
        assertEquals("connect", payload.getString("source_method"), "AC-1: source_method from inflightMap")
        assertEquals(
            "activity_destroyed",
            payload.getString("reason"),
            "AC-1 / DD-5-3-4: reason literal (NOT 'lifecycle_teardown')",
        )

        // DD-5-3-5 unconditional cleanup invocation.
        assertEquals(listOf("conn-ac1"), cleanupCalls, "AC-1: cleanupBreadcrumb invoked once with requestId")

        // cancelInFlight always invoked once after the loop.
        assertEquals(1, cancelInFlightCalls, "AC-1: cancelInFlight invoked once")

        // Post-emit: the in-flight slot was tryTerminate'd (CAS-removed).
        assertEquals(0, inflightMap.size(), "AC-1: in-flight slot must be removed after onDestroy")
    }

    // ---------------- AC-3 ----------------

    @Test
    fun `AC-3 already-terminated requestId not re-emitted on ON_DESTROY`() {
        // Register slot then immediately tryTerminate — simulates a
        // connect_completed already fired before rotation.
        assertTrue(inflightMap.register("conn-ac3", clock(), "connect"))
        assertTrue(inflightMap.tryTerminate("conn-ac3"))
        assertEquals(0, inflightMap.size(), "pre-condition: slot already terminated")

        val observer = buildObserver()
        val mockOwner: LifecycleOwner = mockk(relaxed = true)

        observer.onDestroy(mockOwner)

        // AC-3: NO mwa_cancelled_lifecycle for the already-terminated slot.
        // (snapshot is empty since the entry was tryTerminate'd; the loop
        // does not iterate, so no late-result counter increment either —
        // late-result fires only when CAS LOSES inside the loop, not when
        // the loop is empty to begin with.)
        verify(exactly = 0) { nativeBridge.postMwaCancelledLifecycle(any()) }
        assertTrue(cleanupCalls.isEmpty(), "AC-3: cleanupBreadcrumb NOT invoked for already-terminated slot")

        // cancelInFlight always invoked once regardless of inflight state.
        assertEquals(1, cancelInFlightCalls, "AC-3: cancelInFlight invoked once unconditionally")
    }

    // ---------------- Multi-slot ----------------

    @Test
    fun `multi-slot ON_DESTROY emits one mwa_cancelled_lifecycle per slot with correct source_method`() {
        // Pre-register 3 distinct slots with different source_method values.
        assertTrue(inflightMap.register("multi-1", clock(), "connect"))
        assertTrue(inflightMap.register("multi-2", clock(), "sign_messages"))
        assertTrue(inflightMap.register("multi-3", clock(), "sign_and_send"))
        assertEquals(3, inflightMap.size(), "pre-condition: 3 in-flight slots")

        val observer = buildObserver()
        val mockOwner: LifecycleOwner = mockk(relaxed = true)

        observer.onDestroy(mockOwner)

        // Capture all 3 emit payloads via mutableList (MockK pattern for
        // multi-invocation slot capture).
        val capturedPayloads = mutableListOf<String>()
        verify(exactly = 3) { nativeBridge.postMwaCancelledLifecycle(capture(capturedPayloads)) }
        assertEquals(3, capturedPayloads.size, "exactly 3 emits, one per slot")

        // Parse + index by request_id; assert each (request_id, source_method)
        // pair matches the seeded inflightMap state. Iteration order from
        // ConcurrentHashMap is NOT deterministic, so index by id.
        val byId: Map<String, JSONObject> = capturedPayloads
            .map { JSONObject(it) }
            .associateBy { it.getString("request_id") }
        assertEquals(setOf("multi-1", "multi-2", "multi-3"), byId.keys, "all 3 requestIds emitted")
        assertEquals("connect", byId.getValue("multi-1").getString("source_method"))
        assertEquals("sign_messages", byId.getValue("multi-2").getString("source_method"))
        assertEquals("sign_and_send", byId.getValue("multi-3").getString("source_method"))

        // DD-5-3-4 reason literal applies to ALL emits.
        for (id in listOf("multi-1", "multi-2", "multi-3")) {
            assertEquals(
                "activity_destroyed",
                byId.getValue(id).getString("reason"),
                "DD-5-3-4: reason literal applies to slot $id",
            )
        }

        // DD-5-3-5: cleanupBreadcrumb invoked once per slot (3 total).
        assertEquals(
            setOf("multi-1", "multi-2", "multi-3"),
            cleanupCalls.toSet(),
            "DD-5-3-5: cleanupBreadcrumb invoked once per snapshotted slot (unconditional, idempotent)",
        )
        assertEquals(3, cleanupCalls.size, "DD-5-3-5: exactly 3 cleanup calls (one per slot)")

        // cancelInFlight always invoked once after the loop.
        assertEquals(1, cancelInFlightCalls, "multi-slot: cancelInFlight invoked once after the loop")

        // Post-emit: all 3 slots removed via tryTerminate.
        assertEquals(0, inflightMap.size(), "multi-slot: all slots removed after onDestroy")
    }
}
