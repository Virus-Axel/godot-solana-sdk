"""Tests for tools/validate_fixtures.py (Story 1-6 Task 5).

Two test groups:

1. Filename-parser tests — longest-prefix match against the op enum, covering
   simple ops, compound ops (sign_messages vs a hypothetical sign), the 3-underscore
   sign_and_send op, digit-leading scenarios (Story 1-6 Issue 1 fix), and rejection
   of filenames without an op prefix.

2. Schema + self-consistency tests — valid fixtures pass; each distinct violation
   category produces a named FixtureValidationError. Directory-level validation
   is exercised against the canonical testdata/mwa-fixtures/ (22 files) to prove
   the real fixture set round-trips without changes.

CI wires this via .github/workflows/fixture_schema_validation.yml
(PR-blocking per AC-3).
"""

from __future__ import annotations

from pathlib import Path

import pytest

from tools.validate_fixtures import (
    ERROR_CODES,
    FixtureValidationError,
    OPS_BY_DESC_LENGTH,
    main,
    parse_filename,
    validate_directory,
    validate_file,
)

REPO_ROOT = Path(__file__).resolve().parent.parent
TOOLS_TESTDATA = REPO_ROOT / "tools" / "testdata"
GOOD = TOOLS_TESTDATA / "fixtures_good"
BAD = TOOLS_TESTDATA / "fixtures_bad"
CANONICAL = REPO_ROOT / "testdata" / "mwa-fixtures"


# ---------------------------------------------------------------------------
# Group 1: Filename parser — longest-prefix match against op enum
# ---------------------------------------------------------------------------


class TestParseFilename:
    @pytest.mark.parametrize(
        ("filename", "expected_op", "expected_scenario"),
        [
            ("authorize_success.json", "authorize", "success"),
            ("reauthorize_token_expired.json", "reauthorize", "token_expired"),
            ("deauthorize_success.json", "deauthorize", "success"),
            ("disconnect_success.json", "disconnect", "success"),
            # Compound op — "sign_messages" must match before a hypothetical "sign" entry
            ("sign_messages_wallet_rejected.json", "sign_messages", "wallet_rejected"),
            ("sign_transactions_success.json", "sign_transactions", "success"),
            # 3-underscore op — longest prefix wins even with internal underscores
            ("sign_and_send_success.json", "sign_and_send", "success"),
            ("sign_and_send_network_offline.json", "sign_and_send", "network_offline"),
            # Story 1-6 Issue 1 fix — leading-digit scenario
            ("authorize_10_cycle.json", "authorize", "10_cycle"),
        ],
    )
    def test_longest_prefix_match(
        self, filename: str, expected_op: str, expected_scenario: str
    ) -> None:
        op, scenario = parse_filename(filename)
        assert op == expected_op
        assert scenario == expected_scenario

    @pytest.mark.parametrize(
        "filename",
        [
            "_leading_underscore.json",
            "bogus_scenario.json",  # "bogus" is not in op enum
            "sign_only.json",  # "sign" alone is not in op enum; "sign_messages" et al are
            "scenario_only.json",
            "justanop.json",  # no underscore separator
        ],
    )
    def test_rejects_no_op_prefix(self, filename: str) -> None:
        with pytest.raises(FixtureValidationError, match="no op prefix"):
            parse_filename(filename)

    def test_ops_sorted_desc_by_length(self) -> None:
        """Order matters — longest prefixes first so sign_messages wins over sign*."""
        lengths = [len(op) for op in OPS_BY_DESC_LENGTH]
        assert lengths == sorted(lengths, reverse=True), (
            f"OPS_BY_DESC_LENGTH must be sorted descending by length; got {lengths}"
        )


# ---------------------------------------------------------------------------
# Group 2: Schema + self-consistency
# ---------------------------------------------------------------------------


