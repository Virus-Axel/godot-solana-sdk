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

## A-9: Story 1-2 Task 6 — replace grep-count verification with bytecode-disassembly proof
- **Story:** 1-2 | **Date:** 2026-04-20
- **Original (story 1-2 Task 6 verification hints / must_have artifact spec):** `grep -c MWA_R8_SENTINEL_VERBOSE` / `..._DEBUG` against the decompiled release AAR classes must return 0; `..._INFO_KEEP` must return >= 1. Evidence file at `docs/stories/evidence/1-2-r8-aar-decompile.txt` must contain `VERBOSE=0, DEBUG=0, INFO=1`.
- **Actual:** Release AAR built with `isMinifyEnabled=true` + `-assumenosideeffects class ...SdkLog { public static void v(...); public static void d(...); }` yields `VERBOSE=1, DEBUG=1, INFO=1` grep counts. R8 correctly strips the v/d call sites AND prevents the VERBOSE/DEBUG lambda singletons from ever being instantiated at runtime, BUT R8 does not remove the compiled `.class` files of the stripped lambdas — the sentinel string literals remain in their constant pools as unreachable dead code.
- **Rationale:** The grep test, as written, conflates two different properties:
  1. "R8 stripped the v/d call sites" (the property AC-2 actually wants) — TRUE, verified at bytecode level.
  2. "R8 removed the sentinel string literals from the AAR bytecode" (the property the grep measures) — FALSE, because R8 preserves the `.class` files of unreachable lambdas.
  The authoritative proof of (1) is the bytecode disassembly of `R8Sentinel.exerciseLogs()`:
     - `getstatic R8Sentinel$a.a:I; pop` replaces `invokestatic SdkLog.v` (v call site stripped).
     - `getstatic R8Sentinel$b.a:I; pop` replaces `invokestatic SdkLog.d` (d call site stripped).
     - `getstatic R8Sentinel$c.a:L...$c; invokestatic b/a.a(...)` for the `i` path (retained — `b/a` is the R8-minified `SdkLog`, `a` is `i`).
  Plus: `R8Sentinel$a.<clinit>` and `R8Sentinel$b.<clinit>` do NOT publish their `INSTANCE` singleton field (no `putstatic` instruction), so their lambda bodies are unreachable even if reflectively probed. `R8Sentinel$c.<clinit>` DOES publish `INSTANCE`, so the `i` lambda runs.
  The bytecode disassembly is STRICTLY STRONGER than the grep — it proves the runtime behavior the plan cares about, whereas the grep proves a bytecode-layout property R8 is not obligated to deliver.
- **Amended verification criterion (replaces the grep counts as the Task 6 / AC-2 proof):**
  1. `./gradlew :plugin:assembleRelease` log MUST include `:plugin:minifyReleaseWithR8` (proves R8 ran).
  2. `javap -p -c R8Sentinel.class` MUST show `getstatic` + `pop` at the v and d call sites (NOT `invokestatic SdkLog.v / SdkLog.d`).
  3. `javap -p -c R8Sentinel.class` MUST show an `invokestatic` at the i call site (against whatever R8-minified name the rule preserved).
  4. `javap -p -c R8Sentinel$a.class` and `R8Sentinel$b.class` MUST show `<clinit>` with NO `putstatic` to their `INSTANCE` field.
  5. `javap -p -c R8Sentinel$c.class` MUST show `<clinit>` with a `putstatic` to `INSTANCE`.
  Grep counts are retained in the evidence file as secondary observation (not as the pass/fail criterion).
- **Affected:** Story 1-2 Task 6 verification hints, must_have "Release AAR...contains zero occurrences..." phrasing, and the `docs/stories/evidence/1-2-r8-aar-decompile.txt` contents spec. Architecture §8.5 R8 rules are unchanged — they fire correctly.
- **Follow-through:** Concern **CR-9** added to `docs/concerns.md` (LOW) tracking that future R8/AGP version bumps could change this grep behavior (if a future R8 starts DCE-ing dead lambda classes, grep will naturally go to 0 and the amended criterion still passes). No code change required for CR-9 itself.


