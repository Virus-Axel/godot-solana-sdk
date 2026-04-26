# Story 2-2 — 10-Cycle Silent Reauthorize Release-Gate Procedure

**AC-5 / NFR-5 release-gate signal.**
Run once per release candidate on a real physical Android device with Phantom installed.
NOT a CI test (DD-2-2-6): this procedure is executed by a human pre-RC and the result
is recorded in `docs/release-gates/runs/<date>-<rc>.md`.

---

## 1. Purpose

Validate that the MWA silent-reauthorize path (`MWA.reauthorize({})`) meets AC-5 NFR-5:

- **10/10 cycles** succeed — each produces a `reauthorize_completed` signal.
- **0 wallet-app Activity transitions** — Phantom's UI is never surfaced (silent reauth only).
- **Same `public_key`** across all 10 cycles.
- **Same `auth_token_fingerprint`** across all 10 cycles (or rotation events are recorded; see §4).

---

## 2. Prerequisites

Before running this procedure, confirm ALL of the following:

1. **Real physical Android device** — USB-debug enabled, `adb devices` shows it.
   Phantom-on-emulator is wallet-version-fragile and is NOT supported by this gate
   (DD-2-2-6). An emulator run does NOT satisfy AC-5.
2. **Phantom wallet installed and configured** — a funded devnet account
   (`solana airdrop 1 <your-pubkey> --url devnet`) so the wallet can identify itself.
3. **Demo app deployed** — the WalletAdapterAndroid demo (`example/WalletAdapterAndroid/`)
   built and installed WITH the Story 2-2 T6.1 wiring present.  This wiring includes:
   - the `[CR-32-RELEASE-GATE]` logcat marker in `_on_reauthorize_completed`
   - the `ReauthorizeBtn` and `DiagnosticLabel` in the scene
   - the `_diagnostic_state` persistence (`user://mwa_release_gate_state.json`)
   The procedure depends on the logcat marker; running against a build without T6.1
   will produce no marker lines and the bootstrap step will fail.
4. **`adb` on PATH** — verify with `adb version`.
5. **`adb logcat` tag filter** — the marker is emitted under the `godot:I` tag.
   Confirm `adb logcat -s godot:I` produces output after the app starts.

---

## 3. Procedure

### 3.1 Environment variables

```bash
export PKG=com.godotengine.walletadapterandroid
export WALLET_PKG=app.phantom
```

Adjust `PKG` to match the `applicationId` in your build's `export_presets.cfg`
(the WalletAdapterAndroid demo project default is shown above).

### 3.2 Bootstrap — initial connect

The demo scene exposes two buttons added by Story 2-2 T6.1:

- **"MWA Connect (bootstrap)"** — calls `MWA.connect(...)` directly. This is
  the path that fires `MWA.connect_completed`, which sets `has_seen_connect=true`
  in the persistent state so future launches auto-trigger reauthorize.
- **"Reauthorize"** — calls `MWA.reauthorize({})` on demand for ad-hoc cycles.

The legacy `$WalletAdapter.connect_wallet()` path (scaffold-era node from
Story 1-2) is wired through the original Solana SDK, NOT the MWA autoload, so
it does NOT fire `MWA.connect_completed` — tapping the legacy connect path
will NOT bootstrap the release-gate state. **Always use the "MWA Connect
(bootstrap)" button for §3.2.**

```bash
adb shell am start -n $PKG/.MainActivity
```

On-device:
1. Tap **"MWA Connect (bootstrap)"** in the demo UI.
2. Complete the Phantom authorization chooser when it appears.
3. Wait for the `DiagnosticLabel` to update (should show a Base58 public key
   and an 8-hex fingerprint).
4. The persistent state file `user://mwa_release_gate_state.json` now records
   `{"has_seen_connect": true}`. Subsequent launches auto-trigger reauthorize
   from `_ready()` and SKIP the scaffold demo flow (the demo flow is gated
   on `has_seen_connect == false` to keep AC-5's "0 wallet activity transitions"
   assertion clean).

### 3.3 Capture initial baseline

Force-stop, relaunch, and wait for the auto-triggered reauthorize to complete.
The demo's `_ready()` calls `MWA.reauthorize({})` when `has_seen_connect=true`.

