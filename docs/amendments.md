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
