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

## CR-26: RESOLVED in Story 2-1 T10 via Amendment A-14 (2026-04-24)
- Resolution: A-14 elevates the NoOp 10-key shape (including `message`) as canonical; arch §3.2 reference is subordinate to A-14 for this envelope. All emit sites already populate 10 keys (NoOp since 1-4, null-bridge branch since 1-5 T3, JNI paths since 2-1 T5, Kotlin paths since 2-1 T4). CR-32 desktop test asserts the 10-key shape per-frame.
- Evidence: docs/amendments.md §A-14 (10-key table + closure statement); addons/SolanaSDK/mwa/tests/test_unsupported_platform.gd assertion loop over `required_keys`; GDScript test workflow `gdscript_tests.yml` enforces on push.
- Follow-up: CR-40 (LOW) stays open for the Story 1-1 codegen enhancement that will split `message` vs `user_message` content.

## CR-26 (ORIGINAL): arch §3.2 `message` key reconciliation — NoOp + T3 branch both populate it
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

## CR-32: RESOLVED in Story 2-1 T10 via GDScript desktop test (2026-04-24)
- Resolution: `addons/SolanaSDK/mwa/tests/test_unsupported_platform.gd` (headless-Godot main-scene script) loads MWA autoload, calls `MWA.connect(...)` on desktop, asserts `mwa_error{code=UNSUPPORTED_PLATFORM, layer=cpp, recoverable=false}` fires within 10 frames, AND asserts the A-14 10-key envelope. `.github/workflows/gdscript_tests.yml` runs this on every push against a linux-release extension build + the `example/WalletAdapterAndroid/` project (which pre-registers MWA autoload per T7).
- Evidence: test script + scene + workflow; A-14 10-key assertion loop catches any emit-site regression that drops a key.
- Follow-up: CR-35 (headless-Godot tier for broader C++ coverage) still open as the long-term vehicle; CR-32 as a specific AC-3 backstop is closed.

## CR-32 (ORIGINAL): AC-3 null-bridge branch — no automated enforcement after A-13
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

## CR-34: RESOLVED in Story 2-1 T6+T9 via Callable + JNI thread-hop exercise (2026-04-24)
- Resolution: T6 closed CR-18 structurally (production dispatcher path now uses `godot::Callable::call_deferred` — no raw-ptr virtual dispatch across threads). T9 androidTest AC6WatchdogTimeoutTest + AC3ConnectE2ETest exercise the real `Dispatchers.Default` → main-thread dispatcher hop on the emulator. T10 CR-41 barrier closes the only remaining thread-hop hazard (mid-flight callback vs. teardown race). The thread-hop invariant is now covered by: (a) structural closure at the Callable layer, (b) on-device exercise via androidTest, (c) RAII lease barrier on the teardown path.
- Evidence: src/mwa/godot_main_dispatcher.cpp (Callable-based production path); android/plugin/src/androidTest/ (AC3/AC6 exercise the real threaded dispatch); src/mwa/mwa_android_bridge_jni.cpp MwaJniContext::CallbackLease.
- Follow-up: CR-35 rollup remains open for any additional C++ thread-hop assertions future stories might need; CR-34 as the specific Story 1-5 AC-5 backstop is closed.

## CR-34 (ORIGINAL): AC-5 thread-hop dispatcher — no automated enforcement after A-13
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
- **T6 key rename follow-up (CR-T6-1):** Story 2-1 T6 renamed the `MWA_TESTING` `pending_` entry key from `"payload"` (Dictionary value) to `"args"` (Array value) per D-6. Any revived test harness that reads `snapshot_pending_for_testing()` entries MUST use `entry["args"]` + the arity ladder ({1, 2}-element Array). Worth a check when porting Story 1-5 dispatcher tests to the new headless-Godot tier — the schema flip is intentional and load-bearing for A-12 2-param `*_completed` signals.

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

## CR-38 (LOW): JNI string converters use modified UTF-8 (non-BMP mangling latent)
- **Story:** 2-1 | **Date:** 2026-04-23 | **Severity:** LOW | **Status:** tracked | **Origin:** Story 2-1 T5 code review
- **Summary:** `jstring_to_godot_string` / `godot_string_to_jstring` in `src/mwa/mwa_android_bridge_jni.cpp` use `GetStringUTFChars` + `NewStringUTF`, which emit JNI "modified UTF-8" (embedded U+0000 encoded as 0xC0 0x80; supplementary chars U+10000+ encoded as 3-byte surrogate pair sequences). `godot::String` and `godot::JSON` produce/consume standard UTF-8. Feeding modified UTF-8 to `godot::String(const char*)` silently mangles non-BMP input (emoji, most notably).
- **Why LOW:** All current MWA traffic is ASCII at this boundary — request_id (hex), auth_token (base64), public_key (hex/base58), cluster / chainId (enum-like). Payload JSON from Kotlin `JSONObject.toString()` / `godot::JSON::stringify` tends to escape non-ASCII to `\uXXXX` sequences (though this is implementation-dependent). No current code path produces literal UTF-8 non-BMP bytes at this seam.
- **Mitigation until closed:** Kdoc warnings on both converters document the hazard. Ensure any code that might carry non-BMP strings (wallet_label with emoji, user-configurable identity.name) stays in the JSON-escape path.
- **Trigger to close:** First observed non-BMP character in a real wallet response OR Story 5-5 documentation pass for "wallet compatibility matrix" whichever comes first. Fix: switch converters to `GetStringChars`/`NewString` (UTF-16 path via `jchar*` → `char16_t*`) using `godot::String.utf16()`.

## CR-39 (MEDIUM): `GodotMainDispatcher::post_arity2` is transitional T5 helper — T6 removes it
- **Story:** 2-1 | **Date:** 2026-04-23 | **Severity:** MEDIUM | **Status:** tracked | **Origin:** Story 2-1 T5 code review
- **Summary:** T5 adds `GodotMainDispatcher::post_arity2(signal_name, request_id, result)` as a 2-arg-emit helper so connect_completed fires with A-12's 2-param GDScript shape. This is a transitional surface — T6's `post(String, Array)` + `{1, 2}` arity ladder per D-6 dissolves both `post` and `post_arity2` into a single unified entry point. Any code path that adds a new caller to `post_arity2` between T5 and T6 creates migration friction.
- **Why MEDIUM:** Mechanical migration (single-call-site today), but the transitional API creates a silent contract: "only the T5 JNIEXPORT `postConnectCompletedNative` path may call this." Without T6 landing promptly, future `*_completed` signals (Stories 2-2 / 2-3 / 3-x) might be tempted to use `post_arity2` and proliferate the transitional pattern.
- **Mitigation until closed:** Kdoc on both `GodotMainDispatcher::post_arity2` declaration and the T5 JNI callsite explicitly mark the helper as "remove in T6" + "do NOT add new callers beyond the T5 connect_completed path."
- **Trigger to close:** T6 landing (next task in Story 2-1). T6 deletes `post_arity2` from the header + impl; the T5 JNI callback migrates to `dispatcher->post(signal_name, godot::Array::make(request_id, result))` per D-6.

