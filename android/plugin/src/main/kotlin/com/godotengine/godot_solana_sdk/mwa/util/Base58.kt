package com.godotengine.godot_solana_sdk.mwa.util

import java.math.BigInteger

/**
 * Base58 encoder — Solana / Bitcoin alphabet (RFC: base58 without checksum).
 * Used for 32-byte ed25519 public keys in MWA signal payloads (arch §3.1 /
 * §3.2 explicitly specify `public_key` as "base58 pubkey").
 *
 * Implementation is big-integer-based (~30 lines) — no new transitive
 * dependency, no clientlib or rpc-core coupling. For 32-byte inputs the output
 * is a stable ~44 character string.
 *
 * Alphabet (Solana / Bitcoin): `123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz`
 * — the standard base58 alphabet that omits visually-ambiguous chars `0OIl`.
 */
internal object Base58 {
    private const val ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"
    private val BASE = BigInteger.valueOf(58)

    fun encode(bytes: ByteArray): String {
        if (bytes.isEmpty()) return ""

        // Count leading zero bytes — each becomes a leading '1' in base58 output.
        var leadingZeros = 0
        while (leadingZeros < bytes.size && bytes[leadingZeros] == 0x00.toByte()) {
            leadingZeros++
        }

        // Prefix bytes with a sign-extension 0x00 so BigInteger treats the value
        // as unsigned (top-bit-set inputs would otherwise produce a negative BigInteger
        // and the divmod loop would diverge).
        val magnitude = ByteArray(bytes.size + 1)
        System.arraycopy(bytes, 0, magnitude, 1, bytes.size)
        var num = BigInteger(magnitude)

        val sb = StringBuilder()
        while (num > BigInteger.ZERO) {
            val divMod = num.divideAndRemainder(BASE)
            sb.append(ALPHABET[divMod[1].toInt()])
            num = divMod[0]
        }

        // Prepend one '1' per leading zero byte.
        repeat(leadingZeros) { sb.append(ALPHABET[0]) }

        return sb.reverse().toString()
    }
}
