"""Pytest configuration: ensures `tools.mwa_codegen.*` imports resolve from any CWD.

Tests can be invoked as:

    pytest tools/                    # from repo root
    cd tools && pytest               # from tools/
    pytest                           # from repo root (pytest.ini testpaths=tools)
"""

import sys
from pathlib import Path

# Add the repo root to sys.path so `from tools.mwa_codegen.* import ...` works
# regardless of which directory pytest was invoked from.
_REPO_ROOT = Path(__file__).resolve().parent.parent
if str(_REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(_REPO_ROOT))
