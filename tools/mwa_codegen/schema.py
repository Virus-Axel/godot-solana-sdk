"""Schema loader + validator for the MWA error-codes manifest (v1).

Invariants enforced (mirrored from docs/stories/1-1.md "Schema invariants"):

* ``schema_version`` MUST equal ``1``.
* ``codes`` MUST be a non-empty list; for the authoritative ``tools/error-codes.yaml``
  the generator additionally expects 13 entries, but this module accepts any
  ``N >= 1`` so smaller manifests can be used as test fixtures.
* Every code MUST define all six keys: ``name``, ``cpp_enum``, ``kotlin_enum``,
  ``recoverable``, ``retry_hint``, ``default_user_message``.
* ``name`` and ``cpp_enum`` MUST match ``^[A-Z][A-Z0-9_]*$``.
* ``kotlin_enum`` MUST match ``^[A-Z][A-Za-z0-9]*$``.
* ``recoverable`` MUST be a Python ``bool`` (YAML ``true`` / ``false``, not quoted).
* ``retry_hint`` MUST be one of ``{retry_now, install_wallet, connect, none}``.
* ``default_user_message`` MUST be a non-empty string.
* ``name``, ``cpp_enum``, ``kotlin_enum`` values MUST each be unique across the
  manifest.
"""

from __future__ import annotations

import re
from pathlib import Path
from typing import Optional, Union

import yaml

from .models import ErrorCode, Manifest


_NAME_RE = re.compile(r"^[A-Z][A-Z0-9_]*$")
_KOTLIN_RE = re.compile(r"^[A-Z][A-Za-z0-9]*$")
_VALID_RETRY_HINTS = frozenset({"retry_now", "install_wallet", "connect", "none"})
_REQUIRED_KEYS = (
    "name",
    "cpp_enum",
    "kotlin_enum",
    "recoverable",
    "retry_hint",
    "default_user_message",
)


class ManifestValidationError(Exception):
    """Raised when an error-codes manifest fails schema validation.

    ``field`` identifies the offending key (e.g. ``"retry_hint"``, ``"codes"``,
    ``"schema_version"``). ``code_name`` identifies the offending code's ``name``
    if the failure is tied to a specific entry; it is ``None`` for
    top-level failures.
    """

    def __init__(
        self,
        message: str,
        *,
        field: Optional[str] = None,
        code_name: Optional[str] = None,
    ) -> None:
        super().__init__(message)
        self.field = field
        self.code_name = code_name


def load_manifest(path: Union[str, Path]) -> Manifest:
    """Parse ``path`` as YAML, hydrate into a :class:`Manifest`, and validate.

    Raises :class:`ManifestValidationError` on any schema violation.
    """
    raw_text = Path(path).read_text(encoding="utf-8")
    data = yaml.safe_load(raw_text)
    if data is None:
        raise ManifestValidationError(
            "Manifest is empty", field="codes", code_name=None
        )
    manifest = Manifest.from_dict(data)
    validate(manifest)
    return manifest


def validate(manifest: Manifest) -> None:
    """Raise :class:`ManifestValidationError` if ``manifest`` violates schema v1."""

    if manifest.schema_version != 1:
        raise ManifestValidationError(
            f"schema_version must be 1, got {manifest.schema_version!r}",
            field="schema_version",
        )

    if not manifest.codes:
        raise ManifestValidationError(
            "codes list is empty; at least one entry is required",
            field="codes",
        )

    seen_names: set[str] = set()
    seen_cpp: set[str] = set()
    seen_kotlin: set[str] = set()

    for entry in manifest.codes:
        _validate_entry(entry, seen_names, seen_cpp, seen_kotlin)


def _validate_entry(
    entry: ErrorCode,
    seen_names: set[str],
    seen_cpp: set[str],
    seen_kotlin: set[str],
) -> None:
    code_name = entry.name if isinstance(entry.name, str) else None

    # Required keys: detect missing (None from .get()) before per-field validation.
    for key in _REQUIRED_KEYS:
        if getattr(entry, key) is None:
            raise ManifestValidationError(
                f"missing required key: {key}",
                field=key,
                code_name=code_name,
            )

    if not isinstance(entry.name, str) or not _NAME_RE.match(entry.name):
        raise ManifestValidationError(
            f"name must match {_NAME_RE.pattern!r}, got {entry.name!r}",
            field="name",
            code_name=code_name,
        )

    if not isinstance(entry.cpp_enum, str) or not _NAME_RE.match(entry.cpp_enum):
        raise ManifestValidationError(
            f"cpp_enum must match {_NAME_RE.pattern!r}, got {entry.cpp_enum!r}",
            field="cpp_enum",
            code_name=code_name,
        )

    if not isinstance(entry.kotlin_enum, str) or not _KOTLIN_RE.match(entry.kotlin_enum):
        raise ManifestValidationError(
            f"kotlin_enum must match {_KOTLIN_RE.pattern!r}, got {entry.kotlin_enum!r}",
            field="kotlin_enum",
            code_name=code_name,
        )

    # YAML true/false parse as bool; quoted "true" parses as str. Reject str.
    if not isinstance(entry.recoverable, bool):
        raise ManifestValidationError(
            f"recoverable must be a bool (YAML true/false, unquoted), got "
            f"{type(entry.recoverable).__name__}: {entry.recoverable!r}",
            field="recoverable",
            code_name=code_name,
        )

    if entry.retry_hint not in _VALID_RETRY_HINTS:
        raise ManifestValidationError(
            f"retry_hint must be one of {sorted(_VALID_RETRY_HINTS)}, got "
            f"{entry.retry_hint!r}",
            field="retry_hint",
            code_name=code_name,
        )

    if not isinstance(entry.default_user_message, str) or not entry.default_user_message:
        raise ManifestValidationError(
            "default_user_message must be a non-empty string",
            field="default_user_message",
            code_name=code_name,
        )

    if entry.name in seen_names:
        raise ManifestValidationError(
            f"duplicate name: {entry.name!r}",
            field="name",
            code_name=code_name,
        )
    if entry.cpp_enum in seen_cpp:
        raise ManifestValidationError(
            f"duplicate cpp_enum: {entry.cpp_enum!r}",
            field="cpp_enum",
            code_name=code_name,
        )
    if entry.kotlin_enum in seen_kotlin:
        raise ManifestValidationError(
            f"duplicate kotlin_enum: {entry.kotlin_enum!r}",
            field="kotlin_enum",
            code_name=code_name,
        )

    seen_names.add(entry.name)
    seen_cpp.add(entry.cpp_enum)
    seen_kotlin.add(entry.kotlin_enum)
