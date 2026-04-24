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
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 2-1 T9 — AC-7: `get_auth_token_fingerprint()` is idempotent across
 * multiple calls post-connect. On device the per-install salt lives inside
 * the real EncryptedSharedPreferences file, so a second read genuinely
 * returns the persisted bytes (not a freshly generated salt). This is the
 * device-specific piece the unit test can't exercise.
 */
@RunWith(AndroidJUnit4::class)
class AC7FingerprintIdempotenceTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File

    private val identityJson = JSONObject().apply {
        put("name", "Test Game (AC-7)")
        put("icon_uri", "https://example.com/icon.png")
        put("identity_uri", "https://example.com/godot-mwa-ac7")
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
    fun ac7_fingerprintIsIdempotentAcrossReads() {
        plugin.mwaAuthorize(
            requestId = "req-ac7",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )

        val jsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-ac7", capture(jsonSlot)) }
            }.isSuccess
        }
        val firstFromPayload = JSONObject(jsonSlot.captured).getString("auth_token_fingerprint")
        assertEquals(8, firstFromPayload.length)
        assertTrue(firstFromPayload.all { it in '0'..'9' || it in 'a'..'f' })

        // Read twice from the session snapshot — the values MUST be identical
        // across reads. The per-install salt is persisted (D-9) so re-reading
        // the fingerprint doesn't regenerate the salt, and the authToken
        // bytes haven't changed.
        val firstFromState = GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint()
        val secondFromState = GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint()
        assertEquals(firstFromPayload, firstFromState)
        assertEquals(firstFromState, secondFromState)
    }
}
