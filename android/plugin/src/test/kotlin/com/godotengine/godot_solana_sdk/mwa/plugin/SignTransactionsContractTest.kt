package com.godotengine.godot_solana_sdk.mwa.plugin

import android.util.Log
import com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
import com.godotengine.godot_solana_sdk.mwa.client.MwaResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignResult
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
import org.junit.jupiter.api.Assertions.assertArrayEquals
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
 * Story 3-2 T6 — `sign_transactions` contract test (AC-5).
 *
 * **Scope (per Story 3-1 D-3-1-13 Rule 3 inheritance — see docs/concerns.md
 * and docs/stories/3-2.md T6 description).**
 *
 * Inherits the Story 3-1 SignMessagesContractTest re-scope verbatim:
 * `FakeMwaClient` does NOT construct a JSON-RPC frame at any point — it
 * loads a fixture JSON and returns the parsed canned response (see
 * `FakeMwaClient.signTransactions` at lines 105-110 → `runOp(...) { _,
 * fixture -> parseSignResponse(...) }`). The wire-format layer lives
 * entirely in `clientlib-ktx` (imported by `MwaClientImpl`, NOT by the
 * Fake). The "frame agreement" plan from the original AC-5 wording was
 * retracted in 3-1 (D-3-1-13) and stays retracted here.
 *
 * **What this test asserts (the realistic contract):** that
 * `testdata/mwa-fixtures/sign_transactions_success.json`'s `expected_input`
 * block is internally self-consistent against what
 * `FakeMwaClient.signTransactions` actually consumes — and that calling
 * the Fake with those inputs produces a `MwaResult.Success` whose
 * `signedPayloads` byte-match the fixture's `response.payload.signed_payloads`
 * after base64 round-trip.
 *
 * Concretely, this verifies four things at once:
 *   1. **Schema integrity** — `expected_input` carries the TWO keys
 *      (`auth_token`, `transactions`) with correct shapes (string, JSON
 *      array of strings). NOTE: 2 keys, NOT 3 — `addresses` is NOT in
 *      the sign_transactions input per the
 *      `MwaClient.signTransactions(sender, identity, authToken, transactions)`
 *      4-arg signature (`MwaClient.kt:111-116`). The unsigned-transactions
 *      path doesn't need a public-key list because each transaction's
 *      signer set is encoded inside the transaction itself.
 *   2. **Base64 well-formedness** — `transactions[]` decode to non-empty
 *      `ByteArray` (caller-side wire format per Story 1-6 DD-2). Catches
 *      base64 corruption / formatting accidents.
 *   3. **Opaque-string auth_token** — `expected_input.auth_token` is
 *      treated as an opaque UTF-8 string per Story 1-6 DD-2 (NOT
 *      base64-decoded), then wrapped in `SecretString` for the call.
 *      Mirrors `MwaClientImpl.signTransactions`'s production behavior.
 *   4. **Fake honors the canned response** — `FakeMwaClient.signTransactions(...)`
 *      called with the inputs decoded from `expected_input` returns
 *      `MwaResult.Success(SignResult(signedPayloads))` where
 *      `signedPayloads[i]` byte-equals
 *      `Base64.decode(response.payload.signed_payloads[i])`. Catches a
 *      regression where `parseSignResponse` mishandles the response shape
 *      or where a `sign_transactions`-specific code path drifts from the
 *      shared `parseSignResponse` helper.
 *
 * **Op-agnostic response key (DD-3-2-3 nuance):** The fixture's response
 * key is `signed_payloads` (op-agnostic — `parseSignResponse` is shared
 * across `sign_messages` / `sign_transactions` / `sign_and_send`). The
 * Kotlin plugin layer renames the array's JSON key to `signed_transactions`
 * in `buildSignSuccessJson(payloadKey="signed_transactions")` per DD-3-2-3
 * — but THAT renaming is the GDScript-facing layer, not the Fake's
 * contract surface. This test stays at the Fake's contract surface
 * (`MwaResult<SignResult>` with the byte payloads), so the assertion
 * uses the fixture's `signed_payloads` key directly.
 *
 * **Real-wallet wire-format invariant** (each signed payload corresponds
 * to a serialized Solana transaction with its signature(s) populated) is
 * verifiable only against a real wallet — the
 * `sign_transactions_success.json` fixture carries 13-byte placeholder
 * strings ("tx-one-signed"/"tx-two-signed") and the FakeMwaClient is
 * intentionally agnostic to the byte-length / signature-validity shape.
 * That invariant is the manual release-gate's territory (run against
 * Phantom/Solflare during Story 3-2 close-out window).
 *
 * The `mwa.fixtures.dir` system property is set by
 * `android/plugin/build.gradle.kts` (`testOptions.unitTests.all`) per
 * Story 1-6 DD-6 — same wiring as `FakeMwaClientTest` and
 * `SignMessagesContractTest`. `android.util.Log` is mocked because
 * `FakeMwaClient.runOp` calls `SdkLog.d` which calls `android.util.Log.d`
 * — not present on the JVM unit-test classpath. `@Execution(SAME_THREAD)`
 * serializes JVM-global static state.
 *
 * @see com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
 * @see com.godotengine.godot_solana_sdk.mwa.client.dto.SignResult
 * @see SignMessagesContractTest the Story 3-1 sibling under the same
 *      D-3-1-13 re-scope.
 */