## CR-40 (LOW): MwaError.defaultUserMessage backs both `message` and `user_message` in A-14 payload
- **Story:** 2-1 | **Date:** 2026-04-23 | **Severity:** LOW | **Status:** tracked | **Origin:** Story 2-1 T5 code review
- **Summary:** `buildErrorJson` in `GDExtensionAndroidPlugin` currently sets both `message` (AC-3 technical breadcrumb per A-14) and `user_message` (user-facing graceful prompt) to `error.defaultUserMessage`. The generated `MwaError` sealed class (Story 1-1) only exposes `defaultUserMessage`, so the two keys carry identical content — the A-14 shape's split between developer-readable and user-readable messaging is effectively collapsed.
- **Why LOW:** The A-14 10-key shape is present; consumers reading `developer_details` (the canonical dev-facing field) aren't affected. `message` being equivalent to `user_message` is stylistic debt, not a functional bug.
- **Mitigation until closed:** Kdoc in `buildErrorJson` flags the intentional duplication with a CR-40 pointer.
- **Trigger to close:** Story 1-1 codegen adds `defaultTechnicalMessage` to the `MwaError` enum. Story 2-3 (disconnect) / 4-x (hardening) can pick up the follow-up and re-point `buildErrorJson`'s `message` field.

## CR-41: RESOLVED in Story 2-1 T10 via MwaJniContext::CallbackLease barrier (2026-04-24)
- Resolution: `MwaJniContext` now owns an atomic `g_in_flight_callbacks` counter + `g_draining` flag. JNIEXPORT callbacks (`post_arity1`, `post_arity2_completed`) acquire a scoped `CallbackLease` via stack construction before touching the dispatcher pointer. The lease's ctor increments the counter + checks the draining flag; if set OR the pointer is null, the lease returns `dispatcher()==nullptr` and the callback drops cleanly. `unregister_dispatcher()` sets `g_draining`, spin-waits (bounded 200ms at 100µs polling) for the counter to reach zero, then clears the pointer. Net: no lease-holder is ever mid-`post()` when the dispatcher object is destroyed.
- Evidence: src/mwa/mwa_android_bridge_jni.hpp (CallbackLease decl + contract kdoc); src/mwa/mwa_android_bridge_jni.cpp (counter + flag + spin-drain impl; post_arity1 / post_arity2_completed use the lease).
- Bounded-wait trade-off: if a worker thread holds a lease for >200ms during teardown, the spin-drain gives up with a `push_warning` and destroys the dispatcher anyway. This preserves dtor liveness (no infinite hang) at the cost of a residual UAF risk on the worst-case worker. Documented in the code. Acceptable given (a) real callbacks take ≪200ms, (b) the alternative is deadlocking the app teardown.

## CR-41 (ORIGINAL, HIGH, latent): Bridge-dtor teardown race with in-flight JNIEXPORT callbacks
- **Story:** 2-1 | **Date:** 2026-04-24 | **Severity:** HIGH | **Status:** tracked (T10 REQUIRED) | **Origin:** Story 2-1 T6 review
- **Summary:** `MwaAndroidBridgeJni::~MwaAndroidBridgeJni()` calls `MwaJniContext::unregister_dispatcher()` (atomic store → nullptr), but a JNIEXPORT callback thread can already be past the `g_dispatcher.load(acquire)` null-check and mid-`dispatcher->post(...)` when the dtor runs AND the `GodotMainDispatcher` object is subsequently destroyed by its owning `MobileWalletAdapter` node. Structurally identical hazard-class to CR-18 (closed at the target layer via `godot::Callable`); here the hazard is one layer up, at the dispatcher object itself. UAF.
- **Why LATENT in T6:** The race window opens only on a plugin teardown scenario where a Kotlin `DefaultNativeBridge.postConnect*` call has reached the C++ JNIEXPORT, is between `MwaJniContext::get_dispatcher()` returning non-null and `dispatcher->post(...)` completing, AND the owning node is freed. A-13-retired C++ test tier does not exercise the race; no current kotlin test can reach the JNI boundary.
- **Options (ordered by cost):**
  1. Teardown ordering in node dtor: `MobileWalletAdapter::~MobileWalletAdapter` tears the bridge BEFORE dispatcher destroys (reverse of construction). Plus a draining barrier — `unregister_dispatcher()` sets a flag + waits on an atomic in-flight-callback count; JNIEXPORT callbacks increment/decrement around the `post()` call and drop new calls once the flag is set. **Story 2-1 T10 close-out work.**
  2. `std::shared_ptr<GodotMainDispatcher>` in `MwaJniContext` with JNIEXPORT upgrading to shared_ptr before post(). Cleanly kills the race; introduces a new ownership edge across the C++ node / dispatcher boundary.
  3. Pre-destroy signal from Kotlin to C++ to stop emitting callbacks; larger refactor across clientlib + plugin.
- **Mitigation until closed:** None in T6 code itself.
- **Trigger to close:** Story 2-1 T10 MUST land option (1) before Gate 5. The real-JNI path ships in this story; the race is production-reachable the moment a user rotates wallets or the app teardowns between connect and connect_completed. Do NOT defer past Story 2-1.

## CR-42 (LOW): Arity-1 JSON parse-failure loses request_id correlation
- **Story:** 2-1 | **Date:** 2026-04-24 | **Severity:** LOW | **Status:** accepted trade-off | **Origin:** Story 2-1 T6 review
- **Summary:** For JNIEXPORT arity-1 callbacks (`postMwaError`, `postMwaTimeout`, `postMwaCancelledLifecycle`, `postReauthRequired`) the `request_id` lives INSIDE the payload JSON. If that JSON fails to parse, `post_parse_failure_error` emits a fallback `mwa_error` with empty `request_id`. Kotlin's `InflightMap` can't mark the specific request TERMINATED; that request stays PENDING until the Kotlin watchdog fires.
- **Why LOW:** A mangled JSON payload IS itself a symptom of a deeper fault (wire corruption, Kotlin formatter bug, etc.). Recovering `request_id` by substring-matching the mangled bytes would propagate the corruption into `InflightMap` (wrong request marked TERMINATED). Trade-off: preserve global terminal-signal invariant (GDScript always sees a typed `mwa_error`), at the cost of one orphaned in-flight record per corruption event (watchdog cleans it up).
- **Mitigation until closed:** T6 logs the seam label + mangled-byte-length (NOT body — secret material like `auth_token` ciphertext may be present) via `push_warning`. Kdoc on `post_parse_failure_error` forbids future "fixes" that pull a request_id substring out of mangled bytes.
- **Trigger to close:** Wire-format version bump that moves `request_id` to a separate JNI arg on all arity-1 callbacks (parallel to the arity-2 path). Unlikely before Story 4-x hardening pass.

## CR-43 (LOW): No JNI_OnUnload — GlobalRef on plugin companion class leaks if library unloads
- **Story:** 2-1 | **Date:** 2026-04-24 | **Severity:** LOW | **Status:** tracked | **Origin:** Story 2-1 T6 review
- **Summary:** `src/mwa/mwa_android_bridge_jni.cpp::JNI_OnLoad` creates a `NewGlobalRef` on the `GDExtensionAndroidPlugin$Companion` jclass (`g_plugin_companion_class`). There is no paired `JNI_OnUnload` that calls `DeleteGlobalRef`. Library unloads (JVM dlclose-ing the .so) would leak the GlobalRef.
- **Why LOW:** Android runtimes rarely unload libraries — plugins typically live for the full process lifetime. Even if the JVM did unload, process-local GlobalRef dies with the process.
- **Mitigation until closed:** None in T6 code.
- **Trigger to close:** Story 4-x hardening pass OR first observed plugin-reload scenario.

