# -----------------------------------------------------------------------------
# MWA consumer-rules.pro  (Story 1-2 Task 6)
# -----------------------------------------------------------------------------
# See also: proguard-library-self.pro -- library-self-only rules (-dontobfuscate). (CR-13)
#
# Authoritative source: docs/architecture.md §8.5 (amended A-6).
#
# These rules ship TO CONSUMERS of the AAR via `consumerProguardFiles` AND are
# reused by the AAR's own `assembleRelease` via `buildTypes.release.proguardFiles`
# (see build.gradle.kts; concern CR-7 documents why both paths are non-negotiable
# for R8 to fire against the release AAR itself).
# -----------------------------------------------------------------------------

# Strip verbose/debug logs in release.
#
# Package path MUST match `com.godotengine.godot_solana_sdk.mwa.util.SdkLog`
# exactly (amendment A-6 -- without the `util.` segment the class matcher
# silently binds to nothing and R8 strips no call sites). SdkLog must be
# `object SdkLog` with every v/d member carrying @JvmStatic so the Kotlin
# compiler emits `public static void v(...)` / `public static void d(...)`
# descriptors that match the rule. i/w/e are intentionally retained for
# operator observability.
#
# The method descriptors below are EXACT (not `(...)` wildcards) -- this is
# deliberate. The `(...)` form would silently extend -assumenosideeffects to
# any future overload (e.g., a convenience `v(tag: String, msg: String)` that
# accepts an already-materialized String), which is the exact foot-gun DD-30
# exists to prevent: concrete-String args evaluate secret-containing
# expressions BEFORE R8 can elide the call. Tight descriptors ensure an
# accidental overload would leave its call site + argument evaluation intact
# in the release AAR, surfacing as a Task 6 grep failure where a maintainer
# can catch it.
-assumenosideeffects class com.godotengine.godot_solana_sdk.mwa.util.SdkLog {
    public static void v(java.lang.String, java.lang.String, kotlin.jvm.functions.Function0);
    public static void d(java.lang.String, java.lang.String, kotlin.jvm.functions.Function0);
}

# Keep the Story 1-2 Task 6 R8-strip-evidence sentinel from default DCE.
# R8Sentinel.exerciseLogs() is never called from production code so R8's
# default dead-code-elimination pass would otherwise remove the whole class,
# and the decompile check could not distinguish "R8 correctly stripped v/d
# call sites" from "R8 DCE'd the sentinel class wholesale". The -keep
# preserves R8Sentinel.class in the AAR so the INFO_KEEP sentinel string
# survives as the positive discriminator.
-keep class com.godotengine.godot_solana_sdk.mwa.util.R8Sentinel { *; }

# Preserve JNI native method bindings across the entire mwa.** subtree
# (architecture.md:1010). Required so JNI resolution still works when
# downstream app modules run R8 against this AAR.
-keepclassmembers class com.godotengine.godot_solana_sdk.mwa.** {
    native <methods>;
}

# Preserve Godot's reflection surface: any method annotated @UsedByGodot is
# invoked by the Godot Android runtime via reflection, by name, at game
# runtime. Without these rules a downstream minified app build would rename
# or DCE the methods and the plugin would silently fail mid-game with a
# "method not found" log entry deep in Godot's dispatcher.
#
# Upstream check (2026-04-20 against org.godotengine:godot:4.3.0.stable AAR):
# the Godot AAR ships NO proguard.txt and NO consumer-rules.pro -- the
# annotation retention rules are NOT provided by the engine jar. Every MWA
# plugin that ships an AAR intended to be consumed by a minified Android app
# must plant this rule itself, or the plugin breaks on release builds while
# debug still works.
#
# The `-keepclasseswithmembers` variant preserves the enclosing class; the
# `-keepclassmembers` variant preserves the annotated methods across all
# classes. Both are needed together to ensure both the method name AND the
# signature survive R8 optimization and obfuscation.
-keepclasseswithmembers class * {
    @org.godotengine.godot.plugin.UsedByGodot <methods>;
}
-keepclassmembers class * {
    @org.godotengine.godot.plugin.UsedByGodot <methods>;
}

# -----------------------------------------------------------------------------
# Story 2-1 T5/T6/T10 — JNI bridge methods callable from the C++ extension
# via JNI GetStaticMethodID (forward path) OR via external-fun resolution
# (reverse path). R8 cannot see C++ callers and will DCE these methods
# when they're only reachable through the C++ bridge — producing a runtime
# `NoSuchMethodError` the moment the bridge fires a callback.
#
# Surface kept here (plugin.walletadapterandroid.GDExtensionAndroidPlugin$Companion):
#   - mwa*FromJni  (9 forward wrappers, @JvmStatic) — C++ → Kotlin op dispatch
#   - mwaQuerySessionStateFromJni (@JvmStatic) — sync session-state query (T6)
#   - post*Native (5 external funs, @JvmStatic) — Kotlin → C++ signal reverse
#
# Keeping the entire Companion class is safe: it's a scaffold-era singleton
# with ~20 members total; R8 size impact is negligible and the keep-whole
# form avoids accidentally missing a future-added method (the R8 smoke step
# in .github/workflows/instrumented_tests.yml would catch it, but keeping
# the whole class is simpler than matching name patterns member-by-member).
#
# The enclosing `GDExtensionAndroidPlugin` class is ALREADY kept by the
# @UsedByGodot rule above (Godot plugin loader reflection); the Companion
# rule below is additive.
-keep class plugin.walletadapterandroid.GDExtensionAndroidPlugin$Companion {
    *;
}

# -----------------------------------------------------------------------------
# clientlib-ktx reflection entry points (AC-4)
# -----------------------------------------------------------------------------
# The Solana Mobile mobile-wallet-adapter clientlib-ktx (com.solanamobile:
# mobile-wallet-adapter-clientlib-ktx:2.0.3) reflects across its public surface
# during association/transact dispatch (AssociationContract, MobileWalletAdapter
# entry points, Scenario classes, Web3-Solana payload (de)serializers). Without
# a -keep on the package subtree, R8 in a downstream minified app would rename
# or DCE the symbols the library reaches via reflection -- producing runtime
# `ClassNotFoundException` / `NoSuchMethodError` from inside the library at the
# moment authorize/sign is invoked.
#
# `-dontwarn` paired with the `-keep` suppresses R8 warnings for optional
# transitive references inside the same package subtree (e.g. Bouncy Castle
# providers the library declares but does not always link against in the
# Godot consumer profile).
-keep class com.solanamobile.mobilewalletadapter.** { *; }
-dontwarn com.solanamobile.mobilewalletadapter.**