## A-10: Godot `@UsedByGodot` reflection surface MUST be preserved in every MWA AAR
- **Story:** 1-2 | **Date:** 2026-04-20 | **Scope:** architecture-wide (applies to stories 1-4, 2-1, 2-2, 2-3, every plugin AAR through 1.0 and beyond)
- **Original (docs/architecture.md §8.5 R8 Rules and §2.2 Component G — GDExtensionAndroidPlugin):** The locked R8 ruleset enumerated `-keepclassmembers class com.godotengine.godot_solana_sdk.mwa.** { native <methods>; }` (JNI preservation) and the Story 1-2 `-assumenosideeffects` block, but did not include a keep rule for Godot's reflective method-dispatch surface (`@org.godotengine.godot.plugin.UsedByGodot`).
- **Actual (Story 1-2 Task 6 verification on 2026-04-20):** A direct inspection of the upstream `org.godotengine:godot:4.3.0.stable` AAR at `~/.gradle/caches/modules-2/files-2.1/org.godotengine/godot/4.3.0.stable/...` confirmed the AAR ships NONE of the following:
  - `proguard.txt` at the AAR root
  - `consumer-rules.pro` at the AAR root
  - `META-INF/proguard/*.pro`

  The AAR root contains only `AndroidManifest.xml`, `classes.jar`, `jni`, `META-INF/`, `R.txt`, `res/`. Upstream Godot does NOT preserve `@UsedByGodot` via any consumer rule. Every downstream plugin AAR that ships R8-optimizable methods must plant its own retention rule, or a minified game-app build will rename/DCE the annotated methods and the plugin will fail at game-runtime with a "method not found" deep inside Godot's Java-side dispatcher (typically swallowed as a log line).
- **Rationale:** This is a pure dataflow omission in the original architecture — the rule was never considered because the locked `-keepclassmembers ... native <methods>` entry covered the JNI surface and the team assumed the Godot AAR itself would ship the reflection-surface retention rule. It does not. Promoting this from a Story 1-2 Task 6 deviation (D15) to an architecture-level amendment because every subsequent story that adds `@UsedByGodot` methods to the plugin surface (1-4 node skeleton, 2-1 connect, 2-2 sign, 2-3 disconnect, and every follow-on) would silently re-open the same bug if the rule were not baked into the canonical R8 ruleset.
- **Amended R8 ruleset (addition to docs/architecture.md §8.5 R8 Rules):**
  ```
  # Preserve Godot's reflection-dispatch surface. @UsedByGodot methods are
  # invoked by name at game runtime via reflection; a minified downstream
  # build without these rules silently breaks the plugin.
  # Upstream Godot 4.3.0 AAR verified to ship NO consumer rules (A-10, 2026-04-20).
  -keepclasseswithmembers class * {
      @org.godotengine.godot.plugin.UsedByGodot <methods>;
  }
  -keepclassmembers class * {
      @org.godotengine.godot.plugin.UsedByGodot <methods>;
  }
  ```
  Shipped in `android/plugin/consumer-rules.pro` (Story 1-2 Task 6) and reused by the AAR's own release build via `buildTypes.release.proguardFiles` (see CR-7 / A-9).
- **Affected:**
  - docs/architecture.md §8.5 R8 Rules (add the keep rules above to the authoritative ruleset).
  - docs/architecture.md §2.2 Component G — GDExtensionAndroidPlugin (add note: "All `@UsedByGodot`-annotated methods are protected from R8 renaming/DCE by the consumer-rules.pro rules installed per A-10.").
  - Every future story (1-4, 2-1, 2-2, 2-3, 3-x) adding `@UsedByGodot` methods inherits this protection automatically — no per-story keep rules needed.
- **Verification check for future stories:** on any story that adds new `@UsedByGodot` methods, confirm by `javap -p` on the release AAR class that the method names survive R8 with their source names (not renamed to `a/b/c`). Protocol: `./gradlew :plugin:assembleRelease && javap -p /tmp/mwa-aar/classes/plugin/walletadapterandroid/<ClassName>.class | grep <methodName>`.
- **Follow-through:** No separate concern entry needed. D15 in the Story 1-2 task-6 deviation log remains as the historical trail; A-10 is the forward-looking architectural rule.

## A-11: §2.3.1 ISigner contract — `GDCLASS` → `GDCLASS_CUSTOM` for codebase consistency
- **Story:** 1-3 | **Date:** 2026-04-20
- **Original (docs/architecture.md:128-166, the LOCKED §2.3.1 contract):**
  ```cpp
  class ISigner : public godot::RefCounted {
      GDCLASS(ISigner, godot::RefCounted);
      // ...
  };
  ```
- **Actual:**
  ```cpp
  class ISigner : public godot::RefCounted {
      GDCLASS_CUSTOM(ISigner, godot::RefCounted);
      // ...
  };
  ```
