# Concerns

Deferred items from code reviews and implementation. Tracked for future resolution.

## CR-1: `reauthorize` retry_hint in validator but absent from story 1-1 spec
- **Story:** 1-1 | **Severity:** LOW
- **Detail:** `_VALID_RETRY_HINTS` accepts `reauthorize` per amendment A-1, but the story 1-1 spec only lists 4 values. The 5th value is documented as "reserved for future use" in `docs/amendments.md` A-2 (retracted). No error code currently uses it. Property tests may generate manifests with `reauthorize` that pass validation.
- **Risk:** None today; a future maintainer may be confused by the mismatch between story spec and code.
- **Resolution:** accepted — A-1 documents the full set; story spec is frozen.

## CR-2: Missing `--out-cpp`, `--out-kotlin`, `--out-gdscript` CLI flags
- **Story:** 1-1 | **Severity:** LOW
- **Detail:** Task 2 spec called for custom output path flags. Not implemented; CLI hardcodes canonical paths via `TARGETS` dict. No AC or downstream story depends on these flags.
- **Risk:** Minor DX limitation for non-standard repo layouts.
- **Resolution:** add if a downstream story needs them; not worth the complexity today.

## CR-3: Duplicate `_REPO_ROOT` / `REPO_ROOT` in `gen_error_codes.py`
- **Story:** 1-1 | **Severity:** LOW
- **Detail:** Lines 20 and 27 both compute `Path(__file__).resolve().parent.parent`. One is used for `sys.path`, the other for path construction.
- **Resolution:** consolidate on next touch of the file.

## CR-4: CI drift check uses `git status --porcelain` instead of `--check` CLI mode
- **Story:** 1-1 | **Severity:** LOW
- **Detail:** Story spec said drift check should use `python tools/gen_error_codes.py --check`. CI instead regenerates and uses `git status --porcelain` (which is strictly more capable — catches untracked files too). The `--check` mode is tested via pytest (Group 4 tests) but not exercised in CI.
- **Risk:** `--check` mode code path is tested but not used in the real CI environment.
- **Resolution:** acceptable — `git status --porcelain` is the superior approach per user review feedback. `--check` mode remains useful for local dev workflow.
