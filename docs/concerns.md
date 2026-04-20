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
