package com.godotengine.godot_solana_sdk.mwa.store

import android.content.Context
import android.content.SharedPreferences
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import io.mockk.Runs
import io.mockk.every
import io.mockk.just
import io.mockk.mockk
import io.mockk.mockkConstructor
import io.mockk.mockkStatic
import io.mockk.slot
import io.mockk.unmockkAll
import io.mockk.verify
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertArrayEquals
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertInstanceOf
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Assertions.assertNull
import org.junit.jupiter.api.Assertions.assertThrows
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import java.io.IOException
import java.security.GeneralSecurityException

/**
 * Story 2-1 T1 — corrupt-recovery + happy-path + lazy-init contract for
 * [SecureTokenStore].
 *
 * Uses JUnit 5 + MockK static/constructor mocking (no Robolectric) — matches
 * Story 1-2 `SdkLogTest`'s `mockkStatic(Log::class)` pattern for faking Android
 * framework primitives on plain JVM. The `MasterKey.Builder` constructor and
 * fluent chain are intercepted so `masterKey.build()` returns a relaxed mock
 * without touching the real Android Keystore (which doesn't exist in unit-test
 * JVM). D-T1-2 is rejected under this story for that reason.
 */
class SecureTokenStoreTest {

    private lateinit var context: Context
    private lateinit var fakeMasterKey: MasterKey

    @BeforeEach
    fun setup() {
        context = mockk(relaxed = true)
        fakeMasterKey = mockk(relaxed = true)

        mockkConstructor(MasterKey.Builder::class)
        // `MasterKey.Builder(ctx, alias).setKeyScheme(AES256_GCM).build()` →
        //   1. constructor: intercepted by MockK
        //   2. .setKeyScheme(...): returns `fluent` (separate mock)
        //   3. .build() on `fluent`: returns `fakeMasterKey`
        val fluent = mockk<MasterKey.Builder> {
            every { build() } returns fakeMasterKey
        }
        every { anyConstructed<MasterKey.Builder>().setKeyScheme(any()) } returns fluent

        mockkStatic(EncryptedSharedPreferences::class)
    }

    @AfterEach
    fun teardown() {
        unmockkAll()
    }

    @Test
    fun `corrupt prefs via GeneralSecurityException throws StorageCorruptException and deletes file`() {
        every {
            EncryptedSharedPreferences.create(any<Context>(), any(), any(), any(), any())
        } throws GeneralSecurityException("simulated Tink AEAD failure")

        val store = SecureTokenStore(context)

        val thrown = assertThrows(StorageCorruptException::class.java) { store.deleteAll() }
        assertInstanceOf(GeneralSecurityException::class.java, thrown.cause)
        verify(exactly = 1) { context.deleteSharedPreferences(SecureTokenStore.PREFS_FILE_NAME) }
    }

    @Test
    fun `corrupt prefs via IOException throws StorageCorruptException and deletes file`() {
        // IOException is `EncryptedSharedPreferences.create`'s public-throws parent type.
        // Tink's shaded `InvalidProtocolBufferException` extends `IOException`, so a single
        // catch covers both path (D-T1-4).
        every {
            EncryptedSharedPreferences.create(any<Context>(), any(), any(), any(), any())
        } throws IOException("simulated Tink proto deserialization failure (IOException parent)")

        val store = SecureTokenStore(context)

        val thrown = assertThrows(StorageCorruptException::class.java) { store.deleteAll() }
        assertInstanceOf(IOException::class.java, thrown.cause)
        verify(exactly = 1) { context.deleteSharedPreferences(SecureTokenStore.PREFS_FILE_NAME) }
    }

