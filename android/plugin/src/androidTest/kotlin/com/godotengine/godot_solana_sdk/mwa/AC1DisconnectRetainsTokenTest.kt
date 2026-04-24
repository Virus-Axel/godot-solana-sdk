package com.godotengine.godot_solana_sdk.mwa

import android.content.Context
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.godotengine.godot_solana_sdk.mwa.plugin.NativeBridge
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import io.mockk.mockk
import io.mockk.slot
import io.mockk.verify
import org.json.JSONObject
import org.junit.After
import org.junit.Assert.assertArrayEquals
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 2-3 T5 — AC-1 on-device: `MWA.disconnect()` wipes `MwaSessionState`
 * auth-only fields AND retains the `SecureTokenStore` CacheRecord on real
 * `EncryptedSharedPreferences` + real Keystore.
 *
 * This is the highest-leverage AC-1 assertion per concern C4: the
 * unit-level `MwaAndroidPluginDisconnectTest` uses a map-backed fake store,
 * which by construction returns the same record it was put with — so the
 * byte-equality there is partly anchored in test-harness identity. On
 * device, the store round-trips through AES256_GCM encryption + file I/O;
 * if disconnect's retention invariant were broken at ANY layer (e.g. by
 * calling `deleteToken`, by clobbering the file, by corrupting the salt),
 * this test fails.
 *
 * Flow:
 *   1. Fresh connect via `FakeMwaClient("success")` → assert cached record.
 *   2. Snapshot the CacheRecord (deep-copy token bytes for later compare).
 *   3. Call `plugin.mwaDisconnect("disconnect-ac1")` via the test seam.
 *   4. Assert:
 *      (a) `disconnect_completed` fires within ≤200ms (AC-1 wall-clock).
 *      (b) `MwaSessionState` auth-only fields wiped.
 *      (c) `MwaSessionState` identity + `cluster: Int` preserved (DD-2-3-5).
 *      (d) `SecureTokenStore.getToken(sameKey)` returns a record
 *          byte-equal to the pre-disconnect snapshot.
 *      (e) `SecureTokenStore.listAllKeys()` still has exactly 1 key.
 *
 * Route B harness per Story 2-1 T9 D-T9-1 — in-process `FakeMwaClient`
 * injected via `@VisibleForTesting` ctor, no Fake Wallet APK install.
 */
@RunWith(AndroidJUnit4::class)
class AC1DisconnectRetainsTokenTest {

    private lateinit var ctx: Context
    private lateinit var nativeBridge: NativeBridge
    private lateinit var plugin: GDExtensionAndroidPlugin
    private lateinit var fixturesDir: File

    private val identityUri = "https://example.com/godot-mwa-disconnect-ac1"
    private val identityJson = JSONObject().apply {
        put("name", "Test Game (2-3 AC-1)")
        put("icon_uri", "https://example.com/icon.png")
        put("identity_uri", identityUri)
    }.toString()

    @Before
    fun setup() {
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        val testCtx = InstrumentationRegistry.getInstrumentation().context
        fixturesDir = FakeWalletFixtures.extractToCacheDir(testCtx)
        SecureTokenStore(ctx).deleteAll()
        GDExtensionAndroidPlugin.sessionState.clear()

        nativeBridge = mockk(relaxed = true)
        plugin = PluginTestHarness.freshPlugin(
            ctx = ctx,
            fixturesDir = fixturesDir,
            scenario = "success",
            nativeBridge = nativeBridge,
        )
    }

    @After
    fun teardown() {
        runCatching { SecureTokenStore(ctx).deleteAll() }
        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @Test
    fun ac1_disconnectWipesSessionStateButRetainsSecureTokenStoreEntry() {
        // --- Stage 1: connect ------------------------------------------------
        plugin.mwaAuthorize(
            requestId = "req-ac1-connect",
            identityJson = identityJson,
            cluster = "devnet",
            chainId = "solana:devnet",
            timeoutMs = 3_000L,
        )
        PluginTestHarness.awaitCondition(3_000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postConnectCompleted("req-ac1-connect", any()) }
            }.isSuccess
        }

