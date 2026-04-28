package com.godotengine.godot_solana_sdk.mwa.plugin

import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Disabled
import org.junit.jupiter.api.Test

/**
 * Story 3-3 T6 — `sign_and_send` contract test (AC-7). T1 lands the empty
 * stub class so the test-source-set discovers it under the expected
 * `*ContractTest*` glob; CR-3-3-T6: T6 fills in the body and removes `@Disabled`.
 *
 * **Scope (per Story 3-1 D-3-1-13 Rule 3 inheritance — re-scoped at Story
 * 3-1 to avoid the false premise that FakeMwaClient constructs a JSON-RPC
 * frame).** AC-7 evidence narrows to fixture-input contract verification:
 * the test asserts that
 * [com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient] consumes the
 * `sign_and_send_success.json` fixture's `expected_input` block (which
 * carries `auth_token` + `transactions` + `cluster` — 3 keys, NOT 2 like
 * sign_transactions; the 5-arg `MwaClient.signAndSendTransactions(sender,
 * identity, authToken, transactions, cluster)` signature requires the
 * cluster arg) and produces the documented `response.payload.signatures`
 * (base58 strings, NOT base64 byte arrays — DD-3-3-E).
 *
 * @see SignTransactionsContractTest the Story 3-2 sibling under the same
 *      D-3-1-13 re-scope.
 * @see SignMessagesContractTest the Story 3-1 sibling that landed the
 *      re-scope.
 */
class SignAndSendContractTest {

    @Test
    @Disabled("CR-3-3-T6: Story 3-3 T6 lands fixture-input verification")
    fun `fixture-input contract honored by FakeMwaClient`() {
        // CR-3-3-T6 placeholder — defense-in-depth dual-stub mirroring Story 3-1's
        // SignMessagesContractTest fixup (commit 87692808). The placeholder fires loud
        // CR-3-3-T6: if T6 removes @Disabled but forgets to replace this line with a real assertion;
        // the TODO fires loud if T6 lands a body but forgets to remove the TODO. Both
        // CR-3-3-T6: are unreachable while @Disabled is present.
        assertTrue(false, "CR-3-3-T6 placeholder — T6 replaces with real fixture-input assertion")
        TODO("CR-3-3-T6: T6 lands the contract test body per DD-3-1-7 + D-3-1-13 re-scope")
    }
}