## CR-44 (LOW): NewStringUTF / GetStringUTFChars lack OOM null-check
- **Story:** 2-1 | **Date:** 2026-04-24 | **Severity:** LOW | **Status:** tracked | **Origin:** Story 2-1 T6 review
- **Summary:** `godot_string_to_jstring` / `jstring_to_godot_string` in `src/mwa/mwa_android_bridge_jni.cpp` call `NewStringUTF` / `GetStringUTFChars` without guarding the OOM null-return paths documented by JNI. An edge-case allocation failure could return nullptr jstring; subsequent JNI calls would NPE inside the JVM.
- **Why LOW:** All traffic is short ASCII (request_id, JSON payloads, cluster names). Realistic OOM at these allocation sizes requires adverse heap conditions that would already be manifesting elsewhere in the plugin.
- **Mitigation until closed:** None in T6 code.
- **Trigger to close:** Story 4-x hardening pass OR first field-observed crash trace pointing at these converters.

## CR-39: RESOLVED in T6 (2026-04-24)
- Resolution: `GodotMainDispatcher::post_arity2` transitional helper deleted from header + impl. The T5 JNIEXPORT `postConnectCompletedNative` path migrated to `dispatcher->post(signal_name, godot::Array::make(req_id, result))` per D-6. Grep confirms no remaining callers.
- Evidence: src/mwa/include/godot_main_dispatcher.hpp (post_arity2 declaration removed), src/mwa/godot_main_dispatcher.cpp (post_arity2 body removed), src/mwa/mwa_android_bridge_jni.cpp (post_arity2_completed helper now calls unified `dispatcher->post` with 2-elem Array).

## CR-28: RESOLVED in T6 (2026-04-24)
- Resolution: `GodotMainDispatcher::post` signature evolved to `post(const String& signal_name, const Array& args)` per D-6. Hard-coded arity ladder {1, 2} in both the production `Callable::call_deferred` branch AND the MWA_TESTING `drain_for_testing` branch. Default arm fires `ERR_FAIL_MSG` so out-of-contract arity cannot ship silently. Arity validation added at MWA_TESTING enqueue time per CR-T6-2 review feedback.
- Evidence: src/mwa/godot_main_dispatcher.cpp post() production switch + MWA_TESTING pre-enqueue guard + drain_for_testing switch; all 10 caller sites (NoOp, JNI ops, JNI callbacks, MobileWalletAdapter null-bridge branches, Mock) migrated to `godot::Array::make(...)`.

## CR-18: RESOLVED in T6 (2026-04-24)
- Resolution: Production branch of `GodotMainDispatcher::post` no longer performs `ObjectDB::get_instance(target_id_) → raw-ptr virtual dispatch`. Ctor binds `emit_signal_callable_ = godot::Callable(target, "emit_signal")`; post() calls `emit_signal_callable_.call_deferred(signal_name, args-unpacked)`. godot-cpp's Callable handles target+ObjectID resolution atomically; a freed target produces an internal warning, not a UAF. `target_id_` moved under `#ifdef MWA_TESTING` for `drain_for_testing()`'s synchronous dispatch path (per D-5 + A-13 retain-source clause).
- Evidence: src/mwa/include/godot_main_dispatcher.hpp (emit_signal_callable_ production member; target_id_ under MWA_TESTING), src/mwa/godot_main_dispatcher.cpp (ctor binds Callable; post() uses arity ladder over call_deferred).
- Follow-up: CR-41 tracks the DISPATCHER-LAYER analogue of this race (dispatcher object destroyed mid-JNI-callback); T10 MUST close it before Gate 5.

## CR-29: REFRAMES closed in T6 (2026-04-24)
- Resolution: Production `post` path now reduces to a single `Callable::call_deferred` call whose behavior is verified by godot-cpp's own test suite. No MWA-specific dispatch logic remains in the production branch (aside from the arity ladder, which is a pure switch over `args.size()` with ERR_FAIL_MSG default arm).
- Evidence: src/mwa/godot_main_dispatcher.cpp production switch — 2 cases + default; no ObjectDB lookup, no null-check-then-dispatch.

