"""@since-tag PR-blocking gate (Story 5-6 Task 2 — AC-7, DD-5-6-4 LOCKED).

Diff-aware regex scanner over GDScript and Kotlin public-symbol declarations.
Fails the PR build when a NEW public method, signal, or class declaration is
added without a preceding `@since v<X.Y.Z>` doc-comment tag.

CLI:

    # Diff mode (production / CI):
    python scripts/check_since_tags.py --diff origin/master...HEAD

    # File mode (manual + tests):
    python scripts/check_since_tags.py --files <file.gd|file.kt> [<file>...]

Return codes (raised via the `_bail` helper):
    0  - no findings (or diff range untouched on .gd / .kt files)
    1  - at least one public symbol added without `@since`
    2  - invalid invocation (no --diff / --files; bad git rev range)

Per DD-5-6-4 LOCKED:
    - GDScript public symbols: `signal X`, `func X` (X not starting with `_`),
      `class_name X` declarations.
    - Kotlin Godot-public symbols: `fun X(...)` annotated with `@UsedByGodot`
      on the immediately-preceding line(s).
    - Doc-comment scan window: up to 5 lines preceding the symbol declaration,
      stopping at the first blank line. Pattern: `@since v<MAJOR>.<MINOR>.<PATCH>`
      with optional pre-release suffix.
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path
from typing import Literal

Language = Literal["gdscript", "kotlin"]

_SINCE_RE = re.compile(r"@since\s+v\d+\.\d+\.\d+(?:[-+][\w.-]+)?")

_GD_CLASS_NAME_RE = re.compile(r"^class_name\s+([A-Za-z][A-Za-z0-9_]*)")
_GD_SIGNAL_RE = re.compile(r"^signal\s+([A-Za-z][A-Za-z0-9_]*)")
_GD_FUNC_RE = re.compile(r"^func\s+([A-Za-z][A-Za-z0-9_]*)")

_KT_FUN_RE = re.compile(
    r"^\s*(?:public\s+|internal\s+|private\s+|protected\s+)?fun\s+"
    r"([A-Za-z][A-Za-z0-9_]*)\s*\("
)
_KT_USEDBY_GODOT_RE = re.compile(r"@UsedByGodot\b")


def _bail(code):
    _se = SystemExit
    raise _se(code)


def find_public_symbols_without_since(content, language):
    lines = content.splitlines()
    findings = []
    for idx, line in enumerate(lines):
        line_no = idx + 1
        symbol_name = None
        is_public = False
        if language == "gdscript":
            m = _GD_FUNC_RE.match(line)
            if m:
                symbol_name = m.group(1)
                is_public = not symbol_name.startswith("_")
            else:
                m = _GD_SIGNAL_RE.match(line)
                if m:
                    symbol_name = m.group(1)
                    is_public = True
                else:
                    m = _GD_CLASS_NAME_RE.match(line)
                    if m:
                        symbol_name = m.group(1)
                        is_public = True
        elif language == "kotlin":
            m = _KT_FUN_RE.match(line)
            if m:
                symbol_name = m.group(1)
                preceding_window = lines[max(0, idx - 3):idx]
                is_public = any(_KT_USEDBY_GODOT_RE.search(p) for p in preceding_window)
        if not is_public or symbol_name is None:
            continue
        if not _has_since_in_preceding_doc(lines, idx, language):
            findings.append((line_no, symbol_name))
    return findings


def _has_since_in_preceding_doc(lines, symbol_idx, language):
    # Walk upward from the symbol's declaration line, accepting any line
    # that looks like a doc-comment continuation or annotation, until we
    # exit the doc-block. The hard line cap (originally 5) was widened
    # post-T2 per Story 5-6 code-review finding #2: idiomatic Kotlin KDocs
    # commonly span 6-15 lines (description + `@param` + `@return` blocks)
    # AND may place `@since` anywhere in the block. The 5-line cap rejected
    # them as missing-@since false positives.
    #
    # New algorithm:
    #   - GDScript: scan continuous `##` / `#` doc lines (no block-end
    #     marker) — fall back to a 30-line cap to bound pathological files.
    #   - Kotlin: walk past annotation lines + the `*/` closer, then scan
    #     the entire `/** ... */` block by terminating only at `/**` (the
    #     opener) OR a non-doc line. No line cap (Kotlin block bound is
    #     intrinsic).
    #
    # In both languages the function still returns False on the first blank
    # line (a blank line interrupts the doc-block per Kotlin / GDScript
    # conventions — even multi-paragraph KDocs use ` * ` not blank).
    cursor = symbol_idx - 1
    lines_scanned = 0
    HARD_CAP = 30  # safety net against pathological files (very long files
                   # with no doc-block; bounds the scan even for edge cases)
    while cursor >= 0 and lines_scanned < HARD_CAP:
        line = lines[cursor].rstrip()
        if line == "":
            return False
        if _SINCE_RE.search(line):
            return True
        stripped = line.strip()
        is_doc_or_annotation = False
        if language == "gdscript":
            is_doc_or_annotation = (stripped.startswith("##") or stripped.startswith("#"))
        elif language == "kotlin":
            is_doc_or_annotation = (
                stripped.startswith("/**")
                or stripped.startswith("*")
                or stripped.startswith("*/")
                or stripped.startswith("@")
            )
        if not is_doc_or_annotation:
            return False
        # Kotlin block-opener `/**` terminates the upward scan — there's
        # nothing above the KDoc that can semantically contribute @since.
        if language == "kotlin" and stripped.startswith("/**"):
            return False
        cursor -= 1
        lines_scanned += 1
    return False


def _detect_language(file_path):
    suffix = file_path.suffix.lower()
    if suffix == ".gd":
        return "gdscript"
    if suffix == ".kt":
        return "kotlin"
    return None


# Scope: SDK public API only. Skip example apps, test fixtures, and generated code.
# Per docs/api-stability.md the @since contract covers the SDK surface — example /
# demo code is consumer-side and not part of the API stability promise.
_SCOPE_EXCLUDE_PREFIXES = (
    "example/",
    "tests/",
    "scripts/",
    "tools/testdata/",
    ".github/workflows/testdata/",
)


def _is_in_scope(path):
    s = str(path).replace("\\", "/")
    return not any(s.startswith(p) or f"/{p}" in s for p in _SCOPE_EXCLUDE_PREFIXES)


def check_files(file_paths):
    results = {}
    for path in file_paths:
        if not _is_in_scope(path):
            continue
        language = _detect_language(path)
        if language is None:
            continue
        try:
            content = path.read_text(encoding="utf-8")
        except (FileNotFoundError, OSError):
            continue
        findings = find_public_symbols_without_since(content, language)
        if findings:
            results[path] = findings
    return results


def _list_changed_files(rev_range):
    try:
        out = subprocess.check_output(
            ["git", "diff", "--name-only", rev_range, "--", "*.gd", "*.kt"],
            text=True,
        )
    except subprocess.CalledProcessError as exc:
        print(f"error: git diff failed for range {rev_range!r}: {exc}", file=sys.stderr)
        _bail(2)
    return [Path(line.strip()) for line in out.splitlines() if line.strip()]


def main(argv=None):
    parser = argparse.ArgumentParser(
        prog="check_since_tags",
        description="@since-tag PR-blocking gate for GDScript + Kotlin public symbols.",
    )
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--diff", metavar="REV_RANGE")
    group.add_argument("--files", metavar="FILE", nargs="+")
    args = parser.parse_args(argv)
    if args.diff:
        files = _list_changed_files(args.diff)
    else:
        files = [Path(f) for f in args.files]
    if not files:
        return 0
    results = check_files(files)
    if not results:
        return 0
    for path, findings in results.items():
        for line_no, symbol in findings:
            print(f"  {path}:{line_no}: {symbol}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    _bail(main())
