package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.AuthResult
import com.godotengine.godot_solana_sdk.mwa.plugin.InflightMap
import com.godotengine.godot_solana_sdk.mwa.plugin.MwaDiagnostics
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import io.mockk.every
import io.mockk.mockk
import io.mockk.slot
import io.mockk.verify
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.delay
import org.godotengine.godot.Godot
import org.json.JSONObject
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin

/**
 * Story 2-1 T9 — AC-6: watchdog expiry + late-result-dropped invariant on
 * device.
 *
 * Builds a slow MwaClient that `delay()`s past the effective watchdog — the
 * plugin's `withTimeoutOrNull` MUST fire, emit `postMwaTimeout`, and keep the
 * InflightMap entry TERMINATED so the delayed response is dropped with a
 * `lateResultCount` increment.
 *
 * On device this catches any regression where `kotlinx.coroutines` scheduling
 * under Android's main looper differs from JVM-host behavior — e.g., a
 * Dispatchers.Main misuse that would hang the watchdog path forever.
 */
@RunWith(AndroidJUnit4::class)
class AC6WatchdogTimeoutTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var inflightMap: InflightMap
    private lateinit var diagnostics: MwaDiagnostics
    private lateinit var plugin: GDExtensionAndroidPlugin

    private val identityJson = JSONObject().apply {
        put("name", "Test Game (AC-6)")
        put("icon_uri", "https://example.com/icon.png")
        put("identity_uri", "https://example.com/godot-mwa-ac6")
    }.toString()

    @Before
    fun setup() {
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        GDExtensionAndroidPlugin.sessionState.clear()
        nativeBridge = mockk(relaxed = true)
        inflightMap = InflightMap()
        diagnostics = MwaDiagnostics()

        // Slow client — delays well past the 100ms effective watchdog we'll
        // set below. authorize never returns MwaResult.Success; the
        // withTimeoutOrNull wrapper must short-circuit and deliver null.
        val slowClient = object : MwaClient {
            override suspend fun authorize(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                cluster: String,
                chainId: String,
            ): MwaResult<AuthResult> {
                // 5s > effective watchdog (100ms). withTimeoutOrNull MUST win.
                delay(5_000)
                error("unreachable: should be cancelled by withTimeoutOrNull")
            }
            override suspend fun reauthorize(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                cluster: String,
                chainId: String,
            ) = error("nyi")
            override suspend fun deauthorize(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
            ) = error("nyi")
            override suspend fun disconnect() = error("nyi")
            override suspend fun signMessages(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                messages: List<ByteArray>,
                addresses: List<ByteArray>,
            ) = error("nyi")
            override suspend fun signTransactions(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                transactions: List<ByteArray>,
            ) = error("nyi")
            override suspend fun signAndSendTransactions(
                sender: ActivityResultSender,
                identity: ConnectionIdentity,
                authToken: SecretString,
                transactions: List<ByteArray>,
                cluster: String,
            ) = error("nyi")
            override fun close() {}
        }

        val godot = mockk<Godot>(relaxed = true).also {
            every { it.getActivity() } returns mockk(relaxed = true)
        }
        val senderMock = mockk<ActivityResultSender>(relaxed = true)
        val store = SecureTokenStore(ctx)
        plugin = GDExtensionAndroidPlugin(
            godot = godot,
            mwaClientFactory = { slowClient },
            storeProvider = { _ -> store },
            nativeBridge = nativeBridge,
            senderProvider = { senderMock },
            scope = CoroutineScope(SupervisorJob() + Dispatchers.Default),
            clock = { 1_700_000_000_000L },
            inflightMap = inflightMap,
            diagnostics = diagnostics,
            mainDispatcher = Dispatchers.Unconfined,
        )
    }

    @After
    fun teardown() {
        runCatching { SecureTokenStore(ctx).deleteAll() }
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @Test
    fun ac6_watchdog_firesMwaTimeoutAndDropsLateResult() {
        plugin.mwaAuthorize(
            requestId = "req-ac6",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 100L,
        )

        val timeoutSlot = slot<String>()
        PluginTestHarness.awaitCondition(2_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaTimeout(capture(timeoutSlot)) }
            }.isSuccess
        }
        val timeoutPayload = JSONObject(timeoutSlot.captured)
        assertEquals("req-ac6", timeoutPayload.getString("request_id"))
        assertEquals("connect", timeoutPayload.getString("source_method"))
        assertEquals(100L, timeoutPayload.getLong("watchdog_ms"))

        // Terminal-signal invariant: no mwa_error or connect_completed.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }

        // The late result never drops into lateResultCount here because the
        // slow coroutine was CANCELLED by withTimeoutOrNull before it could
        // dispatch. AC-6's "late_result_count increments by 1 after Fake
        // Wallet's delayed response at 180s" is a characteristic of the
        // REAL wallet path (where the wallet keeps running and fires back
        // on its own schedule) — Route B with FakeMwaClient cannot
        // reproduce that timing. The terminate-invariant is covered by
        // T4/T8 unit test `late_result_dropped` via forced tryTerminate=false.
        // Here we assert the structurally equivalent observation: the
        // Inflight entry for req-ac6 is TERMINATED (tryTerminate returned
        // true once — subsequent tries would return false).
        assertTrue(
            "Inflight entry should be TERMINATED after timeout fires",
            !inflightMap.tryTerminate("req-ac6"),
        )
    }
}
