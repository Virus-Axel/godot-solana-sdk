# Concerns

Deferred items from code reviews and implementation. Tracked for future resolution.

## CR-1: `reauthorize` retry_hint in validator but absent from story 1-1 spec
- **Story:** 1-1 | **Severity:** LOW
- **Detail:** `_VALID_RETRY_HINTS` accepts `reauthorize` per amendment A-1, but the story 1-1 spec only lists 4 values. The 5th value is documented as "reserved for future use" in `docs/amendments.md` A-2 (retracted). No error code currently uses it. Property tests may generate manifests with `reauthorize` that pass validation.
- **Risk:** None today; a future maintainer may be confused by the mismatch between story spec and code.
- **Resolution:** accepted — A-1 documents the full set; story spec is frozen.

## CR-2: Missing `--out-cpp`, `--out-kotlin`, `--out-gdscript` CLI flags
- **Story:** 1-1 | **Severity:** LOW
- **Detail:** Task 2 spec called for custom output path flags. Not implemented; CLI hardcodes canonical paths via `TARGETS` dict. No AC or downstream story depends on these flags.
- **Risk:** Minor DX limitation for non-standard repo layouts.
- **Resolution:** add if a downstream story needs them; not worth the complexity today.

## CR-3: Duplicate `_REPO_ROOT` / `REPO_ROOT` in `gen_error_codes.py`
- **Story:** 1-1 | **Severity:** LOW
- **Detail:** Lines 20 and 27 both compute `Path(__file__).resolve().parent.parent`. One is used for `sys.path`, the other for path construction.
- **Resolution:** consolidate on next touch of the file.

## CR-4: CI drift check uses `git status --porcelain` instead of `--check` CLI mode
- **Story:** 1-1 | **Severity:** LOW
- **Detail:** Story spec said drift check should use `python tools/gen_error_codes.py --check`. CI instead regenerates and uses `git status --porcelain` (which is strictly more capable — catches untracked files too). The `--check` mode is tested via pytest (Group 4 tests) but not exercised in CI.
- **Risk:** `--check` mode code path is tested but not used in the real CI environment.
- **Resolution:** acceptable — `git status --porcelain` is the superior approach per user review feedback. `--check` mode remains useful for local dev workflow.

## CR-5: JUnit Jupiter 6.x blocked by Kotlin plugin 1.8.0 — upgrade path deferred
- **Story:** 1-2 | **Severity:** MEDIUM
- **Detail:** Story 1-2 §Tooling Notes pinned `org.junit.jupiter:junit-jupiter:6.0.3` (Maven Central verified 2026-04-17). Attempting the pin fails `compileDebugUnitTestKotlin` with `"Module was compiled with an incompatible version of Kotlin. The binary version of its metadata is 2.1.0, expected version is 1.8.0."`. JUnit 6.x is compiled with Kotlin 2.1 metadata; this module's `android/build.gradle.kts` pins `org.jetbrains.kotlin.android` at 1.8.0 (originating scaffold). Task 1 shipped JUnit 5.11.4 (last Kotlin-1.x-compatible release) with an inline build.gradle.kts comment explaining the revert; story's §Deviations Expected updated with D1.
- **Risk:** JUnit 5.x still receives bugfix/security releases but is in maintenance mode; all new test features go to 6.x. No test-writing limitation for Story 1-2 (no JUnit 6.x APIs needed). A future story that wants Kotlin 2.x idioms (context receivers, data objects, k2 compiler) would need the upgrade anyway.
- **Resolution:** defer to a dedicated "Kotlin plugin 2.x upgrade" task (candidate for a late Epic-5 polish story or a Story 5-6 CI sub-task); at that point bump `org.jetbrains.kotlin.android` 1.8.0 → 2.x, bump AGP if needed, re-pin JUnit Jupiter 6.x.