        // Snapshot pre-disconnect state. We capture in two parallel tracks:
        //   (i) the store side  — keyBefore + recordBefore + authTokenBytesBefore
        //   (ii) the session side — all 4 identity fields + fingerprint
        // Track (i) feeds the retention invariant + CacheKey re-derivability.
        // Track (ii) feeds the wipe-vs-preserve assertions + DD-2-3-5 anchor.
        val store = SecureTokenStore(ctx)
        val keysBefore = store.listAllKeys()
        assertEquals("Exactly 1 record cached post-connect", 1, keysBefore.size)
        val keyBefore = keysBefore.single()
        val recordBefore: CacheRecord = store.getToken(keyBefore)
            ?: error("Pre-disconnect getToken returned null on a freshly-stored key — pre-condition failure")
        // Deep-copy the auth_token bytes — SecretString.reveal() returns a
        // reference to internal state that could in principle be zeroed by a
        // future CacheRecord.toString() / serialization change.
        val authTokenBytesBefore = recordBefore.authToken.reveal().copyOf()
        // Spec T5 text mentions `recordAfter.authTokenFingerprint` — this is a
        // spec artifact; fingerprint lives on MwaSessionState (wiped, asserted
        // below), NOT on CacheRecord (which stores authToken bytes; fingerprint
        // is re-derivable from bytes + per-install salt via
        // AuthTokenFingerprint.compute). We capture the session-side fingerprint
        // so the wipe assertion can reference a known-non-empty "before" value.
        val fingerprintBefore = GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint()
        val publicKeyBefore = GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58()
        // All 4 preserved-across-disconnect identity fields per DD-2-3-5's
        // `preserves` list in MwaSessionState.clearOnLogout.
        val identityUriBefore = GDExtensionAndroidPlugin.sessionState.getIdentityUri()
        val iconUriBefore = GDExtensionAndroidPlugin.sessionState.getIconUri()
        val identityNameBefore = GDExtensionAndroidPlugin.sessionState.getIdentityName()
        val clusterIntBefore = GDExtensionAndroidPlugin.sessionState.getCluster()

        // Sanity: session state is populated pre-disconnect.
        assertNotNull("authToken set post-connect", GDExtensionAndroidPlugin.sessionState.getAuthToken())
        assertTrue("publicKey populated post-connect", publicKeyBefore.isNotEmpty())
        assertTrue("fingerprint populated post-connect", fingerprintBefore.isNotEmpty())

        // DD-2-3-5 / concern C2 on-device evidence — Story 2-2's silent reauth
        // does NOT hold the original `CacheKey` object; it rebuilds the key
        // from the caller's `(cluster, chainId, walletPackage, identityUri)`
        // arguments at each call site. Verify here that an independently-
        // constructed CacheKey from CacheRecord fields (which mirror what
        // Story 2-2's GDScript caller will pass in) resolves to the same
        // on-disk record. If FakeMwaClient's success fixture leaves
        // walletPackage empty, that empty string IS the real contract surface —
        // Story 2-2 will receive the same empty value from the same wallet.
        val freshKey = CacheKey(
            cluster = recordBefore.cluster,
            chainId = recordBefore.chainId,
            walletPackage = recordBefore.walletPackage,
            identityUri = recordBefore.identityUri,
        )
        assertEquals(
            "CacheKey re-derivable from CacheRecord fields (DD-2-3-5)",
            keyBefore,
            freshKey,
        )
        assertEquals(
            "CacheKey.toHash() stable across call sites (DD-2-3-5 + C2)",
            keyBefore.toHash(),
            freshKey.toHash(),
        )

        // --- Stage 2: disconnect --------------------------------------------
        // AC-1's ≤200ms bound is on SIGNAL FIRE, not method return. The method
        // itself is scope.launch + return, so returnElapsedMs is microseconds
        // regardless of body — a return-timing check here would be tautological.
        // The awaitCondition(200L) below is the actual AC-1 timing spec.
        plugin.mwaDisconnect("disconnect-ac1")