## CR-45: androidTest Route B does not observe the C++/GDScript signal path on disconnect
- **Story:** 2-3 | **Severity:** LOW
- **Detail:** Story 2-3 T5's `AC1DisconnectRetainsTokenTest` + `AC3DoubleDisconnectTest` use the Route B harness (in-process `FakeMwaClient` via `@VisibleForTesting` ctor, `nativeBridge = mockk(relaxed = true)`). This injects a recording `NativeBridge` BEFORE the Kotlin → JNI seam — so the tests prove Kotlin `mwaDisconnect` calls `postDisconnectCompleted` on the bridge, but they do NOT exercise: (a) the T3 `JNIEXPORT Java_..._postDisconnectCompletedNative` C++ function, (b) the `GodotMainDispatcher::post("disconnect_completed", Array::make(...))` call, (c) GDScript consumers receiving the signal via `MWA.disconnect_completed.connect(...)`. Story 2-1's identical harness carries the same gap for the connect path (not tracked as a separate concern there — Story 2-3's addition makes the pattern worth logging).
- **Risk:** LOW — the C++ JNIEXPORT is a trivial delegation to the proven-working `post_arity2_completed` helper (exercised on device by Story 2-1's AC-3 happy-path connect which DOES reach C++ via the real authorize's signal emission). MWA.gd's signal forwarding is verified by T4 inspection. Combined evidence is strong enough for MVP; closing the gap directly would require either a headless-Godot tier (CR-35) or a real-JNI-exercised androidTest (not currently possible without a real `ActivityResultSender` and wallet intent).
- **Mitigation until closed:** R8 smoke (instrumented_tests.yml REQUIRED_METHODS) asserts `mwaDisconnect` + `postDisconnectCompletedNative` symbols survive minification. T6 GDScript desktop backstop (test_unsupported_platform.gd) exercises the NoOp-bridge side of the signal. Story 2-1's AC-3 connect test proves the real-JNI → GDScript signal path for the `connect_completed` symmetry.
- **Trigger to close:** CR-35 headless-Godot tier lands (earliest Story 5-x CI polish) OR a manual on-device smoke walkthrough documents disconnect_completed reception in a real Godot scene (can ride with Story 3-1 signing-path validation).
- **Candidate test:** Add a single headless-Godot GDScript test that (i) loads the plugin, (ii) connects via a FakeMwaClient fixture, (iii) calls MWA.disconnect(), (iv) asserts MWA.disconnect_completed fired with `source_method == "disconnect"`.

## CR-46: `ed25519` submodule uninitialized — host scons build pre-existing fail
- **Story:** 2-2 (surfaced during T3) | **Severity:** LOW
- **Detail:** `git submodule status` shows the `ed25519` submodule with a leading `-` (`-b1f19fab... ed25519`), meaning the submodule directory is empty / not checked out. Host `scons target=template_release` fails in `src/mwa/wallet_adapter_signer.cpp` (or another non-MWA TU) on `ed25519.h not found`. Pre-existing — confirmed by stash-test during Story 2-2 T3 (reverting T3 produces the identical failure at the same TU). Likely deinitialized by an earlier `git submodule deinit` or never-initialized on the dev machine. The same `build-containers` submodule is also uninitialized but has no impact on the build.
- **Impact:** Host scons build cannot complete on this dev environment; story 2-2 truth line 536 ("Non-Android desktop builds compile + link") cannot be locally verified post-T3. Android gradle build is unaffected — the JNI TU is SCons-guarded Android-only and the Android toolchain pulls ed25519 sources differently (or not at all). CI's Linux/macOS scons matrix may also surface this if the runner doesn't `git submodule update --init` before `scons`.
- **Risk:** LOW for Story 2-2 specifically — T3's symbol is Android-only and fully verified by `./gradlew :plugin:assembleRelease` + Kotlin unit baseline. The story's host-build verification line is moot for the JNI TU. Risk is for OTHER TUs (`wallet_adapter_signer.cpp` from Story 1-3) which the host-build matrix legitimately needs.
- **Mitigation until closed:** Run `git submodule update --init ed25519` once locally before any `scons` invocation. Verify CI workflows include `git submodule update --init --recursive` (or equivalent) before scons steps. T7 close-out should confirm the local host build passes after a manual submodule init, OR amend the story to explicitly scope-fence the host-build verification to non-MWA TUs.
- **Trigger to close:** Either (a) a `.gitmodules` audit + CI workflow check confirms automatic init in all build paths, (b) a documentation update in CONTRIBUTING / dev-setup explicitly calls out the manual submodule init, or (c) Story 2-2 T7 verifies host scons green after `git submodule update --init ed25519` and logs the resolution.
- **Candidate fix:** Single command: `git submodule update --init --recursive`. If this does not resolve, deeper investigation (potentially missing remote / archived submodule URL) needed.

## CR-47: DD-2-2-7 multi-match tie-break uncovered by Story 2-2 tests
- **Story:** 2-2 (surfaced during T5 review) | **Severity:** LOW
- **Detail:** Story 2-2 wired a most-recently-used wallet tie-break in `mwaReauthorize` per DD-2-2-7 (story line 158): when `store.listAllKeys().filter { 3-tuple }` returns 2+ candidates, pick `maxByOrNull { store.getToken(it)?.lastUsedAtMs ?: 0L }`. Neither T1 unit tests nor T5 androidTest exercises the multi-match path — every test seeds exactly one CacheRecord. A regression that swapped `maxByOrNull` to `minByOrNull` (selecting LEAST-recent), or that broke the `lastUsedAtMs` comparison (e.g., string-compare instead of numeric), would slip through Story 2-2's test surface entirely.
- **Risk:** LOW — the realistic case (user authorized to game with Phantom, then later with Solflare on the same cluster, then calls reauthorize) is rare. The implementation uses Kotlin stdlib `maxByOrNull` against a `Long` field, leaving a narrow regression surface (mostly: an editor accidentally renaming `max` to `min`, or a refactor that changes `lastUsedAtMs` semantics).
- **Mitigation until closed:** Implementation correctness verified by inspection (T2 `handleReauthSuccess` discovery branch + DD-2-2-7 spec matching). Production logs would surface a wrong-wallet selection if it happened (the wallet rejects the cached token bytes for a different wallet → AC-3 graceful wipe → user retries connect, sees correct wallet chooser).
- **Trigger to close:** Add a unit-test scenario in `MwaAndroidPluginReauthorizeTest` that seeds 2 CacheRecords with the same 3-tuple but distinct `walletPackage` + distinct `lastUsedAtMs`, calls `mwaReauthorize`, asserts `MwaClient.reauthorize` was invoked with the cached `authToken` from the most-recent record. Or fold into a future story (4-x or CI polish) that adds a parameterized multi-match harness.
- **Candidate test (sketch):**
  ```kotlin
  @Test fun `DD-2-2-7 multi-match picks most-recently-used wallet`() {
      seedPostConnectSession()
      seedCacheRecord(walletPackage = "com.phantom.app", lastUsedAtMs = 1000L)
      seedCacheRecord(walletPackage = "com.solflare.app", lastUsedAtMs = 5000L)
      val capturedToken = slot<SecretString>()
      every { mockClient.reauthorize(any(), any(), capture(capturedToken), any(), any()) } returns ...
      plugin.mwaReauthorize("rea-mm", identityJson, "devnet", "solana:devnet", 5000L)
      // Assert capturedToken.captured matches the Solflare seed (max lastUsedAtMs).
  }
  ```

## CR-48: DD-2-2-5 token rotation branch uncovered by Story 2-2 automated tests
- **Story:** 2-2 (surfaced during T5 review) | **Severity:** LOW
- **Detail:** `handleReauthSuccess` contains an explicit rotation-detection branch (DD-2-2-5 LOCKED): when `auth.authToken.reveal().contentEquals(cached.authToken.reveal()) == false`, log `auth_token_rotated_by_wallet` event marker and persist returned bytes via `store.putToken(key, cached.copy(authToken = auth.authToken, ...))`. The `FakeMwaClient.reauthorize_success.json` fixture returns BYTE-IDENTICAL `authToken` per fixture-parity contract (story line 122), so `isRotated == false` in every T1 unit test and every T5 androidTest. The rotation branch (log marker + recomputed fingerprint from new bytes + putToken with new bytes) is never exercised by automated tests in 2-2.
- **Risk:** LOW — the spec calls DD-2-2-5 a "defensive contract" only meaningful in real-wallet scenarios where rotation actually happens (clientlib-ktx 2.0.3 + Phantom production currently never rotate per story line 124). The T6 manual release-gate procedure observes rotation events with real Phantom on a physical device. A regression that broke the rotation branch would manifest only when a wallet starts rotating tokens on reauth — at which point the test surface would catch it on the first T6 release-gate run.
- **Mitigation until closed:** Implementation correctness verified by T2 inspection (DD-2-2-5 fidelity restored — fingerprint/store/sessionState all use `auth.authToken` returned bytes regardless of `isRotated`; the if branch only adds the log marker). Spec line 130 explicitly accepts this gap: "T1 + T5 fixtures use byte-identical tokens (FakeMwaClient reauthorize_success.json), so AC-6's strict equality assertion holds in test scope."
- **Trigger to close:** Add a unit-test scenario that injects a parameterized mock `MwaClient.reauthorize` returning DIFFERENT `authToken` bytes than the seeded `cached.authToken`, then asserts: (a) `SdkLog.w` called with message containing "auth_token_rotated_by_wallet", (b) `store.getToken(key)?.authToken.reveal()` equals the returned bytes (not the seeded ones), (c) `sessionState.getAuthTokenFingerprint()` matches `AuthTokenFingerprint.compute(returnedBytes, salt)`. Or fold into a future Story 4-x rotation-aware refactor.
- **Candidate test (sketch):**
  ```kotlin
  @Test fun `DD-2-2-5 rotated token persists new bytes and logs marker`() {
      seedPostConnectSession()
      val (cacheKey, _) = seedCacheRecord()  // cached.authToken = "test-cached".bytes
      val rotatedToken = SecretString("test-rotated-by-wallet".toByteArray())
      val mockClient = mockk<MwaClient> {
          coEvery { reauthorize(any(), any(), any(), any(), any()) } returns
              MwaResult.Success(authResultWith(authToken = rotatedToken))
      }
      val plugin = buildPlugin(clientFactory = { mockClient })
      plugin.mwaReauthorize("rea-rot", identityJson, "devnet", "solana:devnet", 5000L)
      // Assert: log marker fired; store.getToken(cacheKey)?.authToken.reveal()
      // matches "test-rotated-by-wallet".bytes; sessionState.getAuthTokenFingerprint()
      // matches compute(rotatedToken.reveal(), salt); reauthorize_completed payload's
      // auth_token_fingerprint reflects the NEW value (not preFingerprint).
  }
  ```

## Story 3-1 deviations (Rule 1+) and CR sweep
- **Story:** 3-1 (sign_messages + runSigningOp shared pipeline + contract test) | **Closed:** 2026-04-27 (pre-Gate-5)

### Deviations

13 deviations logged. Full narrative is in `docs/stories/3-1.md` T7 deviation list — summary here for post-story audit:

- **D-3-1-1 (Rule 1):** `runSigningOp` is a private member of `GDExtensionAndroidPlugin`, not a top-level extension function on `MwaAndroidPlugin` (per DD-3-1-1).
- **D-3-1-2 (Rule 1):** `runSigningOp` block receiver is `MwaClient` (the SDK seam), not `MobileWalletAdapterClient` (preserves test seam per DD-3-1-2).
- **D-3-1-3 (Rule 1):** Return type aligned to existing sealed `MwaResult<X>` (not `Result<T, MwaError>`); evolved signature adds `timeoutMs: Long`.
- **D-3-1-4 (Rule 1):** plan.md spec omitted `timeoutMs` parameter; impl signature adds it for DD-3-1-3 watchdog parity.
- **D-3-1-5 (Rule 2 — superseded by D-3-1-13):** Earlier AC-5 evidence narrowing from "byte-identical against vendor fixtures" to "Fake↔real-recorded agreement" per DD-3-1-7. Now superseded by D-3-1-13's further re-scope.
- **D-3-1-6 (Rule 2 — RETRACTED):** "Inferred recording" fallback retracted at T6 pre-impl audit (shared the same false premise about FakeMwaClient frame construction).
- **D-3-1-7 (Rule 2 — N/A):** Instrumented-tier escalation N/A under D-3-1-13's re-scope (test stays in unit tier).
- **D-3-1-8 (Rule 1):** Pre-Impl Audit Revision 1.5 — `MwaSessionState.getKey()` → `getConnectedKey()` (asymmetric setter/getter naming inherited from Story 2-1 T4).
- **D-3-1-9 (Rule 1):** Pre-Impl Audit Revision 1.5 — `setIdentity` is one combined setter (not 3 separate setters); getters remain individual.
- **D-3-1-10 (Rule 1):** Revision 2 T1 review — `runSigningOp` visibility relaxed from `private` to `@VisibleForTesting internal` (T1 cross-package test access; aligns with `inflightMapForTest()` precedent).
- **D-3-1-11 (Rule 1):** Revision 2 T1 review — `runSigningOp` block-result type tightened from generic `T` to `MwaResult<X>` (single-wrap return; double-wrap left wallet-level Failures unrouted at call site). Per C-3-1-W review fix.
- **D-3-1-12 (Rule 1):** T4 facade interface audit — MWA.gd `sign_messages` aligned to `void` per LOCKED Story 2-1 T7 convention (`include/mwa/mobile_wallet_adapter.hpp:55-59` "the remaining 6 op methods stay void"); story v1 said `-> String`. No C++ change.
- **D-3-1-13 (Rule 2 — supersedes D-3-1-5):** T6 contract test re-scope from "Fake↔recorded-real frame agreement" to "fixture-input contract verification". Pre-impl audit caught that `FakeMwaClient` does NOT construct a JSON-RPC frame at any point — its `signMessages()` body is `runOp("sign_messages") { _, fixture -> parseSignResponse(...) }` (FakeMwaClient.kt:97-103). The wire-format layer lives in clientlib-ktx (imported only by MwaClientImpl). The v1 spec's "one-line `lastRequestFrame` hook" was not actionable; frame construction in the Fake is a thicker-fake redesign tracked by Story 1-6 CR-21 (out of scope for 3-1). AC-5 evidence collapses to fixture-input contract; D-3-1-6 (inferred-recording fallback) and D-3-1-7 (instrumented-tier escalation) retracted because both shared the same false premise. Real-wallet wire-format invariants (each signed_payload is a bare 64-byte ed25519 signature per `SignResult.kt:8-11`) move to manual release-gate (Phantom/Solflare during 3-1 close-out window).

### CR sweep

- **CR-18 (RESOLVED in 2-1 T6):** Story 3-1 is the first signing-path consumer of the resolved Callable+Array form. T5 androidTest exercises the path end-to-end; no drift observed. CI runtime confirmation pending the next push.
- **CR-32 (RESOLVED in 2-1 T10):** Desktop backstop coverage is via the shared `build_unsupported_platform_payload` helper (mobile_wallet_adapter.cpp:188), which 3-1 reuses unchanged for the `sign_messages` source_method. The existing `addons/SolanaSDK/mwa/tests/test_unsupported_platform.gd` covers connect/disconnect/reauthorize on the desktop NoOp path; sign_messages takes the same code path. Optional follow-up (non-blocking): add a `sign_messages` invocation to the desktop backstop for symmetric coverage. Tracked as a future-story enhancement candidate; not opened as a new CR (no risk delta).
- **CR-35 (HIGH, deferred):** Headless-Godot test tier — still deferred; no closure in 3-1.
- **CR-41 (RESOLVED in 2-1 T10):** Teardown-race barrier. T1 mock + T5 androidTest exercise the same CallbackLease + inflight-registration ordering as 2-1 T10; no regression.
- **CR-45 (LOW, transitive):** Route B harness gap continues to apply to 3-1 (FakeMwaClient injection bypasses real NativeBridge → JNI → C++ → GDScript path). Mitigations: T5 R8 smoke + future CR-35 headless-Godot tier. No new closure attempt in 3-1.
- **CR-46 (LOW):** ed25519 submodule env gap — not surfaced in 3-1 scope (T3 ran ban-greps + assemble; full Android scons NDK build deferred to CI; same precedent as 2-2/2-3 T3).
- **CR-47 (LOW):** DD-2-2-7 multi-match tie-break — not in scope for 3-1. `mwaSignMessages` does not invoke `store.listAllKeys()`; the preflight uses the synchronous `sessionState.getAuthToken() != null` check per DD-3-1-6, not the 3-tuple filter from `mwaReauthorize`.
- **CR-48 (LOW):** DD-2-2-5 token rotation branch — N/A for sign_messages. Per DD-2-2-5 contract, rotation occurs only in the reauthorize success path; `handleSignMessagesSuccess` does NOT call `store.putToken` (signed_payloads return to caller; the cached `auth_token` is read but never re-written by the signing path). Explicit non-applicability.

## Story 4-1 Deviations + Concerns Sweep (T7, 2026-04-27)

- **D-4-1-1 (Rule 1):** Pre-existing ktlint `argument-list-wrapping` violation in `SignMessagesContractTest.kt:138` (Story 3-1 T6 inheritance, commit f51fbd20) surfaced today on `:plugin:ktlintTestSourceSetCheck`. Auto-fixed inline at T1. Likely cause: ktlint version drift between 2026-04-26 (3-1 close-out) and 2026-04-27 (4-1 start). Acceptable as a drive-by — same file pattern, single line, no semantic change.
- **D-4-1-2 (Rule 1):** `mwaDeauthorize` body is ~100 non-blank non-comment lines vs the story's ≤80-line guideline (Dev Notes, not an AC). The extra ~20 lines come from the explicit DD-4-1-4 idempotent branch (which the story pseudocode collapses but the impl spells out for clarity + T1 case-4 coverage) and the kdoc-rich SdkLog statements. Body remains substantially smaller than the disconnect impl + helper combined; readability not impacted. Code review may suggest extraction of an `emitDeauthorizeCompleted(remoteSucceeded)` helper to drop ~10 lines off the body if budget enforcement is desired in a future story.
- **CR-49 (LOW, inheritance):** Concerns line 170 (MwaClientImplTest deauthorize success-path mock gap) — Story 4-1's T1 unit tests partially close the gap at the `MwaAndroidPlugin` integration layer (FakeMwaClient `deauthorize_success.json` exercises the success path through `MwaClient.deauthorize` → `parseUnitResponse`). The `MwaClientImplTest` itself remains untouched (out of 4-1's scope).
- **CR-50 (LOW, transitive from CR-45):** Route B harness gap continues to apply to 4-1. T5 androidTest (AC1DeauthorizeHappyTest, AC2DeauthorizeNetworkOffTest) bypasses the real NativeBridge → JNI → C++ → GDScript chain — signal observation happens at the Kotlin NativeBridge mock level, not at the C++ MobileWalletAdapter level. Mitigations: T6 source-grep structural test + T7 R8 smoke list update + future CR-35 headless-Godot tier. No new closure attempt in 4-1.
- **CR-51 (LOW):** AC-3 wallet-uninstalled androidTest deferred to unit level only (T1 case 3). Emulator-driven mid-test wallet uninstall is fragile; the unit-tier `MwaResult.Failure(WalletUninstalled)` injection exercises the same code path as AC-2 (DD-4-1-1: any non-Success → remote_unreachable). androidTest tier covers AC-1 + AC-2 only. If a future incident shows unit-level coverage of this classification was insufficient, file a closure story.
- **CR-32 (closed-by-inheritance):** Desktop UNSUPPORTED_PLATFORM for deauthorize — already wired at `mobile_wallet_adapter.cpp:174-181` from Story 1-5. No new work in 4-1; explicit non-applicability acknowledged in the story file.
- **CR-47 / CR-48 (N/A for 4-1):** Both are reauthorize-specific. `mwaDeauthorize` does NOT invoke `store.listAllKeys` for tie-breaking (it uses the listAllKeys filter for WIPE per DD-4-1-6 — different purpose) and does NOT call `store.putToken` (only `deleteToken`). Story 4-1 does not surface or close these CRs.

