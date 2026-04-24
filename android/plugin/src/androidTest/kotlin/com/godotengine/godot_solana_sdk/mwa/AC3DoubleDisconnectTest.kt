package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import io.mockk.mockk
import io.mockk.slot
import io.mockk.verify
import org.json.JSONObject
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNotEquals
import org.junit.Assert.assertNull
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 2-3 T5 — AC-3 on-device: double `MWA.disconnect()` is idempotent at
 * the state level AND emits `disconnect_completed` once per call.
 *
 * Per DD-2-3-2 (CAS-first-then-wipe-then-emit) + DD-2-3-4 (fresh request_id
 * per call, no collapsing): calling `mwaDisconnect` twice with different
 * request_ids produces two independent emissions. The wipe on the second
 * call is a no-op on already-cleared state — this is the idempotence spec.
 *
 * Flow:
 *   1. Connect via `FakeMwaClient("success")`.
 *   2. First disconnect with request_id "dc-ac3-first" — assert emission.
 *   3. Second disconnect with request_id "dc-ac3-second" — assert emission.
 *   4. Assert:
 *      (a) Exactly 2 `postDisconnectCompleted` captures with matching IDs.
 *      (b) 0 `mwa_error` and 0 `mwa_timeout` across both calls.
 *      (c) `MwaSessionState.authToken == null` throughout post-first-call.
 *      (d) Different request_ids → distinct payload `request_id` fields
 *          (proves no collapsing per DD-2-3-4 C5 resolution).
 */
@RunWith(AndroidJUnit4::class)
class AC3DoubleDisconnectTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File

    private val identityJson = JSONObject().apply {
        put("name", "Test Game (2-3 AC-3)")
        put("icon_uri", "https://example.com/icon.png")
        put("identity_uri", "https://example.com/godot-mwa-disconnect-ac3")
    }.toString()

    @Before
    fun setup() {
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        val testCtx = InstrumentationRegistry.getInstrumentation().context
        fixturesDir = FakeWalletFixtures.extractToCacheDir(testCtx)
        SecureTokenStore(ctx).deleteAll()
        GDExtensionAndroidPlugin.sessionState.clear()

        nativeBridge = mockk(relaxed = true)
        plugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
        )
    }

    @After
    fun teardown() {
        runCatching { SecureTokenStore(ctx).deleteAll() }
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @Test
    fun ac3_doubleDisconnectEmitsCompletedTwiceWithoutError() {
        // Stage 1: connect.
        plugin.mwaAuthorize(
            requestId = "req-ac3-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-ac3-connect", any()) }
            }.isSuccess
        }

        // Snapshot the store's cardinality pre-disconnect so we can assert the
        // invariant survives BOTH calls (M-T5-4).
        val store = SecureTokenStore(ctx)
        val keysBefore = store.listAllKeys()
        assertEquals("Exactly 1 record cached post-connect", 1, keysBefore.size)

        // Stage 2: first disconnect. Fresh request_id per DD-2-3-4.
        val firstPayload = slot<String>()
        plugin.mwaDisconnect("dc-ac3-first")
        PluginTestHarness.awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postDisconnectCompleted("dc-ac3-first", capture(firstPayload)) }
            }.isSuccess
        }
        // State is wiped after first call; subsequent calls must see already-clear state.
        assertNull(
            "authToken wiped after first disconnect",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )

        // Stage 3: second disconnect with DIFFERENT request_id — must emit again.
        //  The wipe itself is a no-op on already-clear state, but the emission
        //  is driven by the (fresh) tryTerminate CAS per DD-2-3-2.
        val secondPayload = slot<String>()
        plugin.mwaDisconnect("dc-ac3-second")
        PluginTestHarness.awaitCondition(500L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postDisconnectCompleted("dc-ac3-second", capture(secondPayload)) }
            }.isSuccess
        }

        // (a) Exactly 2 total emissions — one per call.
        verify(exactly = 2) { nativeBridge.postDisconnectCompleted(any(), any()) }

        // (b) Each payload is well-formed and carries the ORIGINATING request_id
        //     (M-T5-1 — direct evidence against collapsing per DD-2-3-4 C5).
        val firstJson = JSONObject(firstPayload.captured)
        assertEquals("dc-ac3-first", firstJson.getString("request_id"))
        assertEquals("disconnect", firstJson.getString("source_method"))
        val secondJson = JSONObject(secondPayload.captured)
        assertEquals("dc-ac3-second", secondJson.getString("request_id"))
        assertEquals("disconnect", secondJson.getString("source_method"))
        assertNotEquals(
            "Distinct request_ids in the two payloads — no collapsing",
            firstJson.getString("request_id"),
            secondJson.getString("request_id"),
        )

        // (c) Terminal-signal invariant — neither call triggered an error/timeout.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }

        // (d) State stays wiped across both calls.
        assertNull(
            "authToken still wiped after second disconnect",
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
        )
        assertEquals(
            "publicKeyBase58 still wiped after second disconnect",
            "",
            GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(),
        )

        // (e) Retention survives BOTH disconnects (M-T5-4). AC-1 covers retention
        //     after a single disconnect; this closes the corner case where a
        //     retention regression might only trigger on the N-th call.
        val keysAfter = store.listAllKeys()
        assertFalse("Cache record retained after double disconnect", keysAfter.isEmpty())
        assertEquals(
            "Store cardinality unchanged after double disconnect",
            keysBefore.size,
            keysAfter.size,
        )
    }
}