- **Rationale:** Codebase-wide registration consistency. Story 1-3's pre-implementation scan on 2026-04-20 verified that ALL three existing GDExtension-registered classes use `GDCLASS_CUSTOM`, not plain `GDCLASS`:
  - `Transaction` — `GDCLASS_CUSTOM(Transaction, SolanaClient)` (`include/transaction/transaction.hpp:42`)
  - `WalletAdapter` — `GDCLASS_CUSTOM(WalletAdapter, Node)` (`include/wallet_adapter/wallet_adapter.hpp:30`)
  - `Keypair` — `GDCLASS_CUSTOM(Keypair, Resource)` (`include/keypair.hpp:30`)

  `GDCLASS_CUSTOM` is a codebase-specific macro that wraps `GDCLASS` with extra registration hooks (the exact additions are project-internal and not part of the public godot-cpp API). The risk of mixing the two macros at a parent/child boundary is real but invisible: a child class using `GDCLASS_CUSTOM(LocalKeypairSigner, ISigner)` whose parent only ran plain `GDCLASS(ISigner, ...)` may silently break metadata/vtable chaining (the codebase-specific hooks expect their parent class to have run them too). The failure mode is the worst kind — silent at compile time, manifests as missing signal/method bindings or registration table corruption at runtime, and the implementer's first instinct is to blame their own subclass code rather than the parent's macro choice.

  Two options were on the table during story creation:
  - **(a) Keep `GDCLASS` for ISigner (per §2.3.1 verbatim).** Document in story Dev Notes that §2.3.1 uses `GDCLASS` deliberately because ISigner is a pure abstract interface with no factory needs. Implementer must verify by reading `GDCLASS_CUSTOM`'s definition and confirm that child classes (`LocalKeypairSigner`, `WalletAdapterSigner`) using `GDCLASS_CUSTOM` work correctly with a `GDCLASS` parent. If broken → file amendment then.
  - **(b) Amend §2.3.1 to use `GDCLASS_CUSTOM`** for codebase consistency. File amendment NOW; eliminate the risk class-mismatch class entirely.

  Option (b) is cheaper and safer: filed at story creation time, no implementer guesswork, no risk of silent metadata divergence in the field. The architecture doc's `GDCLASS` choice was inherited from generic godot-cpp conventions and did not consider the codebase's `GDCLASS_CUSTOM` precedent — a pure dataflow oversight, not an intentional architectural choice.
- **Amended verification criterion (Story 1-3 Task 1):**
  - `grep -c "GDCLASS_CUSTOM(ISigner" src/isigner.hpp` returns 1.
  - `grep -c "^[[:space:]]*GDCLASS(ISigner" src/isigner.hpp` returns 0 (the plain-GDCLASS variant is absent).
- **Affected:**
  - docs/architecture.md §2.3.1 ISigner contract (the verbatim header inlines `GDCLASS_CUSTOM` instead of `GDCLASS`).
  - Story 1-3 §Architecture Guardrails inlines the amended contract.
  - Future stories adding new C++ classes inherit the `GDCLASS_CUSTOM` default — already established codebase convention; amendment makes it explicit at the architecture-doc level.
- **Follow-through:** No separate concern entry needed. A-11 is forward-looking and self-contained. Future stories that add C++ classes registered via ClassDB MUST use `GDCLASS_CUSTOM` unless an explicit reason exists to deviate (and that reason MUST be filed as a separate amendment).

## A-12: §3.2 Signal Payload Schemas — `*_completed` family escalates to 2-param binding with typed `request_id: String` first-class

- **Story:** 1-5 (MobileWalletAdapter C++ Node Skeleton).
- **Source:** Story 1-5 pre-implementation review (2026-04-22) BLOCKER-4 resolution + T1 review follow-up. Revision added the Guardrails signals-table (story lines 88-101) that explicitly enumerates per-signal PropertyInfo arity. On T1 review the spec-vs-arch drift surfaced when the first `_bind_methods` implementation collapsed all 11 signals to the uniform 1-Dict binding arch.md §3.2 originally prescribed.
- **Architecture doc section impacted:** §3.2 Signal Payload Schemas (`docs/architecture.md:218-314`).
- **Original (arch.md §3.2 line 220, pre-amendment):**
  > All signals emit a single `Dictionary` argument (AC-D-10 — additive keys only).
- **Amended binding contract (Story 1-5 Guardrails table is the reference):**
  - **7 `*_completed` family — 2 params:** `PropertyInfo(Variant::STRING, "request_id")` THEN `PropertyInfo(Variant::DICTIONARY, "result")`. Applies to: `connect_completed`, `reauthorize_completed`, `disconnect_completed`, `deauthorize_completed`, `sign_messages_completed`, `sign_transactions_completed`, `sign_and_send_completed`.
  - **4 error/lifecycle family — 1 param:** `PropertyInfo(Variant::DICTIONARY, "payload")`. Applies to: `mwa_error`, `mwa_timeout`, `mwa_cancelled_lifecycle`, `reauth_required`.
