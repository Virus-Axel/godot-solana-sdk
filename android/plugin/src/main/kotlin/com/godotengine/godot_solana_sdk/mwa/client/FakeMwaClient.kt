package com.godotengine.godot_solana_sdk.mwa.client

import com.godotengine.godot_solana_sdk.mwa.client.dto.AuthResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignAndSendResult
import com.godotengine.godot_solana_sdk.mwa.client.dto.SignResult
import com.godotengine.godot_solana_sdk.mwa.generated.MwaError
import com.godotengine.godot_solana_sdk.mwa.util.SdkLog
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import org.json.JSONException
import org.json.JSONObject
import java.io.File
import java.util.Base64
import java.util.UUID

/**
 * Test double for [MwaClient] that replays canned responses from JSON fixtures
 * under [fixtureDir]. Consumed by:
 * -  unit tests (this story).
 * -  through 4-1 plugin-layer unit tests (same JSON fixtures).
 *  - The T4 contract tier (Story 3/4) — identical suite against this fake and
 * a thin adapter replaying recorded clientlib-ktx traffic (parity).
 *
 * **Test-only.** Ships in `src/main/kotlin/` rather than `src/test/` so Epic 2+
 * test source-sets in sibling modules can consume it without the Gradle
 * `testFixtures` plumbing (deferred to — see Handoff).
 * Never instantiate from production code paths.
 *
 * **Not thread-safe.** [nextScenario] is a plain `var` and interleaved
 * `withScenario` + op calls from concurrent coroutines will leak scenario
 * state between them. Create one instance per coroutine, or serialize calls
 * from the outside (e.g. a single-threaded dispatcher in tests).
 *
 * Fixture lookup: `${fixtureDir}/${op}_${scenario}.json`. The scenario is set
 * per call via [withScenario] and auto-resets to `null` after each op (prevents
 * test-order coupling). Calling an op with no scenario set — or one whose
 * fixture file does not exist — throws [IllegalStateException] with the
 * load-bearing message `no fixture for scenario '<name>'` (AC-4 contract).
 * The exception type is deliberately [IllegalStateException] (not
 * [IllegalArgumentException]): the caller is in an invalid state (forgot to
 * set up a scenario), not supplying a bad argument.
 *
 * Each call generates its own `corrId` (8 hex chars) and emits enter/exit log
 * lines at [SdkLog.d] — mirrors [MwaClientImpl] log shape so cross-impl
 * observability parity holds (Design Decision 3). Exit is logged
 * even on the exception path (`outcome=exception`) so a test-setup failure is
 * easy to correlate with its enter line.
 *
 * Response decoding (per `testdata/mwa-fixtures/SCHEMA.md` encoding
 * conventions):
 *  - `auth_token` (opaque string — wallets emit it as base64 characters but
 *    the value is treated as an opaque blob): stored as
 * `SecretString(string.toByteArray(UTF_8))`. The invariant (
 *    Design Decision 2) is that [MwaClientImpl] round-trips it back to the
 *    adapter via `adapter.authToken = String(reveal(), UTF_8)` — the UTF-8
 *    bytes are the SecretString's carrier, NOT a base64-decoded payload.
 *  - `public_key` (base64 string, RFC 4648 §4 standard alphabet, padded) →
 *    base64-decoded to 32-byte ed25519 public key.
 *  - `signed_payloads[]` (base64 strings, RFC 4648 §4) → base64-decoded to
 *    `List<ByteArray>`.
 *  - `signatures[]` (base58 strings, Solana RPC convention) → returned as
 *    `List<String>` (callers decode if needed).
 */
