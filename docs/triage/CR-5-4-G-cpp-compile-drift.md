# CR-5-4-G — godot-cpp API drift in WalletAdapterSigner / Transaction (CI-only constraint)

| Field | Value |
|---|---|
| **Concern** | [`docs/concerns.md#CR-5-4-G`](../concerns.md) |
| **Severity** | HIGH |
| **Status** | Documented as CI-only constraint per Q2=(b) LOCKED 2026-04-30 (Story 5-6 T14) |
| **Origin** | Story 5-4 T2 attempted local release-cutting via `scons addon`; pre-existing C++ compile blocker surfaced |
| **Tracking issue (planned)** | `cpp-compile-drift` label — created by user (origin remote is read-only for the LaiM session; see "Tracking issue creation" below) |
| **Affected releases** | All releases prior to the C++ source fix landing |
| **Affected ACs** | AC-1 (one-command build) — local `scons addon` fails; AC-9 (release-on-tag) — first multi-platform CI run is the integration test |

## Summary

Story 5-4 T2 (2026-04-29) attempted to cut `v0.1.0-rc1` from the local dev machine via `scons addon`. After resolving an unrelated Gradle JVM heap exhaustion, the build then failed at the C++ compile step with errors in source files NOT authored by Story 5-4. Stories 5-1 / 5-2 / 5-3 had not surfaced these errors because their test surfaces (`./gradlew test` + pytest) do not exercise the C++ compile path.

The errors are **real godot-cpp API drift** in the locally-pinned godot-cpp submodule (currently at `714c9e2c` synced against `4c311cbee68c0b66ff8ebb8b0defdd9979dd2a41` — Godot 4.4-stable). They are NOT regressions introduced by Story 5-4 / 5-5 / 5-6 work.

**Q2 resolution path:** Q2=(b) Document as CI-only constraint LOCKED at Story 5-6 story-creation HALT 2026-04-30. The supported install path for fresh-clone consumers is the published GitHub Release produced by `release_on_tag.yml` (Story 5-6 T11). Multi-platform CI builders may pin a different godot-cpp commit, may use a different host toolchain, and may therefore not hit the same compile blocker. The local-build path remains a follow-up; this runbook is the navigation aid.

## Cited offenders (verified at master HEAD `5310fa2e..040abe9e`, 2026-04-30)

> All line numbers confirmed by inspection at the time of T14 close-out. Future edits may shift line numbers — when in doubt, `grep -n 'is_connected\|ObjectID(' src/wallet_adapter/ src/transaction/`.

### 1. `src/wallet_adapter/wallet_adapter_signer.cpp:216,219`

```cpp
// Lines 215-220 (release_signal_connections):
const Callable on_signed(this, "_on_message_signed");
const Callable on_failed(this, "_on_signing_failed");
if (wa_->is_connected("message_signed", on_signed)) {   // line 216
    wa_->disconnect("message_signed", on_signed);
}
if (wa_->is_connected("signing_failed", on_failed)) {   // line 219
    wa_->disconnect("signing_failed", on_failed);
}
```

**Failure mode:** `wa_` is a `godot::Object *` whose runtime type is `WalletAdapter`. `WalletAdapter::is_connected()` (zero-arg, declared at `include/wallet_adapter/wallet_adapter.hpp:153`) shadows the inherited `Object::is_connected(StringName, Callable) const` (2-arg). The compiler's overload resolver prefers the derived-class member with NO arguments and rejects the 2-arg call site with a message-suggestion fallback to `godot::CanvasItem::is_connected`.

**Pre-existing inline NOTE** at `wallet_adapter_signer.cpp:70-75` documents the resolution rule for the **zero-arg** call sites (lines 77, 81, 98) — but the **two-arg** call sites at 216 / 219 were not addressed. Symmetric defensive surfaces under the same overload-resolution rule (per `cpp.md` retrospect "[2026-04-21] Setter symmetry in object lifecycle").

### 2. `src/transaction/transaction.cpp:200,203`

```cpp
// Lines 198-206 (Transaction::partially_sign / try_register_isigner):
const Callable on_signed(this, "_isigner_signed");
const Callable on_failed(this, "_isigner_failed");
if (!isigner_native->is_connected("sign_completed", on_signed)) {  // line 200
    isigner_native->connect("sign_completed", on_signed);
}
if (!isigner_native->is_connected("sign_failed", on_failed)) {     // line 203
    isigner_native->connect("sign_failed", on_failed);
}
```

