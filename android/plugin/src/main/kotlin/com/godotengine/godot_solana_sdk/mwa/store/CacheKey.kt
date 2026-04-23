package com.godotengine.godot_solana_sdk.mwa.store

import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.security.MessageDigest
import java.util.Locale

/**
 * Cache key for [SecureTokenStore] — a 4-tuple identifying a stored MWA auth
 * record (DD-17). On-disk storage keys are `"mwa::v1::" + SHA256(canonical())`
 * where `canonical()` is a length-prefixed concatenation of the 4 fields
 * (DD-27): each field is encoded as `4-byte-big-endian(len(utf8)) ||
 * utf8-bytes`. A raw `"|".join(...)` joiner would collide when, for example,
 * `identityUri` contains a literal `|` in a URL query (AC-2 load-bearing).
 */
data class CacheKey(
    val cluster: String,
    val chainId: String,
    val walletPackage: String,
    val identityUri: String,
) {
    /** `"mwa::v1::" + SHA256(canonical())` per DD-17 / DD-27. */
    fun toHash(): String = KEY_PREFIX + sha256Hex(canonical())

    /**
     * Length-prefixed UTF-8 concatenation. For each of the 4 fields in order
     * (cluster, chainId, walletPackage, identityUri): write 4-byte big-endian
     * length followed by the raw UTF-8 bytes. Prevents delimiter-collision
     * (AC-2) — e.g. `CacheKey("devnet", "solana:devnet", ...)` and
     * `CacheKey("devnet|solana:devnet", ...)` produce distinct byte strings.
     */
    private fun canonical(): ByteArray = ByteArrayOutputStream().use { out ->
        listOf(cluster, chainId, walletPackage, identityUri).forEach { s ->
            val b = s.toByteArray(Charsets.UTF_8)
            out.write(ByteBuffer.allocate(4).order(ByteOrder.BIG_ENDIAN).putInt(b.size).array())
            out.write(b)
        }
        out.toByteArray()
    }

    companion object {
        /**
         * Persisted-key prefix. Used by [SecureTokenStore.listAllKeys] to filter
         * `SharedPreferences.getAll` entries — non-CacheRecord entries (e.g. the
         * fingerprint salt under `FINGERPRINT_SALT_KEY`) live in the same
         * prefs file and MUST be excluded from cache-key iteration.
         */
        const val KEY_PREFIX = "mwa::v1::"
    }
}

/**
 * SHA-256 hex encoder. Lowercase, no separator (`%02x` per byte). Module-internal
 * helper; shared with T3's `AuthTokenFingerprint` HKDF output path.
 *
 * Formatter handles the signed-to-unsigned coercion for `%x` + `Byte` (adds
 * `2^8` to negative bytes — see `java.util.Formatter.print(byte, Locale)`), so
 * we do NOT pre-widen via `b.toInt()`, which WOULD sign-extend. `Locale.ROOT`
 * is belt-and-braces: hex digits `0-9a-f` are locale-invariant, but codec paths
 * should not depend on the default locale.
 */
internal fun sha256Hex(bytes: ByteArray): String = bytesToLowerHex(
    MessageDigest.getInstance("SHA-256").digest(bytes),
)

/** Lower-hex encode — 2 chars per input byte. See [sha256Hex] kdoc for Byte/Int rationale. */
internal fun bytesToLowerHex(bytes: ByteArray): String {
    val sb = StringBuilder(bytes.size * 2)
    for (b in bytes) {
        sb.append("%02x".format(Locale.ROOT, b))
    }
    return sb.toString()
}

/** Inverse of [bytesToLowerHex]. Accepts lowercase hex; throws [IllegalArgumentException] on odd length or invalid chars. */
internal fun hexToBytes(hex: String): ByteArray {
    require(hex.length % 2 == 0) { "hex string must have even length, got ${hex.length}" }
    val out = ByteArray(hex.length / 2)
    for (i in out.indices) {
        val hi = Character.digit(hex[i * 2], 16)
        val lo = Character.digit(hex[i * 2 + 1], 16)
        require(hi >= 0 && lo >= 0) { "invalid hex character at index ${i * 2} in '$hex'" }
        out[i] = ((hi shl 4) or lo).toByte()
    }
    return out
}