- **Rationale:** D-4 (Story 1-5 Guardrails §D-4) makes `request_id` a load-bearing correlation identifier threaded through every op call (C++ side) and every Kotlin-layer log line (matching `corrId` format). For the `*_completed` family the `request_id` is the primary join key every consumer uses to correlate a completion with the originating request. Burying it inside a Dictionary forces every consumer (GDScript facade in Story 2-1, example app in 5-4, API docs in 5-5) to reach into the Dict with `result["request_id"]` and do a typed cast — poor ergonomics for a first-class typed value.

  Two options were considered during the Story 1-5 review:
  - **(a) Keep arch.md §3.2's uniform 1-Dict shape.** Minimal change; matches "additive keys only" (AC-D-10) literally. Forfeits the typed `request_id` API surface.
  - **(b) Escalate `*_completed` to 2-param (chosen).** Makes `request_id` first-class and typed for the family where it is the primary correlation key. Error/lifecycle signals stay 1-Dict because their payloads are cross-cutting (`mwa_error` can arrive without a `request_id` — e.g., `UNSUPPORTED_PLATFORM` from the pre-op branch; `reauth_required` has no associated request) — forcing a 2-param shape there would paper over real semantic heterogeneity.

  AC-D-10 "additive keys only" (the cited rationale in the original §3.2 prose) is a forward-compat posture about Dictionary contents — consumers must tolerate new keys without breaking. It is orthogonal to the question of how many PropertyInfo params a signal binding carries. An `result: Dictionary` that grows additively on the inside is AC-D-10-compliant regardless of whether it's the signal's sole param or its second. The original §3.2 language conflated the two orthogonal postures.
- **Affected:**
  - `docs/architecture.md §3.2 line 220` — the "All signals emit a single `Dictionary` argument" sentence is amended by this document; read §3.2 in conjunction with A-12.
  - Story 1-5 Guardrails signals-table (`docs/stories/1-5.md:88-101`) is the authoritative binding-shape reference.
  - `src/mwa/mobile_wallet_adapter.cpp::_bind_methods` Story 1-5 Task 1 implements the amended shape.
  - Story 1-5 Task 5.1 `BindingSmoke.SignalsAndMethodsRegistered` asserts the PropertyInfo parameter list of each signal against the Guardrails table (name-only presence is insufficient).
  - Story 2-1 MWA.gd facade — GDScript signal-handler signatures must match the 2-param shape for `*_completed` (callable receives `request_id: String, result: Dictionary`).
  - Story 5-5 documentation bundle — API reference signal tables must use the amended shape.
  - Story 3-1 / 3-2 / 3-3 signing-path stories — `sign_messages_completed` / `sign_transactions_completed` / `sign_and_send_completed` emission sites must pass `request_id` as the first `emit_signal` arg before the `result` Dictionary.
- **Follow-through:** No concern entry. A-12 is forward-looking. Future stories that bind new `*_completed`-family signals inherit the 2-param shape; future stories that bind new error/lifecycle-family signals inherit the 1-Dict shape. Any deviation from these family defaults MUST be filed as a separate amendment.

## A-13: Story 1-5 accepts ZERO host-tier C++ test coverage — `scons tests` target, `cpp_tests.yml`, and `third_party/googletest` submodule retired

- **Story:** 1-5 | **Date:** 2026-04-23 | **Scope:** Story 1-5 test infrastructure; architecture-level guidance for every future C++-heavy story.
- **Source:** Story 1-5 post-implementation CI investigation. `./bin/mwa_tests` built GREEN on commit `f8ffd143` but SIGSEGV'd at runtime in `MobileWalletAdapterTest::SetUpTestSuite()` at `godot::ClassDB::register_class<MobileWalletAdapter>()`. Root cause: godot-cpp's `GDExtensionBinding` interface pointers are null in a host binary that never received the `GDExtension entry-point` callbacks from a running Godot engine. Stock `gtest_main.cc` (what the scons tests target linked) does not and cannot initialize those pointers.
- **Four resolution paths were considered (2026-04-23):**
  - **(A) Host stubs:** custom `test_main.cpp` populating ~140 `godot::internal::gdextension_interface_*` function pointers with stubs (memory / variant / classdb / string / signal). Estimated 200-400 LOC of ceremony against a non-public ABI; fragile across godot-cpp version bumps; correctness of stubs is not mechanically verifiable.
  - **(B) `dlopen` + `get_proc_address` shim:** drive godot-cpp init through its public ABI by loading our own `.so` and returning stubs from a get_proc_address callback. Same ~140 stubs required; same fragility.
  - **(C) Scope down host tests:** rewrite tests to NOT require godot-cpp runtime. **Chosen** (see Rationale).
  - **(D) Headless Godot + GUT-for-C++:** replace `./bin/mwa_tests` with a GDScript test harness run inside a headless Godot binary loading the production `.so`. Proper long-term answer; out of grant scope to re-do Story 1-5 Tasks 4-6 + CI.