```bash
adb logcat -c
adb shell am force-stop $PKG
sleep 1
adb shell am start -n $PKG/.MainActivity
sleep 3

MARKER_LINE=$(adb logcat -d -s godot:I | grep "\[CR-32-RELEASE-GATE\]" | tail -1)
if [ -z "$MARKER_LINE" ]; then
    echo "ERROR: No [CR-32-RELEASE-GATE] marker found."
    echo "Check: (a) demo app is a debug build, (b) T6.1 wiring is present."
    exit 1
fi

INITIAL_PK=$(echo "$MARKER_LINE" | grep -oP 'public_key=\K[^ ]+')
INITIAL_FP=$(echo "$MARKER_LINE" | grep -oP 'fingerprint=\K[^ ]+')
echo "Baseline — public_key: $INITIAL_PK"
echo "Baseline — fingerprint: $INITIAL_FP"
```

Record `INITIAL_PK` and `INITIAL_FP` in `docs/release-gates/runs/<date>-<rc>.md`.

### 3.4 10-cycle loop

```bash
ROTATIONS=0
PASS_COUNT=0

for i in $(seq 1 10); do
    echo "=== Cycle $i ==="

    # Clear logcat so this cycle's marker can be isolated.
    adb logcat -c

    adb shell am force-stop $PKG
    sleep 1
    adb shell am start -n $PKG/.MainActivity
    sleep 3   # allow reauthorize to fire and the marker to print

    # --- Capture marker ---
    MARKER=$(adb logcat -d -s godot:I | grep "\[CR-32-RELEASE-GATE\]" | tail -1)
    if [ -z "$MARKER" ]; then
        echo "FAIL cycle $i: logcat marker missing."
        echo "  Triage: demo-app T6.1 wiring is broken or app is a release build."
        echo "  Capture: adb logcat -d > /tmp/cycle_${i}_logcat.txt"
        adb logcat -d > /tmp/cycle_${i}_logcat.txt
        exit 1
    fi

    CURRENT_PK=$(echo "$MARKER" | grep -oP 'public_key=\K[^ ]+')
    CURRENT_FP=$(echo "$MARKER" | grep -oP 'fingerprint=\K[^ ]+')

    # --- Activity-transition assertion (AC-5 verbatim) ---
    ACTIVITY_HITS=$(adb shell dumpsys activity activities | grep -c "$WALLET_PKG" || true)
    if [ "$ACTIVITY_HITS" -ne "0" ]; then
        echo "FAIL cycle $i: $ACTIVITY_HITS wallet Activity transitions detected."
        echo "  Phantom UI was surfaced — this is NOT a silent reauth."
        echo "  Capture: adb shell dumpsys activity activities > /tmp/cycle_${i}_activities.txt"
        adb shell dumpsys activity activities > /tmp/cycle_${i}_activities.txt
        exit 1
    fi

    # --- public_key invariant ---
    if [ "$CURRENT_PK" != "$INITIAL_PK" ]; then
        echo "FAIL cycle $i: public_key drifted."
        echo "  Expected: $INITIAL_PK"
        echo "  Actual:   $CURRENT_PK"
        echo "  Root-cause candidates: SecureTokenStore corruption, CacheKey re-derivation bug, Phantom protocol violation."
        exit 1
    fi

    # --- fingerprint rotation detection (DD-2-2-5) ---
    if [ "$CURRENT_FP" != "$INITIAL_FP" ]; then
        ROTATIONS=$((ROTATIONS + 1))
        echo "ROTATION cycle $i: auth_token_fingerprint changed."
        echo "  Previous: $INITIAL_FP"
        echo "  New:      $CURRENT_FP"
        echo "  This is an INFORMATIONAL event per DD-2-2-5 — not an auto-fail."
        echo "  Record in runs/<date>-<rc>.md."
        INITIAL_FP=$CURRENT_FP   # update baseline for subsequent cycles
    fi

    PASS_COUNT=$((PASS_COUNT + 1))
    echo "  public_key: OK  |  activity_hits: 0  |  fingerprint: ${CURRENT_FP}"
done

# --- Final evaluation ---
echo ""
echo "=== Result: $PASS_COUNT/10 cycles passed (rotations=$ROTATIONS) ==="

if [ "$ROTATIONS" -ge "2" ]; then
    echo "WARNING: $ROTATIONS fingerprint rotations in one 10-cycle run."
    echo "  Per DD-2-2-5: 2+ rotations suggests a wallet bug — escalate manually."
    echo "  Do NOT auto-fail; record and forward to Phantom SDK contacts."
fi

if [ "$PASS_COUNT" -eq "10" ]; then
    echo "PASS: AC-5 release-gate satisfied."
else
    echo "FAIL: Only $PASS_COUNT/10 cycles passed — gate NOT satisfied."
    exit 1
fi
```