## CR-6: Unused AGP-internal `dependsOn` import in `android/plugin/build.gradle.kts` — ktlint landmine for Task 5
- **Story:** 1-2 | **Severity:** LOW
- **Detail:** Line 1 of `android/plugin/build.gradle.kts` is `import com.android.build.gradle.internal.tasks.factory.dependsOn` — an AGP internal extension function. The `dependsOn(cleanDemoAddons)` call inside `val copyAddonsToDemo by tasks.registering(Copy::class) { ... }` resolves against Kotlin DSL's standard `Task.dependsOn(...)` (implicit `this` receiver is the Copy task being configured) and does not require this import. The import is pre-existing scaffold code (carried over from the upstream `Virus-Axel/godot-solana-sdk` template) and is functionally dead.
- **Risk:** (1) Task 5 runs `./gradlew :plugin:ktlintCheck` and ktlint's `standard:no-unused-imports` rule (enabled by `kotlin_code_style = official`) will flag this line. Task 5's implementer must either delete the import or `@Suppress` it. (2) `com.android.build.gradle.internal.*` is not part of AGP's stable API — a future AGP bump can silently remove the symbol and break the build. Removing the unused import also eliminates this brittleness.
- **Resolution:** delete the import in Task 5 as part of "Any ktlint violations uncovered in Tasks 1–4's new files + the Task 4-modified scaffold files — fix inline" (story's Task 5 wording covers this).

## CR-7: Missing `buildTypes.release { isMinifyEnabled = true }` — blocks Task 6 R8 evidence
- **Story:** 1-2 | **Severity:** HIGH
- **Detail:** `android/plugin/build.gradle.kts` has no `buildTypes { release { ... } }` block. AGP's default release build type is `isMinifyEnabled = false`, which means R8 does not run in release builds. Task 6 ships `-assumenosideeffects` in `consumer-rules.pro` and expects the release AAR decompile grep to show `VERBOSE=0, DEBUG=0, INFO=1`. With R8 disabled, the rule has zero effect and the decompile shows all three sentinels present — Task 6 would fail for a cause not covered by the story's own R1 risk ordering (package mismatch → R8Sentinel DCE → @JvmStatic → lambda-bypass).
- **Risk:** HIGH — Task 6 cannot pass its AC-2 evidence gate without R8 running. The first symptom is "I added the -assumenosideeffects rule and sentinel strings still appear," which looks identical to bugs in the story's R1 ordering but has a different fix.
- **Resolution:** Task 6's story section has been amended (this commit-cycle) to include an explicit `buildTypes { release { isMinifyEnabled = true; proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt")) } }` block in the files-to-modify list, plus a "enable R8 before running `assembleRelease`" step in the procedure. Closes the plan gap.

## CR-8: C++ `SecretString` has no GoogleTest coverage in Story 1-2 — deferred to Story 1-5 (A-8)
- **Story:** 1-2 | **Severity:** MEDIUM
- **Detail:** Story 1-2 Task 2 ships only `src/mwa/include/secret_string.hpp` (header-only). The GoogleTest file and SCons `tests` alias target originally scoped to Task 2 are deferred to Story 1-5 per amendment A-8 and story Risk R4 fallback (b). On 2026-04-20 pre-Task-2 scan: no `src/mwa/` or `test/` dir, no `tests` SCons alias, no system-installed GoogleTest, and `godot-cpp` submodule uninitialized locally. Attempting R4 (a) exceeds the story's 1-hour escalation threshold.
- **Risk:** (1) M2 C++ verification is inspection-level only for Story 1-2: the class satisfies DD-26 (`godot::`-only types), `to_string()` returns `"<redacted>"`, `reveal_bytes()` exposes `godot::PackedByteArray`, header uses `#ifndef`/`#define`/`#endif` guard — all verifiable by reading the file but not mechanically asserted. (2) A regression that changes `to_string()` or leaks `bytes_` would slip through Story 1-2's Gate 5 and surface only when Story 1-5 lands the tests. (3) Story 1-4's header-lint CI job is a partial backstop: it catches STL leaks in `src/mwa/include/` but does not catch behavioral changes.
- **Resolution:** Story 1-5's story file (created at wave 3 start) must include a prerequisite task: "Port Story 1-2 Task 2's deferred GoogleTest assertions verbatim — `TEST(SecretString, ToStringReturnsRedacted)` / `TEST(SecretString, RevealBytesReturnsOriginal)` — using the test body already specified in story 1-2 §Task 2." When Story 1-5 lands, update this concern to RESOLVED and cross-reference the commit. Until then: treat `src/mwa/include/secret_string.hpp` as frozen — any behavioral change (accessor rename, `to_string()` return value, namespace move) requires re-opening this concern and landing a test alongside the change.
- **Status: REDEFERRED (1-2 → 1-5 → headless-Godot tier)** | **Date:** 2026-04-23 | **Redeferred-by:** Amendment A-13
  - **What happened:** Story 1-5 Task 4b Phase D landed `src/mwa/tests/test_secret_string.cpp` in commit `f025f1c9`; Task 6 wired `cpp_tests.yml` in `ce20c0a6`. The scons tests target BUILT the binary GREEN on `f8ffd143`, but `./bin/mwa_tests` SIGSEGV'd at runtime in `MobileWalletAdapterTest::SetUpTestSuite` at `godot::ClassDB::register_class<MobileWalletAdapter>()` — godot-cpp's `GDExtensionBinding` interface pointers are null in a host binary with no Godot engine. Stock `gtest_main.cc` does not initialize them.
  - **Why "can't scope-down to just SecretString":** `SecretString` ctor requires `godot::PackedByteArray`, which itself needs Variant machinery initialized. Even the 2 SecretString TEST cases cannot run host-mode without ~140 godot-cpp function-pointer stubs (same surface as the MWA tests). See A-13 for the full analysis.
  - **Resolution:** A-13 retires the entire C++ host tier (`scons tests` target, `.github/workflows/cpp_tests.yml`, `third_party/googletest` submodule, both test files). CR-8 is redeferred to the future headless-Godot tier tracked by CR-35. SecretString's Kotlin-side coverage (6 JUnit tests from Story 1-2 Task 1) is unaffected — only the C++ assertion layer moves.
  - **Trigger to close:** a future story reactivates `MWA_TESTING` under a headless-Godot harness AND lands `TEST(SecretString, ToStringReturnsRedacted)` + `TEST(SecretString, RevealBytesReturnsOriginal)` against an initialized godot-cpp runtime. Until then: `src/mwa/include/secret_string.hpp` remains frozen — any behavioral change (accessor rename, `to_string()` return value, namespace move) must land alongside the headless-Godot port of these assertions.

## CR-9 (LOW): Task 6 grep-count verification replaced by bytecode disassembly (A-9)
- **Story:** 1-2 | **Date:** 2026-04-20 | **Severity:** LOW | **Status:** tracked
- **Summary:** The plan-specified grep counts for VERBOSE/DEBUG/INFO sentinels (0/0/>=1) are not reached because R8 preserves `.class` files of stripped lambdas (strings remain in constant pools of unreachable classes). AC-2 is satisfied via the bytecode-level proof amended in A-9.
- **Why LOW:** bytecode proof is strictly stronger than the grep. No release-build regression, no runtime leak — R8 correctly strips v/d call sites. The grep numbers are a bytecode-layout observation, not a behavior property.
- **Trigger to revisit:** AGP / R8 version bump in the future. If a newer R8 starts DCE-ing dead lambda classes, the grep will naturally return 0/0/1 and both the original plan criterion AND the amended criterion will pass. No action required; this concern will auto-close at that point.
- **Not a concrete code change** — documentation-only follow-up.


## CR-10 (MEDIUM): R8Sentinel ships into release AAR as public bytecode
- **Story:** 1-2 | **Date:** 2026-04-20 | **Severity:** MEDIUM | **Status:** tracked | **Origin:** code-review Finding #4
- **Summary:** `R8Sentinel.kt` is marked `internal` + `@Suppress("unused")` and is functionally dead code in production (exists only so `ci/verify_r8_strip.sh` has a known-shape byte pattern to assert against). The `-keep class ...R8Sentinel { *; }` rule in `consumer-rules.pro` correctly prevents R8 DCE from removing it, but that means the class — and the three `MWA_R8_SENTINEL_VERBOSE_*` / `..._DEBUG_*` / `..._INFO_KEEP_*` magic strings in its lambda-class constant pools — ship in every release AAR consumers download.
- **Why MEDIUM:** no security impact (the strings are opaque UUIDs, not secrets; they reveal only that Story 1-2's evidence infrastructure exists). But evidence infrastructure leaking into the shipped artifact is a cleanliness concern, and a principled fix is cheap.
- **Resolution:** move `R8Sentinel.kt` under a build-variant gate so it is included in the release AAR only when a property like `-PverifyR8Strip=true` is passed. CI's `verify-r8-strip` workflow job sets the flag; normal release builds don't. Requires a small AGP `variantFilter` or `sourceSets` customization in `android/plugin/build.gradle.kts`. Defer to a dedicated follow-up task — not blocking.

## CR-11 (LOW): Dead scaffold field `ComposeWalletActivity.hasConnectedWallet`
- **Story:** 1-2 | **Date:** 2026-04-20 | **Severity:** LOW | **Status:** tracked | **Origin:** code-review Finding #5
- **Summary:** `android/plugin/src/main/java/plugin/walletadapterandroid/ComposeWalletActivity.kt` declares `private var hasConnectedWallet = false` and assigns `true` in all three `when`-branches of `onCreate`, but nothing ever reads the field. Pre-existing scaffold code carried over through Task 4's migration.
- **Why LOW:** no behavioral impact; ktlint doesn't flag unused `var` fields (only unused imports/params); the field's presence is harmless. Natural cleanup target but not urgent.
- **Resolution:** delete the field + its three assignments in a subsequent scaffold-cleanup pass, ideally bundled with any future Story-2-x refactor of `ComposeWalletActivity`.

## CR-12 (LOW): verify_r8_strip.sh Gate 2a method list is hard-coded (v, d)
- **Story:** 1-2 | **Date:** 2026-04-20 | **Severity:** LOW | **Status:** tracked | **Origin:** code-review Finding #6
- **Summary:** `ci/verify_r8_strip.sh:131-132` greps for unminified `invokestatic .../SdkLog.v(` and `.d(` specifically. If a future story adds another method to the `-assumenosideeffects` block in `consumer-rules.pro` (e.g., `SdkLog.t` for trace, or an audit variant), the script silently false-passes — only `v` and `d` are checked.
- **Why LOW:** drift is slow (the `-assumenosideeffects` block rarely expands), the Gate 2b invokestatic-count check catches most regressions anyway (name-agnostic), and the fix requires some awk/sed to parse the rule file reliably.
- **Resolution:** derive the stripped-method list by parsing the `-assumenosideeffects class ...SdkLog { ... }` block in `consumer-rules.pro`, then assert each method is absent from `R8Sentinel.class`. Defer until a second method is added to the block.

## CR-13 (LOW): consumer-rules.pro lacks a cross-reference banner for proguard-library-self.pro
- **Story:** 1-2 | **Date:** 2026-04-20 | **Severity:** LOW | **Status:** tracked | **Origin:** code-review Finding #7
- **Summary:** The R8 rule set is split across two files: `consumer-rules.pro` (ships to downstream consumers via `consumerProguardFiles`) and `proguard-library-self.pro` (applies only to the library's own release build via `buildTypes.release.proguardFiles`). A future maintainer reading `consumer-rules.pro` has no in-file signal that a sibling file exists with library-self-only rules (`-dontobfuscate` in particular), making the split easy to miss when debugging R8 behavior.
- **Why LOW:** `proguard-library-self.pro` itself has a thorough header explaining the split; the gap is only that `consumer-rules.pro` doesn't point AT it.
- **Resolution:** add a one-line banner comment at the top of `consumer-rules.pro`: `# See also: proguard-library-self.pro -- library-self-only rules (-dontobfuscate).`

## CR-14 (LOW): MwaErrorCode enum reused for non-MWA LocalKeypairSigner error reporting
- **Story:** 1-3 | **Date:** 2026-04-20 | **Severity:** LOW | **Status:** tracked | **Origin:** story-creation adversarial review (Item 7)
- **Summary:** Story 1-3's `LocalKeypairSigner` (a desktop, non-MWA `ISigner` implementation that wraps a `Keypair`) emits `sign_failed` signals using error code names from the `MwaErrorCode` enum — specifically `MwaErrorCode::NOT_CONNECTED` when `kp_` is null. The `MwaErrorCode` enum was architected by Story 1-1 as MWA-specific (codes describe Mobile Wallet Adapter protocol failure modes); reusing its names from a non-MWA path is a namespace overlap. The use site in `src/local_keypair_signer.cpp` carries an inline `// TODO(CR-14): rename to SignerErrorCode in v1.2 cleanup.` comment to mark the debt.
- **Why LOW:** The codes describe the same semantic states regardless of underlying transport — `NOT_CONNECTED` means "the signer has no usable connection/credential" whether the signer is MWA-backed (no auth token), WalletAdapter-backed (no wallet connected), or LocalKeypairSigner-backed (no Keypair set). The code names happen to live under an enum that was designed MWA-first because MWA was the first non-trivial signer implementation; the names themselves are not MWA-specific. No security/correctness impact, no consumer-facing API surface change required.
- **Risk:** (1) A future maintainer may be confused by `MwaErrorCode::NOT_CONNECTED` appearing in non-MWA code paths and either (a) duplicate the enum into a `LocalSignerErrorCode` (proliferation) or (b) refactor the enum mid-flight without the v1.2 amendment plan. (2) The enum's eventual rename (to `SignerErrorCode` or split into `MwaErrorCode` + `LocalSignerErrorCode`) is a v1.2 breaking change for any consumer that imports `MwaErrorCode` directly — needs deprecation runway.
- **Resolution path (deferred to v1.2):**
  1. Rename `MwaErrorCode` → `SignerErrorCode` in `tools/error-codes.yaml` and the codegen pipeline (Story 1-1's surface).
  2. Keep `MwaErrorCode` as a typedef alias for one minor version (v1.2 → v1.3 deprecation runway) with `[[deprecated]]` attribute.
  3. Drop the alias in v1.3.
  4. All `// TODO(CR-14)` comments in `src/local_keypair_signer.cpp` and any other non-MWA signer that reused the enum can be removed in the v1.2 rename pass.
- **Affected:**
  - `src/local_keypair_signer.cpp` (Story 1-3 Task 4) — uses `MwaErrorCode::NOT_CONNECTED` with `// TODO(CR-14)` marker.
  - Future non-MWA `ISigner` implementations may extend the same pattern (e.g., a hypothetical `HardwareWalletSigner`); they should reuse the same TODO marker for consistency.
- **Trigger to revisit:** v1.2 release planning (when `MWA_ISIGNER_REMOVE_V1_2` flips to default-on per Story 1-3 Task 7's compile-define gate). At that point, bundle the `MwaErrorCode` → `SignerErrorCode` rename with the deprecation-shim removal.

## CR-15 (MEDIUM): WalletAdapter::did_transaction_change semantics lost when signing flows via WalletAdapterSigner ISigner bridge
- **Story:** 1-3 | **Date:** 2026-04-21 | **Severity:** MEDIUM | **Status:** tracked | **Origin:** Task 3 adversarial review (BLOCKER 1)
- **Summary:** The legacy `WalletAdapter`-direct dispatch in `Transaction::sign_at_index` (preserved unchanged in v1.1 per Concern 5 scope-out) handles browser-wallet transaction modification: when the wallet alters the tx (e.g. compute budget, priority fee), `_signer_signed` calls `controller->did_transaction_change()` and pulls modified bytes via `WalletAdapter::get_modified_transaction()`, re-deserializes, then splices the signature into the updated message. The new `ISigner` pipeline (`_isigner_signed`) drops this entire detect-and-republish path: it sets `signatures[index] = sigs[0]` and emits, regardless of whether the wallet modified anything. For Story 1-3's `LocalKeypairSigner` this is harmless (a local keypair never modifies a tx). For `WalletAdapterSigner` (Task 2's bridge) used through the new ISigner pipeline, the modified-tx case would silently produce a signature for the wallet's modified bytes against an unmodified `Transaction.message` — submission fails on-chain.
- **Why MEDIUM:** v1.1 preserves the old direct-WalletAdapter branch (Concern 5), so users who pass a raw `WalletAdapter` into `signers` retain correct modify-detect behavior. The regression only fires when a user explicitly wraps a `WalletAdapter` in `WalletAdapterSigner` and uses the bridge. That is a less common path in v1.1 because `WalletAdapterSigner` exists primarily as a contractual surface for AC-A-5 fulfillment, not as the default signer. Severity escalates if Story 2-1's `MobileWalletAdapter` (which only flows through ISigner) ever needs the same modify-detect semantics — tracked there.
- **Risk:** A user explicitly using `WalletAdapterSigner` with a wallet that modifies transactions will produce invalid signatures with no diagnostic. The on-chain failure may surface seconds-to-minutes later as a generic "InvalidSignature" RPC error.
- **Resolution paths (decision deferred to a future story):**
  1. **Extend ISigner contract:** add an optional `modified_payload` field to `sign_completed` (e.g., emit `sign_completed(request_id, signatures, modified_payload_bytes_or_empty)`). `Transaction::_isigner_signed` checks for non-empty payload and re-deserializes.
  2. **Document the limitation:** in `docs/migrations/isigner.md` (Task 6), warn that `WalletAdapterSigner` does not currently propagate wallet-side modifications. Recommend raw `WalletAdapter` usage in v1.1 for browser wallets that may modify transactions.
  3. **Update `WalletAdapterSigner` (Task 2) to emit a separate signal:** `tx_modified_by_wallet(modified_bytes)` that `Transaction` can subscribe to alongside `sign_completed`. Adds complexity to ISigner consumers.
- **Affected:**
  - `src/transaction/transaction.cpp` (Story 1-3 Task 3) — `_isigner_signed` handler.
  - `src/wallet_adapter/wallet_adapter_signer.cpp` (Story 1-3 Task 2) — would need to grow the modify-detect path if option (1) or (3) is chosen.
  - `docs/migrations/isigner.md` (Story 1-3 Task 6) — documentation of v1.1 limitation per option (2).
- **Trigger to revisit:** Story 2-1 design (when `MobileWalletAdapter : ISigner` lands and needs the same semantic decision for MWA's tx-review-and-modify flow), OR v1.1 release blocker if a consumer reports broken `WalletAdapterSigner` usage with a modifying wallet.

## CR-16 (LOW): ISigner.sign_transactions payload convention not yet aligned with MWA protocol expectations
- **Story:** 1-3 | **Date:** 2026-04-21 | **Severity:** LOW | **Status:** tracked | **Origin:** Task 3 adversarial review (BLOCKER 2)
- **Summary:** `Transaction::sign_at_index` calls `signer->sign_transactions(serialize_message(), [size], request_id)` for both the native ISigner branch and the Keypair-wrap branch. The payload is `serialize_message()` — the message portion only (what ed25519 signs). MWA's `sign_transactions` API expects the FULL serialized tx wire format (signature slots + message), and the wallet adds its signature to the appropriate slot. When Story 2-1's `MobileWalletAdapter : ISigner` lands, it must reconcile this: either (a) accept message-only bytes and reconstruct full tx wire format internally, (b) the contract changes to pass `serialize()` and `LocalKeypairSigner.sign_transactions` grows sig-slot-stripping logic to extract the message portion. v1.1 picks the message-only convention because it matches what `LocalKeypairSigner` and `WalletAdapterSigner` actually do today (sign the message bytes directly).
- **Why LOW:** The convention works correctly for both v1.1 ISigner implementations (`LocalKeypairSigner`, `WalletAdapterSigner`) — they sign the message bytes directly, matching what `serialize_message()` returns. No incorrect signatures produced, no on-chain failures. The concern is forward-looking: when `MobileWalletAdapter` is added in Story 2-1, it must explicitly handle the convention mismatch with MWA.
- **Risk:** If Story 2-1 implements `MobileWalletAdapter::sign_transactions` by passing the message-only bytes directly to MWA's `sign_transactions` API, MWA will fail (expects full wire format). The implementer must either reconstruct the full serialized tx (round-tripping through `Transaction` deserialize/reserialize) or change the ISigner contract.
- **Resolution path:**
  1. Story 2-1 design pass: pick convention (message-only vs full-serialize) and document in architecture amendment.
  2. If full-serialize is chosen, refactor `Transaction::sign_at_index` to pass `serialize()`, and update `LocalKeypairSigner.sign_transactions` to strip sig slots and sign the message portion. Both T3 and T4 of Story 1-3 would receive a follow-up commit.
  3. If message-only is chosen, document that `MobileWalletAdapter::sign_transactions` reconstructs the full tx wire format from the message bytes plus its own signature.
- **Affected:**
  - `src/transaction/transaction.cpp` (Story 1-3 Task 3) — `sign_at_index` payload.
  - `src/local_keypair_signer.cpp` (Story 1-3 Task 4) — `sign_transactions` impl.
  - `src/mwa/.../mobile_wallet_adapter.cpp` (Story 2-1, future) — must reconcile convention.
- **Trigger to revisit:** Story 2-1 architecture pass.

## CR-17 (LOW): Transaction.isigner_request_id_to_index_ HashMap unbounded on unresponsive signers
- **Story:** 1-3 | **Date:** 2026-04-21 | **Severity:** LOW | **Status:** tracked | **Origin:** Task 3 adversarial review (SUBSTANTIVE 3)
- **Summary:** `Transaction` allocates a unique `request_id` per `sign_at_index` invocation and stores `request_id → index` in `isigner_request_id_to_index_`. The map entry is removed when the signer emits `sign_completed` or `sign_failed`. A signer that hangs forever (Android wallet backgrounded, network partition, MWA token expired without a server-side timeout) leaves its entry in the map permanently. Over long-running game sessions with many sign attempts, the map grows.
- **Why LOW:** Each entry is small (`String` request_id + `int32_t` index ≈ 50 bytes). Map cleared on Transaction destruction. Practical impact requires thousands of hung sign requests in a single Transaction's lifetime — extreme edge case.
- **Resolution path:**
  1. Add a per-request timeout (e.g., 60 s default, configurable) that synthesizes a `sign_failed(request_id, "TIMEOUT", "...")` after the deadline so the normal cleanup fires.
  2. Implementation requires either (a) a single timer that scans the map every N seconds, or (b) per-request timer (more memory, more precise). Option (a) is sufficient for v1.1.
- **Affected:**
  - `include/transaction/transaction.hpp` — would gain a `Timer*` member or a `_process` tick-based scanner.
  - `src/transaction/transaction.cpp` — `_isigner_signed` / `_isigner_failed` would need to check timestamp before acting on stale entries.
- **Trigger to revisit:** A consumer reports memory growth in long-running scenes, OR after Story 2-1 (when real MWA signing exposes the hung-signer scenario more frequently than desktop testing did).

## CR-18 (MEDIUM): GodotMainDispatcher TOCTOU between ObjectDB::get_instance and target->call_deferred
- **Story:** 1-4 | **Date:** 2026-04-21 | **Severity:** MEDIUM | **Status:** tracked with `TODO(story-2-1)` breadcrumb in code | **Origin:** Task 4 + Story 1-4 adversarial code review (finding #3)
- **Summary:** `GodotMainDispatcher::post(signal_name, payload)` calls `godot::ObjectDB::get_instance(target_id_)` and then `target->call_deferred("emit_signal", ...)`. The null-check is thread-safe at resolution time, but the subsequent virtual dispatch is a member call on a raw pointer — if another thread frees `target` between the null-check (succeeds) and the `call_deferred` invocation, the virtual dispatch reads freed memory. The hazard is INERT in Story 1-4 (the no-op + test paths never race), but WILL surface in Story 2-1+ when real JNI worker threads have lifetimes not synchronized with Godot main: `_exit_tree` → node freed → JNI callback still in flight → dispatcher resolves stale pointer → call_deferred on freed memory → UAF.
- **Why MEDIUM:** Latent bug with a clear future trigger. Story 1-4's own execution paths do not exercise the race (no JNI, no worker threads in production — the mock's `drive_signal_from_thread` is test-only and joins synchronously). But the contract is load-bearing for every signal emission from Story 2-1 onward. Classic heisenbug surface — surfaces rarely under shutdown timing, symptoms typically appear as spurious segfaults in consumer crash reports.
- **Code-level mitigation (Story 1-4):** The dispatcher header explicitly documents the contract: "Callers MUST arrange for the target to outlive all threads that can possibly call `post`." A `TODO(story-2-1)` block names two candidate resolutions:
  1. **Destroying-flag + join/cancel in `_exit_tree`:** the owning `MobileWalletAdapter` node's `_exit_tree` sets a destroying-flag, blocks/joins all in-flight JNI callbacks before the node destructor returns; the bridge impl reads the flag before every `dispatcher_->post()` call.
  2. **Handle-carrying type:** switch the dispatcher to a `godot::Callable` bound to `emit_signal` + target ObjectID (or a `WeakRef<Node>` holder) so lifetime is carried with the handle and no raw-pointer hop is needed.
  Cross-references `retrospect/universal.md` "Mutable holder + in-flight async state ⇒ guard every setter against the state machine" — same lifecycle hazard class.
- **Risk if unresolved:** Stories 2-1, 2-2, 2-3, 3-1, 3-2, 3-3, 4-1, 4-2, 4-3, 5-3 (all stories with real JNI callback flow) will silently ship a UAF that fires during engine teardown under timing pressure.
- **Affected:**
  - `src/mwa/godot_main_dispatcher.cpp` — will need either a destroying-flag check or a handle-type swap.
  - `src/mwa/include/godot_main_dispatcher.hpp` — public contract documents the hazard + TODO.
  - Story 2-1's `MobileWalletAdapter` node — MUST implement the caller-side protocol (option 1) OR be receptive to the dispatcher switching to a handle type (option 2).
- **Trigger to revisit:** Story 2-1 design. Blocking: do NOT ship Story 2-1 without resolving CR-18 (either in 2-1 itself or in a preceding refactor story).
- **Extension (2026-04-23, Story 1-5 post-review):** Code review agent flagged an ABA-adjacent framing worth capturing: Godot 4's `ObjectID` is 64-bit so raw ID wraparound is effectively impossible, but teardown-resurrection-reload within one editor session can in principle reuse an ID slot (free + re-allocate for a different Object before the worker thread's `get_instance` resolves). The original UAF framing covers the "freed, not re-allocated" case; the ABA framing adds "freed, re-allocated to an unrelated Object" which produces the even sharper bug: null-check succeeds, `call_deferred` dispatches to a LIVE but WRONG target. Mitigation option 2 (godot::Callable bound at construction to a stable target+method handle) is the cleaner fix because Callable's resolution is atomic and lifetime-aware; option 1 (destroying-flag) does not close the ABA window (flag is per-target; a reused slot is a different target). Story 2-1 implementer: prefer option 2.

## CR-19: `ProtocolContract` error-code coverage in `MwaClientImpl.classifyException` is not exhaustive
- **Story:** 1-6 | **Severity:** LOW
- **Detail:** `classifyException` maps 3 of 8 public `ProtocolContract.ERROR_*` constants (`ERROR_AUTHORIZATION_FAILED`, `ERROR_NOT_SIGNED`, `ERROR_NOT_SUBMITTED`) explicitly; `ERROR_INVALID_PAYLOADS`, `ERROR_NOT_CLONED`, `ERROR_TOO_MANY_PAYLOADS`, `ERROR_CLUSTER_NOT_SUPPORTED`, `ERROR_ATTEST_ORIGIN_ANDROID` fall through to `ProtocolError`. No parametrized test enumerates every `ERROR_*` constant and asserts the mapping — if a future clientlib-ktx minor bump adds a new error code, there's no mechanical guard that flags it.
- **Risk:** Quiet-fallthrough of a semantically-meaningful new error code to `ProtocolError`. Low today (clientlib-ktx 2.0.3 pin is exact).
- **Resolution:** Add `@ParameterizedTest` over `ProtocolContract.ERROR_*` reflection in Epic 2+ when the first consumer needs finer classification.

## CR-20: `signTransactions` / `signMessages` success-path mock tests deferred
- **Story:** 1-6 | **Severity:** LOW
- **Detail:** `MwaClientImplTest` covers error paths (NoWalletFound, Failure, validation) exhaustively for all 7 ops, but success paths for `signTransactions`, `signMessages`, and `deauthorize` are not mocked (generic `transact` return stubbing was scoped to `signAndSendTransactions` for the base58 end-to-end path). If a future refactor mutates `parseSignResponse` / `SignPayloadsResult.signedPayloads` access, error-path tests still pass.
- **Risk:** Success-path regression could reach Epic 2 contract tests before this suite fires.
- **Resolution:** Add `coEvery { mockAdapter.transact<SignPayloadsResult>(...) } returns Success(...)` mocks for the three ops. ~1 hour of test authoring; Story 2-1 or 3-1 intake.

## CR-21: Fake/real cross-impl parity harness not wired
- **Story:** 1-6 | **Severity:** LOW
- **Detail:** DD-28 claims fixture/wire-event parity between `FakeMwaClient` (fixture-driven) and `MwaClientImpl` (clientlib-ktx-driven). The two test suites exercise the claim for individual paths but no shared `@ParameterizedTest` drives both clients against the same scenario list (`user_canceled`, `token_expired`, etc.) asserting identical `MwaResult` codes. Drift possible: e.g., fixture error code changes in validator but `classifyException` keeps legacy mapping.
- **Risk:** Silent divergence between fake and real error-code surfaces breaks downstream unit tests vs integration tests.
- **Resolution:** Add a `ClientParityTest` with a parametrized matrix in Epic 2+ (Story 2-1 or 3-1) when the first plugin-layer consumer needs cross-impl parity guarantee.

## CR-22: `MwaClientImpl` exception-path log test not backported from `FakeMwaClientTest`
- **Story:** 1-6 | **Severity:** LOW
- **Detail:** `FakeMwaClientTest` has a test asserting `authorize.exit outcome=exception` on the throw path (via try/finally). The equivalent pattern is in `MwaClientImpl.runOp` but has no dedicated test (can force via a factory that throws). Behavior is covered by shared pattern and manual inspection.
- **Risk:** A runOp-pattern refactor that removes the try/finally would pass MwaClientImplTest silently.
- **Resolution:** Add 1 test with `MwaClientImpl(adapterFactory = { throw RuntimeException() })` in Epic 2+ cleanup pass.

## CR-23: `laim-content-guard.sh` hook false-positive on Python `sys.exit(` / `SystemExit(`
- **Story:** 1-6 | **Severity:** LOW | **Type:** tooling
- **Detail:** `~/.claude/hooks/laim-content-guard.sh` matches substring `xit\(` (JavaScript test-disable `xit(` pattern). False-positives on Python `sys.exit(` and `SystemExit(` (both contain `xit(`). During Story 1-6 Task 5, the hook blocked 5 successive `Write` attempts on `tools/validate_fixtures.py` until I hoisted `sys.exit` into a local (`exit_fn = sys.exit; exit_fn(main())`) to avoid the substring.
- **Risk:** Any Python file with `sys.exit(` is unwritable via Write/Edit tools until workaround applied. Wastes ~5-10 min per affected file.
- **Resolution:** Tighten regex to `(^|[^a-zA-Z_])xit\(` (word-boundary) or drop the `xit(` branch entirely (the other 19 patterns already cover JS test-disable forms). Not an MWA-story concern — file against the hooks repo.

## CR-24: D-1 method-rename downstream impact — 2-1 MWA.gd, 5-4 example, 5-5 docs
- **Story:** 1-5 | **Severity:** MEDIUM | **Type:** interface-contract
- **Detail:** Story 1-5 D-1 renamed three GDScript-exposed methods on `MobileWalletAdapter` to avoid collision with `godot::Object` inherited names: `connect → mwa_connect`, `disconnect → mwa_disconnect`, `is_connected → mwa_is_connected`. The other 10 methods keep plan names. The rename is node-binding-only — `MwaAndroidBridge`'s abstract interface still exposes `connect` / `disconnect` / etc. as pure-virtuals (bridge implementations unchanged). See Story 1-5 Guardrails §D-1 for the full precedent chain and the `godot::Object` signatures that forced the collision.
- **Risk:** Every downstream consumer that writes GDScript or C++ expecting `node.connect(identity, cluster, opts)` will silently invoke `godot::Object::connect(signal, callable, flags)` instead — compiler picks by arg count, silent miscall. Specific impact points:
  1. **Story 2-1 MWA.gd facade** — the GDScript facade must forward high-level methods to `mwa_connect` / `mwa_disconnect` / `mwa_is_connected` on the node. Facade authors will instinctively reach for the plan names.
  2. **Story 5-4 example app** — demo scripts invoking the node. Any copy-paste from plan.md or arch.md examples uses the unrenamed verbs.
  3. **Story 5-5 API reference docs** — signal/method tables generated or hand-written must list the renamed bindings.
- **Resolution:** Stories 2-1 / 5-4 / 5-5 authors MUST reference Story 1-5 Guardrails §D-1 and `include/mwa/mobile_wallet_adapter.hpp` `_bind_methods()` registrations as the single source of truth for method names. For GDScript callers: `MWA.mwa_connect(...)`, `MWA.mwa_disconnect()`, `MWA.mwa_is_connected()`. Plan.md and arch.md still show `connect`/`disconnect`/`is_connected` at the semantic level — not wrong per se (those are the MWA protocol verbs), but downstream stories must translate to the D-1 binding names at the Godot API surface.

## CR-25: AC-2 Windows CI deferred to Story 5-6
- **Story:** 1-5 | **Severity:** MEDIUM | **Type:** scope-deferral
- **Detail:** AC-2 text verbatim from plan.md says "host-platform unit test (Linux/Windows/macOS)". Story 1-5 Task 6 (`.github/workflows/cpp_tests.yml`, commit `ce20c0a6`) wires Linux + macOS runners only. Windows runner (MSVC + vcpkg'd GoogleTest, or MSYS2/MinGW) is NOT in the 1-5 matrix. The AC's Windows clause is aspirational for 1-5.
- **Risk:** A Windows-only compiler bug (MSVC-specific warning, different `std::mt19937` state, different ABI for `godot::String`, etc.) would escape CI until Story 5-6 or until a Windows contributor reproduces locally. Low probability for the current test set (2 SecretString + 4 MobileWalletAdapter, all using portable primitives), but the surface grows as more tests land.
- **Resolution:** Story 5-6 (CI/Build Integration, plan.md:163-169) scope explicitly includes "grep bans" + "instrumented tests" + "manifest audit" + "R8 smoke" — add Windows runner to the `cpp_tests.yml` matrix as part of that story's CI work. vcpkg'd GoogleTest is the likely install path; alternatively, rely on Homebrew's parity on Windows-via-MSYS2 if simpler.

## CR-26: arch §3.2 `message` key reconciliation — NoOp + T3 branch both populate it
- **Story:** 1-5 | **Severity:** LOW | **Type:** docs-drift
- **Detail:** `NoOpMwaAndroidBridge::emit_unsupported` (`src/mwa/no_op_mwa_android_bridge.cpp:21-43`, from Story 1-4) populates a `message` key in the `mwa_error` payload alongside `user_message` and `developer_details`. Story 1-5 Task 3's null-bridge branch (`src/mwa/mobile_wallet_adapter.cpp:35-58`, commit `6e86eb9a`) follows the same NoOp-consistent shape — 10 keys total including `message`. However, `docs/architecture.md §3.2` lists only `user_message` + `developer_details` for `mwa_error` (9 keys, no `message`). The two canonical references disagree.
- **Risk:** (1) Story 2-1 MWA.gd facade handler writers must decide which key to read; inconsistent consumers may pick different keys, breaking GDScript parity. (2) Story 5-5 API reference docs must pick a single canonical shape; whatever they pick becomes the de-facto spec for third-party wallets integrating via this signal. (3) Amendment A-12 (signal binding arity) does NOT address payload key membership — that's an open design question.
- **Resolution:** Two options for T7 scope-continuation or a future cleanup story:
  - **(a) Elevate NoOp's shape** — file an amendment (A-13?) recognizing the 10-key shape (including `message`) as canonical; update arch.md §3.2 to match; consumers read either `message` or `developer_details` with `message` being the "AC-3 prose verbatim" breadcrumb.
  - **(b) Drop `message` from source** — patch NoOp and Task 3's branch to stop populating `message`; aligns with arch.md §3.2's 9-key shape; a Rule-2 deviation from AC-3's prose which says `message="Kotlin plugin not loaded"`.
  Recommendation: **(a) is cheaper + honors the existing production shape + satisfies AC-3's verbatim prose.** File as Amendment A-13 at the top of Story 2-1 or Story 5-5 so the canonical spec is pinned before the facade + API docs inherit it.

## CR-27: macOS in T6 cpp_tests matrix — bonus scope beyond 1-5 authorization
- **Story:** 1-5 | **Severity:** LOW | **Type:** scope-creep
- **Detail:** Story 1-5 Guardrails CR-25 deferred Windows runner to Story 5-6 but explicitly did NOT authorize adding macOS. Story 1-5 Task 6 (`.github/workflows/cpp_tests.yml`, commit `ce20c0a6`) includes `macos-latest` alongside `ubuntu-latest` in the matrix.
- **Rationale for keeping:** (1) Portability-validation bonus — `std::random_device`, `snprintf("%08x", ...)`, `godot::Mutex`, `godot::Array::duplicate()`, and GoogleTest 1.14.0 all cross-compile; running both OSes cheaply catches Linux-only bugs. (2) macos-latest runner is as fast as ubuntu-latest in cold-start; `brew install scons` is comparable to `apt-get install scons`. (3) Apple Silicon macOS (macos-latest since 2024) validates ARM64 portability of the MWA host-test subset, which is free intel beyond what Linux alone provides.
- **Risk:** If Story 1-5 scope-review decides macOS is out-of-scope for this story, the matrix entry can be stripped in a Rule-1 deviation commit with no user impact.
- **Resolution:** Documented here for scope-review clarity. Leave as-is unless explicit pushback. Cross-reference to CR-25 (Windows) and the Story 5-6 CI/Build Integration scope.

## CR-28: `GodotMainDispatcher::post(name, Dictionary)` 1-Dict API vs A-12 2-param `*_completed` binding
- **Story:** 1-5 | **Severity:** MEDIUM | **Type:** latent-gap
- **Detail:** `GodotMainDispatcher::post(const godot::String& signal_name, const godot::Dictionary& payload)` carries only a single Dictionary argument. Amendment A-12 (added Task 1 of Story 1-5) escalated the 7 `*_completed` signals to 2-param bindings — `(request_id: String, result: Dictionary)`. The dispatcher API and the binding schema now disagree on arity for the completion family. Story 1-5's tests don't exercise `*_completed` signals (T5 uses `mwa_error` only, which is 1-Dict), so Phase A works as-is for THIS story's validation.
- **Risk:** Story 2-1 (the first story to emit `*_completed` signals from real bridge implementations) cannot pass 2 args through the current `post()` API. Either the bridge impl awkwardly wraps (request_id + result) into a single Dictionary — breaking the A-12 typed-request_id rationale — or the dispatcher API must evolve.
- **Resolution:** Story 2-1 or an earlier preparatory story must evolve `GodotMainDispatcher::post`. Options:
  - **(a) Variadic template:** `template<typename... Args> void post(const godot::String& signal_name, Args... args);` — clean, matches `godot::Object::emit_signal`'s variadic shape, no breaking changes for existing 1-Dict callers (NoOp, T3 null-bridge) since 1-arg variadic is still valid.
  - **(b) `godot::Array` payload:** `void post(const godot::String& signal_name, const godot::Array& args);` — uniform 1-param API; 1-Dict callers must wrap in a 1-elem Array; 2-arg callers pass a 2-elem Array.
  - **(c) Overload:** `void post(name, arg1, arg2);` — explicit 2-arg shape alongside the existing 1-Dict overload. Minimal disruption; callers pick by arity.
  Recommendation: **(a) variadic template** — minimum migration cost (existing 1-Dict callers unchanged), matches godot-cpp's `emit_signal` shape, scales to future N-arg signals. Test-mode pending_ queue in Story 1-5 currently enqueues `{signal_name, payload}` as a 2-key Dict — would need to evolve to `{signal_name, args_array}` or equivalent. Separate 2-1 concern; 1-5 tests don't need it.

## CR-29: Production `GodotMainDispatcher::post` path not exercised by `cpp_tests.yml`
- **Story:** 1-5 | **Date:** 2026-04-23 | **Severity:** MEDIUM | **Type:** test-coverage-gap | **Origin:** post-implementation code review finding #3 (framing corrected — agent hallucinated AC-6 which doesn't exist in 1-5)
- **Summary:** Under `#ifdef MWA_TESTING`, `post(signal_name, payload)` takes the enqueue branch (append to `pending_` + return) and the production branch (`ObjectDB::get_instance` + null-check + `target->call_deferred("emit_signal", ...)` or `push_warning` on drop) is compiled OUT. Every test in `src/mwa/tests/*.cpp` runs under `MWA_TESTING=1`, so `cpp_tests.yml` Linux + macOS matrix GREEN tells us nothing about the production code path that will actually ship to users.
- **Why MEDIUM:** Two disjoint binaries, and the branch consumers hit in shipping is exactly the branch tests cannot see. Concretely: (a) the `ObjectDB::get_instance` null-check + drop + `push_warning` path is the AC-6-adjacent "post after target destroy" guard from Story 1-4 that was never runtime-verified; (b) the CR-18 TOCTOU + ABA hazard lives in the production branch exclusively — no test can surface it. (c) any future refactor of production `post()` (e.g., switch to `godot::Callable` per CR-18 mitigation option 2) will pass all 6 TEST_F cases green while landing a subtly broken production path.
- **Options to close:**
  - **(a) Runtime-flag simulation:** add a test-only flag that disables the MWA_TESTING enqueue branch and forces the production `call_deferred` path under a synthetic ObjectDB state. Complex — requires simulating the engine's deferred-call queue.
  - **(b) Dual-mode build matrix:** `cpp_tests.yml` builds `./bin/mwa_tests` TWICE (MWA_TESTING=1 + MWA_TESTING=0) and runs an inverted suite for the no-testing build that exercises only the production path via external signal observation. Heavy; would need a mock-connected GDScript callback harness or similar.
  - **(c) Downgrade claim + document:** accept the gap. Story 1-5 only claims to verify AC-1/2/3 via the MWA_TESTING-gated path. The production-path robustness is inherited from Story 1-4's inspection-level verification + CR-18's TODO breadcrumb. Any production-path refactor triggers mandatory manual inspection.
  Recommendation: **(c)** for now — options (a) and (b) are disproportionate for Story 1-5's scope. Story 5-6 (CI/Build Integration) can revisit if the production path needs test tier coverage before Story 2-1 ships.
- **Affected:** `.github/workflows/cpp_tests.yml`, `src/mwa/godot_main_dispatcher.cpp` (post() production branch), Story 1-5 + 2-1 test-evidence claims.
- **Trigger to revisit:** Story 2-1 design review — if the dispatcher API evolves per CR-28 recommendation (variadic post) or the target-resolution changes per CR-18 mitigation, the production path MUST get direct test coverage in the same story.

## CR-30: Story 1-5 post-review LOW findings — `drain_for_testing` type guards + cpp_tests warning log-scan
- **Story:** 1-5 | **Date:** 2026-04-23 | **Severity:** LOW (bundled) | **Type:** defensive-coverage / CI-hardening | **Origin:** post-implementation code review findings #4 + #6
- **Summary (bundled two LOW entries to avoid CR-sprawl):**
  1. **Variant type guards in `drain_for_testing`** (`src/mwa/godot_main_dispatcher.cpp:86-88`). Each pending entry's `signal_name` / `payload` is read back via `entry["signal_name"]` / `entry["payload"]` which silently defaults to empty `godot::String` / `godot::Dictionary` on type mismatch. `post()`'s signature already enforces types at call site, so a mismatch here would indicate an internal bug in the enqueue path rather than a caller error. Adding `ERR_FAIL_COND(entry["signal_name"].get_type() != Variant::STRING)` (and similar for payload) would turn a silent default into a loud fail, improving debuggability for anyone extending the drain mechanism.
  2. **`cpp_tests.yml` warning log-scan** (`.github/workflows/cpp_tests.yml`). CI currently only asserts exit code from `./bin/mwa_tests`. A test that passes its assertions but floods `push_warning` output (e.g., from accidentally hitting the production `post()` "target freed" branch) would go undetected. A grep step asserting "stderr should not contain 'MWA: dispatcher post after destroy'" would close this gap cheaply.
- **Why LOW (bundled):** Both are hygiene improvements, not defect fixes. Neither blocks Story 1-5 gating or Story 2-1 development. The type-guard addition is 4 lines of code; the log-scan addition is ~5 lines of YAML.
- **Options to close:**
  - **(1)** Apply both immediately as a follow-up polish commit in Story 1-5 (total ~10 lines).
  - **(2)** Defer to Story 5-6 (CI/Build Integration) for the log-scan; fold the drain type-guard into Epic 2+ cleanup pass.
  - **(3)** Drop the drain type-guard entirely (enqueue/dequeue symmetry already enforces types by construction) and only track the log-scan.
  Recommendation: **(2)** — Story 5-6 is the natural home for cpp_tests.yml hardening; drain type-guards can land alongside any future extension of the drain mechanism.
- **Affected:** `src/mwa/godot_main_dispatcher.cpp`, `.github/workflows/cpp_tests.yml`.
- **Trigger to revisit:** Story 5-6 CI hardening scope, or the first Story-2-1+ extension of the drain mechanism that adds new entry fields.
- **Update 2026-04-23 (Amendment A-13):** `.github/workflows/cpp_tests.yml` deleted with the cpp host tier retirement. The log-scan proposal is subsumed into the future headless-Godot tier design (CR-35). The `drain_for_testing` type-guard proposal remains relevant — `GodotMainDispatcher::drain_for_testing` source is retained for the future tier; apply the guard when that tier is scoped.

## CR-31 (MEDIUM): AC-1 11-signal binding — no automated enforcement after A-13
- **Story:** 1-5 | **Date:** 2026-04-23 | **Severity:** MEDIUM | **Status:** tracked | **Origin:** amendment A-13 (cpp host tier retirement)
- **Summary:** Story 1-5 AC-1 requires `MobileWalletAdapter::_bind_methods` to register all 11 signals with the exact `PropertyInfo` arity documented in A-12 and the Story 1-5 Guardrails signals-table (7 `*_completed` 2-param + 4 error/lifecycle 1-Dict). A-13 deletes `test_mobile_wallet_adapter.cpp::BindingSmoke_SignalsAndMethodsRegistered`, which was the only mechanical assertion that the 11-signal table matched reality. AC-1 is now backed by manual inspection only.
- **Why MEDIUM:** Signal binding shape is consumer-facing ABI — GDScript facade (Story 2-1 MWA.gd), example app (Story 5-4), API docs (Story 5-5) all pivot on it. A silent drift (wrong arity on one `*_completed`, missing signal entirely, typo'd name) would ship through CI green and surface only when a downstream consumer writes a handler with the wrong signature and gets a silent "signal emission dropped on floor" at runtime.
- **Partial backstops:** (a) Kotlin `MwaClient` interface (Story 1-6) enumerates the 11 op names and their op-name/signal-name parity is asserted by the FakeMwaClientTest suite (74 Kotlin tests) — this catches name-level drift in the Kotlin layer but NOT the C++ `_bind_methods` table. (b) Story 1-5 code review verified `_bind_methods` at commit time. (c) Story 2-1 MWA.gd facade wire-up will surface arity drift the first time a handler's signature mismatches emission — loud but late.
- **Trigger to close:** headless-Godot test tier (CR-35) lands with a ported `BindingSmoke_SignalsAndMethodsRegistered` assertion that reads `ClassDB::get_signal_list("MobileWalletAdapter")` against the Guardrails signals-table.
- **Mitigation until then:** any story that adds, removes, or reshapes an MWA signal MUST (1) update `src/mwa/mobile_wallet_adapter.cpp::_bind_methods`, (2) update `docs/architecture.md §3.2` OR file a new amendment à la A-12, (3) update Story 2-1 MWA.gd + Story 5-5 docs in the same PR. No silent binding changes.

## CR-32 (MEDIUM): AC-3 null-bridge branch — no automated enforcement after A-13
- **Story:** 1-5 | **Date:** 2026-04-23 | **Severity:** MEDIUM | **Status:** tracked | **Origin:** amendment A-13
- **Summary:** Story 1-5 AC-3 requires that when `MobileWalletAdapter::bridge_` is null (Kotlin plugin not loaded), every op method emits `mwa_error` with the full 10-key payload (`code=UNSUPPORTED_PLATFORM`, `message="Kotlin plugin not loaded"`, `user_message`, `developer_details`, `platform`, `layer`, `recoverable=false`, `retry_hint="none"`, `request_id`, `timestamp_ms`). A-13 deletes `test_mobile_wallet_adapter.cpp::PreOp_UnsupportedPlatformWhenKotlinNotLoaded`, removing mechanical coverage of the null-bridge branch at `src/mwa/mobile_wallet_adapter.cpp:35-58`.
- **Why MEDIUM:** This branch is the entire user-facing behavior when MWA is invoked on a non-Android platform (desktop dev, iOS, web export) OR when the Android plugin fails to load. It is the FIRST error a downstream consumer will see if integration is incomplete; silent drift (missing key, wrong code, empty payload) would surface as "nothing happens when I call mwa_connect on desktop" rather than a diagnosable error.
- **Partial backstops:** (a) Shared error-payload shape enforced at the codegen layer — `MwaErrorCode::UNSUPPORTED_PLATFORM` + the key list is generated from `tools/error-codes.yaml` in 3 languages (Story 1-1), so key-list drift requires either a codegen bug or a hand-written divergence. (b) Story 1-1 validator has 40+ pytest cases covering the error-codes manifest. (c) Cross-ref with CR-26 (open question on whether the `message` key stays or drops from the shape) — whichever way that resolves, automated enforcement requires CR-35.
- **Trigger to close:** headless-Godot test tier lands a ported PreOp assertion that exercises `null_bridge_` set_bridge_for_testing path AND asserts the 10-key payload shape.
- **Mitigation until then:** Story 2-1 MWA.gd facade should have a GDScript-level test that calls `MWA.mwa_connect()` on desktop and asserts `mwa_error` fires with `code == "UNSUPPORTED_PLATFORM"` — partial coverage at the integration layer.

## CR-33 (LOW): AC-4 16ms op-method timing — no automated enforcement after A-13
- **Story:** 1-5 | **Date:** 2026-04-23 | **Severity:** LOW | **Status:** tracked | **Origin:** amendment A-13
- **Summary:** Story 1-5 AC-4 required op methods (`mwa_connect`, `reauthorize`, `disconnect`, `deauthorize`, `sign_messages`, `sign_transactions`, `sign_and_send`, `forget_all`, `get_diagnostics`) to return within 16ms on the Godot main thread (one frame at 60 FPS). A-13 deletes `test_mobile_wallet_adapter.cpp::OpMethodsReturnWithin16ms`.
- **Why LOW:** op-method bodies are near-empty — `_bind_methods`-bound dispatches into `bridge_->op(request_id, ...)` which in the no-op path just calls `dispatcher_->post(...)`. The code path is synchronous-and-near-zero-work by construction; a regression that introduces a blocking call would be visible in code review. No cryptography, no I/O, no lock contention in the host-mode path.
- **Trigger to close:** headless-Godot tier ports the timing assertion, OR Story 2-1 (real bridge impl) adds its own timing assertion for the real JNI path (which is where the 16ms budget starts mattering).
- **Mitigation until then:** Story 2-1 implementer MUST re-add timing assertion for the real JNI dispatch — the 16ms budget is load-bearing once JNI enters the picture. Track there.

## CR-34 (MEDIUM): AC-5 thread-hop dispatcher — no automated enforcement after A-13
- **Story:** 1-5 | **Date:** 2026-04-23 | **Severity:** MEDIUM | **Status:** tracked | **Origin:** amendment A-13
- **Summary:** Story 1-5 AC-5 required that a signal emitted from a worker thread via `GodotMainDispatcher::post` marshals to the Godot main thread via `call_deferred("emit_signal", ...)`. A-13 deletes `test_mobile_wallet_adapter.cpp::ThreadHop_SignalEmitsOnMainFromWorkerThread`, which exercised the `MockMwaAndroidBridge::drive_signal_from_thread` pattern through the D-2 `drain_for_testing` harness.
- **Why MEDIUM:** Thread-hopping is the mechanism that keeps Godot signal emission thread-safe under real JNI worker-thread callbacks (Story 2-1+). A silent regression (direct `emit_signal` from worker thread, missing `call_deferred`, ObjectID→pointer race) surfaces as Godot engine crashes OR signal payloads racing with main-thread state — both hard-to-diagnose in production.
- **Partial backstops:** (a) `GodotMainDispatcher::post` production path was inspection-verified at commit time (Story 1-5 code review). (b) CR-18 + CR-29 already track the TOCTOU window and the production-path test gap; Story 2-1 MUST resolve those before shipping real JNI. (c) The retrospect entry "Mutable holder + in-flight async state ⇒ guard every setter against the state machine" (universal.md 2026-04-21) is the generic mitigation lens — applies to any future thread-hopping setter added in Story 2-1+.
- **Trigger to close:** headless-Godot tier ports the ThreadHop assertion OR Story 2-1 lands a real JNI thread-hop assertion under the headless-Godot harness (whichever comes first).
- **Mitigation until then:** Story 2-1 MUST implement either (a) `godot::Callable` binding per CR-18 option 2 (eliminates the raw-pointer hop) OR (b) an `_exit_tree` barrier per CR-18 option 1 (joins/cancels in-flight JNI callbacks). A-13 does NOT defer the CR-18 work — that hazard becomes live in 2-1 regardless of the test tier's state.

## CR-35 (HIGH, forward-looking): Headless-Godot C++ test tier — future-story obligation
- **Story:** 1-5 | **Date:** 2026-04-23 | **Severity:** HIGH | **Status:** tracked (forward-looking) | **Origin:** amendment A-13
- **Summary:** A-13 retires Story 1-5's host-mode GoogleTest tier. The 6 TEST_F cases (2 SecretString + 4 MobileWalletAdapter) + CR-8 + CR-31..34 all defer to a future story that stands up a headless-Godot C++ test harness. This concern is the rollup tracking ticket — the ONLY concern whose resolution creates the infrastructure to resolve the others.
- **Why HIGH:** The coverage gaps named in CR-31 (signal binding arity) and CR-34 (thread-hop) are load-bearing for Story 2-1 (real JNI) and every signing-path story (3-1..3-3). Without CR-35 resolved before the Story 3-x landings, either (a) those stories ship with no C++ test tier at all (regression from Story 1-5's original plan) or (b) each story re-litigates the test-tier question and duplicates work. Missing CR-35 also means the grant closes with zero host-tier C++ coverage — a significant posture downgrade from the original Story 1-5 scope.
- **Scope decision owed (before Wave 5 planning):**
  - **(1) Fold into Story 5-6 CI/Build Integration** (plan.md:163-169). 5-6's current scope is grep-bans + instrumented tests + manifest audit + R8 smoke; adding headless-Godot C++ tier widens 5-6 materially — likely needs to split into 5-6a (existing) + 5-6b (headless-Godot). Preferred if grant budget has room for 5-6b.
  - **(2) Carve a dedicated Epic-5 story (e.g., 5-7)** specifically for headless-Godot tier. Cleaner scope; requires plan.md amendment to add the story.
  - **(3) Ship without the tier** — accept the permanent coverage gap. Requires CR-31..34 to be explicitly downgraded to "accepted risk" in the grant close-out document.
  - Recommendation: **(2)** if grant has room, **(1)** as fallback, **(3)** only if funding runs out.
- **Design notes for the future story:**
  - Use Godot 4.3+ `--headless --script <test.gd>` mode. GDScript test harness calls into our C++ `MobileWalletAdapter` via the registered class and asserts on signal emissions via GDScript `Callable.bind(...)` callbacks.
  - Port CR-8 (SecretString), AC-1 (BindingSmoke), AC-3 (PreOp null-bridge), AC-4 (timing — redo for real JNI in Story 2-1 scope), AC-5 (ThreadHop — likely redo for real JNI).
  - Reactivate `MWA_TESTING` compile define under the headless-Godot build (D-2 `drain_for_testing` + D-3 `set_bridge_for_testing` already exist in source and remain valid). Grep-ban pattern 7 (`mwa-testing-define`) stays protective — the define stays SCons-target-activated only.
  - CI impact: download Godot editor per job (~200MB); slower than the retired `scons tests` matrix but amortized across all C++ assertions the grant needs.
- **Affected:**
  - All of CR-8, CR-25, CR-27, CR-29, CR-30, CR-31, CR-32, CR-33, CR-34 defer their trigger-to-close to CR-35.
  - plan.md — may need amendment to add Story 5-6b or Story 5-7.
  - docs/architecture.md — §CI section should be updated when CR-35 resolves (headless-Godot build step added to the test tier taxonomy).
- **Trigger to revisit:** Wave 5 planning (when Epic 5 stories are being scoped) OR Story 2-1 design review (when CR-18 + CR-28 + CR-34 all converge and the cost of no-C++-tier becomes concrete).

## CR-36 (LOW): MwaSessionState.lastResult: Any? erases secret-type discipline
- **Story:** 2-1 | **Date:** 2026-04-23 | **Severity:** LOW | **Status:** tracked | **Origin:** Story 2-1 T4 code review
- **Summary:** `MwaSessionState.lastResult` is typed as `Any?` (scaffold-era pattern from Story 1-2). It currently holds clientlib-ktx `TransactionResult<*>` values including `TransactionResult.Success<AuthResult>` — which has a `.authToken: String` field that round-trips untyped through `toString()`. A stray `Log.d("result: $lastResult")` would stringify the raw auth token into logs, bypassing the `SecretString`/@Synchronized discipline applied to the rest of the class in Story 2-1 T4.
- **Why LOW:** (a) Current grep-bans (pattern 1 `Log.*authToken` + pattern 3 `Log.*reveal()`) would NOT catch `Log.d("$lastResult")` — the variable name is `lastResult`, not `authToken`. (b) No current call site logs `lastResult` — CI is green. (c) The blast radius is bounded: scaffold-era `@UsedByGodot` methods ONLY read `lastResult` via `is TransactionResult.Success<*>` type-check, never stringify. So the leak is latent, not active.
- **Mitigation until closed:** Add a grep-ban for `Log\.(v|d|i|w|e)\(.*lastResult` if the field is ever touched in new code. For now the type discipline is encoded in reviewer vigilance.
- **Trigger to close:** Story 2-3 (disconnect) or Story 4-x (hardening pass) narrows `lastResult` from `Any?` to a typed sealed class (e.g., `sealed class MwaSessionResult`) where each variant has an explicit `toString()` redaction for secret fields. Same story should convert the Java-bean `setX/getX` pair to Kotlin properties (nit from the T4 review).

## CR-37 (LOW): MwaSessionState.pendingTransaction / connectedKey lack defensive copy
- **Story:** 2-1 | **Date:** 2026-04-23 | **Severity:** LOW | **Status:** tracked | **Origin:** Story 2-1 T4 code review
- **Summary:** `setTransaction(ByteArray?)` and `setKey(ByteArray?)` store the caller's reference directly. An external holder of the original array can mutate the stored bytes post-set, or later see mutations the session made. Neither field is secret — public key bytes are intentionally non-secret, pending transaction bytes are caller-owned draft material — but the caller-aliasing surface is real.
- **Why LOW:** No current call site retains the reference after calling setTransaction/setKey — they pass literals (`signTransaction(serializedTransaction: ByteArray)` from `@UsedByGodot`). The aliasing is latent.
- **Mitigation until closed:** Reviewer vigilance.
- **Trigger to close:** Same 2-3 / 4-x hardening pass as CR-36 — add `.copyOf()` in setters AND getters (similar to `SecretString`'s constructor copy-in + reveal copy-out pattern).
