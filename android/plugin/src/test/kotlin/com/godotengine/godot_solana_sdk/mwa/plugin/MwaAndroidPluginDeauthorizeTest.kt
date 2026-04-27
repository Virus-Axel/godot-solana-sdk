package com.godotengine.godot_solana_sdk.mwa.plugin

import android.net.Uri
import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.store.CacheKey
import com.godotengine.godot_solana_sdk.mwa.store.CacheRecord
import com.godotengine.godot_solana_sdk.mwa.store.SecureTokenStore
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import io.mockk.coEvery
import io.mockk.coVerify
import io.mockk.every
import io.mockk.mockk
import io.mockk.mockkStatic
import io.mockk.slot
import io.mockk.unmockkStatic
import io.mockk.verify
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import org.godotengine.godot.Godot
import org.json.JSONObject
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertFalse
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Assertions.assertNull
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import plugin.walletadapterandroid.GDExtensionAndroidPlugin
import java.io.File

/**
 * Story 4-1 T1 — `GDExtensionAndroidPlugin.mwaDeauthorize` coverage (TDD-RED at T1;
 * T2 turns these GREEN by replacing the stub body with the full impl per
 * DD-4-1-1..DD-4-1-6).
 *
 * 5 scenarios:
 *   1. AC-1 happy path — multi-key wipe per identityUri (DD-4-1-6); both flags
 *      true; unrelated CacheRecord under a different identityUri is preserved.
 *   2. AC-2 network-off — remote `MwaResult.Failure(NetworkOffline)`; wipe still
 *      runs; warning="remote_unreachable".
 *   3. AC-3 wallet-uninstalled — same code path as AC-2 via `MwaResult.Failure
 *      (WalletUninstalled)`; the test split exists to prove the classification
 *      doesn't accidentally upgrade NoWalletFound to a non-recoverable error.
 *   4. AC-4 idempotent — second call after wipe is a vacuous success per
 *      DD-4-1-4 (no remote, no deleteToken); zero state mutation.
 *   5. DD-4-1-3 evidence (NOT an AC; internal-correctness invariant) — wipe
 *      crash routes to `mwa_error PROTOCOL_ERROR`; flag-based post-finally
 *      branch skips `deauthorize_completed` emission (terminal-signal invariant).
 *
 * Plugin is built via the `@VisibleForTesting` ctor with injected collaborators
 * (identical pattern to [MwaAndroidPluginSignMessagesTest]):
 *   - recording [NativeBridge] mock
 *   - map-backed fake [SecureTokenStore] via `storeProvider`
 *   - FakeMwaClient (success / remote_unreachable / wallet_uninstalled
 *     scenarios) or a raw mock via `mwaClientFactory`
 *   - `senderProvider` returns a relaxed `ActivityResultSender` mock
 *
 * Per DD-4-1-1, remote-revoke failures emit `deauthorize_completed{
 * remote_revoke_succeeded:false, warning:"remote_unreachable"}` rather than
 * `mwa_error`. Per DD-4-1-3, wipe-crash is the ONLY path that emits
 * `mwa_error`; the post-finally branch reads the `wipeCrashed` flag to skip
 * `deauthorize_completed` so terminal-signal invariance holds.
 */
class MwaAndroidPluginDeauthorizeTest {

    private val fixturesDir: File by lazy {
        val prop = System.getProperty("mwa.fixtures.dir")
            ?: error("system property 'mwa.fixtures.dir' must be set (wired in build.gradle.kts)")
        File(prop).also { require(it.isDirectory) { "fixtures dir missing: $it" } }
    }

    private lateinit var nativeBridge: NativeBridge
    private lateinit var senderMock: ActivityResultSender
    private lateinit var storedRecords: MutableMap<CacheKey, CacheRecord>
    private lateinit var store: SecureTokenStore