        val jsonSlot = slot<String>()
        PluginTestHarness.awaitCondition(200L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postDisconnectCompleted("disconnect-ac1", capture(jsonSlot)) }
            }.isSuccess
        }

        // (a) disconnect_completed payload shape per DD-2-3-4 (2-key minimal).
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("disconnect-ac1", payload.getString("request_id"))
        assertEquals("disconnect", payload.getString("source_method"))

        // (b) MwaSessionState auth-only fields wiped.
        assertNull("authToken wiped", GDExtensionAndroidPlugin.sessionState.getAuthToken())
        assertEquals("publicKeyBase58 wiped", "", GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58())
        assertEquals("clusterName wiped", "", GDExtensionAndroidPlugin.sessionState.getClusterName())
        assertEquals("walletLabel wiped", "", GDExtensionAndroidPlugin.sessionState.getWalletLabel())
        assertEquals(
            "authTokenFingerprint wiped",
            "",
            GDExtensionAndroidPlugin.sessionState.getAuthTokenFingerprint(),
        )
        assertEquals("walletIconUri wiped", "", GDExtensionAndroidPlugin.sessionState.getWalletIconUri())
        assertNull("connectedKey wiped", GDExtensionAndroidPlugin.sessionState.getConnectedKey())

        // (c) MwaSessionState identity + cluster Int preserved (DD-2-3-5).
        //     All 4 preserved fields asserted — identityUri is a CacheKey input
        //     and its loss would break Story 2-2's silent reauth directly; the
        //     other 3 are caller-display/diag fields that silent reauth depends
        //     on for UX continuity. The contract is "unchanged across disconnect",
        //     not "non-empty" — authorize path populates all 4 but some may be
        //     empty under FakeMwaClient coverage.
        assertEquals(
            "identityUri preserved across disconnect",
            identityUriBefore,
            GDExtensionAndroidPlugin.sessionState.getIdentityUri(),
        )
        assertEquals(
            "iconUri preserved across disconnect",
            iconUriBefore,
            GDExtensionAndroidPlugin.sessionState.getIconUri(),
        )
        assertEquals(
            "identityName preserved across disconnect",
            identityNameBefore,
            GDExtensionAndroidPlugin.sessionState.getIdentityName(),
        )
        assertEquals(
            "cluster (Int) preserved across disconnect",
            clusterIntBefore,
            GDExtensionAndroidPlugin.sessionState.getCluster(),
        )

        // (d) SecureTokenStore retention — the HIGHEST-LEVERAGE assertion (C4).
        //     Real EncryptedSharedPreferences round-trip: if disconnect
        //     accidentally invoked deleteToken/deleteAll, this fails.
        val recordAfter = store.getToken(keyBefore)
        assertNotNull(
            "CacheRecord MUST be retained post-disconnect — disconnect is NOT deauthorize",
            recordAfter,
        )
        assertArrayEquals(
            "auth_token bytes MUST match pre-disconnect (real-crypto round-trip)",
            authTokenBytesBefore,
            recordAfter!!.authToken.reveal(),
        )
        assertEquals(
            "cluster preserved in persisted record",
            recordBefore.cluster,
            recordAfter.cluster,
        )
        assertEquals(
            "chainId preserved in persisted record",
            recordBefore.chainId,
            recordAfter.chainId,
        )
        assertEquals(
            "identityUri preserved in persisted record",
            recordBefore.identityUri,
            recordAfter.identityUri,
        )

        // (e) Store cardinality — no accidental deletion of other keys either.
        val keysAfter = store.listAllKeys()
        assertEquals(
            "SecureTokenStore key count unchanged across disconnect",
            keysBefore.size,
            keysAfter.size,
        )
        assertEquals(
            "Same CacheKey retained",
            keyBefore,
            keysAfter.single(),
        )

        // (f) DD-2-3-5 / C2 on-device evidence — CacheKey re-derived from
        //     CacheRecord fields retrieves the same record post-disconnect.
        //     This is the REAL-CRYPTO-ROUND-TRIP proof that Story 2-2's silent
        //     reauth path works: the caller's rebuilt key finds the stored
        //     record through AES256_GCM-encrypted SharedPreferences.
        val recordViaFreshKey = store.getToken(freshKey)
        assertNotNull(
            "CacheRecord retrievable via re-derived CacheKey post-disconnect — " +
                "Story 2-2 silent reauth path works",
            recordViaFreshKey,
        )
        assertArrayEquals(
            "auth_token bytes match pre-disconnect when looked up via re-derived key",
            authTokenBytesBefore,
            recordViaFreshKey!!.authToken.reveal(),
        )

        // (g) Terminal-signal invariant — no error/timeout alongside.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
    }
}