- **Rationale for choosing (C):** Reading the MWA production headers after picking (C) revealed a harder floor than the option framing suggested:
  - `SecretString` ctor is `explicit SecretString(godot::PackedByteArray)` — no raw-bytes overload. The ctor, `reveal_bytes()`, and the `operator+` convenience all return `godot::PackedByteArray` / `godot::String`. Testing SecretString requires Variant machinery regardless of how we frame the assertions.
  - `MwaAndroidBridge` is a pure-virtual interface whose entire surface is `godot::String` + `godot::Dictionary` + `godot::TypedArray<godot::PackedByteArray>` (DD-26 compliance). Testing `MockMwaAndroidBridge` or `NoOpMwaAndroidBridge` requires constructing those argument types.
  - `GodotMainDispatcher` holds a `godot::ObjectID` + (under MWA_TESTING) `godot::Array` + `godot::Mutex`; its public `post()` takes `godot::String` + `godot::Dictionary`.
  - `MobileWalletAdapter` is GDCLASS-derived — the original segfault surface — untestable host-mode by definition.

  Per DD-26 [LOCKED] (`docs/architecture.md §7.1.1`), every public header under `src/mwa/include/` is restricted to `godot::` types + `<cstdint>`/`<cstddef>`. This is enforced by Story 1-4's header-lint CI job. The architectural posture that makes DD-26 desirable (hygienic GDExtension ABI, no STL leaks) is the same posture that makes host-mode GoogleTest infeasible: **there is no seam in our MWA production code that takes or returns non-godot types.** "Scope down to test code that doesn't need godot-cpp runtime" has zero targets to aim at.

  Option (C) therefore collapses in practice to: **delete the C++ host test tier entirely.** No production class can be exercised host-mode. The 6 TEST_F cases (2 SecretString + 4 MobileWalletAdapter) that Story 1-5 Task 4b Phase D + Task 5 shipped are retired in-place and deferred to a future headless-Godot tier (tracked as CR-35).
- **Changes this amendment authorizes:**
  1. **Delete** `src/mwa/tests/test_secret_string.cpp` (ported from Story 1-2 §Task 2 per A-8; 2 TEST cases).
  2. **Delete** `src/mwa/tests/test_mobile_wallet_adapter.cpp` (Story 1-5 Task 5; 4 TEST_F cases — `BindingSmoke`, `OpMethodsReturnWithin16ms`, `ThreadHop_SignalEmitsOnMainFromWorkerThread`, `PreOp_UnsupportedPlatformWhenKotlinNotLoaded`).
  3. **Keep** `src/mwa/tests/` as an empty directory (tracked via `.gitkeep`) for the future headless-Godot tier.
  4. **Remove** the `tests` SCons alias target from `SConstruct` (the `if "tests" in COMMAND_LINE_TARGETS:` block introduced by Task 4 Phase C, lines 521-573).
  5. **Remove** the `third_party/googletest` git submodule (v1.14.0 at SHA `f8d7d77c`, added Task 4 Phase B, commit `deb59965`) and its `.gitmodules` entry.
  6. **Delete** `.github/workflows/cpp_tests.yml` (Linux + macOS matrix, Task 6, commit `ce20c0a6`).
  7. **Retain** the `MWA_TESTING` compile-define scaffolding in production source (`GodotMainDispatcher::drain_for_testing`, `snapshot_pending_for_testing`, `MobileWalletAdapter::set_bridge_for_testing`, `MobileWalletAdapter::dispatcher_for_testing` — all `#ifdef MWA_TESTING`-gated). These are dead code in production release builds (no activation path without the retired `scons tests` alias) and remain available for the future headless-Godot tier to reactivate. The grep-ban pattern 7 (`mwa-testing-define`, `ci/grep_bans.sh`) stays — it catches accidental `#define MWA_TESTING` leaks at source level and remains protective even without a `tests` target.
  8. **Retain** amendment A-8's flip: A-8 said "C++ test tier wired in Story 1-5." That flip is now partially reverted — the test tier was wired and found infeasible. A-13 supersedes A-8 for the C++ tier question; A-8's JUnit-side / header-only landing for Story 1-2 is unaffected.
- **Verification coverage impact on Story 1-5:**
  - **AC-1 (11 signals wired via BindingSmoke):** no automated host-mode enforcement. Manual inspection of `src/mwa/mobile_wallet_adapter.cpp::_bind_methods` + Kotlin-side MwaClient parity (Story 1-6 — 74 Kotlin tests) carry partial coverage. Tracked as **CR-31 MEDIUM**.
  - **AC-2 (host-platform unit test Linux/Windows/macOS):** no host-platform tests exist at all. Windows deferral (CR-25) is now moot; the entire AC is deferred to the headless-Godot tier. Tracked in **CR-35 (rollup)**.
  - **AC-3 (null-bridge branch via PreOp test):** no automated enforcement. Manual inspection of `src/mwa/mobile_wallet_adapter.cpp:35-58` + NoOp shape documentation carries the load. Tracked as **CR-32 MEDIUM**.
  - **AC-4 (op methods return within 16ms):** no automated enforcement. Op-method stubs are near-empty (empty-body binds + dispatcher handoff); regression surface is narrow. Tracked as **CR-33 LOW**.
  - **AC-5 (thread-hop signal emits on main from worker):** no automated enforcement. `GodotMainDispatcher::post` + `drain_for_testing` plumbing is inspection-verified; the ObjectID→call_deferred hop has a CR-18 TOCTOU TODO that Story 2-1 must resolve anyway. Tracked as **CR-34 MEDIUM**.
  - **CR-8 (SecretString C++ GoogleTest from 1-2):** REDEFERRED from 1-2→1-5 to 1-5→headless-Godot tier. SecretString Kotlin-side coverage (6 JUnit tests from Story 1-2 Task 1) remains intact; only the C++ assertions move.
  - **CR-29 (production `post()` path not exercised by tests):** partially resolved by deletion — the "tests lie about the production branch" concern ceases to exist because no tests run. Reframed: the production path is now inspection-verified exclusively until the future tier lands. Cross-referenced from CR-35.
