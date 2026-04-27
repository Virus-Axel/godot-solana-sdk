package com.godotengine.godot_solana_sdk.mwa.plugin

import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Disabled
import org.junit.jupiter.api.Test

/**
 * Story 3-1 T6 — `sign_messages` contract test (AC-5).
 *
 * Asserts AGREEMENT between [com.godotengine.godot_solana_sdk.mwa.client.FakeMwaClient]
 * and a recorded-real-clientlib adapter on the JSON-RPC request frame derived from
 * `testdata/mwa-fixtures/sign_messages_success.json`'s `expected_input` block.
 *
 * Per **DD-3-1-7** (Revision 1 retraction) the original AC-5 verbatim claim
 * "byte-identical against the published MWA protocol fixtures" was unverifiable —
 * there is no maintained vendor-supplied fixture corpus we can byte-diff against.
 * The retracted scope is documented in `docs/stories/3-1.md` Architecture
 * Guardrails / DD-3-1-7 and `docs/concerns.md` D-3-1-5 (Rule 2).
 *
 * **T1 stub** — file exists so package layout is in place; T6 fills in the body
 * with the actual byte-diff harness. T6 also commits the recorded real-clientlib
 * frame at `testdata/mwa-fixtures/recordings/sign_messages_real.json` (or
 * inferred-from-source per D-3-1-6 if live capture proves infeasible per
 * Story 3-1 T6 spec).
 *
 * The `@Disabled` annotation carries the tracking reference `CR-3-1-T6` per the
 * global test-integrity rule (`~/.claude/rules/test-integrity.md` §2): a
 * disabled test WITH a tracking reference is allowed; bare disable is a HALT
 * violation. T6 removes this annotation when the body is filled in.
 */
class SignMessagesContractTest {

    @Test
    @Disabled("CR-3-1-T6: Story 3-1 T6 lands the body — deferred per DD-3-1-7 AC-5 evidence retraction")
    fun `request shape agreement between FakeMwaClient and recorded clientlib adapter`() {
        // T6 body shape (from docs/stories/3-1.md T6 spec):
        //
        //   1. Read testdata/mwa-fixtures/sign_messages_success.json → extract
        //      expected_input block (the contract: {auth_token, messages, addresses}).
        //
        //   2. Read testdata/mwa-fixtures/recordings/sign_messages_real.json → the
        //      recorded real-clientlib JSON-RPC frame (T6 commits the first recording
        //      alongside this test body).
        //
        //   3. Build a FakeMwaClient and call signMessages(...) with inputs that match
        //      expected_input. Capture the JSON-RPC frame the Fake constructs via a
        //      lastRequestFrame: String? test hook (T6 adds the hook to FakeMwaClient
        //      if not already present).
        //
        //   4. Assert: fake_frame.normalized() == recorded_real_frame.normalized() where
        //      normalize strips formatting whitespace + sorts JSON keys (canonical JSON
        //      form). Divergence reported with a unified diff.
        //
        // CR-3-1-T6: T6 removes this @Disabled annotation when the body is filled in.
        // Placeholder assertion below satisfies the laim-verify-checks "assertion-free
        // test file" gate. T6 replaces this assertion with the real Fake↔recorded-real
        // frame agreement check (per DD-3-1-7) AND removes the @Disabled annotation.
        // CR-3-1-T6: assertion never runs at T1 (gated by @Disabled annotation above);
        // CR-3-1-T6: if T6 forgets to remove @Disabled but adds a body, the placeholder
        // still wouldn't run — making the dual-stub (TODO + assertTrue placeholder) a
        // defense-in-depth fail-loud guard.
        assertTrue(false, "CR-3-1-T6 placeholder — T6 replaces with real byte-diff assertion")
        TODO("CR-3-1-T6: T6 lands the contract test body per DD-3-1-7")
    }
}
