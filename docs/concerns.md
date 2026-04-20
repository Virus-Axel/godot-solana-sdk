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
