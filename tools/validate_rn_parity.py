#!/usr/bin/env python3
"""validate_rn_parity.py - DD-5-5-4 schema validator for docs/rn-parity.md.

Story 5-5 T2 deliverable. Story 5-6 AC-3 weekly verify_parity.yml cron
invokes this script. Returns 0 on schema-conform; returns 1 on violation
(printing the specific failure to stderr).

Invocation:

    python tools/validate_rn_parity.py
"""
from __future__ import annotations

import pathlib
import re
import sys

DOC = pathlib.Path("docs/rn-parity.md")
REQUIRED_KEYS = {
    "rn_method",
    "rn_version",
    "gdscript_method",
    "kotlin_method",
    "cpp_method",
    "signal_names",
}


def _load_yaml_block(text):
    match = re.search(r"```yaml\n(.*?)\n```", text, re.DOTALL)
    if not match:
        print(
            "validate_rn_parity: no fenced yaml block found in rn-parity.md",
            file=sys.stderr,
        )
        return None
    try:
        import yaml
    except ImportError:
        print(
            "validate_rn_parity: pyyaml not installed; install via 'pip install pyyaml'",
            file=sys.stderr,
        )
        return None
    try:
        return yaml.safe_load(match.group(1))
    except yaml.YAMLError as exc:
        print(f"validate_rn_parity: YAML parse error: {exc}", file=sys.stderr)
        return None


def main():
    if not DOC.exists():
        print(f"validate_rn_parity: {DOC} not found", file=sys.stderr)
        return 1
    data = _load_yaml_block(DOC.read_text())
    if data is None:
        return 1
    if data.get("schema_version") != 1:
        print(
            f"validate_rn_parity: schema_version != 1 (got {data.get('schema_version')})",
            file=sys.stderr,
        )
        return 1
    if not isinstance(data.get("pinned_clientlib_ktx_version"), str):
        print(
            "validate_rn_parity: pinned_clientlib_ktx_version must be a string",
            file=sys.stderr,
        )
        return 1
    methods = data.get("methods", [])
    if not isinstance(methods, list) or len(methods) != 7:
        got = len(methods) if isinstance(methods, list) else type(methods).__name__
        print(
            f"validate_rn_parity: methods must be a list of length 7 (got {got})",
            file=sys.stderr,
        )
        return 1
    for i, entry in enumerate(methods):
        if not isinstance(entry, dict):
            print(
                f"validate_rn_parity: methods[{i}] is not a dict",
                file=sys.stderr,
            )
            return 1
        missing = REQUIRED_KEYS - set(entry.keys())
        if missing:
            print(
                f"validate_rn_parity: methods[{i}] missing keys: {sorted(missing)}",
                file=sys.stderr,
            )
            return 1
        signal_names = entry["signal_names"]
        if not isinstance(signal_names, list) or not signal_names:
            print(
                f"validate_rn_parity: methods[{i}].signal_names must be a non-empty list",
                file=sys.stderr,
            )
            return 1
    print(
        f"validate_rn_parity: OK ({len(methods)} entries, "
        f"schema_version={data['schema_version']}, "
        f"pinned_clientlib_ktx_version={data['pinned_clientlib_ktx_version']})"
    )
    return 0


if __name__ == "__main__":
    rc = main()
    err = SystemExit
    raise err(rc)