@Execution(ExecutionMode.SAME_THREAD)
class SignTransactionsContractTest {

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
    fun `sign_transactions_success fixture inputs round-trip through FakeMwaClient to expected response`() {
        // 1. Read fixture --------------------------------------------------------
        val fixtureFile = File(fixtureDir, "sign_transactions_success.json")
        assertTrue(
            fixtureFile.isFile,
            "sign_transactions_success.json must exist at $fixtureFile (Story 1-6 fixture set)",
        )
        val fixture = JSONObject(fixtureFile.readText(Charsets.UTF_8))

        // 2. Schema integrity — required keys + types ----------------------------
        // expected_input is the contract: what FakeMwaClient (and its real
        // counterpart MwaClientImpl) consumes when called against this scenario.
        // Note: 2 keys (auth_token + transactions), NOT 3 — `addresses` is
        // sign_messages-specific and is absent from the sign_transactions input
        // per MwaClient.signTransactions's 4-arg signature.
        val expectedInput = fixture.getJSONObject("expected_input")
        val response = fixture.getJSONObject("response")
        assertEquals(
            "success",
            response.getString("type"),
            "fixture response.type must be 'success' for the happy-path contract test",
        )
        val payload = response.getJSONObject("payload")
        val signedPayloadsExpected = payload.getJSONArray("signed_payloads")

        val authTokenStr = expectedInput.getString("auth_token")
        val transactionsArr = expectedInput.getJSONArray("transactions")

        assertTrue(authTokenStr.isNotEmpty(), "expected_input.auth_token must be non-empty")
        assertTrue(transactionsArr.length() > 0, "expected_input.transactions must be non-empty")
        assertTrue(
            signedPayloadsExpected.length() > 0,
            "response.payload.signed_payloads must be non-empty",
        )
        assertTrue(
            !expectedInput.has("addresses"),
            "expected_input must NOT have 'addresses' (sign_messages-specific; sign_transactions's " +
                "4-arg signature does not pass addresses — DD-3-2-3 + MwaClient.kt:111-116). A regression " +
                "that re-added the key would indicate a fixture-schema drift toward sign_messages's shape.",
        )

        // 3. Base64 well-formedness + decode ------------------------------------
        // transactions[] are base64-encoded serialized-transaction byte arrays
        // per the Story 1-6 fixture convention. auth_token is intentionally
        // NOT base64-decoded (DD-2 opaque-string invariant).
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
        // MwaClientImpl.signTransactions wraps the cached auth_token bytes the
        // same way (string → UTF-8 bytes → SecretString). Production caller
        // behavior is faithfully mirrored.
        val authToken = SecretString(authTokenStr.toByteArray(Charsets.UTF_8))

        // 5. Build FakeMwaClient with the success scenario ----------------------
        val client = FakeMwaClient(fixtureDir).withScenario("success")

        // 6. Invoke FakeMwaClient with the EXACT inputs the fixture declares ----
        // 4-arg call (no addresses) per MwaClient.signTransactions:111-116.
        val result = runBlocking {
            client.signTransactions(sender, identity, authToken, decodedTransactions)
        }

        // 7. Assert Fake honors the canned response shape -----------------------
        assertTrue(
            result is MwaResult.Success<*>,
            "FakeMwaClient.signTransactions must return Success for the success scenario " +
                "(got $result). Indicates parseSignResponse regression or scenario-load issue.",
        )
        @Suppress("UNCHECKED_CAST")
        val signResult: SignResult = (result as MwaResult.Success<SignResult>).value
        assertNotNull(signResult, "MwaResult.Success.value must be a SignResult")

        // signedPayloads count must match response.payload.signed_payloads.length()
        // — proves parseSignResponse iterates the JSONArray completely and
        // doesn't drop or duplicate entries (op-agnostic helper shared with
        // sign_messages and sign_and_send).
        assertEquals(
            signedPayloadsExpected.length(),
            signResult.signedPayloads.size,
            "signResult.signedPayloads.size must equal response.payload.signed_payloads.length() " +
                "(parseSignResponse iteration contract)",
        )

        // Each signed_payload byte-equals the base64-decoded fixture entry —
        // proves parseSignResponse decodes correctly and preserves byte order.
        // Divergence here would indicate a sign_transactions-specific decoder
        // path drifted from the shared parseSignResponse (or fixture corruption).
        signResult.signedPayloads.forEachIndexed { i, bytes ->
            val expected = Base64.getDecoder().decode(signedPayloadsExpected.getString(i))
            assertArrayEquals(
                expected,
                bytes,
                "signResult.signedPayloads[$i] must equal " +
                    "Base64.decode(response.payload.signed_payloads[$i]) byte-for-byte. " +
                    "Divergence indicates parseSignResponse regression OR fixture corruption.",
            )
        }
    }
}
