package com.godotengine.godot_solana_sdk.mwa.store

import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import org.json.JSONObject
import org.junit.jupiter.api.Assertions.assertDoesNotThrow
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertFalse
import org.junit.jupiter.api.Assertions.assertThrows
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test

/**
 * Story 2-1 T3 — `CacheRecord` redaction + schema-validation + round-trip coverage.
 *
 * The redaction test is defense-in-depth: `SecretString.toString()` currently
 * returns `"<redacted>"`, so even the data-class-default `toString()` would be
 * safe today — but this test pins the invariant at the `CacheRecord` layer so
 * a future change to `SecretString` can never leak the token through
 * `CacheRecord.toString`.
 */
class CacheRecordTest {

    private val secretTokenLiteral = "very-secret-auth-token-7f3a2b"

    private fun sampleRecord() = CacheRecord(
        schema = 1,
        authToken = SecretString(secretTokenLiteral.toByteArray(Charsets.UTF_8)),
        walletPackage = "com.solana.phantom",
        walletLabel = "Phantom",
        walletIconUri = "https://example.com/icon.png",
        publicKey = "HNm4s...base58...",
        cluster = "devnet",
        chainId = "solana:devnet",
        identityUri = "https://game.example.com",
        issuedAtMs = 1_700_000_000_000L,
        lastUsedAtMs = 1_700_000_600_000L,
    )

    @Test
    fun `toString redacts authToken literal (defense-in-depth against SecretString changes)`() {
        val record = sampleRecord()
        val s = record.toString()
        assertFalse(
            s.contains(secretTokenLiteral),
            "CacheRecord.toString must never contain the raw authToken literal, got: $s",
        )
        assertTrue(s.contains("authToken=<redacted>"), "expected 'authToken=<redacted>' marker, got: $s")
        // Other fields SHOULD appear (sanity — toString isn't uselessly opaque).
        assertTrue(s.contains("walletLabel='Phantom'"))
        assertTrue(s.contains("cluster='devnet'"))
    }

    @Test
    fun `toJson then fromJson round-trips all 11 fields faithfully`() {
        val original = sampleRecord()
        val json = original.toJson()
        val restored = CacheRecord.fromJson(JSONObject(json.toString()))

        // `data class` equals is field-by-field; SecretString.equals uses
        // MessageDigest.isEqual on the underlying bytes.
        assertEquals(original, restored)
    }

    @Test
    fun `fromJson throws IllegalArgumentException on schema mismatch (version skew)`() {
        val v2Json = sampleRecord().toJson().apply { put("schema", 2) }
        val thrown = assertThrows(IllegalArgumentException::class.java) {
            CacheRecord.fromJson(v2Json)
        }
        assertTrue(
            thrown.message!!.contains("Unsupported CacheRecord schema"),
            "expected schema-mismatch message, got: ${thrown.message}",
        )
    }

    @Test
    fun `fromJson accepts current schema v1`() {
        val json = sampleRecord().toJson()
        assertDoesNotThrow { CacheRecord.fromJson(json) }
    }

    @Test
    fun `toJson puts authToken UTF-8 string at 'auth_token' key (JSON boundary format)`() {
        val record = sampleRecord()
        val json = record.toJson()
        // Crosses as UTF-8 string (the base64 opaque wallet token); SecretString wraps in memory.
        assertEquals(secretTokenLiteral, json.getString("auth_token"))
    }
}
