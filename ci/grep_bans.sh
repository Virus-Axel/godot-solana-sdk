#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# ci/grep_bans.sh  (Story 1-2 Task 7 — AC-4 enforcement)
# -----------------------------------------------------------------------------
# Scans source files for the four banned patterns documented in AC-4 +
# AC-D-8:
#
#   1. Log.[vdiwe](...authToken...)        -- android.util.Log call with authToken
#   2. Log.[vdiwe](...publicKey...)        -- android.util.Log call with publicKey
#   3. Log.[vdiwe](...SecretString.reveal())  -- Log call that unwraps SecretString
#   4. SdkLog.[vdiwe](...${...)            -- SdkLog call with string interpolation
#                                             outside a lambda (DD-30 foot-gun)
#
# Usage:
#   # Production scan -- exits 0 on clean code, non-zero on any violation.
#   bash ci/grep_bans.sh android/plugin/src/main/ src/mwa/
#
#   # Self-test scan against the fixture files (expected to fire each pattern
#   # exactly once; script exits non-zero because violations are intentional).
#   bash ci/grep_bans.sh --check-only .github/workflows/testdata/grep_ban_fixtures/
#
# The DRY rationale: the workflow YAML invokes this script twice (production
# scan + fixtures self-test) so the pattern list lives in exactly one place.
# Maintainers update the patterns here; the workflow does not need to change.
# -----------------------------------------------------------------------------

set -uo pipefail

# Pattern definitions. Keyed by short id so violation lines can name the
# offending rule. Escaped for ripgrep extended-regex syntax.
# Arrays are aligned: patterns[i] is identified by ids[i] and described by descs[i].
ids=(
    "log-authtoken"
    "log-publickey"
    "log-reveal"
    "sdklog-interp"
)

patterns=(
    'Log\.(v|d|i|w|e)\(.*[aA]uthToken'
    'Log\.(v|d|i|w|e)\(.*[pP]ublicKey'
    'Log\.(v|d|i|w|e)\(.*\.reveal\(\)'
    'SdkLog\.[vdiwe]\([^{]*\$[a-zA-Z_{]'
)

# Case variant `[aA]uthToken` / `[pP]ublicKey`:
# Kotlin exposes both a property-style access and a Java-interop getter.
# `MwaSessionState.authToken` (if the field were public `var`) reads as
# `session.authToken` (lowercase a). The explicit accessor pattern used in
# this story reads as `session.getAuthToken()` (capital A inside a camelCase
# method). Both shapes are realistic leak conduits -- the grep matches the
# first-letter case variant to cover both. `rg --case-sensitive` globally
# (per Concern 1 fix) is preserved; only the keyword's first letter is
# broadened. Same applies to `publicKey` / `getPublicKey()`.

# Regex-design note (Story 1-2 code-review Finding #1):
# Patterns 1, 2, 3 use `.*` (greedy) rather than `[^)]*` between the
# opening `Log.(` and the target token. `[^)]*` stops at the first `)`
# in the log-call argument list, which silently false-negatives on the
# very common real-world shape `Log.d(TAG, getX().authToken)` or
# `Log.d(TAG, getToken().reveal())` -- the nested `()` of `getX()` /
# `getToken()` terminates `[^)]*` before the target keyword is reached.
# `.*` is bounded per-line by grep's default line-scope and catches
# arbitrarily deep receiver chains. (Initial fix used `.*?` lazy-match
# but POSIX ERE / BSD-grep do not support PCRE lazy quantifiers --
# `.*?` parses as `.*` followed by a stray `?` and behaves erratically
# across runner images. Greedy `.*` gives the same pass/fail answer for
# substring presence checks and is portable across grep implementations.)
#
# Pattern 4 continues to use `[^{]*` because the `{` boundary is a
# FEATURE there -- it stops the match at the lambda brace so
# `SdkLog.d(TAG, CORR) { "body with $var" }` is correctly NOT flagged
# (the interpolation is inside the lambda, which R8 will handle).

descs=(
    "android.util.Log call with authToken argument (AC-4 #1, AC-D-8)"
    "android.util.Log call with publicKey argument (AC-4 #2, AC-D-8)"
    "android.util.Log call unwrapping a SecretString via .reveal() (AC-4 #3, AC-D-8) -- matches any receiver, not just the class-qualified form, because SecretString.reveal() is an instance method and real calls look like 'token.reveal()', 'sessionState.authToken.reveal()', etc. The previous class-qualified pattern matched a syntactically impossible call shape and was a silent false-negative (codebase has no other .reveal() methods on 2026-04-20; broaden if future additions appear)"
    "SdkLog.[vdiwe] call with string-interpolation args outside a lambda -- DD-30 foot-gun. Matches BOTH \${expr} braced form AND bare \$identifier form (the tail char class [a-zA-Z_{] disambiguates: { opens a braced expansion, letters/underscore start a bare identifier; literal-\$ in raw strings with non-identifier follow chars like space or end-of-line slip through by design)"
)