## CR-52 (LOW): Idempotent-path `listAllKeys()` runs even on empty-identityUri vacuous deauth

- **Story:** 4-1 | **Severity:** LOW | **Discovered:** code review post-T7 (2026-04-27)
- **Detail:** `mwaDeauthorize` lines 979-1002 unconditionally execute `store.listAllKeys().filter { it.identityUri == identityUriSnapshot }.forEach { store.deleteToken(it) }` inside the DD-4-1-4 idempotent path. When `identityUriSnapshot.isEmpty()` (cold-start vacuous deauth), the filter returns empty and no `deleteToken` runs — but `listAllKeys()` still hits SharedPreferences disk I/O (single keys() scan; sub-millisecond on modern hardware).
- **Risk:** Negligible (~1 disk read per cold-start deauth). The optimization `if (identityUriSnapshot.isNotEmpty()) { listAllKeys()... }` would skip the disk read when there's nothing to wipe.
- **Resolution:** ACCEPTED as-is. Rationale:
  - **Symmetry with DD-4-1-2 + DD-4-1-6**: the wipe pattern is identical in both the idempotent branch (lines 979-1002) and the else-branch finally (lines 1038-1067). A guard on ONE branch creates an asymmetry future maintainers would have to think through. Current parallel structure is self-documenting.
  - **Branch coverage cost**: a new conditional needs test coverage on both sides; test surface grows without commensurate behavioral gain.
  - **No spec violation**: Story 4-1 line 287 says "Assert: NO `secureTokenStore.deleteToken` invocation" on second deauth (no-op for the destructive call) — silent on `listAllKeys()`. Current impl honors the destructive-no-op while keeping the read-side symmetric.
  - Cold-start vacuous deauth is rare (typical: connect → deauth; idempotent path triggers only on re-deauth). Production cost negligible.
