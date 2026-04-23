package com.godotengine.godot_solana_sdk.mwa.store

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test

/**
 * Story 2-1 T2 — `CacheKey` length-prefixed canonicalization coverage.
 *
 * AC-2 (delimiter collision) is the load-bearing test — a raw `"|"` joiner
 * would collapse `("devnet", "solana:devnet", …, "…?a=1|2")` and
 * `("devnet|solana:devnet", "com.p.app", "…?a=1", "2")` into an identical
 * canonical byte string; length-prefixing breaks the collision.
 *
 * The golden-vector tests pin the entire canonical pipeline (field order +
 * big-endian width + UTF-8 + SHA-256) against hex strings computed out-of-band
 * in Python. This catches unified drift that a self-referential reconstruction
 * cannot (e.g., a simultaneous endianness flip in both production and test).
 */
class CacheKeyTest {

    @Test
    fun `AC-2 delimiter collision - length prefixing prevents hash identity between lookalike tuples`() {
        // Tuple A: cluster="devnet", chainId="solana:devnet", walletPackage="com.p.app",
        //         identityUri="https://ex.com/?a=1|2"
        val a = CacheKey("devnet", "solana:devnet", "com.p.app", "https://ex.com/?a=1|2")
        // Tuple B: cluster="devnet|solana:devnet", chainId="com.p.app",
        //         walletPackage="https://ex.com/?a=1", identityUri="2"
        val b = CacheKey("devnet|solana:devnet", "com.p.app", "https://ex.com/?a=1", "2")
        assertNotEquals(
            a.toHash(),
            b.toHash(),
            "length-prefixed canonicalization must break AC-2 delimiter collision",
        )
    }

    @Test
    fun `same tuple produces identical hash across repeated calls (deterministic)`() {
        val key = CacheKey("mainnet", "solana:mainnet", "com.wallet", "https://example.com")
        assertEquals(key.toHash(), key.toHash())
    }

    @Test
    fun `toHash prefix is mwa v1`() {
        val key = CacheKey("devnet", "solana:devnet", "com.p.app", "https://ex.com")
        assertTrue(
            key.toHash().startsWith("mwa::v1::"),
            "toHash must prefix 'mwa::v1::' per DD-17",
        )
    }

    @Test
    fun `toHash matches golden vector computed out-of-band (pinned pipeline)`() {
        // Golden computed via Python hashlib (independent of the Kotlin JVM path):
        //   import hashlib, struct
        //   fields = [b"devnet", b"solana:devnet", b"com.p.app", b"https://ex.com"]
        //   buf = b"".join(struct.pack(">I", len(f)) + f for f in fields)
        //   hashlib.sha256(buf).hexdigest()
        // Re-run that snippet if this assertion ever fails — mismatch indicates drift in
        // ONE OR MORE of: field order, 4-byte big-endian length width, UTF-8 encoding,
        // or SHA-256 instance selection.
        assertEquals(
            "mwa::v1::3ec52dd8178839569cc0d4e474474bb47e5cadb29c631e4c5967700cefbf2dce",
            CacheKey("devnet", "solana:devnet", "com.p.app", "https://ex.com").toHash(),
        )
    }

    @Test
    fun `empty-field tuple hashes 16 zero bytes (len=0 semantics)`() {
        // Canonical = 4 fields × (4-byte zero length-prefix + 0 payload bytes) = 16 zero bytes.
        // Golden = SHA-256 of 16 zero bytes (well-known):
        //   python3 -c "import hashlib; print(hashlib.sha256(b'\0' * 16).hexdigest())"
        //   → 374708fff7719dd5979ec875d56cd2286f6d3cf7ec317a3b25632aab28ec37bb
        // Locks in the len=0 semantics from CacheKey.canonical's kdoc.
        assertEquals(
            "mwa::v1::374708fff7719dd5979ec875d56cd2286f6d3cf7ec317a3b25632aab28ec37bb",
            CacheKey("", "", "", "").toHash(),
        )
    }

    @Test
    fun `sha256Hex produces lowercase 64-char hex with no separator`() {
        // Well-known: SHA256("") = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
        val hex = sha256Hex(ByteArray(0))
        assertEquals(64, hex.length)
        assertEquals(hex, hex.lowercase())
        assertEquals(
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
            hex,
        )
    }
}
