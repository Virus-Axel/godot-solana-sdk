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
 * `putToken` / `getToken` route through [CacheKey.toHash] + [CacheRecord.toJson]
 * / [CacheRecord.fromJson]. `listAllKeys` is consumed by Story 4-2 `forget_all`
 * to enumerate cached wallets across identities.
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

    /**
     * Story 3-3 T1 (DD-3-3-A) — pending-submission breadcrumb storage. String-
     * keyed under the [PENDING_KEY_PREFIX] (`"pending::"`) namespace inside the
     * SAME `EncryptedSharedPreferences` instance as the auth-cache entries.
     * Parallel to [getOrCreatePerInstallSalt]'s special-case [FINGERPRINT_SALT_KEY]
     * — a non-CacheRecord entry that co-lives in the same prefs file. Values
     * are JSON strings (NOT [CacheRecord] instances) carrying the 6-key dict
     * shape per AC-2:
     * `{request_id, op_type, cluster, identity_uri, started_at_ms, tx_count, tx_preview_hashes}`.
     *
     * The breadcrumb body is opaque to this class — the plugin layer builds
     * and parses the JSON. This API is intentionally a thin string-keyed
     * facade so the breadcrumb schema can evolve independently of the storage
     * shell.
     *
     * Story 3-3 T2 — the implementation is a thin string-keyed put/get/list/
     * remove facade over [EncryptedSharedPreferences] under [PENDING_KEY_PREFIX].
     */
    fun putPendingSubmission(requestId: String, breadcrumbDictJson: String) {
        prefs.edit()
            .putString(PENDING_KEY_PREFIX + requestId, breadcrumbDictJson)
            .apply()
    }

    /**
     * Story 3-3 (DD-3-3-A) — returns the breadcrumb JSON string for [requestId],
     * or null if no entry exists.
     */
    fun getPendingSubmission(requestId: String): String? = prefs.getString(PENDING_KEY_PREFIX + requestId, null)

    /**
     * Story 3-3 (DD-3-3-A) — returns every pending-submission entry as a
     * `(requestId, breadcrumbDictJson)` pair list. The requestId is the value
     * AFTER the [PENDING_KEY_PREFIX] strip (callers do NOT see the
     * `"pending::"` prefix).
     *
     * Wrapped by `withStorageOrReauthRequired` at the plugin layer per
     * DD-3-3-G — this method MAY throw [StorageCorruptException] via the
     * lazy `prefs` init.
     */
    fun listAllPendingSubmissions(): List<Pair<String, String>> = prefs.all
        .filterKeys { it.startsWith(PENDING_KEY_PREFIX) }
        .mapNotNull { (k, v) ->
            val value = v as? String ?: return@mapNotNull null
            k.removePrefix(PENDING_KEY_PREFIX) to value
        }

    /**
     * Story 3-3 (DD-3-3-A) — removes the pending entry for [requestId]. No-op
     * if the entry does not exist (SharedPreferences.Editor.remove is a no-op
     * on absent keys).
     */
    fun removePendingSubmission(requestId: String) {
        prefs.edit().remove(PENDING_KEY_PREFIX + requestId).apply()
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

        /**
         * Story 3-3 T1 (DD-3-3-A) — namespace prefix for pending-submission
         * breadcrumbs. Entries under this prefix are NOT [CacheRecord] instances
         * — they are JSON-string blobs carrying the 6-key breadcrumb shape per
         * AC-2. The prefix MUST NOT collide with [CacheKey.KEY_PREFIX]
         * (`"mwa::v1::"`) or [FINGERPRINT_SALT_KEY] (`"godot-sdk-mwa-fingerprint-salt-v1"`).
         * The [listAllKeys] iteration's `startsWith(CacheKey.KEY_PREFIX)` filter
         * excludes pending entries from the auth-cache view per the same
         * disambiguation pattern.
         */
        const val PENDING_KEY_PREFIX = "pending::"
    }
}