---

## 4. Success Criteria

The gate is **PASS** when ALL of the following hold:

| Criterion | Requirement |
|-----------|-------------|
| Cycle count | 10/10 cycles produce a `[CR-32-RELEASE-GATE]` marker line |
| `public_key` stability | Identical across all 10 cycles |
| `auth_token_fingerprint` stability | Identical across all 10 cycles (see rotation note below) |
| Activity transitions | 0 per cycle (`dumpsys activity activities \| grep -c $WALLET_PKG` == 0) |

**Fingerprint rotation note (DD-2-2-5):** If `auth_token_fingerprint` changes between
cycles, that is a Phantom-server rotation event — record it as INFORMATIONAL in the
run record but do NOT auto-fail the gate.  Two or more rotations in a single 10-cycle
run indicate a potential wallet bug; escalate manually before approving the RC.

---

## 5. Failure Triage

Use the following checklist when a cycle fails:

### 5.1 Marker missing (`[CR-32-RELEASE-GATE]` not in logcat)

- Is the installed APK a debug build?  (`adb shell pm dump $PKG | grep -i "debug"`)
  The marker is gated on `OS.is_debug_build()` — it does NOT print in release builds.
- Is the T6.1 wiring present?  Check `wallet_adapter_android.gd` for `_on_reauthorize_completed`.
- Did `reauthorize_completed` fire at all?  Look for `mwa_error` in logcat:
  `adb logcat -d -s godot:I | grep mwa_error`

### 5.2 `public_key` drifted

Capture all three diagnostics for triage:

```bash
# (a) Full logcat dump from the failing cycle
adb logcat -d > /tmp/triage_logcat.txt

# (b) Activity state
adb shell dumpsys activity activities > /tmp/triage_activities.txt

# (c) App data directory listing (file presence/absence; ciphertext is opaque)
adb shell run-as $PKG ls -la app_data files/ shared_prefs/ 2>/dev/null > /tmp/triage_files.txt
```

Root-cause candidates: SecureTokenStore corruption, CacheKey 3-tuple filter returned
wrong record, or Phantom protocol violation (wallet returned a different public key for
the same auth token).

### 5.3 Activity transitions detected

Phantom's UI was surfaced, meaning the token was not silently accepted.  Capture:

```bash
adb shell dumpsys activity activities > /tmp/triage_activities.txt
adb logcat -d -s godot:I | grep -E "mwa_error|mwa_timeout|reauthorize" > /tmp/triage_mwa.txt
```

Check for `TOKEN_EXPIRED` in `mwa_error` payloads — if the token was wiped (AC-3
graceful wipe), a new Connect is required and this is expected behavior, not a bug.

### 5.4 No `reauthorize_completed` and no `mwa_error`

Watchdog timeout (DD-2-2-3) or the persistent state file (`user://mwa_release_gate_state.json`)
was cleared.  Increase `sleep 3` to `sleep 8` and re-run cycle 1.  If still missing,
the `has_seen_connect` flag was reset — re-run the Bootstrap step (§3.2).

---

## 6. Cadence and Record-Keeping

Run this procedure **once per release candidate**.

After each run, create `docs/release-gates/runs/<YYYY-MM-DD>-<rc>.md` with:

```markdown
# Release Gate Run — 2-2 10-Cycle Reauth — <YYYY-MM-DD> RC<N>

| Field          | Value                  |
|----------------|------------------------|
| Date           | <YYYY-MM-DD>           |
| RC             | <rc tag or build hash> |
| Device         | <make/model/OS>        |
| Phantom version| <x.y.z>                |
| Result         | PASS / FAIL            |
| Pass count     | <N>/10                 |
| Rotations      | <N> (0 = ideal)        |
| Tester         | <name>                 |

## Notes
<Any rotation events, anomalies, or triage steps taken>
```

The `docs/release-gates/runs/` directory contains a `.gitkeep` — commit each run
record to version history so RC decisions are traceable.

---

## 7. Future Automation Note

This procedure is intentionally manual for the MVP release cycle.  If a future
story stands up Firebase Test Lab (FTL) with Phantom wallet-automation infra
(candidate: Story 5-x CI polish), this procedure can be ported to an FTL test
matrix.  File CR-46+ when that scope is first planned; until then, AC-5 is verified
manually pre-RC per DD-2-2-6.

---

*Procedure owner: MWA feature team.  Last updated: Story 2-2 T6 (2026-04-26).*