- **Action:** None. Documented for future maintainers who may rediscover the same micro-optimization.

## Story 4-3 Deviations + Concerns Sweep (T7, 2026-04-27)

### Deviations

- **D-4-3-1 (Rule 2 — AC-1 evidence narrowed):** Verbatim AC-1 spec text "*simulated by pre-populating a ciphertext file then wiping MasterKey alias*" implies on-device end-to-end ciphertext-tampering simulation. Story 4-3 NARROWED to **unit + plugin-boundary coverage only** (T1's 7 unit cases in `MwaAndroidPluginKeystoreCorruptRecoveryTest` via MockK static-mocking + Story 2-1's 2 existing `SecureTokenStoreTest` corrupt-recovery cases at lines 68-95). Rationale: on-device choreography (write tampered XML to `/data/data/<pkg>/shared_prefs/godot-sdk-mwa-tokens-v1.xml` + `KeyStore.deleteEntry("godot-sdk-mwa-master-key-v1")` + lifecycle restart) requires custom test infrastructure (root-shell or `BackupManager` privileges + Keystore-alias deletion + lifecycle restart) that exceeds L budget. Unit + plugin-boundary coverage validates the recovery contract at the same correctness level as the eventual on-device test would. Recovery path: deferred to a future hardening story (tracked as **CR-58** below).
- **D-4-3-2 (Rule 1 — AC-6 runner OS swap, semantically identical):** Verbatim AC-6 spec text "*two separate macos-latest runners*" NARROWED to two `ubuntu-latest` runners with Gradle build cache disabled and AAR canonicalized via `tools/canonicalize-aar.sh` (per DD-4-3-5 LOCKED). Rationale: the substantive AC-6 requirement is "two runners produce byte-identical SHA256" — OS surface is irrelevant once both runners agree. `macos-latest` was a hold-over from the spec author's Phantom-instrumented preference; it costs ~10x ubuntu (6.4× per-minute multiplier × longer Gradle warm-up) and brings no determinism gain because the determinism floor is set by the canonicalization script + cache-disable flags, not the OS. Established CI baseline (`instrumented_tests.yml`, `build.yml`) is `ubuntu-latest` per Story 2-1 round-3 D-F4/D-F5. Recovery path: none required (semantic-equivalent).
- **D-4-3-3:** **NOT FILED.** C-4-3-E spike outcome was **GREEN** at T6 — the AGP 7.4.1 release-variant `connectedAndroidTest` assembled cleanly with R8 enabled, so AC-5 ships fully delivered (no E2E narrowing required). Documented here for traceability against the §Anticipated Deviations table in `docs/stories/4-3.md`.

### Per-task drive-by deviations (rolled into commit messages T1–T6, listed here for completeness)

