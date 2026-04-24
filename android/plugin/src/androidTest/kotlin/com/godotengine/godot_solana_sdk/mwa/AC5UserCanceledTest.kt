package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
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
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 2-1 T9 — AC-5: user-canceled on-device.
 *
 * Asserts the FakeMwaClient `user_canceled` scenario produces a terminal
 * `mwa_error` with `code=USER_CANCELED`, `recoverable=true`, and
 * `retry_hint=retry_now`. Same contract as the T8 unit-test coverage, re-run
 * on real Android coroutine dispatchers to catch any scheduling-edge
 * regressions that only surface with live `Dispatchers.Default` +
 * `InflightMap` under the Android VM.
 */
@RunWith(AndroidJUnit4::class)
class AC5UserCanceledTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File

    private val identityJson = JSONObject().apply {
        put("name", "Test Game (AC-5)")
        put("icon_uri", "https://example.com/icon.png")
        put("identity_uri", "https://example.com/godot-mwa-ac5")
    }.toString()

    @Before
    fun setup() {
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        val testCtx = InstrumentationRegistry.getInstrumentation().context
        fixturesDir = FakeWalletFixtures.extractToCacheDir(testCtx)
        GDExtensionAndroidPlugin.sessionState.clear()

        nativeBridge = mockk(relaxed = true)
        plugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "user_canceled",
            nativeBridge = nativeBridge,
        )
    }

    @After
    fun teardown() {
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @Test
    fun ac5_userCanceled_emitsMwaErrorUserCanceledRecoverableRetryNow() {
        plugin.mwaAuthorize(
            requestId = "req-ac5",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 2_000L,
        )

        val jsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(2_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(jsonSlot)) }
            }.isSuccess
        }
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-ac5", payload.getString("request_id"))
        assertEquals("USER_CANCELED", payload.getString("code"))
        assertTrue(payload.getBoolean("recoverable"))
        assertEquals("retry_now", payload.getString("retry_hint"))
        assertEquals("kotlin", payload.getString("layer"))
        assertEquals("connect", payload.getString("source_method"))
        // Terminal-signal invariant.
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