    @Test
    fun `happy path - lazy prefs init succeeds and deleteAll clears via editor`() {
        val editor = mockk<SharedPreferences.Editor>(relaxed = true)
        every { editor.clear() } returns editor
        every { editor.apply() } just Runs

        val prefs = mockk<SharedPreferences>(relaxed = true)
        every { prefs.edit() } returns editor

        every {
            EncryptedSharedPreferences.create(any<Context>(), any(), any(), any(), any())
        } returns prefs

        val store = SecureTokenStore(context)
        store.deleteAll()

        verify(exactly = 1) { editor.clear() }
        verify(exactly = 1) { editor.apply() }
        verify(exactly = 0) { context.deleteSharedPreferences(any()) }
    }

    @Test
    fun `construction does not trigger lazy prefs init (lazy fires on first access)`() {
        every {
            EncryptedSharedPreferences.create(any<Context>(), any(), any(), any(), any())
        } throws AssertionError("prefs must not be accessed during construction")

        // Must NOT throw — constructor touches nothing lazy.
        SecureTokenStore(context)
    }

    @Test
    fun `getOrCreatePerInstallSalt generates 32 bytes and persists as 64-char hex on first call`() {
        val editor = mockk<SharedPreferences.Editor>(relaxed = true)
        every { editor.putString(any(), any()) } returns editor
        every { editor.apply() } just Runs

        val prefs = mockk<SharedPreferences>(relaxed = true)
        every { prefs.getString(SecureTokenStore.FINGERPRINT_SALT_KEY, null) } returns null
        every { prefs.edit() } returns editor

        every {
            EncryptedSharedPreferences.create(any<Context>(), any(), any(), any(), any())
        } returns prefs

        val store = SecureTokenStore(context)
        val salt = store.getOrCreatePerInstallSalt()

        assertEquals(32, salt.size)
        verify(exactly = 1) {
            editor.putString(
                eq(SecureTokenStore.FINGERPRINT_SALT_KEY),
                match<String> { it.length == 64 && it.all { ch -> ch in '0'..'9' || ch in 'a'..'f' } },
            )
        }
        verify(exactly = 1) { editor.apply() }
    }

    @Test
    fun `getOrCreatePerInstallSalt returns persisted salt without rewriting on second access`() {
        val hexSalt = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff"
        val prefs = mockk<SharedPreferences>(relaxed = true)
        every { prefs.getString(SecureTokenStore.FINGERPRINT_SALT_KEY, null) } returns hexSalt

        every {
            EncryptedSharedPreferences.create(any<Context>(), any(), any(), any(), any())
        } returns prefs

        val store = SecureTokenStore(context)
        val salt = store.getOrCreatePerInstallSalt()

        assertEquals(32, salt.size)
        assertEquals(0x00.toByte(), salt[0])
        assertEquals(0xff.toByte(), salt[15])
        assertEquals(0x00.toByte(), salt[16])
        assertEquals(0xff.toByte(), salt[31])
        verify(exactly = 0) { prefs.edit() } // no write when salt already exists
    }

    @Test
    fun `listAllKeys filters out fingerprint-salt entry (non-CacheRecord keys skipped by prefix)`() {
        val prefs = mockk<SharedPreferences>(relaxed = true)
        // Only the salt entry present. If NOT filtered by `"mwa::v1::"` prefix,
        // CacheRecord.fromJson would throw JSONException on the 64-char hex string.
        every { prefs.all } returns mapOf(
            SecureTokenStore.FINGERPRINT_SALT_KEY to
                "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff",
        )

        every {
            EncryptedSharedPreferences.create(any<Context>(), any(), any(), any(), any())
        } returns prefs

        val store = SecureTokenStore(context)
        assertEquals(emptyList<CacheKey>(), store.listAllKeys())
    }

    // ---------------- Story 2-1 T8 — AC-1 round-trip coverage ----------------

