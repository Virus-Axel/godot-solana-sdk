#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# ci/grep_bans.sh  (Story 1-2 Task 7 — AC-4 enforcement;
#                   Story 1-4 Task 6 — AC-5 extension, pattern 5;
#                   Story 1-5 Task 6 — DD-22 / D-2 extensions, patterns 6+7)
# -----------------------------------------------------------------------------
# Scans source files for the banned patterns documented in AC-4 + AC-D-8
# (patterns 1-4, Story 1-2), AC-5 (pattern 5, Story 1-4), and Story 1-5's
# thread-marshalling + test-compile-gate enforcement (patterns 6-7):
#
#   1. Log.[vdiwe](...authToken...)        -- android.util.Log call with authToken
#   2. Log.[vdiwe](...publicKey...)        -- android.util.Log call with publicKey
#   3. Log.[vdiwe](...SecretString.reveal())  -- Log call that unwraps SecretString
#   4. SdkLog.[vdiwe](...${...)            -- SdkLog call with string interpolation
#                                             outside a lambda (DD-30 foot-gun)
#   5. #ifdef __ANDROID__ (+ 3 spelling variants) outside
#                                             src/mwa/platform_selector.cpp (AC-5)
#   6. emit_signal(...) call outside src/mwa/godot_main_dispatcher.cpp — DD-22
#                                             thread-marshalling: every signal
#                                             to GDScript crosses to Godot main
#                                             via the dispatcher's call_deferred
#                                             (or, under MWA_TESTING, via its
#                                             drain_for_testing). Direct
#                                             `emit_signal(...)` anywhere else
#                                             in src/mwa/ is a marshalling bypass.
#   7. #define MWA_TESTING ...             -- MWA_TESTING is a scons-tests-only
#                                             compile define (-DMWA_TESTING=1);
#                                             source-level #define would leak the
#                                             test-mode drain path / setter into
#                                             production builds. Zero legitimate
#                                             #define MWA_TESTING in source files.
#
# Usage:
#   # Production scan -- exits 0 on clean code, non-zero on any violation.
#   bash ci/grep_bans.sh android/plugin/src/main/ src/mwa/
#
#   # Self-test scan against the fixture files.
#   bash ci/grep_bans.sh --check-only .github/workflows/testdata/grep_ban_fixtures/
#
#   Self-test semantics (IMPORTANT — inverts production mode):
#     - Every fixture file is INTENTIONALLY a violation. The test asserts that
#       each pattern FIRES at least once against the fixture set.
#     - Exit 0 = every pattern fired as expected (success).
#     - Exit 1 = one or more patterns failed to fire (pattern regression). The
#       workflow YAML invokes self-test BEFORE the production scan; the two
#       ends agree that "self-test exit 0 is pass" — do NOT invert in YAML.
#     - Rationale: self-test is checking the ASSERTION LOGIC ("the pattern
#       catches what it's meant to catch"), not the SOURCE TREE ("the source
#       has violations"). Production mode checks the source tree.
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
    "ifdef-android-outside-selector"
    "emit-signal-outside-dispatcher"
    "mwa-testing-define"
    "log-payload-json"
    "cluster-mismatch-branch"
)

patterns=(
    'Log\.(v|d|i|w|e)\(.*[aA]uthToken'
    'Log\.(v|d|i|w|e)\(.*[pP]ublicKey'
    'Log\.(v|d|i|w|e)\(.*\.reveal\(\)'
    'SdkLog\.[vdiwe]\([^{]*\$[a-zA-Z_{]'
    '^[[:space:]]*#[[:space:]]*(if(n?def)?[[:space:]]+__ANDROID__|if[[:space:]]+!?[[:space:]]*defined[[:space:]]*\([[:space:]]*__ANDROID__[[:space:]]*\))'
    '\bemit_signal[[:space:]]*\('
    '^[[:space:]]*#[[:space:]]*define[[:space:]]+MWA_TESTING\b'
    '(Log|SdkLog)\.(v|d|i|w|e).*(resultDictJson|errorDictJson|timeoutDictJson|cancelledDictJson)'
    '(cluster_mismatch|cluster[[:space:]]*!=[[:space:]]*activeCluster|cachedRecord\.cluster[[:space:]]*!=)'
)

