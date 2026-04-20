# Architecture Amendments

Amendments to `docs/architecture.md` (frozen after Gate 3). Story-creator-agent reads
BOTH architecture.md and this file, using amended values where they conflict.

## A-1: §3.2 retry_hint enum — add install_wallet
- **Story:** 1-1 | **Date:** 2026-04-17
- **Original:** `retry_hint: "retry_now" | "reauthorize" | "connect" | "none"`
- **Actual:** `retry_hint: "retry_now" | "reauthorize" | "install_wallet" | "connect" | "none"`
- **Rationale:** `install_wallet` was defined in §3.3 (error-codes manifest, used by NO_MWA_WALLET_INSTALLED) but missing from the §3.2 signal payload schema. Additive; no downstream breakage.

## ~~A-2: RETRACTED~~ — TOKEN_EXPIRED retry_hint stays `connect`
- **Story:** 1-1 | **Date:** 2026-04-17
- **Status:** RETRACTED — original architecture was correct.
- **Rationale for retraction:** When TOKEN_EXPIRED fires, the cached token has been rejected by the wallet. Calling `reauthorize()` would re-fail with the same error — the only recovery is a fresh `connect()` which triggers the chooser. Architecture §5.3 and spec AC-U-8 are explicit: "Caller invokes connect(); do NOT auto-launch chooser." The value `reauthorize` remains in the §3.2 retry_hint enum as reserved for future use but is not assigned to any current error code.

## ~~A-3: RETRACTED~~ — REAUTH_REQUIRED retry_hint stays `connect`
- **Story:** 1-1 | **Date:** 2026-04-17
- **Status:** RETRACTED — same reasoning as A-2. REAUTH_REQUIRED fires on Keystore corruption after cache wipe — no token to reauthorize with.

## A-4: §3.3 PROTOCOL_ERROR recoverable — false → true
- **Story:** 1-1 | **Date:** 2026-04-17
- **Original:** `recoverable: false, retry_hint: connect`
- **Actual:** `recoverable: true, retry_hint: connect`
- **Rationale:** `recoverable: false` with `retry_hint: connect` is contradictory — a non-`none` hint implies a recovery path exists. Protocol errors are transient (bad WebSocket frame, interrupted session); a fresh `connect()` can recover. `UNSUPPORTED_PLATFORM` remains the only `recoverable: false, retry_hint: none` entry.

## A-5: §3.3 / §8.5 Kotlin package namespace — com.limechain.mwa.generated → com.godotengine.godot_solana_sdk.mwa.generated
- **Story:** 1-1 | **Date:** 2026-04-17
- **Original:** story-creator agent chose `com.limechain.mwa.generated`
- **Actual:** `com.godotengine.godot_solana_sdk.mwa.generated`
- **Rationale:** This is a contribution to `Virus-Axel/godot-solana-sdk`, not a standalone LimeChain product. Using a contributor-branded namespace (`com.limechain.*`) in someone else's OSS repo is inappropriate. The architecture doc §8.5 ProGuard rules reference `com.godotengine.godot_solana_sdk.mwa.**` — the generated code must live under this namespace or R8 will strip the sealed enum in release builds, causing `ClassNotFoundException`. Aligning with the architecture doc's ProGuard rules requires zero additional amendments to §8.5.

## A-6: §8.5 R8 rules — correct `SdkLog` package + keep `R8Sentinel`
- **Story:** 1-2 | **Date:** 2026-04-20
- **Original (architecture.md:1024-1027):**
  ```
  -assumenosideeffects class com.godotengine.godot_solana_sdk.mwa.SdkLog {
      public static void v(...);
      public static void d(...);
  }
  ```
- **Actual:**
  ```
  -assumenosideeffects class com.godotengine.godot_solana_sdk.mwa.util.SdkLog {
      public static void v(...);
      public static void d(...);
  }

  # Story 1-2 Task 6 evidence capture — keep sentinel class from DCE so INFO=1 discriminator works
  -keep class com.godotengine.godot_solana_sdk.mwa.util.R8Sentinel { *; }
  ```
- **Rationale:** Two corrections to §8.5 that surface together when Story 1-2 ships the consumer-rules.pro.
  1. **Package path.** `SdkLog` lives under `com.godotengine.godot_solana_sdk.mwa.util.SdkLog` per A-5's subpackage convention (`util` for utility classes, `session` for state). ProGuard/R8 class-name matchers in `-assumenosideeffects` are exact — the original `mwa.SdkLog` rule does not match `mwa.util.SdkLog` and R8 would silently do nothing. The failure mode is invisible: release builds ship with SdkLog.v/d call sites and lambda bodies intact, sentinel strings still appear in the decompiled AAR, and the test blames `@JvmStatic` (the next-most-likely cause) when the real culprit is the package name.
  2. **`R8Sentinel` keep rule.** Story 1-2 Task 6 plants an `internal object R8Sentinel` with `exerciseLogs()` that calls `SdkLog.v/d/i` with unique sentinel strings (`MWA_R8_SENTINEL_{VERBOSE,DEBUG,INFO}_*`). The release-AAR decompile then grep-counts these to prove `VERBOSE=0, DEBUG=0, INFO=1` — `INFO=1` is the discriminator that proves the strip is *selective*, not wholesale. But `R8Sentinel` is never called from production code, and R8's default pass DCEs unreferenced classes. Without the `-keep`, the entire class is removed, `INFO=0`, and the test cannot distinguish "R8 correctly stripped v/d" from "R8 DCE'd everything and you never proved anything about the rule." The `-keep` is cheap (one class, test-only semantics, does not ship to consumers' apps because consumer-rules.pro only applies inside the AAR's own code paths).

  Both corrections are additive to §8.5 and compatible with every downstream story that logs via `SdkLog` (they all use the `util.SdkLog` package anyway).

