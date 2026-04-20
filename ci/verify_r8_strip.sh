#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# ci/verify_r8_strip.sh  (Story 1-2 Task 7 -- CR-7 / A-9 AAR-level gate)
# -----------------------------------------------------------------------------
# Guards against the isMinifyEnabled-regression foot-gun (CR-7):
#   * A maintainer flips buildTypes.release.isMinifyEnabled to false (or
#     removes the release block entirely) --> R8 never runs against the
#     release AAR --> -assumenosideeffects on SdkLog.v/d has zero effect
#     --> release AAR ships with SdkLog.v/d call sites + interpolated
#     lambda bodies INTACT, even though ci/grep_bans.sh still passes
#     (source is clean; the violation is a build-config regression, not a
#     code regression).
#
# This script is the complement to ci/grep_bans.sh:
#   * grep_bans.sh guards the SOURCE against Log.* + SdkLog.* leak patterns.
#   * verify_r8_strip.sh guards the RELEASE AAR against R8-strip regressions.
# Both must pass on every PR and merge to master.
#
# Verification strategy (amendment A-9 authoritative criterion):
#   The grep-based check "sentinel string absent from AAR bytecode" was
#   demonstrated to be too strict (R8 preserves compiled .class files of
#   stripped lambdas as unreachable dead code -- the constant-pool strings
#   survive while the lambda bodies do not execute). The bytecode check
#   here is strictly stronger: it inspects R8Sentinel.class directly and
#   asserts that no `invokestatic` to SdkLog.v or SdkLog.d remains. If R8
#   stripped the call sites, those invokes are replaced by `getstatic+pop`
#   stubs. If R8 did NOT run, the invokes are present verbatim.
#
# Usage:
#   bash ci/verify_r8_strip.sh [<aar-path>]
#     Default aar-path: android/plugin/build/outputs/aar/WalletAdapterAndroid-release.aar
#     Exits 0 on success, non-zero + named failure on any regression.
#
# Dependencies: unzip, javap (JDK). grep and awk are POSIX-standard.
# The script does NOT run `./gradlew :plugin:assembleRelease` itself -- the
# workflow invokes that in a separate step so CI logs cleanly separate
# build failures from verification failures.
# -----------------------------------------------------------------------------

set -uo pipefail

AAR_DEFAULT="android/plugin/build/outputs/aar/WalletAdapterAndroid-release.aar"
AAR_PATH="${1:-$AAR_DEFAULT}"

# Sentinel strings -- MUST be kept in sync with android/plugin/src/main/
# kotlin/com/godotengine/godot_solana_sdk/mwa/util/R8Sentinel.kt. If that
# file's magic strings are changed, update both places in lockstep.
SENTINEL_VERBOSE="MWA_R8_SENTINEL_VERBOSE_64cf3a1b"
SENTINEL_DEBUG="MWA_R8_SENTINEL_DEBUG_9b2e07f4"
SENTINEL_INFO="MWA_R8_SENTINEL_INFO_KEEP_4a11c8dd"

# R8Sentinel bytecode path inside the AAR's classes.jar. Package derives
# from amendment A-5 (com.godotengine.godot_solana_sdk.mwa.util).
R8_SENTINEL_PATH="com/godotengine/godot_solana_sdk/mwa/util/R8Sentinel.class"

echo "Verifying R8 strip on: $AAR_PATH"
echo ""

if [[ ! -f "$AAR_PATH" ]]; then
    echo "FAIL: AAR not found at $AAR_PATH"
    echo "      Run ./gradlew :plugin:assembleRelease first, or pass the AAR path as arg 1."
    exit 2
fi

# Unpack into a throwaway temp dir; trap cleanup on any exit path.
workdir=$(mktemp -d 2>/dev/null || mktemp -d -t 'mwa-verify')
trap 'rm -rf "$workdir"' EXIT

unzip -q -o "$AAR_PATH" -d "$workdir" || { echo "FAIL: could not unzip $AAR_PATH"; exit 3; }

if [[ ! -f "$workdir/classes.jar" ]]; then
    echo "FAIL: AAR contains no classes.jar (malformed AAR?)"
    exit 3
fi

mkdir -p "$workdir/classes"
unzip -q -o "$workdir/classes.jar" -d "$workdir/classes" || { echo "FAIL: could not unzip classes.jar"; exit 3; }

fail=0