- **Affected artifacts:**
  - `SConstruct:521-573` — tests alias block removed.
  - `.gitmodules:16-18` — third_party/googletest entry removed.
  - `.github/workflows/cpp_tests.yml` — file deleted.
  - `third_party/googletest/` — submodule deregistered and directory removed.
  - `src/mwa/tests/test_secret_string.cpp` — deleted.
  - `src/mwa/tests/test_mobile_wallet_adapter.cpp` — deleted.
  - `src/mwa/tests/.gitkeep` — added.
  - `docs/concerns.md` — CR-8 status updated (REDEFERRED); CR-31..35 added; CR-25, CR-27, CR-29, CR-30 cross-referenced.
  - `docs/state.json` — Story 1-5 completedStories entry: amendment list += A-13, concerns list += CR-31..35, notes cite this decision.
  - `docs/sprint-status.yaml` — Story 1-5 remains status=done (with A-13 amendment accepted at Gate 5).
- **Architectural guidance for future stories:**
  - **Rule:** for any GDExtension C++ module whose public surface enforces DD-26-style godot-only type hygiene, do NOT plan a host-mode GoogleTest tier. Pick one of: (1) headless Godot + GDScript test harness, (2) Kotlin-side equivalence testing where the C++ layer is a thin wrapper, (3) inspection-only with CI-enforced header hygiene (current posture).
  - **Where this applies in the current plan:** every future Wave story that would add new C++ test coverage (Story 2-1 signer bridge, Story 3-x signing path, Story 5-6 CI hardening) inherits this guidance — do not add `scons tests`-style targets without first scoping a headless-Godot pivot.
- **Follow-through:** **CR-35** tracks the future headless-Godot C++ test tier as a forward-looking obligation. Scope must be decided before the grant close: fold into Story 5-6 (CI/Build Integration) or carve a dedicated Epic-5 story. Missing this dooms the AC-1..5 coverage gap to permanence. No amendment retraction path — A-13 stands until superseded by a future amendment that re-plans the C++ test tier with (D) or equivalent.

## A-14: §3.2 `mwa_error` payload elevates NoOp 10-key shape as canonical — adds `message` alongside `user_message` / `developer_details`

- **Filed:** 2026-04-24 (Story 2-1 T10 close-out — CR-26 resolution)
- **Supersedes:** arch §3.2's 9-key `mwa_error` prose for the keys carried in the payload.
- **Rationale:** Three independent sources specified the `mwa_error` envelope with incompatible key sets:
  1. `docs/architecture.md` §3.2 lists 9 keys (no `message`).
  2. Story 1-5 AC-3 prose (`"Kotlin plugin not loaded"`) cites `message` by name.
  3. `src/mwa/no_op_mwa_android_bridge.cpp::emit_unsupported` shipped a 10-key payload (includes `message`) in production from Story 1-4 onward.

  Consumers that key off `message` (from AC-3 prose) versus `developer_details` (from arch §3.2) would silently diverge. The NoOp production shape was the earliest source to reach code; retrofitting the arch spec to match it is less churn than source-patching three callers (NoOp + JNI + C++ node null-bridge branch) plus the generated error shape in MwaError.Kt (1-1 codegen). A-14 elevates the NoOp shape.
