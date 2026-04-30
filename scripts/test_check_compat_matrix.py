"""Tests for scripts/check_compat_matrix.py (Story 5-6 Task 9 - AC-8).

Per DD-5-6-8 LOCKED: parses docs/compatibility-matrix.md "Pinned values"
table; for each enforced row, reads the source-of-truth file and asserts
the live value matches. Enforced rows: compileSdk, targetSdk, minSdk,
mobile-wallet-adapter-clientlib-ktx, Godot minor, Android Gradle Plugin.

Test groups (4 cases per DD-5-6-8 budget; +1 sanity case):

1. All rows match - exit 0.
2. compileSdk drift - exit 1 + ::error:: annotation naming the drifted row.
3. clientlib-ktx version drift - exit 1.
4. Malformed matrix (missing "Pinned values" header) - exit 2 (graceful).
5. Missing source-of-truth file - exit 1.
6. Live repo state - calls the script with --repo-root pointing at the actual
   repo (sanity: production-source-of-truth files match the matrix). The test
   becomes a no-op early-return when the matrix isn't present (e.g., running
   outside a checkout).

CI wires this via `.github/workflows/compat_matrix_drift.yml` (PR-blocking
per AC-8).
"""

from __future__ import annotations

from pathlib import Path

from scripts.check_compat_matrix import check_matrix


CLEAN_MATRIX = """\
# Compatibility Matrix

## Pinned values (last verified 2026-01-01)

| Component | Pinned value | Source of truth | Notes |
|---|---|---|---|
| Godot minor | `4.3.x` | `godot-solana-sdk.gdextension:4` (`compatibility_minimum = "4.3"`) | locked. |
| `mobile-wallet-adapter-clientlib-ktx` | `2.0.3` | `android/plugin/build.gradle.kts:225` | mirrors rn-parity. |
| `compileSdk` | `34` | `android/plugin/build.gradle.kts` | CI asserts 34. |
| `targetSdk` | `34` | `android/plugin/build.gradle.kts` | aligned. |
| `minSdk` | `23` | `android/plugin/build.gradle.kts` | NFR-2. |
| Android Gradle Plugin | `7.4.1` | `android/build.gradle.kts:28` | Gradle 7.5. |
"""

CLEAN_BUILD_GRADLE = """\
plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
}

android {
    compileSdk = 34
    defaultConfig {
        minSdk = 23
        targetSdk = 34
    }
}

dependencies {
    implementation("com.solanamobile:mobile-wallet-adapter-clientlib-ktx:2.0.3")
}
"""

CLEAN_ROOT_BUILD_GRADLE = """\
plugins {
    id("com.android.library") version "7.4.1" apply false
    id("org.jetbrains.kotlin.android") version "1.8.0" apply false
}
"""

CLEAN_GDEXTENSION = """\
[configuration]
entry_symbol = "godot_solana_sdk_init"
compatibility_minimum = "4.3"

[libraries]
linux.x86_64 = "res://addons/SolanaSDK/bin/libgodot-solana-sdk.linux.template_release.x86_64.so"
"""


def _write_repo(tmp_path: Path,
                build_gradle: str = CLEAN_BUILD_GRADLE,
                root_build_gradle: str = CLEAN_ROOT_BUILD_GRADLE,
                gdextension: str = CLEAN_GDEXTENSION,
                matrix: str = CLEAN_MATRIX) -> Path:
    """Stage a synthetic repo layout under tmp_path and return its root."""
    (tmp_path / "docs").mkdir()
    (tmp_path / "docs" / "compatibility-matrix.md").write_text(matrix)
    (tmp_path / "android" / "plugin").mkdir(parents=True)
    (tmp_path / "android" / "plugin" / "build.gradle.kts").write_text(build_gradle)
    (tmp_path / "android" / "build.gradle.kts").write_text(root_build_gradle)
    (tmp_path / "godot-solana-sdk.gdextension").write_text(gdextension)
    return tmp_path


def test_all_rows_match_exit_zero(tmp_path):
    repo = _write_repo(tmp_path)
    rc, drifts, errors = check_matrix(repo)
    assert errors == []
    assert drifts == [], f"unexpected drifts: {drifts}"
    assert rc == 0


def test_compile_sdk_drift_exit_one(tmp_path):
    bad_build = CLEAN_BUILD_GRADLE.replace("compileSdk = 34", "compileSdk = 35")
    repo = _write_repo(tmp_path, build_gradle=bad_build)
    rc, drifts, errors = check_matrix(repo)
    assert errors == []
    assert rc == 1
    drift_components = [d.component for d in drifts]
    assert "compileSdk" in drift_components
    drift = next(d for d in drifts if d.component == "compileSdk")
    assert drift.expected == "34"
    assert drift.actual == "35"


def test_clientlib_ktx_drift_exit_one(tmp_path):
    bad_build = CLEAN_BUILD_GRADLE.replace(
        "mobile-wallet-adapter-clientlib-ktx:2.0.3",
        "mobile-wallet-adapter-clientlib-ktx:2.0.4",
    )
    repo = _write_repo(tmp_path, build_gradle=bad_build)
    rc, drifts, errors = check_matrix(repo)
    assert errors == []
    assert rc == 1
    drift_components = [d.component for d in drifts]
    assert "mobile-wallet-adapter-clientlib-ktx" in drift_components


def test_malformed_matrix_exit_two(tmp_path):
    bad_matrix = "# Compatibility Matrix\n\nNo pinned values header here.\n"
    repo = _write_repo(tmp_path, matrix=bad_matrix)
    rc, drifts, errors = check_matrix(repo)
    assert rc == 2
    assert errors, "expected at least one error message for malformed matrix"
    assert any("Pinned values" in e for e in errors)


def test_missing_source_of_truth_exit_one(tmp_path):
    """Source-of-truth file referenced by matrix but not present on disk."""
    repo = _write_repo(tmp_path)
    # Remove .gdextension to simulate a missing source-of-truth.
    (repo / "godot-solana-sdk.gdextension").unlink()
    rc, drifts, errors = check_matrix(repo)
    assert rc == 1
    drift_components = [d.component for d in drifts]
    assert "Godot minor" in drift_components
    drift = next(d for d in drifts if d.component == "Godot minor")
    assert "missing" in drift.actual.lower() or "not found" in drift.actual.lower()


def test_live_repo_matrix_clean_when_present():
    """Sanity: if invoked from the real repo root, the production matrix is drift-clean.

    Becomes a no-op early-return when the matrix isn't present (out-of-checkout
    test runs).
    """
    real_repo = Path(__file__).resolve().parent.parent
    matrix = real_repo / "docs" / "compatibility-matrix.md"
    if not matrix.exists():
        # Out-of-checkout: nothing to verify.
        return
    rc, drifts, errors = check_matrix(real_repo)
    assert errors == [], f"unexpected errors against live repo: {errors}"
    assert drifts == [], f"unexpected drifts against live repo: {drifts}"
    assert rc == 0