class TestValidateFile:
    @pytest.mark.parametrize(
        "filename",
        [
            "authorize_minimal.json",
            "authorize_10_cycle.json",
            "sign_messages_compound.json",
            "sign_and_send_compound.json",
        ],
    )
    def test_good_fixtures_pass(self, filename: str) -> None:
        validate_file(GOOD / filename)  # must not raise

    @pytest.mark.parametrize(
        ("filename", "expected_fragment"),
        [
            ("authorize_nover.json", "schema_version"),
            ("authorize_badversion.json", "schema_version"),
            ("authorize_badopinside.json", "'op'"),
            ("authorize_badcode.json", "response"),
            ("authorize_unknowncode.json", "code"),
            ("authorize_badscenario.json", "scenario"),
            # A stray top-level property on ``response`` triggers an oneOf
            # failure (neither branch accepts it — the success branch forbids
            # extras via ``additionalProperties: False``, the error branch
            # fails the type=error const). Message surfaces the mismatch via
            # either branch; we assert the ``response`` path appears in the
            # error so we know the schema tightening is in effect.
            ("authorize_extraprop.json", "response"),
            # Schema-valid at the top level but the success ``payload`` is
            # empty — the per-op REQUIRED_PAYLOAD_KEYS second-stage check
            # catches this (FakeMwaClient would blow up at parse time
            # otherwise).
            ("authorize_emptypayload.json", "auth_token"),
        ],
        # Notes on the chosen fragments:
        #   * ``authorize_badcode.json``'s error is at path ``response/type``
        #     (the error-branch complains type must be 'error'); we match on
        #     ``response`` — the prefix of that absolute_path.
        #   * Message-substring matching is a pragmatic choice for now; the
        #     typed-code suite (``TestErrorCodes``) below is the stable way
        #     to assert WHICH kind of violation fired.
    )
    def test_schema_violations(
        self, filename: str, expected_fragment: str
    ) -> None:
        with pytest.raises(FixtureValidationError, match=expected_fragment):
            validate_file(BAD / filename)

    def test_filename_has_no_op_prefix(self) -> None:
        with pytest.raises(FixtureValidationError, match="no op prefix"):
            validate_file(BAD / "_leading_underscore.json")

    def test_filename_op_mismatch(self) -> None:
        # Fixture is deliberately schema-valid (schema_version=1, op in enum,
        # scenario matches regex, response.oneOf valid) so the ONLY trigger is
        # the filename↔json self-consistency check. Confirms the self-consistency
        # stage runs AFTER schema validation as documented in the validator.
        with pytest.raises(FixtureValidationError, match="op mismatch"):
            validate_file(BAD / "authorize_opmismatch.json")

    def test_filename_scenario_mismatch(self) -> None:
        # Same schema-clean setup as the op-mismatch fixture — the only trigger
        # here is json.scenario != filename-derived scenario.
        with pytest.raises(FixtureValidationError, match="scenario mismatch"):
            validate_file(BAD / "authorize_scenmismatch.json")

    def test_parse_filename_error_includes_full_path(self) -> None:
        """When ``source_path`` is supplied, the raised error carries the full path
        (not just the basename) so CI logs point at the offending file."""
        full_path = BAD / "_leading_underscore.json"
        try:
            validate_file(full_path)
        except FixtureValidationError as err:
            assert str(err.file) == str(full_path), (
                f"expected full path in error.file, got {err.file}"
            )
        else:
            raise AssertionError("expected FixtureValidationError")


class TestValidateDirectory:
    def test_canonical_fixture_set_passes(self) -> None:
        """All canonical fixtures under testdata/mwa-fixtures/ validate.

        Asserts an exact file count so an accidental fixture deletion (or an
        accidental add of a malformed one) surfaces here rather than silently
        passing. Story 1-6 ships 22; update this constant intentionally when
        adding fixtures for future stories.
        """
        files = sorted(CANONICAL.glob("*.json"))
        assert len(files) == 22, f"expected 22 canonical fixtures, found {len(files)}"
        validate_directory(CANONICAL)

    def test_bad_fixture_directory_raises(self) -> None:
        with pytest.raises(FixtureValidationError):
            validate_directory(BAD)

    def test_empty_directory_rejected(self, tmp_path: Path) -> None:
        with pytest.raises(FixtureValidationError, match="no \\*\\.json"):
            validate_directory(tmp_path)

    def test_nonexistent_directory_rejected(self) -> None:
        with pytest.raises(FixtureValidationError, match="not a directory"):
            validate_directory(REPO_ROOT / "does-not-exist")


class TestErrorCodesAlignWithCodegen:
    def test_error_codes_contain_13_entries(self) -> None:
        """Story 1-1 ships 13 error codes — the fixture schema enum must match."""
        assert len(ERROR_CODES) == 13


