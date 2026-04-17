"""Tests for the MWA error-codes codegen toolchain.

Three test groups per docs/stories/1-1.md §10.3.2 / AC-D-23:

1. Golden-file tests — 3 fixtures (1, 2, 13 codes) × 3 artifacts (hpp, kt, gd) =
   9 byte-for-byte assertions, plus an idempotence check (regenerate -> byte-identical).

2. Property tests (Hypothesis) — random valid manifests (1-20 codes) round-trip
   through the generator; emitted artifacts are regex-parsed back and
   ``recoverable`` / ``retry_hint`` fields are cross-verified for consistency
   across all 3 artifacts.

3. Failure tests — malformed manifests each raise ``ManifestValidationError``
   with ``.field`` / ``.code_name`` populated for the offending entry.

These tests are PR-blocking via ``.github/workflows/generator_tests.yml`` and run
INDEPENDENTLY of the drift-check workflow (DD-33).
"""

from __future__ import annotations

import re
import string
from pathlib import Path
from typing import List, Tuple

import pytest
import yaml
from hypothesis import HealthCheck, given, settings, strategies as st

from tools.mwa_codegen.emit import (
    render_cpp,
    render_gdscript,
    render_kotlin,
)
from tools.mwa_codegen.models import ErrorCode, Manifest
from tools.mwa_codegen.schema import (
    ManifestValidationError,
    load_manifest,
    validate,
)


REPO_ROOT = Path(__file__).resolve().parent.parent
TESTDATA = REPO_ROOT / "tools" / "testdata"

RETRY_HINTS = ["retry_now", "reauthorize", "install_wallet", "connect", "none"]


# ---------------------------------------------------------------------------
# Group 1: Golden-file tests
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("fixture", ["golden_1_code", "golden_2_codes", "golden_13_codes"])
def test_golden_hpp(fixture: str) -> None:
    manifest = load_manifest(TESTDATA / fixture / "manifest.yaml")
    expected = (TESTDATA / fixture / "expected.hpp").read_text(encoding="utf-8")
    actual = render_cpp(manifest)
    assert actual == expected, _diff_summary(fixture, "hpp", expected, actual)


@pytest.mark.parametrize("fixture", ["golden_1_code", "golden_2_codes", "golden_13_codes"])
def test_golden_kotlin(fixture: str) -> None:
    manifest = load_manifest(TESTDATA / fixture / "manifest.yaml")
    expected = (TESTDATA / fixture / "expected.kt").read_text(encoding="utf-8")
    actual = render_kotlin(manifest)
    assert actual == expected, _diff_summary(fixture, "kt", expected, actual)


@pytest.mark.parametrize("fixture", ["golden_1_code", "golden_2_codes", "golden_13_codes"])
def test_golden_gdscript(fixture: str) -> None:
    manifest = load_manifest(TESTDATA / fixture / "manifest.yaml")
    expected = (TESTDATA / fixture / "expected.gd").read_text(encoding="utf-8")
    actual = render_gdscript(manifest)
    assert actual == expected, _diff_summary(fixture, "gd", expected, actual)


@pytest.mark.parametrize("fixture", ["golden_1_code", "golden_2_codes", "golden_13_codes"])
def test_idempotent(fixture: str) -> None:
    manifest = load_manifest(TESTDATA / fixture / "manifest.yaml")
    first_hpp = render_cpp(manifest)
    first_kt = render_kotlin(manifest)
    first_gd = render_gdscript(manifest)
    # Re-render; outputs MUST be byte-identical.
    assert render_cpp(manifest) == first_hpp
    assert render_kotlin(manifest) == first_kt
    assert render_gdscript(manifest) == first_gd


# ---------------------------------------------------------------------------
# Group 2: Hypothesis property tests
# ---------------------------------------------------------------------------


_UPPER_FIRST = st.sampled_from(list(string.ascii_uppercase))
_UPPER_REST = st.text(
    alphabet=list(string.ascii_uppercase) + list(string.digits) + ["_"],
    min_size=0,
    max_size=12,
)
_PASCAL_REST = st.text(
    alphabet=list(string.ascii_letters) + list(string.digits),
    min_size=0,
    max_size=12,
)
_MESSAGE_ALPHABET = string.ascii_letters + string.digits + " .,!?"


