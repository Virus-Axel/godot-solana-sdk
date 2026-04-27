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
 * Story 3-1 T6 — `sign_messages` contract test (AC-5).
 *
 * **Scope (re-scoped per D-3-1-13 Rule 3 — see docs/concerns.md and
 * docs/stories/3-1.md T6 description).**
 *
 * The original Story 3-1 T6 spec (story v1) called for an "agreement"
 * assertion comparing the JSON-RPC frame `FakeMwaClient` constructs against
 * a recorded-real-clientlib frame. Pre-impl audit caught that
 * `FakeMwaClient` does NOT construct a JSON-RPC frame at any point — it
 * loads a fixture JSON and returns the parsed canned response (see
 * `FakeMwaClient.signMessages` at lines 97-103 → `runOp(...) { _, fixture ->
 * parseSignResponse(...) }`). The wire-format layer lives entirely in
 * `clientlib-ktx` (imported by `MwaClientImpl`, NOT by the Fake). The
 * "one-line addition" of a `lastRequestFrame` hook envisioned by the
 * original spec was therefore not actionable — adding frame construction
 * to FakeMwaClient is a thicker-fake redesign tracked by Story 1-6 CR-21
 * (fake/real parity harness) and a candidate Story 5-x or release-gate
 * deliverable.
 *
 * **What this test asserts (the realistic contract):** that
 * `testdata/mwa-fixtures/sign_messages_success.json`'s `expected_input`
 * block is internally self-consistent against what `FakeMwaClient.signMessages`
 * actually consumes — and that calling the Fake with those inputs produces
 * a `MwaResult.Success` whose `signedPayloads` byte-match the fixture's
 * `response.payload.signed_payloads` after base64 round-trip.
 *
 * Concretely, this verifies four things at once:
 *   1. **Schema integrity** — `expected_input` carries the three keys
 *      (`auth_token`, `messages`, `addresses`) with correct shapes
 *      (string, JSON array of strings, JSON array of strings). Catches
 *      fixture schema drift.
 *   2. **Base64 well-formedness** — `messages[]` and `addresses[]` decode
 *      to non-empty `ByteArray` (caller-side wire format per Story 1-6
 *      DD-2). Catches base64 corruption / formatting accidents.
 *   3. **Opaque-string auth_token** — `expected_input.auth_token` is
 *      treated as an opaque UTF-8 string per Story 1-6 DD-2 (NOT
 *      base64-decoded), then wrapped in `SecretString` for the call.
 *      Mirrors `MwaClientImpl.signMessages`'s production behavior.
 *   4. **Fake honors the canned response** — `FakeMwaClient.signMessages(...)`
 *      called with the inputs decoded from `expected_input` returns
 *      `MwaResult.Success(SignResult(signedPayloads))` where
 *      `signedPayloads[i]` byte-equals `Base64.decode(response.payload.signed_payloads[i])`.
 *      Catches a regression where `parseSignResponse` mishandles the
 *      response shape.
 *
 * **Real-wallet wire-format invariant** (each signed payload is a bare
 * 64-byte ed25519 signature per `SignResult.kt:8-11`) is verifiable only
 * against a real wallet — the `sign_messages_success.json` fixture carries
 * 18-byte placeholder strings ("signed-message-one"/"signed-message-two")
 * and the FakeMwaClient is intentionally agnostic to the byte-length
 * shape. That invariant is the manual release-gate's territory (run
 * against Phantom/Solflare during Story 3-1 close-out window).
 *
 * The `mwa.fixtures.dir` system property is set by
 * `android/plugin/build.gradle.kts` (`testOptions.unitTests.all`) per
 * Story 1-6 DD-6 — same wiring as `FakeMwaClientTest`. `android.util.Log`
 * is mocked because `FakeMwaClient.runOp` calls `SdkLog.d` which calls
 * `android.util.Log.d` — not present on the JVM unit-test classpath.
 * `@Execution(SAME_THREAD)` serializes JVM-global static state.
 *
 * @see com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient
 * @see com.godotengine.godot_solana_sdk.mwa.client.dto.SignResult
 */
