"""Deterministic 3-language emitter for the MWA error-codes manifest.

Each ``render_*`` function takes a validated :class:`~tools.mwa_codegen.models.Manifest`
and returns a UTF-8 string (LF line endings, trailing newline) that is byte-identical
across repeat invocations. No timestamps, no absolute paths, no generator-version
strings — determinism is an AC-1 invariant.
"""

from __future__ import annotations

from pathlib import Path

from jinja2 import Environment, FileSystemLoader

from .models import Manifest

_TEMPLATE_DIR = Path(__file__).resolve().parent / "templates"

_env = Environment(
    loader=FileSystemLoader(str(_TEMPLATE_DIR)),
    keep_trailing_newline=True,
    lstrip_blocks=True,
    trim_blocks=True,
    autoescape=False,
)


def _normalize_lf(text: str) -> str:
    return text.replace("\r\n", "\n")


def render_cpp(manifest: Manifest) -> str:
    template = _env.get_template("mwa_error_codes.hpp.j2")
    return _normalize_lf(template.render(codes=manifest.codes))


def render_kotlin(manifest: Manifest) -> str:
    template = _env.get_template("MwaError.kt.j2")
    return _normalize_lf(template.render(codes=manifest.codes))


def render_gdscript(manifest: Manifest) -> str:
    template = _env.get_template("mwa_error_codes.gd.j2")
    return _normalize_lf(template.render(codes=manifest.codes))