**Failure mode:** Same root cause as offender 1. `isigner_native` is `godot::Object *` whose runtime type may be `LocalKeypairSigner` or `WalletAdapterSigner` (both `ISigner` implementations); the 2-arg `Object::is_connected` overload should resolve, but the godot-cpp current pin's overload set has shifted such that the resolver picks a wrong overload.

### 3. `src/transaction/transaction.cpp:209` — STATUS UPDATED

The original CR-5-4-G citation listed an `ObjectID(uint64_t_value)` constructor call as a third compile error. **Verified at master HEAD 2026-04-30:** no `ObjectID(...)` constructor invocation exists in `src/transaction/transaction.cpp` at any line. The closest-matching call at line 209 is:

```cpp
isigner_connected_signer_ids_.insert(isigner_native->get_instance_id());
```

`get_instance_id()` returns `godot::ObjectID` directly — no rvalue construction. This third error may have been:
- Resolved by a source edit between the CR-5-4-G filing (Story 5-4 T2 attempt, 2026-04-29) and master HEAD (2026-04-30) — but no such fix is visible in `git log src/transaction/transaction.cpp`.
- Misattributed in the original CR — the line citation may have been off-by-N.
- Transient under a different godot-cpp pin.

**Action:** the runbook tracks this as an uncertain third offender. T11's synthetic test-tag run is the definitive multi-platform check.

### 4. `src/transaction/transaction.cpp:365,368` (related, NOT in original CR)

```cpp
// Lines 360-370 (Transaction::disconnect_all_isigner_signers):
for (const ObjectID &id : isigner_connected_signer_ids_) {
    godot::Object *obj = ObjectDB::get_instance(id);
    ...
    if (obj->is_connected("sign_completed", on_signed)) {   // line 365
        obj->disconnect("sign_completed", on_signed);
    }
    if (obj->is_connected("sign_failed", on_failed)) {      // line 368
        obj->disconnect("sign_failed", on_failed);
    }
}
```

**Failure mode:** Same overload-resolution drift as offenders 1 + 2. Lines 365 + 368 use the 2-arg `is_connected("string", callable)` against a `godot::Object *` whose runtime type may be a derived class with a zero-arg `is_connected()` shadow. NOT explicitly cited in CR-5-4-G but exhibits the same root cause; will need the same fix when Q2 is escalated to (a).

## Root cause

Two distinct godot-cpp API drift modes:

1. **Overload-resolution shadowing** (offenders 1, 2, 4): When a derived class declares `bool is_connected() const` (zero-arg member function) shadowing the inherited `Object::is_connected(StringName, Callable) const` (2-arg method), a call against a derived-class pointer with 2 args fails to find an overload. C++ name-hiding rules do not consider base-class overloads automatically; an explicit `using Object::is_connected;` (or a static_cast at the call site) would unhide them.

2. **`ObjectID` lifetime tightening** (offender 3 — uncertain): If reproducible, godot-cpp likely tightened `ObjectID` to no longer accept rvalue `uint64_t` direct-initialization in favor of an explicit factory or `Object::get_instance_id()` call. Cannot confirm at master HEAD because the cited line no longer matches the cited error.

## Workarounds for local-build users

Until Q2 is escalated to (a) C++ source fix, fresh-clone consumers should use the **published GitHub Release** as the install path. Local-build users who need to reproduce the addon should:

```bash
# 1. Verify the same godot-cpp pin you're building against
git submodule status godot-cpp
# Expected: 714c9e2c (or newer per godot-cpp track)

# 2. Try a different godot-cpp pin (4.3-stable instead of 4.4-stable):
cd godot-cpp && git checkout godot-4.3-stable && cd ..

# 3. Re-run scons. If the compile errors disappear, the issue is the
# godot-cpp 4.4-stable pin. If they persist, escalate to Q2=(a).
scons addon

# 4. Multi-platform CI (release_on_tag.yml) is the canonical reference
# for "does this build cleanly?" — until that workflow's first non-
# synthetic run, treat ANY local scons failure as a separate datapoint,
# not a release blocker.
```

## Escalation criterion (Q2=(b) → Q2=(a))

