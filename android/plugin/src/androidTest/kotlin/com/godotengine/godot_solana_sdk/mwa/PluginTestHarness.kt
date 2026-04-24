package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.plugin.InflightMap
import com.godotengine.godot_solana_sdk.mwa.plugin.MwaDiagnostics
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import io.mockk.mockk
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import org.godotengine.godot.Godot
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 2-1 T9 — shared androidTest harness for plugin construction.
 *
 * Each instrumented test spawns a fresh [GDExtensionAndroidPlugin] via the
 * `@VisibleForTesting` ctor with injected collaborators. Key decisions:
 *   - `MwaClient` is always a [FakeMwaClient] configured with a fixture
 *     scenario (Route B per Story 2-1 T9 scope question — the real Fake
 *     Wallet APK install step is replaced by the in-process fake; D-T9-1).
 *   - `storeProvider` receives a [SecureTokenStore] built on the real
 *     instrumentation target context, so `EncryptedSharedPreferences` +
 *     MasterKey (Keystore-backed) round-trip for real. This is where
 *     androidTest earns its seat vs. the JVM unit-test tier — AC-1 plaintext
 *     scan + AC-8 logcat scan can only be meaningful on a real Keystore.
 *   - `nativeBridge` is a relaxed MockK so tests can capture
 *     `postConnectCompleted` / `postMwaError` / `postMwaTimeout` without a
 *     real JNI bridge. (Real JNI roundtrip coverage is CR-35 headless-Godot
 *     tier — out of scope for this story.)
 *   - `mainDispatcher = Dispatchers.Unconfined` keeps `withContext(Main)`
 *     running inline in the calling thread — avoids needing a live
 *     Handler main-looper wired into the test. Matches T4 unit-test
 *     pattern.
 */
internal object PluginTestHarness {

    fun freshPlugin(
        ctx: Context,
        fixturesDir: File,
        scenario: String,
        nativeBridge: NativeBridge,
        inflightMap: InflightMap = InflightMap(),
        diagnostics: MwaDiagnostics = MwaDiagnostics(),
        clock: () -> Long = { 1_700_000_000_000L },
        storeOverride: SecureTokenStore? = null,
    ): GDExtensionAndroidPlugin {
        val godot = mockk<Godot>(relaxed = true).also {
            // `requireContext()` drills through `godot.getActivity()` — the returned
            // Activity is only used as a Context, so a relaxed mock with the real
            // target context injected via the storeProvider closure suffices.
            io.mockk.every { it.getActivity() } returns mockk(relaxed = true)
        }
        val senderMock = mockk<ActivityResultSender>(relaxed = true)
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.Default)
        val clientFactory: () -> MwaClient = { FakeMwaClient(fixturesDir).withScenario(scenario) }
        val resolvedStore = storeOverride ?: SecureTokenStore(ctx)
        return GDExtensionAndroidPlugin(
            godot = godot,
            mwaClientFactory = clientFactory,
            // Ignore the passed-in context — route to the instrumentation target
            // context that already hosts the real Keystore-backed store.
            storeProvider = { _ -> resolvedStore },
            nativeBridge = nativeBridge,
            senderProvider = { senderMock },
            scope = scope,
            clock = clock,
            inflightMap = inflightMap,
            diagnostics = diagnostics,
            mainDispatcher = Dispatchers.Unconfined,
        )
    }

    /** Polls every 20ms up to `timeoutMs` until `check` returns true. Fails the test otherwise. */
    fun awaitCondition(timeoutMs: Long, check: () -> Boolean) {
        val deadline = System.currentTimeMillis() + timeoutMs
        while (System.currentTimeMillis() < deadline) {
            if (check()) return
            Thread.sleep(20)
        }
        error("condition not met within ${timeoutMs}ms")
    }
}
