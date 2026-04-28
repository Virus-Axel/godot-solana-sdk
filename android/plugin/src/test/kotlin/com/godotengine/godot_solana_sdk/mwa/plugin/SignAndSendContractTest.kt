package com.godotengine.godot_solana_sdk.mwa.plugin

import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignAndSendResult
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import io.mockk.every
import io.mockk.mockk
import io.mockk.mockkStatic
import io.mockk.unmockkStatic
import kotlinx.coroutines.runBlocking
import org.json.JSONObject
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import org.junit.jupiter.api.parallel.Execution
import org.junit.jupiter.api.parallel.ExecutionMode
import java.io.File
import java.util.Base64

/**
 * Story 3-3 T6 — `sign_and_send` contract test (AC-7).
 *
 * **Scope (per Story 3-1 D-3-1-13 Rule 3 inheritance — see docs/concerns.md
 * and docs/stories/3-3.md T6 description).**
 *
 * Inherits the Story 3-1 SignMessagesContractTest re-scope verbatim:
 * `FakeMwaClient` does NOT construct a JSON-RPC frame at any point — it
 * loads a fixture JSON and returns the parsed canned response (see
 * `FakeMwaClient.signAndSendTransactions` at lines 112-118 → `runOp(...) {
 * _, fixture -> parseSignAndSendResponse(...) }`). The wire-format layer
 * lives entirely in `clientlib-ktx` (imported by `MwaClientImpl`, NOT by
 * the Fake). The "frame agreement" plan from the original AC-7 wording
 * was retracted in 3-1 (D-3-1-13) and stays retracted here.
 *
 * **What this test asserts (the realistic contract):** that
 * `testdata/mwa-fixtures/sign_and_send_success.json`'s `expected_input`
 * block is internally self-consistent against what
 * `FakeMwaClient.signAndSendTransactions` actually consumes — and that
 * calling the Fake with those inputs produces a `MwaResult.Success` whose
 * `signatures` string-equal the fixture's `response.payload.signatures`
 * (NOTE: base58 strings, NOT base64 byte arrays — DD-3-3-E; the
 * assertion uses string equality on the base58 list, NOT byte-array
 * equality).
 *
 * Concretely, this verifies four things at once:
 *   1. **Schema integrity** — `expected_input` carries the THREE keys
 *      (`auth_token`, `transactions`, `cluster`) with correct shapes
 *      (string, JSON array of strings, string). NOTE: 3 keys, NOT 2 like
 *      sign_transactions — `cluster` is the third key per the
 *      `MwaClient.signAndSendTransactions(sender, identity, authToken,
 *      transactions, cluster)` 5-arg signature (`MwaClient.kt:131-137`).
 *      The wallet uses `cluster` to select the RPC endpoint for
 *      submission; the signing path family at large does not need this
 *      arg, but `sign_and_send` does because it goes BEYOND signing into
 *      the cluster-submission step.
 *   2. **Base64 well-formedness** — `transactions[]` decode to non-empty
 *      `ByteArray` (caller-side wire format per Story 1-6 DD-2). Catches
 *      base64 corruption / formatting accidents.
 *   3. **Opaque-string auth_token** — `expected_input.auth_token` is
 *      treated as an opaque UTF-8 string per Story 1-6 DD-2 (NOT
 *      base64-decoded), then wrapped in `SecretString` for the call.
 *      Mirrors `MwaClientImpl.signAndSendTransactions`'s production
 *      behavior.
 *   4. **Fake honors the canned response** —
 *      `FakeMwaClient.signAndSendTransactions(...)` called with the
 *      inputs decoded from `expected_input` returns
 *      `MwaResult.Success(SignAndSendResult(signatures))` where
 *      `signatures[i] == response.payload.signatures.getString(i)`
 *      (string equality on the base58 list — NOT byte-array equality;
 *      Solana tx signatures are base58 strings on the wire, distinct
 *      from the base64-byte-array convention used in `signed_payloads` /
 *      `signed_transactions`). Catches a regression where
 *      `parseSignAndSendResponse` drifts from the fixture-input contract
 *      OR where the FakeMwaClient incorrectly base64-decodes the
 *      base58-encoded signatures.
 *
 * **Distinct response key (DD-3-3-E):** Unlike `sign_messages_success.json`
 * / `sign_transactions_success.json` which use the op-agnostic
 * `signed_payloads` key (consumed by the shared `parseSignResponse`
 * helper), `sign_and_send_success.json` uses `signatures` — a distinct
 * key because the response shape is different (base58 strings vs base64
 * byte arrays). The Fake routes through `parseSignAndSendResponse` (a
 * separate helper from `parseSignResponse`) — this test exercises that
 * helper end-to-end.
 *
 * **Real-wallet wire-format invariant** (each base58 signature
 * corresponds to a real submitted transaction signature on a real
 * cluster) is verifiable only against a real wallet + real RPC — the
 * `sign_and_send_success.json` fixture carries a placeholder base58
 * string ("5CrhasMLvBzCo8...") and the FakeMwaClient is intentionally
 * agnostic to the on-chain validity. That invariant is the manual
 * release-gate's territory (run against Phantom/Solflare during
 * Story 3-3 close-out window, parallel to Story 3-1 / 3-2's
 * procedure).
 *
 * The `mwa.fixtures.dir` system property is set by
 * `android/plugin/build.gradle.kts` (`testOptions.unitTests.all`) per
 * Story 1-6 DD-6 — same wiring as `FakeMwaClientTest` /
 * `SignMessagesContractTest` / `SignTransactionsContractTest`.
 * `android.util.Log` is mocked because `FakeMwaClient.runOp` calls
 * `SdkLog.d` which calls `android.util.Log.d` — not present on the JVM
 * unit-test classpath. `@Execution(SAME_THREAD)` serializes JVM-global
 * static state.
 *
 * @see com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
 * @see com.godotengine.godot_solana_sdk.mwa.client.dto.SignAndSendResult
 * @see SignTransactionsContractTest the Story 3-2 sibling under the same
 *      D-3-1-13 re-scope.
 * @see SignMessagesContractTest the Story 3-1 sibling that landed the
 *      re-scope.
 */