@st.composite
def _error_code_strategy(draw) -> ErrorCode:
    name = draw(_UPPER_FIRST) + draw(_UPPER_REST)
    kotlin_enum = draw(_UPPER_FIRST) + draw(_PASCAL_REST)
    return ErrorCode(
        name=name,
        cpp_enum=name,
        kotlin_enum=kotlin_enum,
        recoverable=draw(st.booleans()),
        retry_hint=draw(st.sampled_from(RETRY_HINTS)),
        default_user_message=draw(
            st.text(alphabet=_MESSAGE_ALPHABET, min_size=1, max_size=64)
        ),
    )


@st.composite
def _manifest_strategy(draw) -> Manifest:
    codes = draw(
        st.lists(
            _error_code_strategy(),
            min_size=1,
            max_size=20,
            unique_by=(lambda c: c.name, lambda c: c.cpp_enum, lambda c: c.kotlin_enum),
        )
    )
    return Manifest(schema_version=1, codes=tuple(codes))


@settings(
    max_examples=25,
    suppress_health_check=[HealthCheck.too_slow, HealthCheck.data_too_large],
    deadline=None,
)
@given(manifest=_manifest_strategy())
def test_property_cross_language_consistency(manifest: Manifest) -> None:
    """Every code's recoverable/retry_hint appears identically in all 3 artifacts."""
    # Sanity check: manifest itself is valid.
    validate(manifest)

    hpp = render_cpp(manifest)
    kt = render_kotlin(manifest)
    gd = render_gdscript(manifest)

    # Every code name must appear in all 3 artifacts.
    for code in manifest.codes:
        assert f"MwaErrorCode::{code.cpp_enum}" in hpp, (code.cpp_enum, hpp)
        assert f"object {code.kotlin_enum}" in kt, (code.kotlin_enum, kt)
        assert f'MWA_ERR_{code.name} := "{code.name}"' in gd, (code.name, gd)

    # Cross-verify retry_hint and recoverable round-trip into Kotlin and GDScript.
    kt_retries = _parse_kotlin_retries(kt)
    kt_recoverables = _parse_kotlin_recoverables(kt)
    for code in manifest.codes:
        assert kt_retries.get(code.kotlin_enum) == code.retry_hint, (
            code.kotlin_enum,
            kt_retries,
        )
        assert kt_recoverables.get(code.kotlin_enum) == code.recoverable, (
            code.kotlin_enum,
            kt_recoverables,
        )

    # Manifest order preserved in ALL_CODES block of the GDScript output.
    all_codes_block = gd.split("ALL_CODES: Array[String] = [", 1)[1]
    order = re.findall(r'"([A-Z][A-Z0-9_]*)"', all_codes_block.split("]", 1)[0])
    assert order == [c.name for c in manifest.codes], (order, manifest.codes)


# ---------------------------------------------------------------------------
# Group 3: Failure tests — ManifestValidationError.field / .code_name
# ---------------------------------------------------------------------------


def _write_and_load(tmp_path: Path, doc: str) -> None:
    """Helper: write ``doc`` to a temp YAML file and load it via load_manifest."""
    manifest_path = tmp_path / "manifest.yaml"
    manifest_path.write_text(doc, encoding="utf-8")
    load_manifest(manifest_path)


def test_failure_schema_version_wrong(tmp_path: Path) -> None:
    doc = """
schema_version: 2
codes:
  - name: A
    cpp_enum: A
    kotlin_enum: A
    recoverable: true
    retry_hint: none
    default_user_message: "m"
"""
    with pytest.raises(ManifestValidationError) as excinfo:
        _write_and_load(tmp_path, doc)
    assert excinfo.value.field == "schema_version"
    assert excinfo.value.code_name is None


def test_failure_empty_codes(tmp_path: Path) -> None:
    doc = """
schema_version: 1
codes: []
"""
    with pytest.raises(ManifestValidationError) as excinfo:
        _write_and_load(tmp_path, doc)
    assert excinfo.value.field == "codes"


def test_failure_missing_key(tmp_path: Path) -> None:
    doc = """
schema_version: 1
codes:
  - name: ONLY_NAME
    kotlin_enum: OnlyName
    recoverable: true
    retry_hint: none
    default_user_message: "m"
"""
    with pytest.raises(ManifestValidationError) as excinfo:
        _write_and_load(tmp_path, doc)
    assert excinfo.value.field == "cpp_enum"
    assert excinfo.value.code_name == "ONLY_NAME"