- **The 10-key `mwa_error` payload (canonical post-A-14):**

  | Key                   | Type       | Source       | Notes                                                                  |
  |-----------------------|------------|--------------|------------------------------------------------------------------------|
  | `request_id`          | String     | op dispatch  | 8 lowercase hex (D-4); correlation for terminal signal.                |
  | `code`                | String     | MwaError enum (codegen 1-1) | e.g. `UNSUPPORTED_PLATFORM`, `USER_CANCELED`, `NO_MWA_WALLET_INSTALLED`. |
  | `message`             | String     | AC-3 breadcrumb | Dev-readable short message. **NEW in A-14** — was absent from §3.2.  |
  | `user_message`        | String     | MwaError.defaultUserMessage | User-facing graceful prompt.                               |
  | `developer_details`   | String     | op-site specific | Context string or empty. Canonical dev-facing field for logs.        |
  | `recoverable`         | Bool       | MwaError.recoverable | false for UNSUPPORTED_PLATFORM; true for USER_CANCELED; etc.       |
  | `retry_hint`          | String     | MwaError.retryHint | Enum-like: `none`, `retry_now`, `connect`, `install_wallet` (per A-1). |
  | `layer`               | String     | emit site | `"kotlin"` (clientlib classification), `"cpp"` (null-bridge), `"wallet"` (wallet rejection). |
  | `cause`               | String\|NULL | op-site specific | Machine-parseable cause tag or JSON null; `""` for NoOp.           |
  | `source_method`       | String     | emit site | `"connect"` / `"disconnect"` / `"reauthorize"` / ... — operation that failed. |

- **Consumer guidance:**
  - Prefer `developer_details` for canonical dev-facing reads (structured, op-specific).
  - `message` is the AC-3 verbatim breadcrumb and retained for cross-AC consistency; consumers MAY read it but SHOULD NOT rely on it being distinct from `user_message` (CR-40 tracks the Story 1-1 codegen follow-up that would split them).
  - All three string keys (`message`, `user_message`, `developer_details`) are guaranteed present under A-14; consumers MUST tolerate any of them being the empty string on specific emit sites.
- **Affected code (all paths already comply at T10 commit time):**
  - `src/mwa/no_op_mwa_android_bridge.cpp::emit_unsupported` — 10 keys since Story 1-4 ✓
  - `src/mwa/mobile_wallet_adapter.cpp::build_unsupported_platform_payload` — 10 keys since Story 1-5 T3 ✓
  - `src/mwa/mwa_android_bridge_jni.cpp::emit_jni_exception_error` + `emit_jni_unavailable` + `post_parse_failure_error` — 10 keys since Story 2-1 T5 ✓
  - `android/plugin/src/main/java/plugin/walletadapterandroid/GDExtensionAndroidPlugin.kt::buildErrorJson` + `buildInstanceNullErrorJson` — 10 keys since Story 2-1 T4 ✓
- **GDScript-level enforcement:** the CR-32 desktop test (`addons/SolanaSDK/mwa/tests/test_unsupported_platform.gd`, Story 2-1 T10) asserts every key in the table above is present in the `mwa_error` payload on the NoOp bridge path. If a future contributor drops one of the 10 keys at an emit site on a desktop build, the `gdscript_tests.yml` workflow fails.
- **Forward-compat note:** when the Story 1-1 codegen adds a `defaultTechnicalMessage` field to `MwaError` (CR-40 follow-up), `message` and `user_message` can finally carry distinct content. A-14 does NOT require that; the shape is stable either way.
- **Closes:** CR-26 (arch §3.2 `message` key reconciliation).
- **Follow-through:** CR-40 (LOW) remains open as the Story 1-1 codegen enhancement; it is non-blocking for A-14.

## A-15: Story 1-5 7-op skeleton — `MobileWalletAdapter::forget_all` impl is empty-stub, NOT delegating; Story 4-2 T3 fills it in via canonical sibling pattern

- **Status:** Filed 2026-04-28 by Story 4-2 pre-implementation interface audit (post-story-creation).
- **Severity:** Rule 3 (interface contract delta) — moderate scope; T3 work expands from "audit-only or 1-line bind_method add" to "fill in ~10 LOC delegation body". No public surface change (the bind_method exists, the bridge interface declares the method); the change is internal to the C++ TU.
- **Surface:** `void MobileWalletAdapter::forget_all() {}` at `src/mwa/mobile_wallet_adapter.cpp:262` (empty stub body, marked `// 2 utility — stubs in 1-5.` in the comment block above at line 257).
- **Diagnosis:** Story 1-5 landed the 7-op skeleton with a mix of delegating impls (mwa_connect / mwa_disconnect / reauthorize / deauthorize / sign_messages / sign_transactions / sign_and_send) and empty-body stubs (`forget_all`, `get_diagnostics`). The empty stubs were intentional placeholders for stories that own the corresponding capability — Story 4-2 owns `forget_all`; Story 5-2 owns `get_diagnostics`. Story 4-2's pre-creation interface-row table (in `docs/stories/4-2.md`) assumed the delegation pattern was already wired; the audit found the stub.
- **Resolution:** Story 4-2 T3 lands the canonical delegation body (mirror of `mwa_disconnect` / `deauthorize` at the same TU). Pattern:
  ```cpp
  void MobileWalletAdapter::forget_all() {
      const godot::String request_id = generate_request_id();
      if (bridge_ == nullptr) {
          dispatcher_.post(godot::String("mwa_error"),
                  godot::Array::make(build_unsupported_platform_payload(request_id, godot::String("forget_all"))));
          return;
      }
      bridge_->forget_all(request_id);  // 1-arg per MwaAndroidBridge::forget_all (no opts Dictionary)
  }
  ```
  Note: `MwaAndroidBridge::forget_all` at `src/mwa/include/mwa_android_bridge.hpp:84` is **1-arg** (`const godot::String& request_id`) — DIFFERENT from `disconnect`/`deauthorize` which take 2-arg `(request_id, opts: Dictionary)`. The `forget_all` facade does NOT accept an opts dictionary per Story 4-2 DD-4-2-7 (no caller-tunable timeout; per-deauth budget locked at 2s).
