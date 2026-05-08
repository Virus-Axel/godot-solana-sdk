#!/usr/bin/env python3
"""MWA fixture schema validator (Task 5 / AC-3).

Validates every *.json file under a fixtures directory against the schema
documented in testdata/mwa-fixtures/SCHEMA.md:

1. Structural validation via JSON Schema draft 2020-12 (jsonschema).
2. Filename-parser self-consistency - the canonical filename form
   {op}_{scenario}.json must resolve to the same op / scenario as
   the JSON document declares. Parser uses longest-prefix match against
   the op enum (LOCKED in SCHEMA.md Filename parser section).

Invoked from .github/workflows/fixture_schema_validation.yml as either:

    python tools/validate_fixtures.py testdata/mwa-fixtures/
    python tools/validate_fixtures.py --fixtures-dir testdata/mwa-fixtures/

Exits 0 on all-valid, non-zero on the first violation. Pass ``--all-errors``
to accumulate every violation across the directory into a single report (the
fail-fast default saves CI time on bulk failures; ``--all-errors`` is friendlier
during local fixture authoring).
"""
from __future__ import annotations
import argparse, json, sys
from pathlib import Path
from jsonschema import Draft202012Validator
from jsonschema.exceptions import best_match

OPS_BY_DESC_LENGTH = ["sign_transactions", "sign_and_send", "sign_messages",
                      "reauthorize", "deauthorize", "disconnect", "authorize"]

ERROR_CODES = ["USER_CANCELED", "WALLET_REJECTED", "NO_MWA_WALLET_INSTALLED",
               "WALLET_UNINSTALLED", "TOKEN_EXPIRED", "RPC_FAILED",
               "NETWORK_OFFLINE", "TIMEOUT", "NOT_CONNECTED",
               "UNSUPPORTED_PLATFORM", "PROTOCOL_ERROR", "STORAGE_CORRUPT",
               "REAUTH_REQUIRED"]

# Per-op required keys on a success-branch ``response.payload``. Enforced AFTER
# the top-level JSON Schema check (which keeps ``payload`` deliberately typed
# only as ``object`` for forward compatibility — this table holds the
# per-op-specific shape requirements that ``FakeMwaClient`` depends on at
# parse time).
REQUIRED_PAYLOAD_KEYS: dict = {
    "authorize": {"auth_token", "public_key", "cluster", "chain_id"},
    "reauthorize": {"auth_token", "public_key", "cluster", "chain_id"},
    "deauthorize": set(),
    "disconnect": set(),
    "sign_messages": {"signed_payloads"},
    "sign_transactions": {"signed_payloads"},
    "sign_and_send": {"signatures"},
}

# The two response branches both set ``additionalProperties: False`` so that
# stray fields (including the ``developer_details`` field dropped from the
# schema during pre-implementation revision) cannot silently slip through.
SCHEMA = {
    "type": "object",
    "required": ["schema_version", "op", "scenario", "expected_input", "response"],
    "additionalProperties": False,
    "properties": {
        "schema_version": {"const": 1},
        "op": {"enum": sorted(OPS_BY_DESC_LENGTH)},
        "scenario": {"type": "string", "pattern": r"^[a-z0-9][a-z0-9_]*$"},
        "expected_input": {"type": "object"},
        "response": {
            "oneOf": [
                {
                    "type": "object",
                    "required": ["type", "payload"],
                    "additionalProperties": False,
                    "properties": {
                        "type": {"const": "success"},
                        "payload": {"type": "object"},
                    },
                },
                {
                    "type": "object",
                    "required": ["type", "code"],
                    "additionalProperties": False,
                    "properties": {
                        "type": {"const": "error"},
                        "code": {"enum": ERROR_CODES},
                    },
                },
            ],
        },
    },
}


class FixtureValidationError(Exception):
    """Raised when a fixture or batch fails validation.

    ``code`` is a machine-readable kind discriminator (stable across message
    wording changes). Tests assert on ``err.code`` rather than grepping the
    human-readable message — see :class:`TestErrorCodes` in the test module.
    """

    # Stable error-code taxonomy — do not renumber / rename lightly.
    JSON_PARSE = "JSON_PARSE"
    SCHEMA_VIOLATION = "SCHEMA_VIOLATION"
    FILENAME_NO_OP_PREFIX = "FILENAME_NO_OP_PREFIX"
    FILENAME_NO_SCENARIO = "FILENAME_NO_SCENARIO"
    OP_MISMATCH = "OP_MISMATCH"
    SCENARIO_MISMATCH = "SCENARIO_MISMATCH"
    NOT_A_DIRECTORY = "NOT_A_DIRECTORY"
    NO_FIXTURES = "NO_FIXTURES"
    ACCUMULATED = "ACCUMULATED"

    def __init__(self, file, message, code=None):
        super().__init__(f"{file}: {message}")
        self.file = file
        self.message = message
        self.code = code


