#!/usr/bin/env bash
# tools/check_license_diff.sh — verifies repo-root LICENSE body matches the OSI
# canonical MIT template, byte-identical except for the copyright-holder line.
#
# Story 5-5 T7 deliverable per A-18 (AC-5 rewording: original "upstream
# solana-mobile/mobile-wallet-adapter LICENSE (MIT text)" was a spec error;
# upstream is Apache-2.0, not MIT). Story 5-6 release-on-tag CI invokes this
# script as a PR-blocking gate.
#
# Exits 0 on body-match (after stripping any line starting with "Copyright").
# Exits 1 on body drift, prints the diff to stderr.

set -euo pipefail

LICENSE_PATH="LICENSE"

if [ ! -f "$LICENSE_PATH" ]; then
    echo "check_license_diff: $LICENSE_PATH not found at repo root" >&2
    exit 1
fi

# OSI canonical MIT template (https://opensource.org/license/mit/), embedded to
# avoid network dependency in CI. Copyright line is the placeholder
# "<YEAR> <COPYRIGHT HOLDER>" — both are stripped before comparison via
# `grep -v '^Copyright'`.
OSI_MIT=$(cat <<'EOF'
MIT License

Copyright (c) <YEAR> <COPYRIGHT HOLDER>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
EOF
)

if diff <(echo "$OSI_MIT" | grep -v '^Copyright') <(grep -v '^Copyright' "$LICENSE_PATH"); then
    echo "PASS: LICENSE body matches OSI canonical MIT template (excluding copyright-holder line)."
    exit 0
else
    echo "FAIL: LICENSE body diverges from OSI canonical MIT template. See diff above." >&2
    exit 1
fi