@Execution(ExecutionMode.SAME_THREAD)
class SignAndSendContractTest {

    private lateinit var fixtureDir: File
    private val sender: ActivityResultSender = mockk(relaxed = true)
    private val identity: ConnectionIdentity = mockk(relaxed = true)

    @BeforeEach
    fun setup() {
        val path = System.getProperty("mwa.fixtures.dir")
            ?: error(
                "mwa.fixtures.dir system property is required; see android/plugin/build.gradle.kts " +
                    "testOptions.unitTests.all (Story 1-6 Design Decision 6).",
            )
        fixtureDir = File(path)
        assertTrue(fixtureDir.isDirectory, "fixture dir must exist: $fixtureDir")

        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0
    }

    @AfterEach
    fun teardown() {
        unmockkStatic(Log::class)
    }

    @Test
    fun `fixture-input contract honored by FakeMwaClient`() {
        // 1. Read fixture --------------------------------------------------------
        val fixtureFile = File(fixtureDir, "sign_and_send_success.json")
        assertTrue(
            fixtureFile.isFile,
            "sign_and_send_success.json must exist at $fixtureFile (Story 1-6 fixture set)",
        )
        val fixture = JSONObject(fixtureFile.readText(Charsets.UTF_8))

        // 2. Schema integrity — required keys + types ----------------------------
        // expected_input carries 3 keys (auth_token + transactions + cluster) per
        // the 5-arg MwaClient.signAndSendTransactions signature
        // (MwaClient.kt:131-137). `cluster` is the third key — the wallet uses it
        // to select the RPC endpoint for submission. NOTE: NO `addresses` key
        // (sign_messages-specific) AND NO `chain_id` either; the cluster string
        // is sufficient for the wallet's endpoint selection.
        val expectedInput = fixture.getJSONObject("expected_input")
        val response = fixture.getJSONObject("response")
        assertEquals(
            "success",
            response.getString("type"),
            "fixture response.type must be 'success' for the happy-path contract test",
        )
        val payload = response.getJSONObject("payload")
        val signaturesExpected = payload.getJSONArray("signatures")

        val authTokenStr = expectedInput.getString("auth_token")
        val transactionsArr = expectedInput.getJSONArray("transactions")
        val clusterStr = expectedInput.getString("cluster")

        assertTrue(authTokenStr.isNotEmpty(), "expected_input.auth_token must be non-empty")
        assertTrue(transactionsArr.length() > 0, "expected_input.transactions must be non-empty")
        assertTrue(clusterStr.isNotEmpty(), "expected_input.cluster must be non-empty")
        assertTrue(
            signaturesExpected.length() > 0,
            "response.payload.signatures must be non-empty",
        )
        assertTrue(
            !expectedInput.has("addresses"),
            "expected_input must NOT have 'addresses' (sign_messages-specific; sign_and_send's " +
                "5-arg signature does not pass addresses). A regression that re-added the key " +
                "would indicate a fixture-schema drift toward sign_messages's shape.",
        )
        assertTrue(
            !payload.has("signed_payloads") && !payload.has("signed_transactions"),
            "DD-3-3-E: response.payload must NOT carry signed_payloads or signed_transactions " +
                "(sign_messages / sign_transactions response keys). sign_and_send's distinct " +
                "response shape is `signatures` (base58 strings) — a regression that emitted " +
                "the wire-format byte arrays here would indicate parseSignAndSendResponse " +
                "drift toward parseSignResponse's shape.",
        )

        // 3. Base64 well-formedness + decode ------------------------------------
        // transactions[] are base64-encoded serialized-transaction byte arrays
        // per the Story 1-6 fixture convention. auth_token is intentionally
        // NOT base64-decoded (DD-2 opaque-string invariant). cluster is also
        // NOT base64-decoded — it's a literal cluster name ("devnet", etc.).
        val decodedTransactions = (0 until transactionsArr.length()).map { i ->
            Base64.getDecoder().decode(transactionsArr.getString(i))
        }
        decodedTransactions.forEachIndexed { i, t ->
            assertTrue(
                t.isNotEmpty(),
                "expected_input.transactions[$i] must decode to non-empty bytes",
            )
        }

        // 4. Wrap auth_token as opaque SecretString (DD-2) ----------------------
        // MwaClientImpl.signAndSendTransactions wraps the cached auth_token bytes
        // the same way (string → UTF-8 bytes → SecretString). Production caller
        // behavior is faithfully mirrored.
        val authToken = SecretString(authTokenStr.toByteArray(Charsets.UTF_8))

        // 5. Build FakeMwaClient with the success scenario ----------------------
        val client = FakeMwaClient(fixtureDir).withScenario("success")

        // 6. Invoke FakeMwaClient with the EXACT inputs the fixture declares ----
        // 5-arg call (sender, identity, authToken, transactions, cluster) per
        // MwaClient.signAndSendTransactions:131-137.
        val result = runBlocking {
            client.signAndSendTransactions(sender, identity, authToken, decodedTransactions, clusterStr)
        }

        // 7. Assert Fake honors the canned response shape -----------------------
        assertTrue(
            result is MwaResult.Success<*>,
            "FakeMwaClient.signAndSendTransactions must return Success for the success scenario " +
                "(got $result). Indicates parseSignAndSendResponse regression or scenario-load issue.",
        )
        @Suppress("UNCHECKED_CAST")
        val signAndSendResult: SignAndSendResult = (result as MwaResult.Success<SignAndSendResult>).value
        assertNotNull(signAndSendResult, "MwaResult.Success.value must be a SignAndSendResult")

        // signatures count must match response.payload.signatures.length()
        // — proves parseSignAndSendResponse iterates the JSONArray completely
        // and doesn't drop or duplicate entries.
        assertEquals(
            signaturesExpected.length(),
            signAndSendResult.signatures.size,
            "signAndSendResult.signatures.size must equal response.payload.signatures.length() " +
                "(parseSignAndSendResponse iteration contract)",
        )

        // Each signature STRING-equals the fixture entry — proves
        // parseSignAndSendResponse preserves the base58 strings unchanged
        // (NOT base64-decoded). DD-3-3-E: signatures are base58 strings on
        // the wire; the FakeMwaClient is intentionally a string-pass-through
        // here. Divergence would indicate parseSignAndSendResponse regression
        // (e.g., accidentally base64-decoding a base58 string would corrupt
        // the bytes silently).
        signAndSendResult.signatures.forEachIndexed { i, signature ->
            assertEquals(
                signaturesExpected.getString(i),
                signature,
                "signAndSendResult.signatures[$i] must string-equal " +
                    "response.payload.signatures[$i] (base58 pass-through). " +
                    "Divergence indicates parseSignAndSendResponse regression " +
                    "OR fixture corruption.",
            )
        }
    }
}