    // Caller identity preserved across the test class. The DD-4-1-6 listAllKeys
    // filter keys on `identityUri` — case 1 seeds 3 records with TWO matching
    // identityUri (cluster=devnet + cluster=mainnet) and ONE unrelated identityUri
    // to prove the filter is precise.
    private val identityUri = "https://example.com/godot-mwa-sample"
    private val identityName = "Test Game"
    private val iconUri = "https://example.com/icon.png"
    private val unrelatedIdentityUri = "https://other.example/different-app"

    private val walletPackageA = "com.wallet.phantom"
    private val walletLabelA = "Phantom"
    private val walletIconUriA = "https://phantom.app/icon.png"
    private val publicKeyA = "AbcDefGhiJkl1234567890AbcDefGhiJkl1234567890"

    // FakeMwaClient `deauthorize_success.json` fixture's `expected_input.auth_token`
    // (parseUnitResponse does NOT validate input, but matching the fixture's
    // declared bytes is cheaper than diverging).
    private val authTokenBytes = "dGVzdC1hdXRoLXRva2VuLWRlYXV0aA==".toByteArray(Charsets.UTF_8)
    private val salt = ByteArray(32) { 0x01 }

    @BeforeEach
    fun setup() {
        // Companion init touches android.util.Log; mock before first touch.
        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0

        // ConnectionIdentity construction goes through Uri.parse(...) per DD-3-1-8 /
        // Story 3-1 inheritance — mock to a relaxed mock so JVM-side construction
        // succeeds without an Android runtime.
        mockkStatic(Uri::class)
        every { Uri.parse(any()) } returns mockk(relaxed = true)

        nativeBridge = mockk(relaxed = true)
        senderMock = mockk(relaxed = true)

        storedRecords = mutableMapOf()
        store = mockk(relaxed = true) {
            every { putToken(any(), any()) } answers {
                val key: CacheKey = firstArg()
                val record: CacheRecord = secondArg()
                storedRecords[key] = record
            }
            every { getToken(any()) } answers {
                val key: CacheKey = firstArg()
                storedRecords[key]
            }
            every { deleteToken(any()) } answers {
                val key: CacheKey = firstArg()
                storedRecords.remove(key)
                Unit
            }
            every { listAllKeys() } answers { storedRecords.keys.toList() }
            every { getOrCreatePerInstallSalt() } returns salt
        }

        GDExtensionAndroidPlugin.sessionState.clear()
    }

    @AfterEach
    fun teardown() {
        unmockkStatic(Log::class)
        unmockkStatic(Uri::class)
    }

    private fun buildPlugin(
        clientFactory: () -> MwaClient = {
            error("unexpected mwaClientFactory invocation")
        },
        inflightOverride: InflightMap = InflightMap(),
        storeOverride: SecureTokenStore = store,
    ): GDExtensionAndroidPlugin {
        val godot = mockk<Godot>(relaxed = true)
        val scope = CoroutineScope(SupervisorJob() + Dispatchers.Default)
        return GDExtensionAndroidPlugin(
            godot = godot,
            mwaClientFactory = clientFactory,
            storeProvider = { storeOverride },
            nativeBridge = nativeBridge,
            senderProvider = { senderMock },
            scope = scope,
            clock = { 1_700_000_000_000L },
            inflightMap = inflightOverride,
            diagnostics = MwaDiagnostics(),
            // Tests run without an Android main looper — Unconfined runs the
            // body in the caller's thread.
            mainDispatcher = Dispatchers.Unconfined,
        )
    }

    /** Polls [check] every 5ms up to [timeoutMs] ms; errors otherwise. */
    private fun awaitCondition(timeoutMs: Long, check: () -> Boolean) {
        val deadline = System.currentTimeMillis() + timeoutMs
        while (System.currentTimeMillis() < deadline) {
            if (check()) return
            Thread.sleep(5)
        }
        error("condition not met within ${timeoutMs}ms")
    }

