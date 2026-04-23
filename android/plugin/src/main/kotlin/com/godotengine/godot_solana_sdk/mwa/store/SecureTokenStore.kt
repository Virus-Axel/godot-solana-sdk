package com.godotengine.godot_solana_sdk.mwa.store

import android.content.Context
import android.content.SharedPreferences
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey
import org.json.JSONObject
import java.io.IOException
import java.security.GeneralSecurityException
import java.security.SecureRandom

/**
 * Encrypted on-disk token storage for Mobile Wallet Adapter authorize tokens.
 * Follows arch §8.3 verbatim: `MasterKey` (Keystore-backed, `AES256_GCM`) wraps
 * `EncryptedSharedPreferences` (`AES256_SIV` keys, `AES256_GCM` values). Storage
 * is keyed by [CacheKey.toHash] — the `"mwa::v1::" + SHA256(canonical())` form
 * from DD-17 / DD-27.
 *
 * Corrupt-recovery: the lazy `prefs` init catches [GeneralSecurityException]
 * (Tink AEAD failures, MasterKey rotation mismatch) and [IOException] (file
 * system errors + Tink's shaded `InvalidProtocolBufferException`, which extends
 * `IOException`), deletes the prefs file (the MasterKey Keystore entry
 * survives), and rethrows [StorageCorruptException] so the plugin layer can
 * emit `reauth_required{reason=keystore_corrupt}` and prompt the user to
 * reconnect. Arch §8.3 names the shaded `InvalidProtocolBufferException`
 * explicitly; the shaded namespace is not on the AAR's public compile
 * classpath, so we catch its JDK parent `IOException` — substantively
 * equivalent (D-T1-4, Rule 1). Because `prefs` is lazy, the recovery fires on first READ rather
 * than at store construction — constructing the store is always safe.
 *
 * Story 2-1 T1 lands this shell; `putToken` / `getToken` go through
 * [CacheKey.toHash] + [CacheRecord.toJson] / [CacheRecord.fromJson], whose
 * bodies are stubbed until T2 / T3 (D-T1-3). `listAllKeys` is provisioned
 * here for Story 4-2 `forget_all` and carries a `TODO`.
 */
class SecureTokenStore(private val context: Context) {

    private val masterKey: MasterKey by lazy {
        MasterKey.Builder(context, MASTER_KEY_ALIAS)
            .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
            .build()
    }

    private val prefs: SharedPreferences by lazy {
        try {
            EncryptedSharedPreferences.create(
                context,
                PREFS_FILE_NAME,
                masterKey,
                EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
                EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM,
            )
        } catch (e: GeneralSecurityException) {
            wipeCorruptPrefs()
            throw StorageCorruptException(e)
        } catch (e: IOException) {
            wipeCorruptPrefs()
            throw StorageCorruptException(e)
        }
    }

    fun getToken(key: CacheKey): CacheRecord? {
        val json = prefs.getString(key.toHash(), null) ?: return null
        return CacheRecord.fromJson(JSONObject(json))
    }

    fun putToken(key: CacheKey, record: CacheRecord) {
        prefs.edit()
            .putString(key.toHash(), record.toJson().toString())
            .apply()
    }

    fun deleteToken(key: CacheKey) {
        prefs.edit().remove(key.toHash()).apply()
    }

    fun deleteAll() {
        prefs.edit().clear().apply()
    }

    /**
     * Lists every stored [CacheKey] by iterating entries whose KEY starts with
     * [CacheKey.KEY_PREFIX] (`"mwa::v1::"`) and reconstructing each tuple from
     * the record's first-class fields. The on-disk cache key is `sha256`-based
     * (one-way), but [CacheRecord] already carries `cluster`, `chainId`,
     * `walletPackage`, and `identityUri` per arch §3.1 — so the CacheKey tuple
     * is recoverable directly from each stored value. No side index, no hash
     * reversal.
     *
     * The key-prefix filter excludes non-CacheRecord entries that co-live in
     * the same prefs file — currently [FINGERPRINT_SALT_KEY] for the
     * per-install salt (D-9). Any future internal entry MUST use a key that
     * does NOT start with `"mwa::v1::"` to stay out of this iteration.
     *
     * Used by `forget_all` in Story 4-2.
     */
    fun listAllKeys(): List<CacheKey> = prefs.all
        .filterKeys { it.startsWith(CacheKey.KEY_PREFIX) }
        .values
        .filterIsInstance<String>()
        .map { CacheRecord.fromJson(JSONObject(it)) }
        .map { CacheKey(it.cluster, it.chainId, it.walletPackage, it.identityUri) }

    /**
     * Returns the 32-byte per-install salt for [AuthTokenFingerprint]. On
     * first access generates a fresh salt via [SecureRandom] and persists it
     * under [FINGERPRINT_SALT_KEY] inside the same `EncryptedSharedPreferences`
     * file (AES256_GCM-encrypted at rest via the MasterKey). Subsequent calls
     * return the persisted value unchanged.
     *
     * Storage format: lower-hex string (64 chars for 32 bytes). Hex over
     * base64 for API-23 compat — `java.util.Base64` is API-26+ and
     * `android.util.Base64` isn't available in unit-test JVM (D-T3-1).
     *
     * Arch §8.3 / D-9 specified `SecureRandom.getInstanceStrong()`; that
     * method is API-26+ and this module targets `minSdk = 23`. The default
     * `SecureRandom()` constructor is cryptographically secure on all API-23+
     * Android devices (delegates to `/dev/urandom`) — logged as D-T3-1
     * (Rule 2 substitution).
     *
     * Rotated only by `forget_all()` (Story 4-2 scope).
     */
    fun getOrCreatePerInstallSalt(): ByteArray {
        val existing = prefs.getString(FINGERPRINT_SALT_KEY, null)
        if (existing != null) {
            return hexToBytes(existing)
        }
        val salt = ByteArray(FINGERPRINT_SALT_BYTES).also { SecureRandom().nextBytes(it) }
        prefs.edit()
            .putString(FINGERPRINT_SALT_KEY, bytesToLowerHex(salt))
            .apply()
        return salt
    }

    private fun wipeCorruptPrefs() {
        context.deleteSharedPreferences(PREFS_FILE_NAME)
        // MasterKey Keystore entry remains; next `create` regenerates ciphertext.
    }

    companion object {
        const val MASTER_KEY_ALIAS = "godot-sdk-mwa-master-key-v1"
        const val PREFS_FILE_NAME = "godot-sdk-mwa-tokens-v1"

        /** D-9: alias for the per-install HKDF salt entry. Must NOT collide with [CacheKey.KEY_PREFIX]. */
        const val FINGERPRINT_SALT_KEY = "godot-sdk-mwa-fingerprint-salt-v1"
        const val FINGERPRINT_SALT_BYTES = 32
    }
}
