package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import org.junit.After
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import java.io.File

/**
 * Story 2-1 T9 — AC-1: plaintext auth_token MUST NOT appear in the on-disk
 * shared-prefs XML file after [SecureTokenStore.putToken].
 *
 * On emulator this runs against a real `EncryptedSharedPreferences` backed
 * by a Keystore MasterKey — the only tier that meaningfully validates the
 * at-rest confidentiality invariant. The unit test tier (T8) mocks the
 * SharedPreferences interface and cannot detect plaintext-leak regressions
 * in the real Tink-backed encryption path.
 *
 * The test seeds a recognizable UTF-8 auth-token byte string, flushes via
 * `apply()`, reads the `.xml` file raw, and asserts the seeded bytes do
 * NOT appear anywhere. The SharedPreferences keys are AES256_SIV-encrypted
 * (PrefKeyEncryptionScheme) and the values are AES256_GCM-encrypted
 * (PrefValueEncryptionScheme), so the seeded plaintext must survive as
 * zero-byte overlap on disk.
 */
@RunWith(AndroidJUnit4::class)
class AC1PlaintextScanTest {

    private lateinit var ctx: Context
    private lateinit var store: SecureTokenStore

    @Before
    fun setup() {
        // Use the target-app context so SecureTokenStore writes into the app's
        // data dir — the same path the production plugin would use. The
        // instrumentation context (test APK) would produce a different prefs
        // file out of line with the production AC-1 guarantee.
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        store = SecureTokenStore(ctx)
        // Start from a clean slate so the XML file we inspect below was
        // written by THIS test's putToken call (and nothing else).
        store.deleteAll()
    }

    @After
    fun teardown() {
        runCatching { store.deleteAll() }
    }

    @Test
    fun ac1_plaintextTokenDoesNotAppearInSharedPrefsXml() {
        // Distinctive marker bytes — not base58/base64/hex so a real token
        // encoder won't produce this pattern by coincidence.
        val plaintextToken = "PLAINTEXT-TOKEN-TRIPWIRE-b827eab3-beef-9afc".toByteArray(Charsets.UTF_8)
        val key = CacheKey(
            cluster = "devnet",
            chainId = "solana:devnet",
            walletPackage = "app.phantom",
            identityUri = "https://example.com/godot-mwa-ac1",
        )
        val record = CacheRecord(
            schema = CacheRecord.SCHEMA_V1,
            authToken = SecretString(plaintextToken.copyOf()),
            walletPackage = "app.phantom",
            walletLabel = "Phantom",
            walletIconUri = "https://phantom.com/icon.png",
            publicKey = "11111111111111111111111111111111",
            cluster = "devnet",
            chainId = "solana:devnet",
            identityUri = "https://example.com/godot-mwa-ac1",
            issuedAtMs = 1_700_000_000_000L,
            lastUsedAtMs = 1_700_000_000_000L,
        )

        store.putToken(key, record)

        // Prefs file path per `EncryptedSharedPreferences.create(PREFS_FILE_NAME, ...)`.
        // The `.xml` extension is implicit in SharedPreferences' on-disk format.
        val prefsXml = File(ctx.filesDir.parent, "shared_prefs/${SecureTokenStore.PREFS_FILE_NAME}.xml")
        assertTrue(
            "Expected shared_prefs .xml to exist after putToken — path=$prefsXml",
            prefsXml.exists(),
        )
        val onDisk = prefsXml.readBytes()
        assertFalse(
            "AC-1 VIOLATION: plaintext auth_token bytes found in ${prefsXml.absolutePath} " +
                "(${onDisk.size} bytes scanned). EncryptedSharedPreferences MUST AES256_GCM-encrypt " +
                "values; a match indicates the MasterKey is missing, misconfigured, or the store " +
                "regressed to plaintext write.",
            indexOfSubArray(onDisk, plaintextToken) >= 0,
        )
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
}