# Parse mode flag.
mode="production"
if [[ "${1:-}" == "--check-only" ]]; then
    mode="self-test"
    shift
fi

if [[ $# -eq 0 ]]; then
    echo "usage: $0 [--check-only] <path> [<path> ...]" >&2
    exit 2
fi

paths=("$@")

# Resolve grep binary. Prefer ripgrep for speed + exact regex semantics;
# fall back to grep -E so the script also runs without rg installed.
#
# --case-sensitive on rg is critical: rg's default is smart-case (patterns
# with no uppercase are matched case-insensitively), while grep -E is always
# case-sensitive. Without this flag the script's behavior silently depends
# on whether the CI runner image ships ripgrep -- a violation reported as
# `Log.v(... AUTHTOKEN ...)` would fire on rg-equipped runners and pass on
# rg-less ones. We pick the stricter (case-sensitive) behavior on both
# sides so the source of truth is the pattern literal alone. If the intent
# ever shifts to catch misspelled casings, flip BOTH sides together: add
# `-i` to the grep branch and drop `--case-sensitive` from the rg branch.
if command -v rg >/dev/null 2>&1; then
    grep_cmd=(rg --no-heading --line-number --color=never --case-sensitive)
    grep_flavor="rg"
else
    grep_cmd=(grep -rnE)
    grep_flavor="grep"
fi

violations=0
hits_per_pattern=()

echo "Running $mode scan against:"
for p in "${paths[@]}"; do
    echo "  - $p"
done
echo "Using $grep_flavor."
echo ""

for i in "${!patterns[@]}"; do
    id="${ids[$i]}"
    pat="${patterns[$i]}"
    desc="${descs[$i]}"

    # Collect matches. `|| true` because non-matching runs return non-zero on
    # grep/rg, which `set -e` would treat as a fatal error.
    if [[ "$grep_flavor" == "rg" ]]; then
        matches=$("${grep_cmd[@]}" "$pat" "${paths[@]}" 2>/dev/null || true)
    else
        # grep -rnE with --include support on BSD grep. Scan all regular files
        # under the given paths; exclude binary matches.
        matches=$("${grep_cmd[@]}" --exclude-dir=".git" --exclude-dir="build" "$pat" "${paths[@]}" 2>/dev/null || true)
    fi

    count=$(printf "%s" "$matches" | grep -c . 2>/dev/null || true)
    hits_per_pattern+=("$count")

    if [[ "$count" -gt 0 ]]; then
        if [[ "$mode" == "production" ]]; then
            echo "VIOLATION [$id]: $desc"
            printf "%s\n" "$matches" | sed 's/^/  /'
            echo ""
            violations=$((violations + count))
        else
            echo "FIRED    [$id]: $count match(es) -- $desc"
        fi
    else
        if [[ "$mode" == "self-test" ]]; then
            echo "MISS     [$id]: 0 matches (fixture did not fire pattern)"
        fi
    fi
done

echo ""

if [[ "$mode" == "production" ]]; then
    if [[ "$violations" -gt 0 ]]; then
        echo "FAIL: $violations total violation(s) across ${#patterns[@]} banned patterns."
        exit 1
    fi
    echo "OK: 0 violations across ${#patterns[@]} banned patterns."
    exit 0
fi

# Self-test mode: every pattern MUST fire at least once; any MISS is a bug in
# the grep patterns or the fixtures and must fail CI regardless of whether
# production code is clean.
missed=0
for i in "${!ids[@]}"; do
    if [[ "${hits_per_pattern[$i]}" -eq 0 ]]; then
        missed=$((missed + 1))
    fi
done

if [[ "$missed" -gt 0 ]]; then
    echo "FAIL self-test: $missed of ${#patterns[@]} patterns did not fire against the fixture set."
    exit 1
fi

echo "OK self-test: all ${#patterns[@]} banned patterns fire as expected against the fixture set."
# Self-test passes when every pattern matched -- return 0 so CI treats the
# green state as success. (The story's verification hint mentions "exits
# non-zero" for the self-test, referring to the violation-list being
# non-empty; the workflow wraps this in a separate step that is EXPECTED
# to find violations.)
exit 0