    @Test
    fun `AC-1 round-trip - putToken then getToken recovers identical CacheRecord fields`() {
        // Simulated prefs file. Mocks the EncryptedSharedPreferences contract at
        // the put/get-String layer so the test exercises the SecureTokenStore ↔
        // CacheRecord serialization path without needing a real Android Keystore
        // (D-T1-2 — Robolectric rejected for this tier). On-disk AES256_GCM +
        // AES256_SIV confidentiality is validated separately by T9's androidTest
        // AC1_PlaintextScanTest (emulator-only, real Keystore).
        val storedBlobs = mutableMapOf<String, String>()
        val editor = mockk<SharedPreferences.Editor>(relaxed = true)
        val capturedKey = slot<String>()
        val capturedValue = slot<String>()
        every { editor.putString(capture(capturedKey), capture(capturedValue)) } answers {
            storedBlobs[capturedKey.captured] = capturedValue.captured
            editor
        }
        every { editor.apply() } just Runs

        val prefs = mockk<SharedPreferences>(relaxed = true)
        every { prefs.edit() } returns editor
        every { prefs.getString(any(), null) } answers {
            val requestedKey = firstArg<String>()
            storedBlobs[requestedKey]
        }

        every {
            EncryptedSharedPreferences.create(any<Context>(), any(), any(), any(), any())
        } returns prefs

        val store = SecureTokenStore(context)
        val key = CacheKey(
            cluster = "devnet",
            chainId = "solana:devnet",
            walletPackage = "app.phantom",
            identityUri = "https://example.com/godot-mwa-sample",
        )
        val original = CacheRecord(
            schema = CacheRecord.SCHEMA_V1,
            authToken = SecretString("opaque-base64-token".toByteArray(Charsets.UTF_8)),
            walletPackage = "app.phantom",
            walletLabel = "Phantom",
            walletIconUri = "https://phantom.com/icon.png",
            publicKey = "11111111111111111111111111111111",
            cluster = "devnet",
            chainId = "solana:devnet",
            identityUri = "https://example.com/godot-mwa-sample",
            issuedAtMs = 1_700_000_000_000L,
            lastUsedAtMs = 1_700_000_000_000L,
        )

        store.putToken(key, original)
        val recovered = store.getToken(key)

        assertNotNull(recovered, "getToken must recover a record after putToken with the same key")
        recovered!!
        assertEquals(CacheRecord.SCHEMA_V1, recovered.schema)
        assertArrayEquals(
            "opaque-base64-token".toByteArray(Charsets.UTF_8),
            recovered.authToken.reveal(),
            "auth_token bytes must round-trip unchanged through SecretString JSON boundary",
        )
        assertEquals("app.phantom", recovered.walletPackage)
        assertEquals("Phantom", recovered.walletLabel)
        assertEquals("https://phantom.com/icon.png", recovered.walletIconUri)
        assertEquals("11111111111111111111111111111111", recovered.publicKey)
        assertEquals("devnet", recovered.cluster)
        assertEquals("solana:devnet", recovered.chainId)
        assertEquals("https://example.com/godot-mwa-sample", recovered.identityUri)
        assertEquals(1_700_000_000_000L, recovered.issuedAtMs)
        assertEquals(1_700_000_000_000L, recovered.lastUsedAtMs)
        // Key on-disk is hashed per DD-17 / DD-27 — assert the prefix so callers
        // can't accidentally put raw identity_uri on disk.
        assertEquals(true, capturedKey.captured.startsWith(CacheKey.KEY_PREFIX))
    }

    @Test
    fun `AC-1 round-trip - getToken returns null for an absent key`() {
        // Absence contract: missing keys map to null (not throw). The plugin
        // layer uses this to decide between "new session" vs "reconnect existing"
        // on authorize success.
        val prefs = mockk<SharedPreferences>(relaxed = true)
        every { prefs.getString(any(), null) } returns null

        every {
            EncryptedSharedPreferences.create(any<Context>(), any(), any(), any(), any())
        } returns prefs

        val store = SecureTokenStore(context)
        val absentKey = CacheKey("devnet", "solana:devnet", "app.not-installed", "https://example.com/x")

        assertNull(store.getToken(absentKey))
    }
}