Q2=(b) was selected at story-creation HALT 2026-04-30 on the rationale that **multi-platform CI builders may dodge the local compile blocker** because they may pin different toolchain versions or different godot-cpp commits. T11's `release_on_tag.yml` synthetic `v0.0.1-test*` tag verification is the integration test for this assumption.

**Escalate to Q2=(a) C++ source fix iff:**

1. T11's synthetic test-tag run on `release_on_tag.yml` triggers the same compile errors at the multi-platform builders (not just locally). Evidence: workflow log lines mentioning `wallet_adapter_signer.cpp:216` or `transaction.cpp:200` overload-resolution failures.
2. OR a real `v0.1.0` release-cut attempt fails on the same lines.

**Q2=(a) source-fix sketch (deferred — DO NOT execute as part of T14):**

```cpp
// Option A: Add a using-declaration to unhide Object::is_connected.
// In include/wallet_adapter/wallet_adapter.hpp, inside class WalletAdapter:
using godot::Object::is_connected;  // Unhide the 2-arg overload.

// Option B: Cast at each 2-arg call site.
if (static_cast<godot::Object *>(wa_)->is_connected("message_signed", on_signed)) {
    ...
}
```

Option A is cleaner (one-line fix per affected derived class — `WalletAdapter`, possibly `LocalKeypairSigner` and `WalletAdapterSigner` if their public surfaces shadow the 2-arg overload too). Option B is a per-call-site fix and accumulates noise.

## Tracking issue creation

The current LaiM session operates against an upstream remote (`Virus-Axel/godot-solana-sdk`) that is **read-only for the user** (per state.json `notes.remoteWarning`). Issue creation requires either:

1. Pushing the `feature/mwa-android` branch to the user's own fork first, then `gh issue create` against that fork; or
2. Filing the tracking issue under whichever fork hosts the v0.1.0 release.

**Suggested invocation (run by user when fork is ready):**

```bash
gh issue create \
  --repo "<owner>/godot-solana-sdk" \
  --title "C++ compile-drift: godot-cpp Object::is_connected overload resolution shadowed by derived-class zero-arg overrides" \
  --label cpp-compile-drift \
  --body "Tracks CR-5-4-G HIGH per docs/triage/CR-5-4-G-cpp-compile-drift.md.

Affected files / lines (verified at commit \$(git rev-parse --short HEAD)):
- src/wallet_adapter/wallet_adapter_signer.cpp:216,219
- src/transaction/transaction.cpp:200,203,365,368
- src/transaction/transaction.cpp:209 (uncertain — runbook tracks)

Workaround: fresh-clone consumers use the GitHub Release as install path.
Escalation criterion: T11 release_on_tag.yml synthetic test-tag failure on multi-platform builders.
Fix sketch: 'using godot::Object::is_connected' in derived classes (Option A in runbook).
"
```

When the issue is filed, update this runbook with the issue number AND update `docs/concerns.md#CR-5-4-G` Status field with the issue link.

## Closure path

CR-5-4-G closes when ONE of the following lands:

1. **Q2=(a) source fix** — one-line `using godot::Object::is_connected;` in derived classes; `scons addon` succeeds locally; baseline + integration tests pass; concerns.md CR-5-4-G Status → CLOSED.
2. **Multi-platform CI dodges the issue** — T11's first non-synthetic `release_on_tag.yml` run produces a working `v0.1.0` release; concerns.md CR-5-4-G Status → CLOSED-with-CI-fallback (local builds still broken; runbook stays for future contributors).

If T11 itself fails on the same lines, this runbook stays open AND escalation to Q2=(a) becomes mandatory before v0.1.0 release.

## Related concerns

- [`CR-5-4-F`](../concerns.md) — release-on-tag CI MANDATED (CLOSED at Story 5-6 T11). The MANDATE was reclassified from LOW-deferred to HIGH because CR-5-4-G blocks local release-cutting.
- [`CR-5-4-D`](../concerns.md) — `instrumented_emulator_fakewallet.yml` external-repo redesign (CLOSED at Story 5-6 T7 via DD-5-6-9 LOCKED option (iii)).
- [`CR-5-6-G`](../concerns.md) — Android Lint NewApi baseline (HIGH; pre-existing in production code; baselined at Story 5-6 T1 per Q2=(b) Kotlin analog of this runbook's pattern).