def test_failure_invalid_retry_hint(tmp_path: Path) -> None:
    doc = """
schema_version: 1
codes:
  - name: BAD_HINT
    cpp_enum: BAD_HINT
    kotlin_enum: BadHint
    recoverable: true
    retry_hint: yeet
    default_user_message: "m"
"""
    with pytest.raises(ManifestValidationError) as excinfo:
        _write_and_load(tmp_path, doc)
    assert excinfo.value.field == "retry_hint"
    assert excinfo.value.code_name == "BAD_HINT"


def test_failure_recoverable_not_bool(tmp_path: Path) -> None:
    doc = """
schema_version: 1
codes:
  - name: STRINGY
    cpp_enum: STRINGY
    kotlin_enum: Stringy
    recoverable: "true"
    retry_hint: none
    default_user_message: "m"
"""
    with pytest.raises(ManifestValidationError) as excinfo:
        _write_and_load(tmp_path, doc)
    assert excinfo.value.field == "recoverable"
    assert excinfo.value.code_name == "STRINGY"


def test_failure_duplicate_name(tmp_path: Path) -> None:
    doc = """
schema_version: 1
codes:
  - name: DUP
    cpp_enum: DUP
    kotlin_enum: DupA
    recoverable: true
    retry_hint: none
    default_user_message: "a"
  - name: DUP
    cpp_enum: DUPE
    kotlin_enum: DupB
    recoverable: true
    retry_hint: none
    default_user_message: "b"
"""
    with pytest.raises(ManifestValidationError) as excinfo:
        _write_and_load(tmp_path, doc)
    assert excinfo.value.field == "name"
    assert excinfo.value.code_name == "DUP"


def test_failure_duplicate_cpp_enum(tmp_path: Path) -> None:
    doc = """
schema_version: 1
codes:
  - name: A
    cpp_enum: SHARED
    kotlin_enum: A
    recoverable: true
    retry_hint: none
    default_user_message: "a"
  - name: B
    cpp_enum: SHARED
    kotlin_enum: B
    recoverable: true
    retry_hint: none
    default_user_message: "b"
"""
    with pytest.raises(ManifestValidationError) as excinfo:
        _write_and_load(tmp_path, doc)
    assert excinfo.value.field == "cpp_enum"
    assert excinfo.value.code_name == "B"


def test_failure_lowercase_name(tmp_path: Path) -> None:
    doc = """
schema_version: 1
codes:
  - name: lowercase
    cpp_enum: lowercase
    kotlin_enum: Lowercase
    recoverable: true
    retry_hint: none
    default_user_message: "m"
"""
    with pytest.raises(ManifestValidationError) as excinfo:
        _write_and_load(tmp_path, doc)
    assert excinfo.value.field == "name"
    assert excinfo.value.code_name is None  # name rejected before adding to seen


def test_failure_empty_message(tmp_path: Path) -> None:
    doc = """
schema_version: 1
codes:
  - name: EMPTY_MSG
    cpp_enum: EMPTY_MSG
    kotlin_enum: EmptyMsg
    recoverable: true
    retry_hint: none
    default_user_message: ""
"""
    with pytest.raises(ManifestValidationError) as excinfo:
        _write_and_load(tmp_path, doc)
    assert excinfo.value.field == "default_user_message"
    assert excinfo.value.code_name == "EMPTY_MSG"


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _diff_summary(fixture: str, ext: str, expected: str, actual: str) -> str:
    """Short diff summary to aid debugging when goldens drift."""
    exp_lines = expected.splitlines()
    act_lines = actual.splitlines()
    for i, (e, a) in enumerate(zip(exp_lines, act_lines)):
        if e != a:
            return f"{fixture}/expected.{ext} diverges at line {i + 1}:\n  expected: {e!r}\n  actual:   {a!r}"
    if len(exp_lines) != len(act_lines):
        return (
            f"{fixture}/expected.{ext} length differs: "
            f"expected={len(exp_lines)} actual={len(act_lines)}"
        )
    return f"{fixture}/expected.{ext} differs (whitespace-only?)"


_KT_RETRY_RE = re.compile(r'object (\w+) : MwaError\([^)]*?retryHint = "([^"]*)"', re.DOTALL)
_KT_RECOVERABLE_RE = re.compile(
    r"object (\w+) : MwaError\([^)]*?recoverable = (true|false)", re.DOTALL
)


def _parse_kotlin_retries(kt: str) -> dict:
    return {m.group(1): m.group(2) for m in _KT_RETRY_RE.finditer(kt)}


def _parse_kotlin_recoverables(kt: str) -> dict:
    return {
        m.group(1): (m.group(2) == "true") for m in _KT_RECOVERABLE_RE.finditer(kt)
    }