    /**
     * Seeds [GDExtensionAndroidPlugin.sessionState] with a connected snapshot.
     * The COMBINED `setIdentity(identityUri, iconUri, identityName)` setter at
     * `MwaSessionState.kt:136` (NOT three separate setters) is the only way to
     * populate identity fields per DD-3-1-8 inheritance.
     */
    private fun seedConnectedSession() {
        GDExtensionAndroidPlugin.sessionState.apply {
            setIdentity(identityUri, iconUri, identityName)
            setCluster(2)
            setClusterName("devnet")
            setAuthToken(SecretString(authTokenBytes.copyOf()))
            setAuthTokenFingerprint("00000000")
            setWalletIconUri(walletIconUriA)
            setPublicKeyBase58(publicKeyA)
            setWalletLabel(walletLabelA)
            setKey(ByteArray(32) { 0x42 })
        }
    }

    /**
     * Seeds the fake store with a CacheRecord. Used in case 1 to assemble a
     * 3-record fixture (2 matching + 1 unrelated identityUri) to prove
     * DD-4-1-6's listAllKeys filter is precise.
     */
    private fun seedCacheRecord(
        cluster: String,
        chainId: String,
        walletPackage: String,
        identityUri: String,
        publicKey: String = publicKeyA,
    ): CacheKey {
        val key = CacheKey(cluster, chainId, walletPackage, identityUri)
        val record = CacheRecord(
            schema = 1,
            authToken = SecretString(authTokenBytes.copyOf()),
            walletPackage = walletPackage,
            walletLabel = walletLabelA,
            walletIconUri = walletIconUriA,
            publicKey = publicKey,
            cluster = cluster,
            chainId = chainId,
            identityUri = identityUri,
            issuedAtMs = 1_700_000_000_000L,
            lastUsedAtMs = 1_700_000_000_000L,
        )
        store.putToken(key, record)
        return key
    }

    // ---------------- AC-1 ----------------

