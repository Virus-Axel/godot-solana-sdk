"""Tests for scripts/check_since_tags.py (Story 5-6 Task 2 — AC-7).

Per DD-5-6-4 LOCKED: regex-based scanner over GDScript and Kotlin public-symbol
declarations + the immediately-preceding doc-comment block. Verifies each
NEW public symbol added in a PR carries an `@since v<X.Y.Z>` tag.

Test groups (8 cases per DD-5-6-4 budget; symmetric GDScript ↔ Kotlin coverage):

1. GDScript signal WITH `## @since` doc-comment      → no finding
2. GDScript signal WITHOUT `@since`                  → finding emitted
3. GDScript public func WITH `## @since`             → no finding
4. GDScript public func WITHOUT `@since`             → finding emitted
5. GDScript private `_func` (always exempt)         → no finding (regardless of since)
6. Kotlin `@UsedByGodot fun` WITH `/** @since */`    → no finding
7. Kotlin `@UsedByGodot fun` WITHOUT `@since`        → finding emitted
8. Kotlin regular `fun` (no `@UsedByGodot`)         → no finding (not Godot-public)

CI wires this via `.github/workflows/since_tags_check.yml` (PR-blocking per AC-7).
"""

from __future__ import annotations

import pytest

from scripts.check_since_tags import find_public_symbols_without_since


# ---------------------------------------------------------------------------
# GDScript fixtures
# ---------------------------------------------------------------------------

GD_SIGNAL_WITH_SINCE = """\
extends Node

## Emitted when authorize completes.
## @since v0.1.0
signal connect_completed(payload: Dictionary)
"""

GD_SIGNAL_WITHOUT_SINCE = """\
extends Node

## Emitted when authorize completes.
signal connect_completed(payload: Dictionary)
"""

GD_FUNC_WITH_SINCE = """\
extends Node

## Initiates the MWA connect handshake.
## @since v0.1.0
func connect(cluster: String) -> void:
    pass
"""

GD_FUNC_WITHOUT_SINCE = """\
extends Node

## Initiates the MWA connect handshake.
func connect(cluster: String) -> void:
    pass
"""

GD_PRIVATE_FUNC_NO_SINCE = """\
extends Node

func _process_response(payload: Dictionary) -> void:
    pass
"""


# ---------------------------------------------------------------------------
# Kotlin fixtures
# ---------------------------------------------------------------------------

KT_USEDBY_GODOT_WITH_SINCE = """\
class MwaAndroidPlugin {
    /**
     * Initiates the connect handshake.
     * @since v0.1.0
     */
    @UsedByGodot
    fun mwaConnect(identityName: String, identityUri: String): Boolean {
        return false
    }
}
"""

KT_USEDBY_GODOT_WITHOUT_SINCE = """\
class MwaAndroidPlugin {
    /** Initiates the connect handshake. */
    @UsedByGodot
    fun mwaConnect(identityName: String, identityUri: String): Boolean {
        return false
    }
}
"""

KT_PLAIN_FUN_NO_SINCE = """\
class MwaAndroidPlugin {
    private fun internalHelper(): String {
        return "ok"
    }

    fun publicHelperNotGodotVisible(): Int {
        return 42
    }
}
"""


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

def test_gdscript_signal_with_since_passes():
    findings = find_public_symbols_without_since(GD_SIGNAL_WITH_SINCE, "gdscript")
    assert findings == []


def test_gdscript_signal_without_since_fails():
    findings = find_public_symbols_without_since(GD_SIGNAL_WITHOUT_SINCE, "gdscript")
    assert len(findings) == 1
    line_no, symbol = findings[0]
    assert symbol == "connect_completed"
    assert line_no == 4  # 1-indexed line of `signal connect_completed(...)`


def test_gdscript_public_func_with_since_passes():
    findings = find_public_symbols_without_since(GD_FUNC_WITH_SINCE, "gdscript")
    assert findings == []


def test_gdscript_public_func_without_since_fails():
    findings = find_public_symbols_without_since(GD_FUNC_WITHOUT_SINCE, "gdscript")
    assert len(findings) == 1
    line_no, symbol = findings[0]
    assert symbol == "connect"
    assert line_no == 4  # 1-indexed line of `func connect(...) -> void:`


def test_gdscript_private_func_exempt():
    """Private functions (leading underscore) never require @since."""
    findings = find_public_symbols_without_since(GD_PRIVATE_FUNC_NO_SINCE, "gdscript")
    assert findings == []


def test_kotlin_usedby_godot_with_since_passes():
    findings = find_public_symbols_without_since(KT_USEDBY_GODOT_WITH_SINCE, "kotlin")
    assert findings == []


def test_kotlin_usedby_godot_without_since_fails():
    findings = find_public_symbols_without_since(KT_USEDBY_GODOT_WITHOUT_SINCE, "kotlin")
    assert len(findings) == 1
    line_no, symbol = findings[0]
    assert symbol == "mwaConnect"
    # `fun mwaConnect(...)` is on line 4 of the fixture (1-indexed).
    assert line_no == 4


def test_kotlin_plain_fun_not_godot_visible_exempt():
    """Functions without @UsedByGodot are not part of the Godot-public surface."""
    findings = find_public_symbols_without_since(KT_PLAIN_FUN_NO_SINCE, "kotlin")
    assert findings == []
