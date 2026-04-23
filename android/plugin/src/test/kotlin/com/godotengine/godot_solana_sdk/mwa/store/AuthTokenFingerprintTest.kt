package com.godotengine.godot_solana_sdk.mwa.store

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test

/**
 * Story 2-1 T3 — `AuthTokenFingerprint` HKDF-SHA256 coverage (AC-7).
 *
 * The golden-vector test pins the entire HKDF pipeline (extract HMAC +
 * expand HMAC + info string + truncation + hex encoding) against a hex
 * computed out-of-band in Python `hmac`. This catches unified drift that
 * a self-referential reconstruction cannot.
 */
class AuthTokenFingerprintTest {

    private val ikm = "test-auth-token-v1".toByteArray(Charsets.UTF_8)
    private val saltZeros = ByteArray(32)
    private val saltOnes = ByteArray(32) { 0x01 }

    @Test
    fun `AC-7 idempotence - same token and salt produce identical 8-char hex across calls`() {
        val first = AuthTokenFingerprint.compute(ikm, saltZeros)
        val second = AuthTokenFingerprint.compute(ikm, saltZeros)
        assertEquals(first, second)
    }

    @Test
    fun `different salt changes fingerprint (same token)`() {
        val fpZeros = AuthTokenFingerprint.compute(ikm, saltZeros)
        val fpOnes = AuthTokenFingerprint.compute(ikm, saltOnes)
        assertNotEquals(fpZeros, fpOnes)
    }

    @Test
    fun `different token changes fingerprint (same salt)`() {
        val ikm2 = "another-auth-token".toByteArray(Charsets.UTF_8)
        val fp1 = AuthTokenFingerprint.compute(ikm, saltZeros)
        val fp2 = AuthTokenFingerprint.compute(ikm2, saltZeros)
        assertNotEquals(fp1, fp2)
    }

    @Test
    fun `output is exactly 8 lowercase hex chars`() {
        val fp = AuthTokenFingerprint.compute(ikm, saltZeros)
        assertEquals(8, fp.length)
        assertEquals(fp, fp.lowercase())
        assertTrue(fp.all { it in '0'..'9' || it in 'a'..'f' }, "fingerprint must be [0-9a-f]+, got '$fp'")
    }

    @Test
    fun `golden vector - HKDF-SHA256 matches Python hmac reference`() {
        // Computed out-of-band via Python (see Story 2-1 T3 commit message for
        // the reproducer snippet). Inputs:
        //   ikm   = b"test-auth-token-v1"
        //   salt  = b"\x00" * 32
        //   info  = b"mwa-fingerprint-v1"
        //   L     = 4 bytes
        //   HKDF  = extract: PRK = HMAC-SHA256(salt, ikm)
        //           expand: T(1) = HMAC-SHA256(PRK, info || 0x01)
        //           output: T(1)[:4]
        // Expected: c2f22861
        assertEquals("c2f22861", AuthTokenFingerprint.compute(ikm, saltZeros))

        // Secondary salt pin — catches drift in the Extract stage.
        assertEquals("7eaa364d", AuthTokenFingerprint.compute(ikm, saltOnes))
    }
}