# -----------------------------------------------------------------------------
# Gate 1 -- R8Sentinel class must survive DCE (consumer-rules.pro `-keep`).
# -----------------------------------------------------------------------------
# If this class is missing from the AAR, either the `-keep class R8Sentinel`
# rule was removed, the class was deleted by a maintainer, or R8 ran an
# unexpectedly aggressive pass. Without the sentinel class, the rest of
# this script's checks have nothing to verify against and the Task 6
# evidence pipeline is broken.
sentinel_class="$workdir/classes/$R8_SENTINEL_PATH"
if [[ ! -f "$sentinel_class" ]]; then
    echo "FAIL [Gate 1]: R8Sentinel.class missing from release AAR."
    echo "               Expected at: $R8_SENTINEL_PATH"
    echo "               Likely cause: '-keep class ...R8Sentinel' removed from consumer-rules.pro"
    echo "                             OR R8Sentinel.kt deleted from src/main (see @Suppress KDoc)."
    fail=$((fail + 1))
fi

# -----------------------------------------------------------------------------
# Gate 2 -- R8 must have stripped SdkLog.v and SdkLog.d call sites.
# -----------------------------------------------------------------------------
# The crux of the CR-7 gate. Post-R8, exerciseLogs()'s bytecode shows
# `getstatic LambdaClass.a:I; pop` where the v/d invokestatic used to be
# (A-9 documents this). Pre-R8 (or isMinifyEnabled=false), bytecode shows
# `invokestatic Method com/godotengine/godot_solana_sdk/mwa/util/SdkLog.v`
# and `...SdkLog.d`. We scan the javap disassembly for the LITERAL
# unminified method reference -- R8 either renames these (minification on)
# or removes them entirely (-assumenosideeffects on) under any correct
# config; the presence of the raw unminified reference is a sufficient
# regression signal.
if [[ -f "$sentinel_class" ]]; then
    if ! command -v javap >/dev/null 2>&1; then
        echo "FAIL [Gate 2]: javap not found on PATH (JDK required)."
        fail=$((fail + 1))
    else
        javap_out=$(javap -p -c "$sentinel_class" 2>&1)

        # ---------------------------------------------------------------
        # Gate 2a -- literal FQN check (primary, depends on name stability).
        # ---------------------------------------------------------------
        # Fires on scenarios where the unminified SdkLog name survives in
        # the AAR bytecode:
        #   * isMinifyEnabled=false (R8 never ran) -- name + call both intact.
        #   * isMinifyEnabled=true + -dontobfuscate on library self-build
        #     (proguard-library-self.pro) + rule broken -- name survives,
        #     call survives, gate catches it.
        # Name stability is guaranteed by proguard-library-self.pro's
        # -dontobfuscate directive for the library's own :assembleRelease;
        # if that rule file is ever removed, Gate 2a can silently
        # false-pass (the renamed-FQN blind spot) -- that is the scenario
        # Gate 2b below defends against.
        unminified_v=$(printf "%s" "$javap_out" | grep -cE "invokestatic .* (Method )?com/godotengine/godot_solana_sdk/mwa/util/SdkLog\.v\(" || true)
        unminified_d=$(printf "%s" "$javap_out" | grep -cE "invokestatic .* (Method )?com/godotengine/godot_solana_sdk/mwa/util/SdkLog\.d\(" || true)

        if [[ "$unminified_v" -gt 0 || "$unminified_d" -gt 0 ]]; then
            echo "FAIL [Gate 2a]: R8 did NOT strip SdkLog.v/d call sites in the release AAR."
            echo "                Unminified invokestatic SdkLog.v hits: $unminified_v"
            echo "                Unminified invokestatic SdkLog.d hits: $unminified_d"
            echo "                Likely cause: buildTypes.release.isMinifyEnabled == false (CR-7)"
            echo "                              OR -assumenosideeffects rule broken/removed (A-6 package path)"
            echo "                              OR SdkLog lost @JvmStatic on v/d (method descriptor mismatch)"
            echo "                Remediation: see docs/concerns.md CR-7 + docs/amendments.md A-6 / A-9."
            fail=$((fail + 1))
        else
            echo "OK   [Gate 2a]: no unminified invokestatic to SdkLog.v / SdkLog.d in R8Sentinel."
        fi

        # ---------------------------------------------------------------
        # Gate 2b -- invokestatic instruction-count sanity (name-agnostic).
        # ---------------------------------------------------------------
        # Complement to Gate 2a. Closes the "R8 ran + rule broken +
        # obfuscation on" blind spot where SdkLog gets renamed to `a`/`b`/...
        # and the literal-FQN grep in 2a returns zero matches even though
        # the v/d invokes are intact against the renamed class.
        #
        # Calibration (observed on a correct strip, 2026-04-20):
        #   * exerciseLogs() contains exactly 2 invokestatic instructions:
        #     1x Intrinsics.checkNotNullParameter (Kotlin null check on corrId)
        #     1x SdkLog.i (the retained i call, minified or not).
        #   * Total across R8Sentinel.class (including <clinit>, etc.): 2.
        # On a broken strip (R8 ran, rule missed v/d, obfuscation on):
        #   * exerciseLogs() contains 4 invokestatic: Intrinsics + renamed_v
        #     + renamed_d + renamed_i.
        #
        # Threshold is set at `<= 3` (STRICTLY less than 4). This is
        # generous -- the calibrated correct value is 2. Bump the threshold
        # ONLY when the calibration value legitimately changes (e.g. a new
        # i/w/e call is added to exerciseLogs, or AGP/R8 version changes
        # the emitted instruction shape). In that case, re-calibrate by
        # counting invokestatic in a known-good R8Sentinel.class and pick a
        # threshold that's (observed + 1).
        invokestatic_count=$(printf "%s" "$javap_out" | grep -c "invokestatic" || true)
        invokestatic_threshold=3
        if [[ "$invokestatic_count" -gt "$invokestatic_threshold" ]]; then
            echo "FAIL [Gate 2b]: R8Sentinel.class has $invokestatic_count invokestatic instruction(s);"
            echo "                threshold is <= $invokestatic_threshold (calibrated 2 on a correct strip)."
            echo "                Likely cause: R8 ran + -assumenosideeffects rule broken + obfuscation on"
            echo "                              (SdkLog renamed; Gate 2a's literal-FQN grep missed it)."
            echo "                Remediation: verify proguard-library-self.pro ships -dontobfuscate AND"
            echo "                             verify consumer-rules.pro rule descriptors match SdkLog's"
            echo "                             actual compiled bytecode (package, @JvmStatic, descriptor)."
            fail=$((fail + 1))
        else
            echo "OK   [Gate 2b]: R8Sentinel.class has $invokestatic_count invokestatic instruction(s)"
            echo "                (name-agnostic sanity -- threshold <= $invokestatic_threshold)."
        fi
    fi
