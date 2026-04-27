#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# tools/canonicalize-aar.sh  (Story 4-3 T6 — AC-6 / DD-4-3-5)
# -----------------------------------------------------------------------------
# Hand-rolled `strip-nondeterminism`-equivalent: takes one Android AAR path,
# unzips it to a temp dir, zeros every file's mtime to the Unix epoch
# (1970-01-01 00:00:00), repacks via `zip -X` (no extra-fields) with a stable
# `LC_ALL=C`-sorted file ordering, and emits the SHA256 of the canonicalized
# AAR to stdout.
#
# Why each step matters (per DD-4-3-5):
#   - Gradle 8.x AAR output embeds `META-INF/MANIFEST.MF` with `Created-By:`
#     and `Build-Jdk` headers, plus per-entry `lastModifiedTime` in the inner
#     ZIP central directory. Even on identical Gradle/JDK versions the byte
#     stream differs across two runners.
#   - `find ... -exec touch -t 197001010000 {} \;` zeros every entry's mtime
#     so the ZIP central-directory `lastModifiedTime` field is identical
#     across runs.
#   - `zip -X` strips extra-fields (UID/GID, extended timestamps) that are
#     OS-dependent.
#   - `LC_ALL=C sort` ensures the file enumeration order is identical across
#     locales (macOS vs. ubuntu can sort case differently otherwise).
#
# Usage:
#   tools/canonicalize-aar.sh path/to/WalletAdapterAndroid-release.aar
#   # → emits a single 64-hex SHA256 line to stdout
#
# Determinism contract:
#   - Same machine, two consecutive runs on the same input → identical SHA256.
#   - Two different machines with the same Gradle + JDK build of the same
#     source → identical SHA256 (verified by .github/workflows/reproducible_build.yml).
#
# Determinism boundary (Story 4-3 code-review F1 + universal retrospect rule
# 2026-04-27 "Build-output canonicalization scripts must document their
# determinism boundary") — what THIS script owns vs. what we DELEGATE to
# Gradle's documented reproducible-jar contract:
#   - HANDLED here (outer AAR layer):
#       * file mtimes → zeroed via `touch -t 197001010000`
#       * central-directory ordering → enforced via `LC_ALL=C sort`
#       * extra-fields (UID/GID, extended timestamps) → stripped via `zip -X`
#   - DELEGATED to Gradle's reproducible-jar contract (nested archives —
#     `classes.jar`, `R.jar`, etc. inside the AAR):
#       * inner-jar entry mtimes → Gradle `Jar.preserveFileTimestamps=false`
#         (AGP-default since AGP 7.x; verifiable via `unzip -l classes.jar`
#         showing all entries dated `01-01-1981 01:01` — Gradle's deterministic
#         constant)
#       * inner-jar central-directory ordering → Gradle
#         `Jar.reproducibleFileOrder=true` (AGP-default since AGP 7.x)
#       * Refs: https://docs.gradle.org/current/userguide/working_with_files.html#sec:reproducible_archives
#   - RE-EVALUATION TRIGGERS — audit this script's delegation list AND watch
#     reproducible_build.yml on next push whenever ANY of these happen:
#       * AGP major upgrade (7.x → 8.x — known AGP 8.x build-uuid header risk
#         tracked as CR-60 MEDIUM)
#       * Gradle major upgrade (8.x → 9.x)
#       * JDK toolchain bump (e.g., temurin-17 → temurin-21)
#       * New file inside AAR that this script doesn't enumerate (e.g., a new
#         META-INF/com/android/build/gradle/app-metadata.properties carrying
#         a time-of-build field)
#
# Exit codes:
#   0 — success (SHA256 emitted to stdout)
#   1 — usage error (missing or invalid AAR argument)
#   2 — runtime error (unzip/zip/sha256sum unavailable or failed)
# -----------------------------------------------------------------------------

set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <path-to-aar>" >&2
    exit 1
fi

AAR_PATH="$1"

if [ ! -f "$AAR_PATH" ]; then
    echo "error: AAR not found: $AAR_PATH" >&2
    exit 1
fi

# Resolve to absolute path so subshell `cd` does not break the reference.
case "$AAR_PATH" in
    /*) ABS_AAR="$AAR_PATH" ;;
    *)  ABS_AAR="$PWD/$AAR_PATH" ;;
esac

# Detect SHA256 tool (sha256sum on Linux, shasum on macOS).
SHA_TOOL=""
if command -v sha256sum >/dev/null 2>&1; then
    SHA_TOOL="sha256sum"
elif command -v shasum >/dev/null 2>&1; then
    SHA_TOOL="shasum -a 256"
else
    echo "error: neither 'sha256sum' nor 'shasum' found in PATH" >&2
    exit 2
fi

# Guard required tooling.
for tool in unzip zip find touch; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "error: required tool '$tool' not found in PATH" >&2
        exit 2
    fi
done

WORK_DIR="$(mktemp -d -t canonicalize-aar.XXXXXX)"
trap 'rm -rf "$WORK_DIR"' EXIT

EXTRACT_DIR="$WORK_DIR/extract"
mkdir -p "$EXTRACT_DIR"

# (a) Extract the AAR (it's a ZIP).
unzip -q -o "$ABS_AAR" -d "$EXTRACT_DIR"

# (b) Zero every file mtime to 1970-01-01 00:00:00. `-t 197001010000` is the
# `touch` portable form (CCYYMMDDhhmm). Apply to BOTH files and directories so
# directory entries in the central directory also get the deterministic mtime.
find "$EXTRACT_DIR" -exec touch -t 197001010000 {} \;

# (c) Repack with `zip -X` (strip extra-fields) and stable LC_ALL=C-sorted
# entry ordering. `-X` omits eXtra file attributes (UID/GID, extended
# timestamps, Mac OS resource forks). Reading the file list from `-@` (stdin)
# lets us control ordering precisely.
CANON_AAR="$WORK_DIR/canonical.aar"
(
    cd "$EXTRACT_DIR"
    # `find . -type f` enumerates files; `LC_ALL=C sort` produces a
    # locale-independent stable order. Strip the leading "./" so zip stores
    # paths consistently.
    find . -type f | LC_ALL=C sort | sed 's|^\./||' | zip -X -@ "$CANON_AAR" >/dev/null
)

# (d) Emit SHA256 of the canonicalized AAR.
# `cut -d' ' -f1` strips the trailing filename printed by sha256sum/shasum.
$SHA_TOOL "$CANON_AAR" | cut -d' ' -f1
