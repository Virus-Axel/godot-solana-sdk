package com.godotengine.godot_solana_sdk.mwa.util

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test

class SecretStringTest {

    @Test
    fun `toString returns literal redacted`() {
        assertEquals("<redacted>", SecretString(byteArrayOf(1, 2, 3)).toString())
    }

    @Test
    fun `reveal returns original bytes`() {
        val original = byteArrayOf(1, 2, 3)
        assertTrue(SecretString(original).reveal().contentEquals(original))
    }

    @Test
    fun `reveal returns a defensive copy so mutation does not leak`() {
        val s = SecretString(byteArrayOf(1, 2, 3))
        val first = s.reveal()
        first[0] = 99
        val second = s.reveal()
        assertTrue(
            second.contentEquals(byteArrayOf(1, 2, 3)),
            "mutating a revealed copy must not affect subsequent reveal() return values; got ${second.joinToString()}",
        )
    }

    @Test
    fun `ctor does not alias caller-provided array`() {
        val source = byteArrayOf(1, 2, 3)
        val s = SecretString(source)
        source[0] = 99
        assertTrue(
            s.reveal().contentEquals(byteArrayOf(1, 2, 3)),
            "SecretString must not alias the caller's array; mutation of the source leaked into reveal()",
        )
    }

    @Test
    fun `equals and hashCode honor content equality`() {
        val a = SecretString(byteArrayOf(1, 2, 3))
        val b = SecretString(byteArrayOf(1, 2, 3))
        val c = SecretString(byteArrayOf(4, 5, 6))
        assertEquals(a, b)
        assertEquals(a.hashCode(), b.hashCode())
        assertNotEquals(a, c)
    }

    @Test
    fun `no public accessor besides reveal leaks bytes`() {
        // Java-reflection guard against accidental `value` / `getBytes` leaks. Uses java.lang.reflect
        // instead of Kotlin's kotlin-reflect so the test does not require an extra runtime dependency.
        // Kotlin properties compile to `getX()` methods in Java; private fields do not appear in .fields.
        val publicMembers = buildSet {
            SecretString::class.java.methods.forEach { add(it.name) }
            SecretString::class.java.fields.forEach { add(it.name) }
        }
        val banned = setOf(
            "bytes", "value", "content", "data",
            "getBytes", "getValue", "getContent", "getData",
        )
        banned.forEach { name ->
            assertTrue(
                name !in publicMembers,
                "SecretString must not expose public member '$name'; AC-1 requires reveal() to be the sole accessor. Public members: $publicMembers",
            )
        }
    }
}
