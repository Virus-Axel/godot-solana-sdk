#!/usr/bin/env python3
"""Generate 3-language error-code artifacts from tools/error-codes.yaml.

Usage:
    python tools/gen_error_codes.py                 # regenerate in-place
    python tools/gen_error_codes.py --check         # exit non-zero if drift detected
    python tools/gen_error_codes.py --manifest X    # use a custom manifest path

CI workflows:
    codegen_drift_check.yml  — runs with --check
    generator_tests.yml      — runs pytest tools/ (independent, per DD-33)
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parent.parent
if str(_REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(_REPO_ROOT))

from tools.mwa_codegen.emit import render_cpp, render_gdscript, render_kotlin
from tools.mwa_codegen.schema import ManifestValidationError, load_manifest

REPO_ROOT = Path(__file__).resolve().parent.parent

DEFAULT_MANIFEST = REPO_ROOT / "tools" / "error-codes.yaml"

TARGETS = {
    "cpp": REPO_ROOT / "src" / "generated" / "mwa_error_codes.hpp",
    "kotlin": (
        REPO_ROOT
        / "android"
        / "plugin"
        / "src"
        / "generated"
        / "kotlin"
        / "com"
        / "godotengine"
        / "godot_solana_sdk"
        / "mwa"
        / "generated"
        / "MwaError.kt"
    ),
    "gdscript": REPO_ROOT / "addons" / "SolanaSDK" / "mwa_error_codes.gd",
}

RENDERERS = {
    "cpp": render_cpp,
    "kotlin": render_kotlin,
    "gdscript": render_gdscript,
}


def generate(manifest_path: Path) -> dict[str, str]:
    manifest = load_manifest(manifest_path)
    return {name: renderer(manifest) for name, renderer in RENDERERS.items()}


def write_targets(outputs: dict[str, str]) -> None:
    for name, content in outputs.items():
        target = TARGETS[name]
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(content, encoding="utf-8", newline="\n")


def check_targets(outputs: dict[str, str]) -> list[str]:
    diffs: list[str] = []
    for name, expected in outputs.items():
        target = TARGETS[name]
        if not target.exists():
            diffs.append(f"{target}: file does not exist")
            continue
        actual = target.read_text(encoding="utf-8")
        if actual != expected:
            diffs.append(f"{target}: content differs")
    return diffs


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--manifest",
        type=Path,
        default=DEFAULT_MANIFEST,
        help="Path to error-codes.yaml (default: %(default)s)",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="Exit non-zero if generated output differs from committed files.",
    )
    args = parser.parse_args()

    try:
        outputs = generate(args.manifest)
    except ManifestValidationError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        if exc.field:
            print(f"  field: {exc.field}", file=sys.stderr)
        if exc.code_name:
            print(f"  code: {exc.code_name}", file=sys.stderr)
        return 1

    if args.check:
        diffs = check_targets(outputs)
        if diffs:
            print("DRIFT DETECTED:", file=sys.stderr)
            for d in diffs:
                print(f"  {d}", file=sys.stderr)
            return 1
        print("OK: generated artifacts match committed files.")
        return 0

    write_targets(outputs)
    for name in TARGETS:
        print(f"  wrote {TARGETS[name]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
