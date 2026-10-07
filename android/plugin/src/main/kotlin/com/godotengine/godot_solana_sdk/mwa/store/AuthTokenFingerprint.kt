package com.godotengine.godot_solana_sdk.mwa.store

import javax.crypto.Mac
import javax.crypto.spec.SecretKeySpec

/**
 * HKDF-SHA256 auth-token fingerprint per AC-7.
 *
 *   fingerprint = lower_hex(HKDF-SHA256(IKM=authTokenBytes,
 *                                        salt=perInstallSalt,
 *                                        info="mwa-fingerprint-v1",
 *                                        L=4 bytes))
 *
 * Returns an 8-char lowercase hex string. Idempotent for a fixed `(ikm, salt)`
 * pair across calls.
 *
 * Intended purpose: a log-friendly / diagnostic-display disambiguator so users
 * and support engineers can identify which wallet session they have (arch §7.2
 * correlation IDs, `get_diagnostics`) — NOT a cryptographically
 * identifying credential. The 4-byte truncation gives roughly 32 bits of
 * preimage resistance and only ~16 bits of birthday-collision resistance, so
 * do not use the fingerprint as an authorization input or a primary key.
 * Unforgeability of the authToken itself rests on the full HMAC-SHA256 output
 * that HKDF derives; the truncation is for display ergonomics only.
 *
 * Per-install salt is a 32-byte `SecureRandom()` value persisted in the same
 * `EncryptedSharedPreferences` file under alias
 * `"godot-sdk-mwa-fingerprint-salt-v1"` — see
 * [SecureTokenStore.getOrCreatePerInstallSalt]. Rotation is scoped to
 * `forget_all`.
 *
 * RFC 5869 compliance:
 * - Extract:  PRK = HMAC-SHA256(salt, IKM)
 * - Expand:   T(1) = HMAC-SHA256(PRK, info || 0x01)   — single iteration, since L=4 ≤ HashLen=32
 * - Output:   first L=4 bytes of T(1), hex-encoded.
 */
object AuthTokenFingerprint {
    private const val HMAC_ALGO = "HmacSHA256"
    private const val INFO = "mwa-fingerprint-v1"
    private const val OUTPUT_BYTES = 4

    fun compute(authTokenBytes: ByteArray, salt: ByteArray): String {
        val prk = hmacSha256(salt, authTokenBytes)
        val t1 = hmacSha256(prk, INFO.toByteArray(Charsets.UTF_8) + 0x01.toByte())
        return bytesToLowerHex(t1.copyOfRange(0, OUTPUT_BYTES))
    }

    private fun hmacSha256(key: ByteArray, data: ByteArray): ByteArray {
        val mac = Mac.getInstance(HMAC_ALGO)
        mac.init(SecretKeySpec(key, HMAC_ALGO))
        return mac.doFinal(data)
    }
}