# Per-pattern production-scan exclusion (Story 1-4 Task 6, AC-5).
# Parallel to patterns[]: excludes[i] names a single path that the pattern
# must NOT flag when running the PRODUCTION scan. Empty string = no
# exclusion (patterns 1-4 have no sanctioned violators). Pattern 5 excludes
# `src/mwa/platform_selector.cpp` — the sole TU in the C++ source tree
# allowed to contain `#ifdef __ANDROID__` (by design; see file-header
# comment in that file + AC-5 in docs/stories/1-4.md). The exclusion is
# intentionally keyed on path, not on an "allow-list" comment marker,
# because the platform-selector pattern is a one-off architectural choice
# and should not invite lookalikes elsewhere in the tree.
#
# Self-test mode ignores excludes[]: the fixture directory is the whole
# scan scope, and every fixture file IS a violation — the excludes[] path
# (src/mwa/platform_selector.cpp) never appears under the fixtures dir so
# applying it there would be a silent no-op anyway.
excludes=(
    ""
    ""
    ""
    ""
    "src/mwa/platform_selector.cpp"
    "src/mwa/godot_main_dispatcher.cpp"
    ""
    ""
    ""
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

# Pattern 5 (ifdef-android-outside-selector, Story 1-4 AC-5) is a single
# regex that collapses the four `__ANDROID__` spelling variants into one
# alternation:
#
#     #ifdef __ANDROID__
#     #ifndef __ANDROID__
#     #if defined(__ANDROID__)
#     #if !defined(__ANDROID__)
#
# The anchor `^[[:space:]]*#[[:space:]]*` tolerates the two whitespace
# shapes real code uses: `#ifdef` (token-adjacent, C89-style) and `# ifdef`
# (space-separated, allowed by the standard but rare). `if(n?def)?` packs
# `if`, `ifdef`, `ifndef` into a single optional-`n`-optional-`def` group;
# the second branch handles `if defined(...)` + `if !defined(...)` with an
# optional `!`. The parens around `__ANDROID__` are matched literally by
# the ERE `\(...\)` escape (rg and grep -E treat `\(` as a literal paren
# inside a character-class-free context; this matches the documented
# pattern in docs/stories/1-4.md:194 verbatim). The `[[:space:]]*` padding
# inside the parens allows `defined ( __ANDROID__ )` (uncommon but legal).
#
# Production-scan exclusion lives in excludes[4] above, NOT inside the
# regex: a negative-lookahead-on-path would need PCRE and would not work
# in the grep fallback branch. Keeping the exclusion out-of-band also
# means a maintainer reading the pattern does not have to decode the
# exception boundary.

descs=(
    "android.util.Log call with authToken argument (AC-4 #1, AC-D-8). Pattern uses greedy .* which correctly handles nested receiver calls like Log.d(TAG, getX().authToken) — see regex-design note above. Still a bypass surface if future code writes Log.d(formatMessage(authToken)) (authToken hidden inside nested call args rather than the log-call args); if such a shape appears in review, add a broader pattern or switch to a Kotlin-AST-aware scanner."
    "android.util.Log call with publicKey argument (AC-4 #2, AC-D-8). Same nested-call caveat as pattern 1 — greedy .* catches receiver chains but not publicKey buried inside a nested function-call arg like Log.d(formatMessage(publicKey)); flag for broadening if that shape appears."
    "android.util.Log call unwrapping a SecretString via .reveal() (AC-4 #3, AC-D-8) -- matches any receiver, not just the class-qualified form, because SecretString.reveal() is an instance method and real calls look like 'token.reveal()', 'sessionState.authToken.reveal()', etc. The previous class-qualified pattern matched a syntactically impossible call shape and was a silent false-negative (codebase has no other .reveal() methods on 2026-04-20; broaden if future additions appear)"
    "SdkLog.[vdiwe] call with string-interpolation args outside a lambda -- DD-30 foot-gun. Matches BOTH \${expr} braced form AND bare \$identifier form (the tail char class [a-zA-Z_{] disambiguates: { opens a braced expansion, letters/underscore start a bare identifier; literal-\$ in raw strings with non-identifier follow chars like space or end-of-line slip through by design)"
    "'#ifdef __ANDROID__' (or #ifndef / #if defined(__ANDROID__) / #if !defined(__ANDROID__)) outside src/mwa/platform_selector.cpp — AC-5 bans all four spelling variants. The production scan in .github/workflows/grep_bans.yml must exclude src/mwa/platform_selector.cpp via ripgrep's --glob '!src/mwa/platform_selector.cpp' or equivalent."
    "'emit_signal(...)' direct call outside src/mwa/godot_main_dispatcher.cpp (Story 1-5 Task 6, DD-22). Every MWA signal to GDScript MUST cross to Godot main via the dispatcher's call_deferred path (production) or drain_for_testing (MWA_TESTING). A direct emit_signal(...) in any other src/mwa/ TU bypasses both paths. The word-boundary regex matches emit_signal when followed by optional whitespace + '('; string literals like '\"emit_signal\"' (the call_deferred arg) do NOT match because no paren follows the identifier. Excludes src/mwa/godot_main_dispatcher.cpp via excludes[5]."
    "'#define MWA_TESTING' at source level (Story 1-5 Task 6, D-2 / D-3 gate integrity). MWA_TESTING is strictly a scons-tests-only compile define (-DMWA_TESTING=1 set by the tests alias in SConstruct); zero legitimate source-level #define uses exist. A #define in production headers or .cpp files would activate the test-mode drain queue + set_bridge_for_testing symbol in release builds — immediate behavioral regression. #ifdef MWA_TESTING (testing for the flag) is intentionally NOT matched by this pattern; only the directive '#[[:space:]]*define[[:space:]]+MWA_TESTING' fires."
    "Log.* or SdkLog.* call referencing an MWA NativeBridge payload variable name (Story 2-1 Task 4). 'resultDictJson' / 'errorDictJson' / 'timeoutDictJson' / 'cancelledDictJson' are serialized MWA Dictionary payloads that include auth_token (connect path) or enough correlation data to reconstruct a session (error/timeout/cancel paths). The existing log-authtoken ban (pattern 1) only catches direct 'authToken' references — payload vars wrap the token inside a JSON string where the literal 'authToken' substring is not present in the logging call arguments (the LITERAL is inside the payload at runtime). This pattern closes that gap at the payload-var name level: if it's named *DictJson and you're logging it, that's a token-leak candidate regardless of how the payload was built."
    "Forbidden cluster-mismatch branch in mwaReauthorize (Story 2-2 T2, DD-2-2-1 LOCKED). The pattern 'cluster_mismatch', 'cluster != activeCluster', or 'cachedRecord.cluster !=' in the Kotlin plugin source signals a prohibited separate cluster-comparison branch in mwaReauthorize. DD-2-2-1 mandates that cluster mismatch is detected IMPLICITLY via the 3-tuple listAllKeys() filter (DD-2-2-7): a caller who connected on devnet but calls reauthorize with cluster=mainnet finds ZERO records under the (mainnet, solana:mainnet, identityUri) filter — the empty result drives the NOT_CONNECTED branch. A separate cachedRecord.cluster comparison is redundant, misleading, and violates DD-2-2-1. Any addition of such a branch is a Rule 2 deviation requiring an amendment. This static guard enforces the invariant at CI-time without relying on unit-test coverage of the implementation's internal control flow."
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
    exclude="${excludes[$i]:-}"

    # Per-pattern production-scan exclusion plumbing. Skip in self-test mode
    # (fixture dir never contains the excluded real-source path; applying it
    # there is a no-op anyway, but suppressing it keeps the self-test scan
    # path identical in shape to the original Story 1-2 implementation).
    rg_extra=()
    if [[ "$mode" == "production" && -n "$exclude" ]]; then
        rg_extra=(--glob "!${exclude}")
    fi

    # Collect matches. `|| true` because non-matching runs return non-zero on
    # grep/rg, which `set -e` would treat as a fatal error.
    if [[ "$grep_flavor" == "rg" ]]; then
        matches=$("${grep_cmd[@]}" "${rg_extra[@]}" "$pat" "${paths[@]}" 2>/dev/null || true)
    else
        # grep -rnE with --include support on BSD grep. Scan all regular files
        # under the given paths; exclude binary matches.
        matches=$("${grep_cmd[@]}" --exclude-dir=".git" --exclude-dir="build" "$pat" "${paths[@]}" 2>/dev/null || true)
        # Post-filter the excluded path in the grep fallback branch: grep's
        # --exclude matches on basename only, which would over-exclude any
        # same-basename file elsewhere. A simple grep -v on the full path is
        # both tighter and portable across BSD/GNU grep.
        if [[ "$mode" == "production" && -n "$exclude" && -n "$matches" ]]; then
            matches=$(printf "%s\n" "$matches" | grep -v -F "$exclude" || true)
        fi
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
