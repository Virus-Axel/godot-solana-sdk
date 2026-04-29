package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.core.app.ActivityScenario
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.InflightMap
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import io.mockk.mockk
import io.mockk.slot
import io.mockk.verify
import org.json.JSONObject
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.ComposeWalletActivity
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 5-3 T3 — AC-2 rotation E2E on-device: an in-flight `connect` slot at
 * `Lifecycle.Event.ON_DESTROY` (triggered by `ActivityScenario.recreate()`)
 * fires `mwa_cancelled_lifecycle{request_id, source_method:"connect",
 * reason:"activity_destroyed"}` within the AC-2 1-second budget AND no
 * further terminal signal fires for the same `request_id`.
 *
 * **Pre-registered-slot simplification (Rule 1 deviation from AC-2 literal
 * spec).** AC-2's spec text at `plan.md:402` says "calls `MWA.connect(...)`
 * and triggers a configuration change (rotation) mid-flight". This test
 * pre-registers an in-flight `connect` slot directly in `inflightMap`
 * (bypassing the slow async connect path) because:
 *   1. AC-2's load-bearing invariant is "in-flight slot at ON_DESTROY →
 *      mwa_cancelled_lifecycle fires within 1s + no later terminal", NOT
 *      "the connect call itself runs end-to-end" (the latter is covered by
 *      Story 4-2 / 3-3 androidTests).
 *   2. Pre-seeding eliminates a race where a fast FakeMwaClient connect
 *      could complete before `scenario.recreate()` fires ON_DESTROY,
 *      making the test useless (the slot is already terminated; AC-3 path
 *      runs instead of AC-1/AC-2 path).
 *   3. The simpler shape mirrors the unit-tier `MwaLifecycleObserverTest`
 *      pattern from T1 — one InflightMap, one observer, one ON_DESTROY,
 *      assertions on the resulting emit.
 *
 * **Manual observer attach (per DD-5-3-2).** [PluginTestHarness.freshPlugin]
 * mocks `godot.getActivity()` to return a relaxed-mock Activity that is NOT
 * a `LifecycleOwner`, so the plugin's init-block lifecycle observer
 * registration silently skips with an `SdkLog.w` per the DD-5-3-2 fallback.
 * This test manually attaches a fresh [com.godotengine.godot_solana_sdk.mwa
 * .plugin.MwaLifecycleObserver] (built via the plugin's `@VisibleForTesting`
 * `buildLifecycleObserver()` helper — exercises the production wiring with
 * `::cleanupBreadcrumb` + `::buildCancelledLifecycleJson` method references)
 * to the launched [ComposeWalletActivity]'s `Lifecycle` so the
 * `scenario.recreate()` ON_DESTROY dispatch fires the observer.
 *
 * **Cross-Activity-instance observer continuity is OUT OF SCOPE.** Per
 * DD-5-3-2 step 3 + CR-5-3-A, this test only verifies the OLD Activity's
 * ON_DESTROY correctly fires the observer; in-flight ops on the NEW
 * post-rotation Activity are NOT covered (no fresh-registration code
 * runs across recreate cycles in Story 5-3). CR-5-3-A tracks the future
 * cross-Activity continuity work.
 */
@RunWith(AndroidJUnit4::class)
class AC2RotationCancelsInflightE2ETest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File
    private lateinit var sharedInflight: InflightMap

    @Before
    fun setup() {
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        val testCtx = InstrumentationRegistry.getInstrumentation().context
        fixturesDir = FakeWalletFixtures.extractToCacheDir(testCtx)
        // Hygiene: reset real ESP + sessionState so a leftover record/state
        // from a prior test cannot affect this one (mirrors AC1ForgetAll
        // setup pattern).
        SecureTokenStore(ctx).deleteAll()
        GDExtensionAndroidPlugin.sessionState.clear()

        nativeBridge = mockk(relaxed = true)
        sharedInflight = InflightMap()
        plugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
            inflightMap = sharedInflight,
        )
    }

    @After
    fun teardown() {
        runCatching { SecureTokenStore(ctx).deleteAll() }
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @Test
    fun ac2_rotationCancelsInflightConnectAndEmitsLifecycleSignalWithin1s() {
        // --- Stage 1: pre-register an in-flight `connect` slot ----------------
        // 3-arg per Story 4-2 DD-4-2-1; sourceMethod="connect" so the emit
        // payload's source_method field carries the right literal.
        assertTrue(
            sharedInflight.register("rot-ac2-conn", 1_700_000_000_000L, "connect"),
        )
        assertEquals("pre-condition: 1 in-flight slot", 1, sharedInflight.size())

        ActivityScenario.launch(ComposeWalletActivity::class.java).use { scenario ->
            // Stage 1b: attach the plugin's observer to the launched Activity's
            // Lifecycle. `onActivity` runs on the main thread; the observer
            // is registered against the OLD Activity instance (the one that
            // recreate() will destroy). DefaultLifecycleObserver auto-detaches
            // on ON_DESTROY per the androidx.lifecycle:lifecycle-runtime
            // contract — no explicit removeObserver in teardown.
            scenario.onActivity { activity ->
                activity.lifecycle.addObserver(plugin.buildLifecycleObserver())
            }

            // --- Stage 2: trigger Activity recreation -------------------------
            // recreate() blocks until the OLD Activity is destroyed AND the
            // NEW Activity is created and resumed. ON_DESTROY on the OLD
            // Activity's Lifecycle fires our observer synchronously on the
            // main thread.
            scenario.recreate()

            // --- Stage 3: assert mwa_cancelled_lifecycle fired within 1s ------
            // awaitCondition(1_000L) enforces the AC-2 budget: it `error`s out
            // if postMwaCancelledLifecycle hasn't fired within 1000ms. No
            // separate elapsedMs guard layered on top — the harness's deadline
            // IS the budget assertion, and adding a manual elapsedMs check
            // would have ±20ms polling granularity + post-return overhead that
            // tightens the effective budget below spec (CR-5-3-B flake-risk
            // amplifier per code-review fix #3).
            val payloadSlot = slot<String>()
            PluginTestHarness.awaitCondition(1_000L) {
                runCatching {
                    verify(exactly = 1) {
                        nativeBridge.postMwaCancelledLifecycle(capture(payloadSlot))
                    }
                }.isSuccess
            }

            // Payload shape — DD-5-3-3 LOCKED (3-key dict via
            // buildCancelledLifecycleJson) + DD-5-3-4 LOCKED (reason literal
            // "activity_destroyed").
            val payload = JSONObject(payloadSlot.captured)
            assertEquals(
                "rot-ac2-conn",
                payload.getString("request_id"),
            )
            assertEquals(
                "connect",
                payload.getString("source_method"),
            )
            assertEquals(
                "activity_destroyed",
                payload.getString("reason"),
            )

            // --- Stage 4: AC-2 negative-key guards ---------------------------
            // No further terminal signal fires for the cancelled requestId
            // within the bounded window. Captures regressions where an in-
            // flight op accidentally double-emits (e.g., postConnectCompleted
            // racing with the lifecycle cancel).
            verify(exactly = 0) { nativeBridge.postConnectCompleted(eq("rot-ac2-conn"), any()) }
            verify(exactly = 0) { nativeBridge.postMwaError(any()) }
            verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }

            // CAS-on-tryTerminate removed the slot from inflightMap (DD-5-3-3
            // inheritance from Story 4-2 mwaForgetAll cancel-loop pattern).
            assertEquals(
                "AC-2: in-flight slot removed via tryTerminate post-ON_DESTROY",
                0,
                sharedInflight.size(),
            )
        }
    }
}
