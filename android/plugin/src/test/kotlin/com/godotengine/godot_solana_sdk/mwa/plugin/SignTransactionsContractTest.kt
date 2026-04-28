package com.godotengine.godot_solana_sdk.mwa.plugin

import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Disabled
import org.junit.jupiter.api.Test

/**
 * Story 3-2 T6 — `sign_transactions` contract test (AC-5). T1 lands the empty
 * stub class so the test-source-set discovers it under the expected
 * `*ContractTest*` glob; T6 fills in the body and removes `@Disabled`.
 *
 * **Scope (per D-3-1-13 Rule 3 — re-scoped at Story 3-1 to avoid the false
 * premise that FakeMwaClient constructs a JSON-RPC frame).** AC-5 evidence
 * narrows to fixture-input contract verification: the test asserts that
 * [com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient] consumes the
 * `sign_transactions_success.json` fixture's `expected_input` block and
 * produces the documented `response.payload` — i.e., the fixture is the
 * contract surface, NOT a hypothetical wire-format frame.
 *
 * @see SignMessagesContractTest the Story 3-1 T6 sibling that landed first
 *      under the same D-3-1-13 re-scope.
 * @see docs/stories/3-2.md T6 Tasks section for the full filled-in body
 *      checklist (fixture-input field-by-field assertions, parseSignResponse
 *      passthrough, MwaResult.Success.value contents).
 */
class SignTransactionsContractTest {

    @Test
    @Disabled("CR-3-2-T6: Story 3-2 T6 lands fixture-input verification")
    fun `fixture-input contract honored by FakeMwaClient`() {
        // CR-3-2-T6 placeholder — defense-in-depth dual-stub mirroring Story 3-1's
        // SignMessagesContractTest fixup (commit 87692808). The placeholder fires loud
        // if T6 removes @Disabled but forgets to replace this line with a real assertion;
        // the TODO fires loud if T6 lands a body but forgets to remove the TODO. Both
        // are unreachable while @Disabled is present.
        assertTrue(false, "CR-3-2-T6 placeholder — T6 replaces with real fixture-input assertion")
        TODO("CR-3-2-T6: T6 lands the contract test body per DD-3-1-7 + D-3-1-13 re-scope")
    }
}