- **Affected story-file row:** `docs/stories/4-2.md` §Verified Interfaces row "MobileWalletAdapter::forget_all C++ binding" status changes from "✓ VERIFIED at story-creation" to "⚠ STUB — T3 fills in delegation". §Tasks row "T3 — C++ surface audit" description changes from "verification-only or 1-line bind_method addition (if missing)" to "land the canonical delegation body (~10 LOC mirroring mwa_disconnect / deauthorize)".
- **Forward-look:** the same stub-vs-delegation pattern likely applies to `MobileWalletAdapter::get_diagnostics` at line 258 (empty Dictionary return). Story 5-2 will fill that in with the canonical sibling pattern. No action needed at Story 4-2 T3 (out of scope).
- **Closes:** none (no prior CR tracked this; surfaced fresh by Story 4-2's pre-implementation audit).
- **Follow-through:** Story 5-2 should run a similar audit at story-creation time for `get_diagnostics`'s empty-Dictionary stub.

## A-16: DD-34 wording aligned to actual `FINGERPRINT_SALT_BYTES = 32` impl (was 16-byte spec text)

- **Status:** Filed 2026-04-29 by Story 5-2 pre-implementation interface audit (story-creation HALT).
- **Severity:** Rule 1 (spec wording drift; no behavior change) — the impl has been 32-byte since Story 1-2 landed `AuthTokenFingerprint` + the per-install salt; DD-34's "16-byte" wording in `docs/architecture.md:1210` is the drift, not the impl.
- **Surface:** `docs/architecture.md:1210` DD-34 text reads `"...uses HKDF-SHA256 with per-install salt stored under Keystore alias godot-sdk-mwa-fingerprint-salt-v1; 8 hex chars output; stable across token refresh, rotated only on forget_all."`. Earlier prose at `architecture.md:737` describes the salt as `"a 16-byte random value generated at first addon init, stored under Keystore alias godot-sdk-mwa-fingerprint-salt-v1"`. Both wordings need to be aligned.
- **Diagnosis:** Story 5-2's pre-implementation interface audit cross-checked DD-34 spec against the actual impl. Found:
  - `SecureTokenStore.kt:202` declares `const val FINGERPRINT_SALT_BYTES = 32`.
  - `SecureTokenStore.kt:128` allocates `ByteArray(FINGERPRINT_SALT_BYTES)` and fills via `SecureRandom().nextBytes(it)` — confirming 32 bytes.
  - `AuthTokenFingerprint.compute(authTokenBytes, salt)` accepts a `ByteArray` of any length; the HKDF-SHA256 algorithm is salt-length-agnostic (all bytes feed the HMAC IKM step). Output is 8 hex chars regardless of salt size.
- **Why 32 bytes is correct:** NIST SP 800-108 §5 recommends salt length ≥ output length of the underlying hash (32 bytes for SHA256). 16-byte salt is not unsafe, but 32-byte is the conservative best-practice and matches the codebase's actual choice. The DD-34 spec text was written aspirationally before Story 1-2 landed the impl; nobody updated the wording.
- **Resolution:** Update both `architecture.md:737` prose and `architecture.md:1210` DD-34 text:
  - `:737` change `"a 16-byte random value generated at first addon init"` → `"a 32-byte random value generated at first addon init"`.
  - `:1210` DD-34 add explicit salt-size literal: `"...with per-install 32-byte random salt stored under Keystore alias godot-sdk-mwa-fingerprint-salt-v1..."`.
- **Affected story-file row:** `docs/stories/5-2.md` §Dev Notes Pre-Implementation Audit item #7 changes status from `OPEN QUESTION` to `RESOLVED at story-creation HALT — A-16 aligns DD-34 wording`. No D-5-2-T1-* deviation needed.
- **Closes:** none (no prior CR tracked this; surfaced fresh by Story 5-2 pre-implementation interface audit).
- **Follow-through:** none. Future stories that touch the fingerprint surface should rely on `FINGERPRINT_SALT_BYTES` const for the salt size, not on prose wording. DD-34 is now spec-vs-impl consistent.
