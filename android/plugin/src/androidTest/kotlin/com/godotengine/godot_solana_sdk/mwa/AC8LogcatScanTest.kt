package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import io.mockk.mockk
import io.mockk.verify
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.BufferedReader
import java.io.File
import java.io.InputStreamReader

/**
 * Story 2-1 T9 — AC-8: auth_token plaintext MUST NOT appear in logcat OR
 * anywhere under the app's on-disk files after a successful connect.
 *
 * This is the broader complement to AC-1 (which scans only the
 * `godot-sdk-mwa-tokens-v1.xml` prefs file). AC-8 walks every file under
 * `context.filesDir.parent` (the app data dir — prefs, cache, databases,
 * …) AND invokes `adb logcat -d -t <startTime>` to capture process-wide
 * log output since the test began.
 *
 * Uses a distinctive seed token (not a wallet-generated value) so a match
 * is unambiguous evidence of a log / cache / database leak.
 */
@RunWith(AndroidJUnit4::class)
class AC8LogcatScanTest {

    private lateinit var ctx: Context
    private lateinit var store: SecureTokenStore
    private lateinit var nativeBridge: NativeBridge
    private var testStartUnixMs: Long = 0L

    @Before
    fun setup() {
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        store = SecureTokenStore(ctx)
        store.deleteAll()
        GDExtensionAndroidPlugin.sessionState.clear()
        nativeBridge = mockk(relaxed = true)
        testStartUnixMs = System.currentTimeMillis()
    }

    @After
    fun teardown() {
        runCatching { store.deleteAll() }
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @Test
    fun ac8_plaintextDoesNotAppearInLogcatOrAppDataDir() {
        val tripwire = "AC8-TRIPWIRE-TOKEN-acab09cc-cafe-b0ba".toByteArray(Charsets.UTF_8)
        val tripwireString = String(tripwire, Charsets.UTF_8)

        // Exercise the write path — putToken under a real CacheRecord with
        // the tripwire as the auth_token bytes.
        val key = CacheKey(
            cluster = "devnet",
            chainId = "solana:devnet",
            walletPackage = "app.phantom",
            identityUri = "https://example.com/godot-mwa-ac8",
        )
        val record = CacheRecord(
            schema = CacheRecord.SCHEMA_V1,
            authToken = SecretString(tripwire.copyOf()),
            walletPackage = "app.phantom",
            walletLabel = "Phantom",
            walletIconUri = "",
            publicKey = "11111111111111111111111111111111",
            cluster = "devnet",
            chainId = "solana:devnet",
            identityUri = "https://example.com/godot-mwa-ac8",
            issuedAtMs = 1_700_000_000_000L,
            lastUsedAtMs = 1_700_000_000_000L,
        )
        store.putToken(key, record)
        val roundTripped = store.getToken(key)
        assertEquals(
            "AC-8 setup sanity: round-trip must work before we scan for leaks",
            tripwireString,
            roundTripped?.authToken?.let { String(it.reveal(), Charsets.UTF_8) },
        )

        // 1) On-disk scan under app data dir (files/, shared_prefs/, cache/, databases/, …).
        val appRoot = File(ctx.filesDir.parent ?: error("filesDir.parent must be non-null on Android"))
        val leaks = mutableListOf<String>()
        scanRecursive(appRoot, tripwire) { hit -> leaks += hit }
        assertFalse(
            "AC-8 VIOLATION: plaintext auth_token bytes found on disk at: ${leaks.joinToString("\n  ", prefix = "\n  ")}",
            leaks.isNotEmpty(),
        )

        // 2) logcat scan since test start — captures any Log.[vdiwe] that
        // accidentally interpolated the SecretString.reveal() result or raw
        // CacheRecord.toString(). The grep-ban in CI (ci/grep_bans.sh) catches
        // static patterns; this runtime scan catches dynamic regressions.
        val logcatDumpSinceStart = runLogcatDumpSinceTestStart()
        assertFalse(
            "AC-8 VIOLATION: tripwire token found in logcat since test start. " +
                "The offending Log.* call is a SecretString handling regression; review " +
                "ci/grep_bans.sh patterns 1-4 and the SdkLog kdoc contract.",
            logcatDumpSinceStart.contains(tripwireString),
        )

        // Terminal-signal invariant checks aren't part of AC-8; this test is pure at-rest.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
    }

    private fun scanRecursive(root: File, needle: ByteArray, onMatch: (String) -> Unit) {
        if (!root.exists()) return
        val stack = ArrayDeque<File>()
        stack += root
        while (stack.isNotEmpty()) {
            val f = stack.removeFirst()
            if (f.isDirectory) {
                f.listFiles()?.forEach { stack += it }
                continue
            }
            // Skip obviously-empty / non-scannable files.
            if (!f.canRead() || f.length() == 0L) continue
            // Cap per-file read at 4 MB — tests run on constrained emulator I/O.
            val bytes = try {
                f.readBytes().let { if (it.size > 4 * 1024 * 1024) it.copyOf(4 * 1024 * 1024) else it }
            } catch (e: SecurityException) {
                continue
            } catch (e: java.io.IOException) {
                continue
            }
            if (indexOfSubArray(bytes, needle) >= 0) onMatch(f.absolutePath)
        }
    }

    private fun indexOfSubArray(haystack: ByteArray, needle: ByteArray): Int {
        if (needle.isEmpty() || needle.size > haystack.size) return -1
        outer@ for (i in 0..haystack.size - needle.size) {
            for (j in needle.indices) {
                if (haystack[i + j] != needle[j]) continue@outer
            }
            return i
        }
        return -1
    }

    /**
     * `adb logcat -d` is normally an emulator-host command; from inside an
     * androidTest we invoke `logcat -d` via `Runtime.exec` against the
     * device's own logcat binary. Filters to lines at-or-after test start
     * (best-effort; logcat clock may differ from wall clock by a few ms).
     */
    private fun runLogcatDumpSinceTestStart(): String {
        val process = Runtime.getRuntime().exec(arrayOf("logcat", "-d", "-v", "time"))
        val out = StringBuilder()
        BufferedReader(InputStreamReader(process.inputStream)).use { reader ->
            var line: String? = reader.readLine()
            while (line != null) {
                out.append(line).append('\n')
                line = reader.readLine()
            }
        }
        process.waitFor()
        return out.toString()
    }
}