class TestCLI:
    """Exercises the argparse-driven entrypoint — both positional and ``--fixtures-dir``
    flag forms, ``--all-errors`` accumulation mode, and non-zero exit on failure.

    Note: the CLI accepts both a positional arg and a ``--fixtures-dir`` flag
    for callsite convenience. Callers SHOULD pick one; mixing them is
    undocumented and we do not test cross-form precedence (too-opinionated
    a semantic to lock in without a real use case).
    """

    def test_positional_form_returns_0_on_valid(self, capsys) -> None:
        assert main([str(CANONICAL)]) == 0
        assert "OK" in capsys.readouterr().out

    def test_flag_form_returns_0_on_valid(self, capsys) -> None:
        assert main(["--fixtures-dir", str(CANONICAL)]) == 0
        assert "OK" in capsys.readouterr().out

    def test_returns_1_on_failure(self, capsys) -> None:
        assert main([str(BAD)]) == 1
        captured = capsys.readouterr()
        assert "FAIL" in captured.err

    def test_all_errors_mode_accumulates(self, capsys) -> None:
        """``--all-errors`` reports EVERY violation in one run (vs fail-fast).

        Counts distinct file-paths mentioned in the stderr report by matching
        ``.json:`` occurrences (which only appear in per-violation lines, not
        in the header). Asserts strict parity with the number of bad fixtures
        on disk — drift in either direction (new bad fixture, deleted fixture,
        or fail-fast regression) trips the test.
        """
        import re

        assert main(["--all-errors", str(BAD)]) == 1
        captured = capsys.readouterr()
        assert "violation" in captured.err

        expected_count = len(sorted(BAD.glob("*.json")))
        violation_lines = re.findall(r"^\s+\S+\.json:", captured.err, re.MULTILINE)
        assert len(violation_lines) == expected_count, (
            f"expected {expected_count} violations (one per bad fixture), got "
            f"{len(violation_lines)}. Report:\n{captured.err}"
        )


class TestErrorCodes:
    """Assert the ``FixtureValidationError.code`` discriminator — stable across
    human-readable message refactors.
    """

    @pytest.mark.parametrize(
        ("filename", "expected_code"),
        [
            ("authorize_nover.json", FixtureValidationError.SCHEMA_VIOLATION),
            ("authorize_badversion.json", FixtureValidationError.SCHEMA_VIOLATION),
            ("authorize_badopinside.json", FixtureValidationError.SCHEMA_VIOLATION),
            ("authorize_badcode.json", FixtureValidationError.SCHEMA_VIOLATION),
            ("authorize_unknowncode.json", FixtureValidationError.SCHEMA_VIOLATION),
            ("authorize_badscenario.json", FixtureValidationError.SCHEMA_VIOLATION),
            ("authorize_extraprop.json", FixtureValidationError.SCHEMA_VIOLATION),
            ("_leading_underscore.json", FixtureValidationError.FILENAME_NO_OP_PREFIX),
            ("authorize_opmismatch.json", FixtureValidationError.OP_MISMATCH),
            ("authorize_scenmismatch.json", FixtureValidationError.SCENARIO_MISMATCH),
        ],
    )
    def test_codes_via_validate_file(self, filename, expected_code) -> None:
        with pytest.raises(FixtureValidationError) as excinfo:
            validate_file(BAD / filename)
        assert excinfo.value.code == expected_code

    def test_code_not_a_directory(self) -> None:
        with pytest.raises(FixtureValidationError) as excinfo:
            validate_directory(REPO_ROOT / "does-not-exist")
        assert excinfo.value.code == FixtureValidationError.NOT_A_DIRECTORY

    def test_code_no_fixtures(self, tmp_path) -> None:
        with pytest.raises(FixtureValidationError) as excinfo:
            validate_directory(tmp_path)
        assert excinfo.value.code == FixtureValidationError.NO_FIXTURES

    def test_code_accumulated(self) -> None:
        with pytest.raises(FixtureValidationError) as excinfo:
            validate_directory(BAD, all_errors=True)
        assert excinfo.value.code == FixtureValidationError.ACCUMULATED

    def test_code_filename_no_scenario(self) -> None:
        # ``authorize_`` with empty scenario suffix — parse_filename should
        # raise FILENAME_NO_SCENARIO before touching the filesystem.
        with pytest.raises(FixtureValidationError) as excinfo:
            parse_filename("authorize_.json")
        assert excinfo.value.code == FixtureValidationError.FILENAME_NO_SCENARIO
