"""Pytest configuration for scripts/ — mirrors tools/conftest.py shape.

Tests can be invoked as:

    pytest scripts/                  # from repo root
    cd scripts && pytest             # from scripts/
    pytest                           # from repo root (pytest.ini testpaths=tools scripts)
"""

import sys
from pathlib import Path

# Add the repo root to sys.path so `from scripts.X import ...` works regardless
# of which directory pytest was invoked from.
_REPO_ROOT = Path(__file__).resolve().parent.parent
if str(_REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(_REPO_ROOT))
