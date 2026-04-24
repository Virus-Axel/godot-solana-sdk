package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import java.io.File

/**
 * Story 2-1 T9 — androidTest fixture extractor.
 *
 * `FakeMwaClient` (used by the injected `MwaClient` factory on device — D-T9-1
 * Rule 2 substitution for the Fake-Wallet-APK route) consumes fixtures from
 * a `File` directory. In the androidTest runtime the fixtures ship as APK
 * assets (under `src/androidTest/assets/mwa-fixtures/`) because the host
 * `testdata/` directory is not reachable from a device process. This helper
 * copies the assets into the instrumentation context's cache dir once per
 * test class and returns the extracted `File` root.
 *
 * Idempotent: a second call with the same context returns the same dir
 * without re-copying (the canonical cache-dir pattern — the files already
 * exist, the copy is a no-op).
 */
object FakeWalletFixtures {

    private const val ASSET_DIR = "mwa-fixtures"
    private const val CACHE_SUBDIR = "mwa-fixtures-extracted"

    /**
     * Extract `androidTest/assets/mwa-fixtures/` JSON files into
     * `context.cacheDir/mwa-fixtures-extracted/` and return the extracted
     * root. `context` MUST be the instrumentation context (not the target
     * app context) because the assets live in the test APK, not the app APK.
     */
    fun extractToCacheDir(context: Context): File {
        val outDir = File(context.cacheDir, CACHE_SUBDIR).apply { mkdirs() }
        val assetManager = context.assets
        val names = assetManager.list(ASSET_DIR) ?: emptyArray()
        for (name in names) {
            val dest = File(outDir, name)
            if (dest.exists()) continue
            assetManager.open("$ASSET_DIR/$name").use { input ->
                dest.outputStream().use { output -> input.copyTo(output) }
            }
        }
        return outDir
    }
}