    @Test
    fun `deauthorize happy path emits both flags true and wipes all matching CacheKeys for the identity`() {
        // Pre-seed 3 records: 2 under the SAME identityUri (devnet + mainnet), 1
        // under a DIFFERENT identityUri to prove the listAllKeys filter is precise.
        val matchKey1 = seedCacheRecord(
            cluster = "devnet",
            chainId = "solana:devnet",
            walletPackage = walletPackageA,
            identityUri = identityUri,
        )
        val matchKey2 = seedCacheRecord(
            cluster = "mainnet",
            chainId = "solana:mainnet",
            walletPackage = walletPackageA,
            identityUri = identityUri,
        )
        val unrelatedKey = seedCacheRecord(
            cluster = "devnet",
            chainId = "solana:devnet",
            walletPackage = "com.wallet.other",
            identityUri = unrelatedIdentityUri,
        )

        seedConnectedSession()

        val plugin = buildPlugin(
            clientFactory = { FakeMwaClient(fixturesDir).withScenario("success") },
        )

        val jsonSlot = slot<String>()
        plugin.mwaDeauthorize("req-ac1")

        awaitCondition(3000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postDeauthorizeCompleted("req-ac1", capture(jsonSlot)) }
            }.isSuccess
        }

        // (a) Result payload — 4-key shape per arch.md:669 + DD-4-1-1.
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-ac1", payload.getString("request_id"))
        assertTrue(payload.getBoolean("remote_revoke_succeeded"), "remote_revoke_succeeded == true")
        assertTrue(payload.getBoolean("local_cache_cleared"), "local_cache_cleared == true")
        assertEquals("", payload.getString("warning"), "warning empty on happy path")

        // (b) Multi-key wipe per DD-4-1-6: BOTH same-identity records are gone.
        assertNull(store.getToken(matchKey1), "devnet record under target identityUri wiped")
        assertNull(store.getToken(matchKey2), "mainnet record under target identityUri wiped")

        // (c) Filter precision: unrelated record under a different identityUri is PRESERVED.
        assertNotNull(store.getToken(unrelatedKey), "unrelated record under different identityUri preserved")

        // (d) Full sessionState clear (NOT clearOnLogout) — DD-4-1-2.
        assertNull(GDExtensionAndroidPlugin.sessionState.getAuthToken(), "authToken")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getIdentityUri(), "identityUri wiped")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getIconUri(), "iconUri wiped")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getIdentityName(), "identityName wiped")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(), "publicKeyBase58 wiped")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getClusterName(), "clusterName wiped")
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getWalletLabel(), "walletLabel wiped")

        // (e) Terminal-signal invariant — exactly one terminal signal.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postConnectCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postDisconnectCompleted(any(), any()) }
    }

    // ---------------- AC-2 ----------------

    @Test
    fun `deauthorize with remote-revoke failure still wipes records and emits remote_unreachable warning`() {
        val matchKey1 = seedCacheRecord(
            cluster = "devnet",
            chainId = "solana:devnet",
            walletPackage = walletPackageA,
            identityUri = identityUri,
        )
        val matchKey2 = seedCacheRecord(
            cluster = "mainnet",
            chainId = "solana:mainnet",
            walletPackage = walletPackageA,
            identityUri = identityUri,
        )
        seedConnectedSession()

        val plugin = buildPlugin(
            clientFactory = {
                FakeMwaClient(fixturesDir).withScenario("remote_unreachable")
            },
        )

        val jsonSlot = slot<String>()
        plugin.mwaDeauthorize("req-ac2")

        awaitCondition(3000L) {
            runCatching {
                verify(exactly = 1) {
                    nativeBridge.postDeauthorizeCompleted("req-ac2", capture(jsonSlot))
                }
            }.isSuccess
        }

        // (a) Payload — `remote_revoke_succeeded:false` + warning.
        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-ac2", payload.getString("request_id"))
        assertFalse(
            payload.getBoolean("remote_revoke_succeeded"),
            "remote_revoke_succeeded == false on network failure",
        )
        assertTrue(
            payload.getBoolean("local_cache_cleared"),
            "local_cache_cleared == true regardless of remote outcome (AC-NFR-SEC-5)",
        )
        assertEquals(
            "remote_unreachable",
            payload.getString("warning"),
            "warning == remote_unreachable on remote failure",
        )

        // (b) Wipe still runs — both matching records gone (AC-NFR-SEC-5).
        assertNull(store.getToken(matchKey1), "devnet record wiped despite remote failure")
        assertNull(store.getToken(matchKey2), "mainnet record wiped despite remote failure")

        // (c) AC-2 spec: remote failure routes to deauthorize_completed, NOT mwa_error.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
    }

    // ---------------- AC-3 ----------------

    @Test
    fun `deauthorize with WalletUninstalled classification still wipes records and emits remote_unreachable warning`() {
        val matchKey = seedCacheRecord(
            cluster = "devnet",
            chainId = "solana:devnet",
            walletPackage = walletPackageA,
            identityUri = identityUri,
        )
        seedConnectedSession()

        val plugin = buildPlugin(
            clientFactory = {
                FakeMwaClient(fixturesDir).withScenario("wallet_uninstalled")
            },
        )

        val jsonSlot = slot<String>()
        plugin.mwaDeauthorize("req-ac3")

        awaitCondition(3000L) {
            runCatching {
                verify(exactly = 1) {
                    nativeBridge.postDeauthorizeCompleted("req-ac3", capture(jsonSlot))
                }
            }.isSuccess
        }

        val payload = JSONObject(jsonSlot.captured)
        assertEquals("req-ac3", payload.getString("request_id"))
        assertFalse(
            payload.getBoolean("remote_revoke_succeeded"),
            "WalletUninstalled classification surfaces as remote_revoke_succeeded:false",
        )
        assertTrue(
            payload.getBoolean("local_cache_cleared"),
            "local_cache_cleared == true regardless of wallet-uninstalled state",
        )
        assertEquals(
            "remote_unreachable",
            payload.getString("warning"),
            "warning == remote_unreachable for WalletUninstalled (DD-4-1-1: any non-Success → remote_unreachable)",
        )

        assertNull(store.getToken(matchKey), "record wiped despite wallet-uninstalled state")

        // AC-3 spec: same code path as AC-2 — never routes to mwa_error.
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }
    }

    // ---------------- AC-4 ----------------

    @Test
    fun `second deauthorize after wipe emits remote_revoke_succeeded=true vacuously with zero state mutation`() {
        // First call: full happy path. Use a recording mock instead of FakeMwaClient
        // so we can verify the deauthorize() suspend fun was invoked exactly once
        // across both calls (confirms second call took the DD-4-1-4 idempotent
        // skip-remote path).
        val mockClient = mockk<MwaClient>(relaxed = true)
        coEvery {
            mockClient.deauthorize(any(), any(), any())
        } returns MwaResult.Success(Unit)

        seedCacheRecord(
            cluster = "devnet",
            chainId = "solana:devnet",
            walletPackage = walletPackageA,
            identityUri = identityUri,
        )
        seedConnectedSession()

        val plugin = buildPlugin(clientFactory = { mockClient })

        plugin.mwaDeauthorize("req-ac4-first")
        awaitCondition(3000L) {
            runCatching {
                verify(exactly = 1) {
                    nativeBridge.postDeauthorizeCompleted("req-ac4-first", any())
                }
            }.isSuccess
        }

        // After first call: sessionState fully cleared, store.listAllKeys()
        // returns empty (the seeded record was wiped under the matching identity).
        assertNull(GDExtensionAndroidPlugin.sessionState.getAuthToken())
        assertEquals("", GDExtensionAndroidPlugin.sessionState.getIdentityUri())
        assertTrue(storedRecords.isEmpty(), "all records gone after first deauth")

        // Second call: idempotent path per DD-4-1-4 (authTokenForRevoke == null
        // AND identityUri.isEmpty()). The impl skips the remote-revoke and the
        // listAllKeys filter is empty → wipe loop is a no-op.
        plugin.mwaDeauthorize("req-ac4-second")
        awaitCondition(3000L) {
            runCatching {
                verify(exactly = 1) {
                    nativeBridge.postDeauthorizeCompleted("req-ac4-second", any())
                }
            }.isSuccess
        }

        // (a) Total deauthorize_completed emissions == 2 (once per call), ZERO
        // mwa_error emissions across both calls.
        verify(exactly = 2) { nativeBridge.postDeauthorizeCompleted(any(), any()) }
        verify(exactly = 0) { nativeBridge.postMwaError(any()) }

        // (b) DD-4-1-4 idempotent path: remote `mwaClient.deauthorize` called
        // EXACTLY ONCE across both calls (only on the first call, when the token
        // existed). The second call short-circuited on the null-token branch.
        coVerify(exactly = 1) {
            mockClient.deauthorize(any(), any<ConnectionIdentity>(), any<SecretString>())
        }

        // (c) Second call payload — vacuous success (DD-4-1-4 spec).
        val secondJson = slot<String>()
        verify(exactly = 1) {
            nativeBridge.postDeauthorizeCompleted("req-ac4-second", capture(secondJson))
        }
        val payload = JSONObject(secondJson.captured)
        assertTrue(
            payload.getBoolean("remote_revoke_succeeded"),
            "vacuous success: nothing to revoke ⇒ remote_revoke_succeeded:true",
        )
        assertTrue(
            payload.getBoolean("local_cache_cleared"),
            "local_cache_cleared:true (no-op clear is still 'cleared')",
        )
        assertEquals(
            "",
            payload.getString("warning"),
            "warning empty on vacuous success (no remote attempted)",
        )

        // (d) Spec-mandated assertion (review finding #3): sessionState
        // remains fully cleared after the second call. `clear()` on
        // already-empty state is a no-op, but this anchors the invariant.
        assertNull(
            GDExtensionAndroidPlugin.sessionState.getAuthToken(),
            "authToken still null after second deauthorize",
        )
        assertEquals(
            "",
            GDExtensionAndroidPlugin.sessionState.getIdentityUri(),
            "identityUri still empty after second deauthorize",
        )
        assertEquals(
            "",
            GDExtensionAndroidPlugin.sessionState.getIdentityName(),
            "identityName still empty after second deauthorize",
        )
        assertEquals(
            "",
            GDExtensionAndroidPlugin.sessionState.getPublicKeyBase58(),
            "publicKeyBase58 still empty after second deauthorize",
        )

        // (e) Idempotence is at the listAllKeys-filter level, not the
        // deleteToken-call level: zero deleteToken invocations on the second
        // call (storedRecords was already empty from the first call's wipe).
        // The post-mockk-clear count would require `clearMocks` or a
        // sub-spy; instead we rely on (b) above — if remote was skipped,
        // wipe-on-empty-listAllKeys is implied.
    }

    // ---------------- DD-4-1-3 evidence (NOT an AC) ----------------

    @Test
    fun `deauthorize with wipe crash routes to mwa_error PROTOCOL_ERROR and skips deauthorize_completed`() {
        // Spy: listAllKeys returns one fake key; deleteToken throws on that key.
        // The wipe is INSIDE the outer finally block (DD-4-1-3) so the
        // remote-revoke succeeds first, then the wipe attempt detonates.
        val crashingStore = mockk<SecureTokenStore>(relaxed = true) {
            every { putToken(any(), any()) } answers {
                val key: CacheKey = firstArg()
                val record: CacheRecord = secondArg()
                storedRecords[key] = record
            }
            every { getToken(any()) } answers {
                val key: CacheKey = firstArg()
                storedRecords[key]
            }
            every { listAllKeys() } returns listOf(
                CacheKey("devnet", "solana:devnet", walletPackageA, identityUri),
            )
            every { deleteToken(any()) } throws IllegalStateException("corrupt prefs")
            every { getOrCreatePerInstallSalt() } returns salt
        }

        seedConnectedSession()

        val mockClient = mockk<MwaClient>(relaxed = true)
        coEvery {
            mockClient.deauthorize(any(), any(), any())
        } returns MwaResult.Success(Unit)

        val plugin = buildPlugin(
            clientFactory = { mockClient },
            storeOverride = crashingStore,
        )

        val errorSlot = slot<String>()
        plugin.mwaDeauthorize("req-wipecrash")

        awaitCondition(3000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(errorSlot)) }
            }.isSuccess
        }

        // (a) mwa_error 10-key envelope per A-14 + DD-4-1-3 contract.
        val err = JSONObject(errorSlot.captured)
        assertEquals("req-wipecrash", err.getString("request_id"))
        assertEquals("PROTOCOL_ERROR", err.getString("code"))
        assertEquals("kotlin", err.getString("layer"))
        assertEquals("deauthorize", err.getString("source_method"))
        // developer_details should describe the wipe crash; do not assert exact
        // text (impl detail) — assert it mentions the failure mode.
        val devDetails = err.getString("developer_details")
        assertTrue(
            devDetails.contains("wipe", ignoreCase = true) ||
                devDetails.contains("IllegalStateException", ignoreCase = true) ||
                devDetails.contains("corrupt", ignoreCase = true),
            "developer_details mentions wipe crash; got: $devDetails",
        )

        // (b) Terminal-signal invariant — wipe-crash branch MUST NOT also emit
        // deauthorize_completed (the flag-based post-finally branch reads
        // wipeCrashed and skips). This is the DD-4-1-3 evidence assertion.
        verify(exactly = 0) { nativeBridge.postDeauthorizeCompleted(any(), any()) }

        // (c) Likewise no other terminal signal.
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postMwaCancelledLifecycle(any()) }
    }

    // ---------------- DD-4-1-3 evidence (idempotent-path wipe-crash) ----------------

    @Test
    fun `idempotent path with wipe crash also routes to mwa_error PROTOCOL_ERROR`() {
        // Symmetric coverage with case 5: DD-4-1-3's defensive `try { wipe }
        // catch { wipeCrashed=true; mwa_error }` exists in BOTH the idempotent
        // path (lines 979-1002 of GDExtensionAndroidPlugin.kt) and the
        // else-branch finally (lines 1038-1067). Case 5 covers the else-branch;
        // this case covers the idempotent path, so a regression that breaks
        // just one of the two catches (e.g. forgets `wipeCrashed = true` in
        // the idempotent branch) is caught — preventing a phantom
        // `deauthorize_completed` + `mwa_error` double-emission that would
        // violate the terminal-signal invariant.
        //
        // Realistic state setup: post-disconnect (Story 2-3 path), authToken
        // is null but identityUri is preserved. If the user calls deauth on
        // this state, the idempotent path activates (DD-4-1-4: authToken==null
        // skip-remote). The wipe loop runs because identityUri is non-empty
        // and listAllKeys returns matching records. Throwing on the first
        // deleteToken triggers the inner catch.
        val crashingStore = mockk<SecureTokenStore>(relaxed = true) {
            every { putToken(any(), any()) } answers {
                val key: CacheKey = firstArg()
                val record: CacheRecord = secondArg()
                storedRecords[key] = record
            }
            every { getToken(any()) } answers {
                val key: CacheKey = firstArg()
                storedRecords[key]
            }
            every { listAllKeys() } returns listOf(
                CacheKey("devnet", "solana:devnet", walletPackageA, identityUri),
            )
            every { deleteToken(any()) } throws IllegalStateException("corrupt prefs")
            every { getOrCreatePerInstallSalt() } returns salt
        }

        // Post-disconnect state: identity preserved, auth-only fields wiped.
        // Mirrors the result of `clearOnLogout()` from Story 2-3's mwaDisconnect.
        GDExtensionAndroidPlugin.sessionState.apply {
            setIdentity(identityUri, iconUri, identityName)
            setCluster(2)
            // Note: NO setAuthToken(...) — explicitly leaving authToken null
            // to trigger the DD-4-1-4 idempotent fork.
        }

        val plugin = buildPlugin(
            clientFactory = {
                error("DD-4-1-4 violation: idempotent path invoked mwaClientFactory")
            },
            storeOverride = crashingStore,
        )

        val errorSlot = slot<String>()
        plugin.mwaDeauthorize("req-idempotent-wipecrash")

        awaitCondition(3000L) {
            runCatching {
                verify(exactly = 1) { nativeBridge.postMwaError(capture(errorSlot)) }
            }.isSuccess
        }

        // (a) mwa_error envelope — same shape as case 5's else-branch crash.
        val err = JSONObject(errorSlot.captured)
        assertEquals("req-idempotent-wipecrash", err.getString("request_id"))
        assertEquals("PROTOCOL_ERROR", err.getString("code"))
        assertEquals("deauthorize", err.getString("source_method"))
        val devDetails = err.getString("developer_details")
        assertTrue(
            devDetails.contains("wipe", ignoreCase = true) ||
                devDetails.contains("IllegalStateException", ignoreCase = true) ||
                devDetails.contains("corrupt", ignoreCase = true),
            "developer_details mentions wipe crash; got: $devDetails",
        )

        // (b) Terminal-signal invariant — idempotent-path wipe-crash MUST NOT
        // also emit deauthorize_completed (the wipeCrashed flag-based
        // post-finally branch reads the flag and skips). This is the
        // load-bearing assertion: if a regression forgets to set
        // `wipeCrashed = true` in the idempotent branch, this test fails
        // because both `mwa_error` AND `deauthorize_completed` would fire.
        verify(exactly = 0) { nativeBridge.postDeauthorizeCompleted(any(), any()) }

        // (c) No other terminal signal.
        verify(exactly = 0) { nativeBridge.postMwaTimeout(any()) }
        verify(exactly = 0) { nativeBridge.postMwaCancelledLifecycle(any()) }
    }
}