@Execution(ExecutionMode.SAME_THREAD)
class SignMessagesContractTest {

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
    fun `sign_messages_success fixture inputs round-trip through FakeMwaClient to expected response`() {
        // 1. Read fixture --------------------------------------------------------
        val fixtureFile = File(fixtureDir, "sign_messages_success.json")
        assertTrue(
            fixtureFile.isFile,
            "sign_messages_success.json must exist at $fixtureFile (Story 1-6 fixture set)",
        )
        val fixture = JSONObject(fixtureFile.readText(Charsets.UTF_8))

        // 2. Schema integrity — required keys + types ----------------------------
        // expected_input is the contract: what FakeMwaClient (and its real
        // counterpart MwaClientImpl) consumes when called against this scenario.
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
        val messagesArr = expectedInput.getJSONArray("messages")
        val addressesArr = expectedInput.getJSONArray("addresses")

        assertTrue(authTokenStr.isNotEmpty(), "expected_input.auth_token must be non-empty")
        assertTrue(messagesArr.length() > 0, "expected_input.messages must be non-empty")
        assertTrue(addressesArr.length() > 0, "expected_input.addresses must be non-empty")
        assertTrue(
            signedPayloadsExpected.length() > 0,
            "response.payload.signed_payloads must be non-empty",
        )

        // 3. Base64 well-formedness + decode ------------------------------------
        // messages[] and addresses[] are base64-encoded byte arrays per the
        // wire format — Story 1-6 fixture convention. auth_token is intentionally
        // NOT base64-decoded (DD-2 opaque-string invariant).
        val decodedMessages = (0 until messagesArr.length()).map { i ->
            Base64.getDecoder().decode(messagesArr.getString(i))
        }
        val decodedAddresses = (0 until addressesArr.length()).map { i ->
            Base64.getDecoder().decode(addressesArr.getString(i))
        }
        decodedMessages.forEachIndexed { i, m ->
            assertTrue(
                m.isNotEmpty(),
                "expected_input.messages[$i] must decode to non-empty bytes",
            )
        }
        decodedAddresses.forEachIndexed { i, a ->
            assertTrue(
                a.isNotEmpty(),
                "expected_input.addresses[$i] must decode to non-empty bytes",
            )
        }

        // 4. Wrap auth_token as opaque SecretString (DD-2) ----------------------
        // MwaClientImpl.signMessages wraps the cached auth_token bytes the same
        // way (string → UTF-8 bytes → SecretString). Production caller behavior
        // is faithfully mirrored.
        val authToken = SecretString(authTokenStr.toByteArray(Charsets.UTF_8))

        // 5. Build FakeMwaClient with the success scenario ----------------------
        val client = FakeMwaClient(fixtureDir).withScenario("success")

        // 6. Invoke FakeMwaClient with the EXACT inputs the fixture declares ----
        val result = runBlocking {
            client.signMessages(sender, identity, authToken, decodedMessages, decodedAddresses)
        }

        // 7. Assert Fake honors the canned response shape -----------------------
        assertTrue(
            result is MwaResult.Success<*>,
            "FakeMwaClient.signMessages must return Success for the success scenario " +
                "(got $result). Indicates parseSignResponse regression or scenario-load issue.",
        )
        @Suppress("UNCHECKED_CAST")
        val signResult: SignResult = (result as MwaResult.Success<SignResult>).value
        assertNotNull(signResult, "MwaResult.Success.value must be a SignResult")

        // signedPayloads count must match response.payload.signed_payloads.length()
        // — proves parseSignResponse iterates the JSONArray completely and
        // doesn't drop or duplicate entries.
        assertEquals(
            signedPayloadsExpected.length(),
            signResult.signedPayloads.size,
            "signResult.signedPayloads.size must equal response.payload.signed_payloads.length() " +
                "(parseSignResponse iteration contract)",
        )

        // Each signed_payload byte-equals the base64-decoded fixture entry —
        // proves parseSignResponse decodes correctly and preserves byte order.
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