fi

# -----------------------------------------------------------------------------
# Gate 3 -- INFO sentinel MUST appear in the AAR (positive discriminator).
# -----------------------------------------------------------------------------
# If VERBOSE + DEBUG sentinel strings are absent but INFO is ALSO absent,
# we can't distinguish "R8 correctly stripped v/d" from "R8 DCE'd the
# sentinel class wholesale" (both produce zero sentinel hits). Gate 3
# asserts INFO is still present as the anchor.
info_hits=$(grep -rl "$SENTINEL_INFO" "$workdir/classes/" 2>/dev/null | wc -l | tr -d ' ')
if [[ "$info_hits" -lt 1 ]]; then
    echo "FAIL [Gate 3]: INFO sentinel '$SENTINEL_INFO' not present in release AAR."
    echo "               R8 likely DCE'd the sentinel class wholesale -- the -keep rule"
    echo "               did not anchor R8Sentinel.INSTANCE, or R8Sentinel.i() call is gone."
    fail=$((fail + 1))
else
    echo "OK   [Gate 3]: INFO sentinel present in $info_hits class file(s) (positive discriminator)."
fi

# -----------------------------------------------------------------------------
# Gate 4 -- VERBOSE / DEBUG sentinel string PRESENCE check (soft, informational).
# -----------------------------------------------------------------------------
# Per amendment A-9, R8 (AGP 7.4.1 era) preserves the .class files of
# stripped lambdas as unreachable dead code -- the VERBOSE/DEBUG sentinel
# STRINGS remain in their constant pools even though the lambda bodies
# never execute at runtime. Gate 2 (call-site strip) is the authoritative
# runtime-behavior gate; this gate is informational and will become a
# hard assertion if a future R8 / AGP version starts DCE-ing the dead
# lambda classes (CR-9 auto-closure condition).
verbose_hits=$(grep -rl "$SENTINEL_VERBOSE" "$workdir/classes/" 2>/dev/null | wc -l | tr -d ' ')
debug_hits=$(grep -rl "$SENTINEL_DEBUG" "$workdir/classes/" 2>/dev/null | wc -l | tr -d ' ')
echo "INFO [Gate 4]: VERBOSE sentinel class-file hits: $verbose_hits (A-9: expected 0-1; bytecode is authoritative)"
echo "INFO [Gate 4]: DEBUG   sentinel class-file hits: $debug_hits (A-9: expected 0-1; bytecode is authoritative)"

echo ""

if [[ "$fail" -gt 0 ]]; then
    echo "verify_r8_strip.sh: $fail gate(s) FAILED -- release AAR has lost R8 protection."
    exit 1
fi

echo "verify_r8_strip.sh: all hard gates PASS -- R8 strip is intact in the release AAR."
exit 0