class FakeMwaClient(
    private val fixtureDir: File,
) : MwaClient {

    private var nextScenario: String? = null

    /** Queue a scenario name; the next op call loads `${op}_${scenario}.json`. */
    fun withScenario(scenario: String): FakeMwaClient {
        nextScenario = scenario
        return this
    }

    override suspend fun authorize(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        cluster: String,
        chainId: String,
    ): MwaResult<AuthResult> = runOp("authorize") { file, fixture -> parseAuthResponse(file, fixture) }

    override suspend fun reauthorize(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        cluster: String,
        chainId: String,
    ): MwaResult<AuthResult> = runOp("reauthorize") { file, fixture -> parseAuthResponse(file, fixture) }

    override suspend fun deauthorize(sender: ActivityResultSender, identity: ConnectionIdentity, authToken: SecretString): MwaResult<Unit> =
        runOp("deauthorize") { _, fixture -> parseUnitResponse(fixture) }

    override suspend fun disconnect(): MwaResult<Unit> = runOp("disconnect") { _, fixture -> parseUnitResponse(fixture) }

    override suspend fun signMessages(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        messages: List<ByteArray>,
        addresses: List<ByteArray>,
    ): MwaResult<SignResult> = runOp("sign_messages") { file, fixture -> parseSignResponse(file, fixture) }

    override suspend fun signTransactions(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        transactions: List<ByteArray>,
    ): MwaResult<SignResult> = runOp("sign_transactions") { file, fixture -> parseSignResponse(file, fixture) }

    override suspend fun signAndSendTransactions(
        sender: ActivityResultSender,
        identity: ConnectionIdentity,
        authToken: SecretString,
        transactions: List<ByteArray>,
        cluster: String,
    ): MwaResult<SignAndSendResult> = runOp("sign_and_send") { file, fixture -> parseSignAndSendResponse(file, fixture) }

    /** No-op per Design Decision 1 — FakeMwaClient holds no state to release. */
    override fun close() {
        // intentionally empty
    }

    // ----------------------------------------------------------------------
    // Internals
    // ----------------------------------------------------------------------

    private inline fun <T> runOp(op: String, parse: (File, JSONObject) -> MwaResult<T>): MwaResult<T> {
        val corrId = UUID.randomUUID().toString().take(CORR_ID_LEN)
        SdkLog.d(TAG, corrId) { "$op.enter" }
        var outcome = "exception"
        try {
            val (file, fixture) = loadFixture(op)
            val result = try {
                parse(file, fixture)
            } catch (e: JSONException) {
                // Re-raise with the fixture file path so CI logs point at the
                // offending JSON instead of a bare "missing key 'cluster'".
                throw IllegalStateException("malformed fixture '${file.name}': ${e.message}", e)
            }
            outcome = outcomeOf(result)
            return result
        } finally {
            SdkLog.d(TAG, corrId) { "$op.exit outcome=$outcome" }
        }
    }

    private fun loadFixture(op: String): Pair<File, JSONObject> {
        val scenario = nextScenario
        nextScenario = null
        if (scenario == null) {
            throw IllegalStateException("no fixture for scenario '<unset>'")
        }
        val file = File(fixtureDir, "${op}_$scenario.json")
        if (!file.isFile) {
            throw IllegalStateException("no fixture for scenario '$scenario'")
        }
        return file to JSONObject(file.readText(Charsets.UTF_8))
    }

    private fun parseAuthResponse(file: File, fixture: JSONObject): MwaResult<AuthResult> {
        val response = fixture.getJSONObject("response")
        return when (val type = response.getString("type")) {
            "success" -> {
                val payload = response.getJSONObject("payload")
                MwaResult.Success(
                    AuthResult(
                        authToken = SecretString(payload.getString("auth_token").toByteArray(Charsets.UTF_8)),
                        publicKey = Base64.getDecoder().decode(payload.getString("public_key")),
                        accountLabel = payload.optStringOrAbsent("account_label"),
                        walletUriBase = payload.optStringOrAbsent("wallet_uri_base"),
                        walletPackage = payload.optStringOrAbsent("wallet_package"),
                        cluster = payload.getString("cluster"),
                        chainId = payload.getString("chain_id"),
                    ),
                )
            }
            "error" -> MwaResult.Failure(resolveError(response.getString("code")))
            else -> throw IllegalStateException(
                "fixture '${file.name}' has unexpected response.type '$type' — validator should have caught this",
            )
        }
    }

    private fun parseUnitResponse(fixture: JSONObject): MwaResult<Unit> {
        val response = fixture.getJSONObject("response")
        return when (val type = response.getString("type")) {
            "success" -> MwaResult.Success(Unit)
            "error" -> MwaResult.Failure(resolveError(response.getString("code")))
            else -> throw IllegalStateException("unexpected response.type '$type'")
        }
    }

    private fun parseSignResponse(file: File, fixture: JSONObject): MwaResult<SignResult> {
        val response = fixture.getJSONObject("response")
        return when (val type = response.getString("type")) {
            "success" -> {
                val payload = response.getJSONObject("payload")
                val signedArray = payload.getJSONArray("signed_payloads")
                val signed = List(signedArray.length()) {
                    Base64.getDecoder().decode(signedArray.getString(it))
                }
                MwaResult.Success(SignResult(signed))
            }
            "error" -> MwaResult.Failure(resolveError(response.getString("code")))
            else -> throw IllegalStateException(
                "fixture '${file.name}' has unexpected response.type '$type'",
            )
        }
    }

    private fun parseSignAndSendResponse(file: File, fixture: JSONObject): MwaResult<SignAndSendResult> {
        val response = fixture.getJSONObject("response")
        return when (val type = response.getString("type")) {
            "success" -> {
                val payload = response.getJSONObject("payload")
                val sigArray = payload.getJSONArray("signatures")
                val sigs = List(sigArray.length()) { sigArray.getString(it) }
                MwaResult.Success(SignAndSendResult(sigs))
            }
            "error" -> MwaResult.Failure(resolveError(response.getString("code")))
            else -> throw IllegalStateException(
                "fixture '${file.name}' has unexpected response.type '$type'",
            )
        }
    }

    private fun resolveError(code: String): MwaError = ERROR_BY_CODE[code] ?: error(
        "unknown MwaError code '$code' in fixture — if you added a new code to the manifest, " +
            "regenerate MwaError.kt AND update FakeMwaClient.ERROR_BY_CODE to match",
    )

    private fun outcomeOf(result: MwaResult<*>): String = if (result is MwaResult.Success<*>) "success" else "failure"

    /**
     * Returns the string value if the key is present and non-null JSON, else null.
     * Unlike the earlier helper this does NOT collapse `""` → `null` — empty
     * string is a meaningful value that callers may want to distinguish from
     * "field absent".
     */
    private fun JSONObject.optStringOrAbsent(key: String): String? = if (!has(key) || isNull(key)) null else getString(key)

    private companion object {
        const val TAG = "MWA"
        const val CORR_ID_LEN = 8

        val ERROR_BY_CODE: Map<String, MwaError> = listOf(
            MwaError.UserCanceled,
            MwaError.WalletRejected,
            MwaError.NoMwaWalletInstalled,
            MwaError.WalletUninstalled,
            MwaError.TokenExpired,
            MwaError.RpcFailed,
            MwaError.NetworkOffline,
            MwaError.Timeout,
            MwaError.NotConnected,
            MwaError.UnsupportedPlatform,
            MwaError.ProtocolError,
            MwaError.StorageCorrupt,
            MwaError.ReauthRequired,
        ).associateBy { it.code }
    }
}