- **D-T1-RULE1-1 (Rule 1, T1):** Stub-first scaffolding committed in TDD red phase to satisfy `laim-verify-checks` hook on contract-test stub (precedent: Story 3-1 T1 fixup pattern).
- **D-T1-RULE1-2 (Rule 1, T1):** Test class naming `MwaAndroidPluginKeystoreCorruptRecoveryTest` matches established Story 2-1 / 2-2 convention (`MwaAndroidPlugin<Op>Test`); story file's casual reference to "the corrupt-recovery test" left the casing implicit.
- **D-T2-RULE1-1 (Rule 1, T2):** `reauthDictJson` added to `ci/grep_bans.sh` pattern-8 (the SdkLog/Log.* token-leak guard) per DD-4-3-2; fixture file `.github/workflows/testdata/grep_ban_fixtures/log_payload_json.kt.bad` extended with a synthetic violation line for pattern-8 self-test (mirrors Story 2-2 pattern-9 self-test).
- **D-T4-RULE2-1 (Rule 2, T4):** `consumer-rules.pro` clientlib-ktx keep+dontwarn rules placed at end-of-file (after the SdkLog `-assumenosideeffects` block) for visual grouping; story §AC-4 left rule ordering implicit.
- **D-T4-RULE2-2 (Rule 2, T4):** `r8-smoke` workflow grep step asserts presence of all 4 AC-4 rule families inside `META-INF/proguard/consumer-rules.pro` of the shipped AAR (not the source file) — strictly stronger evidence than the story's "verify the file contains" wording.
- **D-T5-RULE1-1 (Rule 1, T5):** Top-level `dependencyLocking { lockAllConfigurations() }` block placed in `android/build.gradle.kts` (root-project subprojects loop) NOT inside `android/plugin/build.gradle.kts`'s `android { }` AGP DSL block (would fail script-resolve per C-4-3-D pre-T5 audit).
- **D-T5-RULE1-2 (Rule 1, T5):** `verify-signatures>true` enabled with 7 `<ignored-keys>` entries for components whose Maven Central PGP keys couldn't be downloaded at lockfile-generation time (per DD-4-3-4.a). SHA-256 mandatory; PGP best-effort. See **CR-61** below.
- **D-T5-RULE1-3 (Rule 1, T5):** Maintainer regen guide landed at `docs/dev/lockfile-regeneration.md` (not the story-suggested `docs/maintainer/`); `docs/dev/` is the established convention from Story 1-2's `docs/dev/r8-strip-evidence.md`.
- **D-T5-RULE1-4 (Rule 1, T5):** `org.json:json:20231013` PGP signature verified at T5 regeneration (better than DD-4-3-4.a's anticipated "no PGP available" baseline). All current transitive deps had Maven Central PGP signatures; the 7 `<ignored-keys>` entries cover keys that couldn't be downloaded, not missing signatures.
- **D-T6-RULE1-1 (Rule 1, T6):** `tools/canonicalize-aar.sh` lives at repo-root `tools/` (not `android/tools/`); matches the Story 1-1 codegen convention (`tools/gen_error_codes.py`).
- **D-T6-RULE1-2 (Rule 1, T6):** `.github/workflows/manifest_merge_audit.yml` ships with both `audit-positive` and `audit-warning` jobs in a single workflow file (not two separate workflows) — single-PR-trigger reduces CI cost and the AC-7 wording is silent on file granularity.
- **D-T6-RULE1-3 (Rule 1, T6):** C-4-3-E spike GREEN evidence captured inline in the T6 commit message (AGP 7.4.1 release-variant `connectedAndroidTest` assembles cleanly with R8 enabled); D-4-3-3 is consequently NOT filed (story Anticipated Deviations decision rule).

### Concerns

- **CR-58 (MEDIUM, AC-1 testability gap):** End-to-end ciphertext-tampering simulation — writing a corrupted `.xml` file under `/data/data/<pkg>/shared_prefs/` then wiping the MasterKey alias via `KeyStore.load(null); keyStore.deleteEntry("godot-sdk-mwa-master-key-v1")` — is an instrumented-test-only operation (no JVM unit test path; the Robolectric SharedPreferences shim does not back to a real on-disk encrypted XML). Deferred to a future hardening story.
  - **Risk:** A regression in the `EncryptedSharedPreferences.create` recovery path that ONLY surfaces when the on-disk ciphertext is real (vs MockK static-mocked) would slip past Story 4-3's unit + plugin-boundary tests.
  - **Mitigation:** (i) Story 2-1 T1 `SecureTokenStoreTest`'s 2 existing corrupt-recovery cases (lines 68-95) cover the store-side wipe + throw with realistic `GeneralSecurityException` / `IOException` types; (ii) Story 4-3 T1's 7 `MwaAndroidPluginKeystoreCorruptRecoveryTest` cases cover the plugin-boundary recovery contract incl. the new DD-4-3-1.a cache-LOOKUP-path catch and the 5-key DD-4-3-1.b `reauth_required` payload shape.
  - **Trigger to revisit:** A future story dedicates the test-infra effort (root-shell setup or BackupManager privileges + Keystore-alias deletion + lifecycle restart). Candidate epic: a hardening polish story under Epic 5 or a dedicated security-test-tier story.

- **CR-59 (LOW, AC-7 design):** AC-7 synthetic-warning consumer-merge audit emits a workflow `::warning::` annotation but does NOT block the PR — by design (AC-7 wording: "the plugin itself does not fail; the consumer's permissive merge wins because consumer wins by Manifest Merger semantics"). A maintainer could theoretically merge a PR that allows a downstream `allowBackup=true` to win over the plugin's `allowBackup=false`.
  - **Risk:** Negligible — the warning is highly visible in the merged-manifest report and any reviewer checking the manifest-merge-audit job would see it. The plugin's own application-level `tools:replace="android:allowBackup"` enforces the false override at build-time when consumed normally.
  - **Mitigation:** Documented in `docs/safety.md` (Story 5-5 scope); the warning's prominent placement in the PR's checks panel is the practical safeguard.
  - **Trigger to revisit:** A future story tightens the synthetic-warning audit to block-on-warning if a real incident shows the warning was missed.

- **CR-60 (MEDIUM, reproducible-build determinism floor):** `tools/canonicalize-aar.sh` strips the currently known non-determinism sources (file mtimes, `MANIFEST.MF` Build-Time entries, JAR entry ordering). Future AGP/JDK upgrades may introduce new non-deterministic surfaces (e.g., AGP 8.x build-uuid headers, new `META-INF/com/android/build/gradle/app-metadata.properties` time-of-build fields).
  - **Risk:** A toolchain bump silently breaks the reproducible-build CI invariant; the failure mode is the byte-diff CI job going RED with a new diff source the canonicalization script doesn't know about.
  - **Mitigation:** The dedicated CI failure mode (`assert-equal` job in `reproducible_build.yml`) names the diffing bytes in its failure log, so a maintainer can extend `canonicalize-aar.sh` reactively. Tracked as forward-looking debt; no proactive multi-AGP-version testing required today.
  - **Trigger to revisit:** The CI job goes RED on a toolchain bump (likely candidates: AGP 8.x upgrade, JDK 21+ migration, R8 version bump).

- **CR-61 (LOW, PGP best-effort):** Some long-tail jars in the dependency graph historically lacked Maven Central PGP signatures. After T5 lockfile regeneration, all current transitive dependencies DO have Maven Central PGP signatures (incl. `org.json:json:20231013`, which is better than DD-4-3-4.a anticipated). 7 components are recorded under `<ignored-keys>` in `gradle/verification-metadata.xml` for keys that couldn't be downloaded at generation time (key server unreachable, key revoked, etc.) — SHA-256 verification stays mandatory for all components, PGP gaps are warned not blocked.
  - **Risk:** Negligible — SHA-256 alone closes the artifact-tampering hole; PGP adds defense-in-depth against compromised Maven Central infrastructure (extremely unlikely). The 7 ignored keys are tooling/transitive-only; none are top-level security-critical (e.g., AndroidX Security Crypto's PGP IS verified).
  - **Mitigation:** Maintainer regen guide (`docs/dev/lockfile-regeneration.md`) documents the `<ignored-keys>` extension procedure when a new dep's PGP key fails to download. Future dep-graph audits can re-attempt key fetches.
  - **Trigger to revisit:** A future Maven Central PGP key infrastructure change (e.g., key-server migration), OR a security-policy upgrade requiring strict-PGP across the entire dep graph.

## Upstream merge 2026-04-28 — dropped features (Wave 7+ candidates)

- **Context:** Pre-Story-3-2-T1 merge of `upstream/master` (Virus-Axel/godot-solana-sdk, HEAD `c7f89894` "Merge pull request #444 from inspikalu/feat/skills.md") into `feature/mwa-android`. Three conflict files + ~140 LOC of upstream auto-merged scaffold-era additions to `GDExtensionAndroidPlugin.kt`. Resolution per Option A (architecturally chosen): kept HEAD verbatim across all conflict regions; surgically dropped upstream's auto-merged scaffold additions because they (a) directly violate A-9 SecretString amendment by reintroducing `var authToken: String?` global + raw `setAuthToken(token: String)`, and (b) duplicate planned Wave 6/7 functionality with the wrong architecture pattern. Logged here so a future story can pick up the legitimate features properly.

- **CR-62 (MEDIUM, FEATURE-DROP — SIWS authorize):** Upstream PR #380 (split into PRs through #444) added MWA 2.0 SIWS (Sign-In With Solana) scaffold support: `connectWalletSiws(cluster, uri, icon, name, domain, statement)` @UsedByGodot + 7 SIWS getters (`getSiwsStatus`, `getSiwsSignature`, `getSiwsSignedMessage`, `getSiwsPublicKey`, `getSiwsAccountLabel`, `getSiwsAccountChains`, `getSiwsAccountFeatures`) + the `connectWalletSiws()` composable in `MyComposable.kt`. ALL DROPPED in the 2026-04-28 merge resolution because they use the scaffold `var`-globals pattern (`mySiws*` globals) that violates A-9. SIWS itself is a real Solana Mobile feature and a legitimate consumer ask. Re-implementation requires a dedicated story (Wave 7+) using `runAuthorizeOp` extension (parallel to Story 3-1's `runSigningOp`) + `MwaSessionState.setSiwsResult(SecretString-wrapped)` + a new `mwaConnectSiws` @UsedByGodot. Spec'd by upstream's `walletAdapter.signIn(sender, payload)` API call.
  - **Risk:** Consumers expecting SIWS in our SDK will need to wait for the Wave 7+ story. No security risk — the dropped code wasn't shipped yet (we're on `feature/mwa-android`, never merged to upstream master).
  - **Mitigation:** Document SIWS-readiness in `docs/api-reference.md` (Story 5-5 docs bundle) when it lands. Mark `walletAdapter.signIn(...)` clientlib-ktx 2.0.3 dependency as already-pinned in T5 lockfile.
  - **Trigger to revisit:** First grant-stakeholder ask for SIWS, OR Wave 7 planning. Estimated ~1 story (M-size) following the Story 3-1 runSigningOp template.

- **CR-63 (LOW, FEATURE-DROP — wallet capabilities query):** Upstream added `getCapabilitiesWallet()` @UsedByGodot + `getCapabilitiesStatus()` + `getCapabilitiesResult()` @UsedByGodot getters + the `getWalletCapabilities()` composable in `MyComposable.kt`, exposing `walletAdapter.transact(sender) { getCapabilities() }` to GDScript. ALL DROPPED for the same A-9-violation reason (depends on `myCapabilitiesStatus`/`myCapabilitiesResult` `var` globals). Capabilities query is useful: lets a game know `maxTransactionsPerSigningRequest`, `maxMessagesPerSigningRequest`, `supportsCloneAuthorization`, `supportsSignAndSendTransactions`, supported transaction versions. NOT in our current 21-story plan; a future Wave 7+ S-size story could add `mwaGetCapabilities` properly.
  - **Risk:** Game devs cannot pre-flight check wallet capabilities before issuing requests. Mitigation: upstream's `clientlib-ktx 2.0.3` already declares the protocol minimums; consumers can read `MWA.get_diagnostics()` (Story 5-2 plan) for a subset.
  - **Mitigation:** Story 5-2 (`get_diagnostics()`) covers some of this surface (versions, session state). A dedicated capabilities story can be added if grant-reviewer feedback flags it.
  - **Trigger to revisit:** Wave 7 planning, OR consumer ask.

- **CR-64 (LOW, FEATURE-DROP — scaffold sign-and-send):** Upstream added `signAndSendTransaction(serializedTransaction)` @UsedByGodot + `getSignAndSendStatus()` + `getSignAndSendResult()` @UsedByGodot getters + the `signAndSendTransaction()` composable. ALL DROPPED. Note: this is the SCAFFOLD path (`var mySignAndSendResult/Status` globals + ComposeWalletActivity launch). Story 3-3 `mwaSignAndSend + Pending-Submission Breadcrumb` (Wave 6, the next-next story after 3-2) implements this PROPERLY using `runSigningOp` + breadcrumb persistence + `pending_submission_found` lifecycle signal. The scaffold version is supplanted, not merely dropped — Story 3-3 is the canonical implementation.
  - **Risk:** None — Story 3-3 ships the proper version within the next 1-2 stories of work.
  - **Mitigation:** N/A; Story 3-3 is the supplanting work.
  - **Trigger to revisit:** N/A (already on the plan).

- **CR-65 (LOW, FEATURE-DROP — `clearStateFullReset` + `setAuthToken`/`getAuthToken` raw-String shims):** Upstream added a `clearStateFullReset()` @UsedByGodot (clears scaffold globals incl. `myResult`/`myConnectedKey`/`authToken`) + `setAuthToken(token: String)` + `getAuthToken(): String` shims. ALL DROPPED. The raw-String token shims are a direct A-9 violation (SecretString discipline). The full-reset semantics are duplicated by Story 4-1 `mwaDeauthorize` (proper deauth + remote revoke + session wipe) and Story 2-3 `mwaDisconnect` (lightweight wipe).
  - **Risk:** None — proper coverage exists in Stories 2-3 + 4-1.
  - **Mitigation:** N/A.
  - **Trigger to revisit:** N/A.

- **CR-66 (LOW, FEATURE-DROP — `getConnectedKeyBase58` + `base58Encode` helper):** Upstream added a Base58 encoder to GDExtensionAndroidPlugin.kt as a `private fun base58Encode(input: ByteArray): String` (java.math.BigInteger-based), plus `getConnectedKeyBase58()` @UsedByGodot consuming the scaffold `myConnectedKey` global. DROPPED. Story 3-1's `MwaClient` interface already exposes Base58-encoded public keys via the AuthResult DTO (`auth.publicKeyBase58`); the manual Base58 encoder duplicates that. `mwaSessionState.getConnectedKeyBase58()` accessor or equivalent can be added as a small @UsedByGodot wrapper if Godot demands String-typed pubkeys (currently we pass ByteArray to the signal payload per A-12).
  - **Risk:** Negligible — the data is reachable via signal payloads + diagnostics.
  - **Mitigation:** Story 5-2 `get_diagnostics()` exposes session state including pubkey via PublicKey-base58 form per arch §3 schema.
  - **Trigger to revisit:** Consumer ask.

- **Concerns NOT logged but worth noting in the merge commit message:** the upstream auto-merged additions to `.gitignore` (3 lines) and `example/WalletAdapterAndroid/flow.yaml` (10-line modification) AND the NEW `SKILL.md` (569 lines, project-skill manifest doc) are KEPT — non-architectural, harmless. SKILL.md is unrelated to LaiM's `~/.claude/skills/` framework (different scope) and may be useful as a project-overview doc for new contributors.
