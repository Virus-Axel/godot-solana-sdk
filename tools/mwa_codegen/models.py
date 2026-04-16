"""Immutable data types for the MWA error-codes manifest."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Mapping, Tuple


@dataclass(frozen=True)
class ErrorCode:
    """A single entry in the MWA error-codes manifest.

    All fields are required. Validation (type, character-set, enum membership)
    is performed by :func:`tools.mwa_codegen.schema.validate`.
    """

    name: str
    cpp_enum: str
    kotlin_enum: str
    recoverable: bool
    retry_hint: str
    default_user_message: str

    @classmethod
    def from_dict(cls, data: Mapping[str, Any]) -> "ErrorCode":
        return cls(
            name=data.get("name"),  # type: ignore[arg-type]
            cpp_enum=data.get("cpp_enum"),  # type: ignore[arg-type]
            kotlin_enum=data.get("kotlin_enum"),  # type: ignore[arg-type]
            recoverable=data.get("recoverable"),  # type: ignore[arg-type]
            retry_hint=data.get("retry_hint"),  # type: ignore[arg-type]
            default_user_message=data.get("default_user_message"),  # type: ignore[arg-type]
        )


@dataclass(frozen=True)
class Manifest:
    """The full `error-codes.yaml` document after parsing, pre-validation."""

    schema_version: int
    codes: Tuple[ErrorCode, ...]

    @classmethod
    def from_dict(cls, data: Any) -> "Manifest":
        if not isinstance(data, Mapping):
            return cls(schema_version=None, codes=())  # type: ignore[arg-type]
        raw_codes = data.get("codes") or ()
        if not isinstance(raw_codes, (list, tuple)):
            raw_codes = ()
        codes = tuple(
            ErrorCode.from_dict(c) if isinstance(c, Mapping) else ErrorCode.from_dict({})
            for c in raw_codes
        )
        return cls(
            schema_version=data.get("schema_version"),
            codes=codes,
        )
