package com.godotengine.godot_solana_sdk.mwa.store

import android.content.Context
import android.content.SharedPreferences
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey
import io.mockk.Runs
import io.mockk.every
import io.mockk.just
import io.mockk.mockk
import io.mockk.mockkConstructor
import io.mockk.mockkStatic
import io.mockk.unmockkAll
import io.mockk.verify
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertInstanceOf
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
}
