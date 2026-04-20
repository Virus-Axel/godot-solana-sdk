# -----------------------------------------------------------------------------
# MWA consumer-rules.pro  (Story 1-2 Task 6)
# -----------------------------------------------------------------------------
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