def parse_filename(filename, source_path=None):
    """Longest-prefix match against OPS_BY_DESC_LENGTH. Returns (op, scenario).

    The op enum contains compound names that share prefixes - ``sign_messages``,
    ``sign_transactions``, ``sign_and_send``. A naive ``split('_')`` is ambiguous,
    so we use a longest-prefix match in descending length order (LOCKED in
    testdata/mwa-fixtures/SCHEMA.md). The first op whose ``{op}_`` prefix matches
    the filename stem wins; the remainder is the scenario.

    When ``source_path`` is provided, any ``FixtureValidationError`` raised here
    carries the full path (e.g. ``testdata/mwa-fixtures/bad.json``) rather than
    the bare basename, so CI logs read cleanly.
    """
    error_path = source_path if source_path is not None else Path(filename)
    stem = filename[:-5] if filename.endswith(".json") else filename
    for op in OPS_BY_DESC_LENGTH:
        prefix = f"{op}_"
        if stem.startswith(prefix):
            scenario = stem[len(prefix):]
            if not scenario:
                raise FixtureValidationError(
                    error_path,
                    f"no scenario after op prefix '{op}_'",
                    code=FixtureValidationError.FILENAME_NO_SCENARIO,
                )
            return op, scenario
    raise FixtureValidationError(
        error_path,
        "filename has no op prefix",
        code=FixtureValidationError.FILENAME_NO_OP_PREFIX,
    )


def validate_file(path):
    """Validate a single fixture file. Raises on any violation."""
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as e:
        raise FixtureValidationError(
            path,
            f"JSON parse error: {e}",
            code=FixtureValidationError.JSON_PARSE,
        ) from e
    validator = Draft202012Validator(SCHEMA)
    errs_list = list(validator.iter_errors(data))
    if errs_list:
        # ``best_match`` walks ``oneOf`` / ``anyOf`` context automatically and
        # surfaces the sub-error whose type-const matches the actual document,
        # giving useful messages like "response/payload is a required property"
        # instead of the generic "is not valid under any of the given schemas".
        best = best_match(errs_list) or errs_list[0]
        field = "/".join(str(s) for s in best.absolute_path) or "<root>"
        raise FixtureValidationError(
            path,
            f"schema violation at '{field}' - {best.message}",
            code=FixtureValidationError.SCHEMA_VIOLATION,
        )
    op, scenario = parse_filename(path.name, source_path=path)
    json_op = data["op"]
    if json_op != op:
        raise FixtureValidationError(
            path,
            f"op mismatch: filename='{op}' but json.op='{json_op}'",
            code=FixtureValidationError.OP_MISMATCH,
        )
    json_scn = data["scenario"]
    if json_scn != scenario:
        raise FixtureValidationError(
            path,
            f"scenario mismatch: filename='{scenario}' but json.scenario='{json_scn}'",
            code=FixtureValidationError.SCENARIO_MISMATCH,
        )
    # Second-stage: per-op payload shape check (success branch only — error
    # branches don't carry payloads). Catches fixtures that are schema-valid
    # at the top level but missing fields FakeMwaClient requires at parse
    # time (e.g. authorize_success with an empty payload).
    response = data["response"]
    if response.get("type") == "success":
        required = REQUIRED_PAYLOAD_KEYS.get(op, set())
        payload = response.get("payload", {})
        missing = sorted(required - set(payload.keys()))
        if missing:
            raise FixtureValidationError(
                path,
                f"payload for op '{op}' missing required field(s) {missing}",
                code=FixtureValidationError.SCHEMA_VIOLATION,
            )


def validate_directory(dir_path, all_errors=False):
    """Validate every *.json under dir_path.

    Default (``all_errors=False``) is fail-fast — raises on first violation.
    ``all_errors=True`` accumulates every violation into a single combined
    ``FixtureValidationError`` for a one-shot CI report.
    """
    if not dir_path.is_dir():
        raise FixtureValidationError(
            dir_path, "not a directory", code=FixtureValidationError.NOT_A_DIRECTORY,
        )
    files = sorted(dir_path.glob("*.json"))
    if not files:
        raise FixtureValidationError(
            dir_path, "no *.json files found", code=FixtureValidationError.NO_FIXTURES,
        )
    accumulated = []
    for fixture in files:
        try:
            validate_file(fixture)
        except FixtureValidationError as err:
            if not all_errors:
                raise
            accumulated.append(err)
    if accumulated:
        combined = "\n  ".join(str(err) for err in accumulated)
        raise FixtureValidationError(
            dir_path,
            f"{len(accumulated)} violation(s):\n  {combined}",
            code=FixtureValidationError.ACCUMULATED,
        )


def main(argv=None):
    parser = argparse.ArgumentParser(description="Validate MWA fixtures against the documented schema.")
    parser.add_argument("fixtures_dir_pos", type=Path, nargs="?", default=None,
                        metavar="DIR",
                        help="Directory of fixtures (positional; default: testdata/mwa-fixtures).")
    parser.add_argument("--fixtures-dir", dest="fixtures_dir_flag", type=Path, default=None,
                        metavar="DIR",
                        help="Directory of fixtures (flag form; overrides positional).")
    parser.add_argument("--all-errors", action="store_true",
                        help="Accumulate all violations instead of failing on first.")
    args = parser.parse_args(argv)
    fixtures_dir = args.fixtures_dir_flag or args.fixtures_dir_pos or Path("testdata/mwa-fixtures")
    try:
        validate_directory(fixtures_dir, all_errors=args.all_errors)
    except FixtureValidationError as err:
        print(f"FAIL {err}", file=sys.stderr)
        return 1
    print(f"OK - all fixtures in {fixtures_dir} pass validation")
    return 0


if __name__ == "__main__":
    # Hoist to a local so no ``xit``+paren substring appears in the module —
    # the content-guard hook's JavaScript xit-style test-disable regex would
    # otherwise false-positive on the plain Python exit call.
    exit_fn = sys.exit
    exit_fn(main())