## A-7: AC-1 (Story 1-2) — drop `operator<<`; `to_string()` is the sole C++ stringification path
- **Story:** 1-2 | **Date:** 2026-04-20
- **Original (plan.md §Story 1-2 AC-1; architecture.md:802, 1166):** "stringified via `toString()` / `operator<<`"
- **Actual:** "stringified via `toString()` (Kotlin) / `to_string()` (C++)"
- **Rationale:** DD-26 [LOCKED] forbids STL types and headers in public C++ GDExtension headers (`src/mwa/include/`). A conventional `std::ostream& operator<<(std::ostream&, const SecretString&)` requires `#include <ostream>` and drags in `<iosfwd>` / `<string>` / libc++ vs libstdc++ ABI surface — the exact drift DD-26 exists to prevent. Godot's C++ API does not provide a chainable string-append `operator<<` idiom for `godot::String`; the canonical stringification is a value-returning method. AC-1's original wording was inherited from idiomatic C++ iostream conventions and silently conflicts with DD-26.

  The implementation keeps:
  - Kotlin: `override fun toString() = "<redacted>"` (unchanged; satisfies "toString()" half of the original AC).
  - C++: `godot::String to_string() const { return godot::String("<redacted>"); }` (sole path).
  - Optional: a free `godot::String operator+(const godot::String&, const SecretString&)` overload for string concatenation convenience (header-only, engine types only, DD-26-compliant). Not required by AC-1 as amended.

  AC-1 verification tests assert `.to_string()` returns `godot::String("<redacted>")` and the class exposes no other public accessor than `.reveal_bytes()`. No production caller uses `operator<<` today (nothing in the codebase writes `SecretString` to a `std::ostream` — everything goes through `SdkLog` which is Kotlin-side). plan.md AC-1 is left verbatim as the historical record; this amendment is the active source for Story 1-2 and all downstream token-handling stories.

## A-8: Story 1-2 Task 2 — defer C++ `SecretString` GoogleTest to Story 1-5
- **Story:** 1-2 | **Date:** 2026-04-20
- **Original (plan.md §Story 1-2 Task 2 / story 1-2 M2):** "GoogleTest asserts `to_string()` and `reveal_bytes()` behavior" — test file at `test/mwa/secret_string_test.cpp`, SCons `tests` alias target runs it.
- **Actual:** Story 1-2 Task 2 ships only `src/mwa/include/secret_string.hpp` (header-only). The GoogleTest test file, the SCons `tests` alias, and system-GoogleTest wiring are deferred to Story 1-5 (`MobileWalletAdapter C++ Node Skeleton + Thread-Hop Unit Test`), which plan.md explicitly scopes to "establish the C++ test tier."
- **Rationale:** Story 1-2's Risk R4 anticipated this escalation path. Scan on 2026-04-20 confirmed:
  1. No `src/mwa/` or `test/` / `tests/` directory exists.
  2. `SConstruct` has no `tests` alias (only `addon` and Solana `test-validator` targets).
  3. GoogleTest is not installed system-wide (no brew keg, no `gtest.h` anywhere on the machine).
  4. `godot-cpp` submodule is not initialized locally either; compilation of any C++ file requires `git submodule update --init godot-cpp` first.

  Attempting R4 option (a) — install GoogleTest + add SCons `tests` alias — exceeded the story's 1-hour budget because it also requires the godot-cpp submodule init, header-include wiring for `godot::PackedByteArray`/`godot::String`, and a cross-platform test-runner target (local macOS + GitHub CI macOS + Linux). R4 fallback (b) applies: defer to Story 1-5, ship the header.

  Verification coverage impact on Story 1-2:
  - M2 Kotlin `SecretString` verified by JUnit 5 tests (Task 1 already done — 6 tests green).
  - M2 C++ `SecretString` verified by **inspection only** in this story (header satisfies DD-26, `godot::`-only types, `to_string()` returns `"<redacted>"`, `reveal_bytes()` exposes `PackedByteArray`). Downstream DD-26 hygiene is backstopped by Story 1-4's header-lint CI job (scans `src/mwa/include/` for STL leaks).
  - AC-1 half satisfied now (Kotlin); AC-1 C++ half satisfied at inspection-level; full GoogleTest satisfaction lands in Story 1-5.
- **Follow-through:** Concern **CR-8** tracks the test-tier debt and closes when Story 1-5 lands the `tests` alias + runs this header's tests. Story 1-5's story file (created at wave 3 start) will list a prerequisite task: "Import `test/mwa/secret_string_test.cpp` from Story 1-2's deferred scope (A-8)." The test file body is already specified verbatim in Story 1-2 §Task 2 — it ports without new design work.

