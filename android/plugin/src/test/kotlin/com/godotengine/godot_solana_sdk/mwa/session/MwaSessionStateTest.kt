package com.godotengine.godot_solana_sdk.mwa.session

import org.junit.jupiter.api.Assertions.assertArrayEquals
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNull
import org.junit.jupiter.api.Test

class MwaSessionStateTest {

    @Test
    fun `defaults are all null or zero on fresh instance`() {
        val s = MwaSessionState()

        assertNull(s.getLastResult())
        assertEquals(0, s.getPendingAction())
        assertNull(s.getPendingTransaction())
        assertEquals("", s.getPendingTextMessage())
        assertNull(s.getLastSignature())
        assertEquals(0, s.getSigningStatus())
        assertNull(s.getConnectedKey())
        assertEquals(0, s.getCluster())
        assertEquals("", s.getIdentityName())
        assertNull(s.getAuthToken())
    }

    @Test
    fun `setResult and getLastResult round-trip`() {
        val s = MwaSessionState()
        val sentinel = Any()

        s.setResult(sentinel)

        assertEquals(sentinel, s.getLastResult())
    }

    @Test
    fun `setAction and getPendingAction round-trip`() {
        val s = MwaSessionState()

        s.setAction(2)

        assertEquals(2, s.getPendingAction())
    }

    @Test
    fun `setTransaction and getPendingTransaction round-trip`() {
        val s = MwaSessionState()
        val tx = byteArrayOf(1, 2, 3, 4)

        s.setTransaction(tx)

        assertArrayEquals(tx, s.getPendingTransaction())
    }

    @Test
    fun `setTextMessage and getPendingTextMessage round-trip`() {
        val s = MwaSessionState()

        s.setTextMessage("hello")

        assertEquals("hello", s.getPendingTextMessage())
    }

    @Test
    fun `setSignature and getLastSignature round-trip`() {
        val s = MwaSessionState()
        val sig = byteArrayOf(9, 8, 7)

        s.setSignature(sig)

        assertArrayEquals(sig, s.getLastSignature())
    }

    @Test
    fun `setStatus getSigningStatus and clearStatus reset`() {
        val s = MwaSessionState()

        s.setStatus(1)
        assertEquals(1, s.getSigningStatus())

        s.clearStatus()
        assertEquals(0, s.getSigningStatus())
    }

    @Test
    fun `setKey and getConnectedKey round-trip`() {
        val s = MwaSessionState()
        val key = byteArrayOf(0xA, 0xB, 0xC)

        s.setKey(key)

        assertArrayEquals(key, s.getConnectedKey())
    }

    @Test
    fun `setCluster and getCluster round-trip`() {
        val s = MwaSessionState()

        s.setCluster(1)

        assertEquals(1, s.getCluster())
    }

    @Test
    fun `setIdentity populates uri icon and name`() {
        val s = MwaSessionState()

        s.setIdentity("https://example.com", "https://example.com/icon.png", "MyApp")

        assertEquals("https://example.com", s.getIdentityUri())
        assertEquals("https://example.com/icon.png", s.getIconUri())
        assertEquals("MyApp", s.getIdentityName())
    }

    @Test
    fun `setAuthToken and getAuthToken round-trip`() {
        val s = MwaSessionState()

        s.setAuthToken("token-abc")

        assertEquals("token-abc", s.getAuthToken())
    }

    @Test
    fun `clear resets every field to its default`() {
        val s = MwaSessionState()
        s.setResult(Any())
        s.setAction(5)
        s.setTransaction(byteArrayOf(1, 2))
        s.setTextMessage("x")
        s.setSignature(byteArrayOf(3, 4))
        s.setStatus(2)
        s.setKey(byteArrayOf(5, 6))
        s.setCluster(1)
        s.setIdentity("u", "i", "n")
        s.setAuthToken("t")

        s.clear()

        assertNull(s.getLastResult())
        assertEquals(0, s.getPendingAction())
        assertNull(s.getPendingTransaction())
        assertEquals("", s.getPendingTextMessage())
        assertNull(s.getLastSignature())
        assertEquals(0, s.getSigningStatus())
        assertNull(s.getConnectedKey())
        assertEquals(0, s.getCluster())
        assertEquals("", s.getIdentityUri())
        assertEquals("", s.getIconUri())
        assertEquals("", s.getIdentityName())
        assertNull(s.getAuthToken())
    }

    @Test
    fun `two instances do not share state`() {
        val a = MwaSessionState()
        val b = MwaSessionState()

        a.setAction(7)
        b.setAction(3)

        assertEquals(7, a.getPendingAction())
        assertEquals(3, b.getPendingAction())
    }
}
