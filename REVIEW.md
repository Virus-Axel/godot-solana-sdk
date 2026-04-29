# REVIEW.md — Grant Reviewer Quick-Path

This document is for grant reviewers who want to verify the Godot Mobile Wallet Adapter (MWA) SDK end-to-end without reading source code. It takes you from a clean machine to a running 7/7-method demo on an Android emulator in roughly **10 minutes** with warm caches.

The SDK source lives in **this repository**. The demo Godot project lives **separately** at [`ValentinVPK/godot-mwa-example`](https://github.com/ValentinVPK/godot-mwa-example) — that separation is intentional (see [`docs/amendments.md` A-17](./docs/amendments.md)): the demo consumes this repo's addon as a published release artifact, exactly the way a real downstream Godot game would. If the published artifact is wrong, the demo fails — which is the FR-1 audit working as intended.

---

## Prerequisites

- **Godot 4.3.x editor** (project targets 4.3.x; addon's `compatibility_minimum` is `"4.3"`).
- **Android device or emulator** (API 23+, matches the SDK's `minSdk`). The fastest setup is `reactivecircus/android-emulator-runner@v2`'s default API 33 image, but any API 23+ image works.
- **`adb`** on PATH (Android SDK platform-tools).
- **`gh` CLI** authenticated against GitHub (only needed to download release assets; `curl` works as a fallback).
- **Fake Wallet APK** built from upstream — see Step 3.

You do **not** need: Java, Kotlin, C++, scons, NDK, or any familiarity with the Android Gradle build. The single-addon-install path is the FR-1 promise being audited here.

---

## Step 1 — Clone the example repo

```bash
git clone https://github.com/ValentinVPK/godot-mwa-example.git
cd godot-mwa-example
```

The example repo ships only the GDScript example (one scene, one config script, two helper scripts, `tools/review-demo.sh`). It does **not** contain the SDK; you install the SDK in Step 2.

---

## Step 2 — Download this repo's addon release

Open this repo's GitHub Releases page and grab the latest `v0.1.0-rc*` (or `v0.1.0` once promoted):

- Releases: <https://github.com/ValentinVPK/godot-solana-sdk/releases>
- Direct download (rc1): <https://github.com/ValentinVPK/godot-solana-sdk/releases/tag/v0.1.0-rc1>

Download `addons-bundle.zip` from the release assets, then extract into the example repo's `addons/` directory:

```bash
# from inside godot-mwa-example/
mkdir -p addons
unzip -o ~/Downloads/addons-bundle.zip -d addons/
# you should now have addons/SolanaSDK/ with GDExtension binaries + AAR + GDScript
```

Verify the addon's `compatibility_minimum`:

```bash
grep '^compatibility_minimum' addons/SolanaSDK/godot-solana-sdk.gdextension
# expected: compatibility_minimum = "4.3"
```

If it reads `"4.4"`, the addon release predates [DD-5-4-1](./docs/stories/5-4.md) — pull a newer release.

---

## Step 3 — Install Fake Wallet

The Fake Wallet is the canonical reference wallet used in `solana-mobile`'s own MWA test suite. Build it from upstream:

```bash
git clone https://github.com/solana-mobile/mobile-wallet-adapter.git
cd mobile-wallet-adapter/android
./gradlew :fakewallet:assembleDebug
adb install fakewallet/build/outputs/apk/debug/fakewallet-debug.apk
cd ../..
```

(If you already have `fakewallet-debug.apk` from a prior session, skip the build and just run the `adb install`.)

Verify the install:

```bash
adb shell pm list packages | grep com.solanamobile.fakewallet
# expected: package:com.solanamobile.fakewallet
```

---

## Step 4 — Run the demo

Boot the emulator (or attach a device), open `godot-mwa-example/project.godot` in Godot 4.3.x, then export-and-install the example app and run the scripted demo:

```bash
# from godot-mwa-example/
./tools/review-demo.sh           # builds export, installs, drives all 7 methods
```

The script orchestrates:

1. Godot headless export to `./build/godot-mwa-example.apk`.
2. `adb install -r ./build/godot-mwa-example.apk`.
3. `adb shell am start` to launch the example.
4. Drives the 7 MWA op buttons in sequence with a small delay between each.
5. Pulls `review-pass.log` from the example's `user_data_dir` and asserts exactly 7 `|PASS|` markers.

Expected output (one line per method):

```
PASS: connect
PASS: disconnect
PASS: reauthorize
PASS: deauthorize
PASS: sign_messages
PASS: sign_transactions
PASS: sign_and_send
TOTAL: 7/7 PASS in <wallclock> seconds
```

If any line is missing or the script exits non-zero, the demo failed; consult the example repo's CI logs (Section 10) before treating it as a regression in the SDK.

---

## What to look for (FR-1 / FR-16 audit checklist)

A grant reviewer should be able to mechanically check each of these:

- [ ] Addon loads on Godot 4.3.x without `incompatible godot version` / `addon requires Godot 4.4` errors in the editor's import log (validates [DD-5-4-1](./docs/stories/5-4.md)).
- [ ] All 9 buttons in the example's main scene render with screen-reader-visible accessible names (validates AC-4).
- [ ] The cluster label reads `Cluster: devnet` by default; switching to `mainnet-beta` requires both editing a const **and** confirming an `"I understand — use mainnet"` dialog (validates AC-2 + AC-NFR-SEC-10).
- [ ] `tools/review-demo.sh` (in the example repo) prints exactly 7 `PASS` markers — one per MWA op (validates AC-3).
- [ ] Total wall-clock from `git clone` to `7/7 PASS` is ≤10 minutes on a modern laptop with warm caches (validates AC-3 SLO; soft target — [Story 5-6](./docs/plan.md) wires the long-term CI SLO monitor).
- [ ] On any `mwa_error`, the example's log area renders the `user_message` as **text** (color is decoration, not the only signal) — validates AC-4 accessibility.

If any of these fails, that **is** the audit signal: file a finding against the SDK or the example repo as appropriate.

---

## Design guidance for the example-repo implementer

These constraints are LOCKED in [`docs/amendments.md` A-17](./docs/amendments.md) and must be followed by `ValentinVPK/godot-mwa-example`. They are documented here because REVIEW.md is the canonical reviewer-facing surface; Story 5-5's documentation bundle and Story 5-6's CI all reference this section.

- **Default cluster.** `cluster = "devnet"` is hard-coded in `config.gd`. Switching to `"mainnet-beta"` requires both a `const`-edit **and** an in-app `ConfirmationDialog` with body text `"I understand — use mainnet"`. Confirmation is **per-app-launch only** (not persisted to `user://`) — fresh attention-checkpoint each launch. This is a security control, not a UX nicety.
- **Identity URI placeholder.** `identity_uri = "https://example.com/godot-mwa-sample"` is intentional. The external repo MUST NOT pick a real domain, per AC-NFR-SEC-10's identity-metadata attacker-controllability concern.
- **PASS marker shape.** The example writes a single line per success-signal handler to `OS.user_data_dir() + "/review-pass.log"`:

  ```
  <ISO8601 timestamp>|<method>|PASS|<correlation_id>
  ```

  `method` is one of `connect / disconnect / reauthorize / deauthorize / sign_messages / sign_transactions / sign_and_send`. `correlation_id` is the 8-hex `request_id` echoed in the corresponding `*_completed` payload. Zero PII (no auth_token, no public key). `tools/review-demo.sh` does `adb pull` followed by `grep -c '|PASS|'`.

- **`_race_signals` helper.** Godot 4.3 has no built-in `Signal.race()`. Ship `static func _race_signals(success_sig: Signal, error_sig: Signal) -> Dictionary` at `<example-repo>/scripts/await_helpers.gd`. Returns `{ok: true, value: <success-payload>}` or `{ok: false, error: <error-payload>}`. Connect both signals once, await whichever fires first, disconnect the other. **Example-internal only** — do not contribute back to `addons/SolanaSDK/`; the addon's public API stays minimal (FR-1 single-addon-install discipline).
- **Cancel button.** The Cancel button (label `[Forget All]` per AC-1) calls `MWA.forget_all()` directly — that's the documented cancel-in-flight path; it emits `mwa_cancelled_lifecycle{reason: "forget_all_invoked"}` per in-flight slot.
- **`pending_submission_found` is one-shot.** Subscribe in `_ready()`:

  ```gdscript
  MWA.pending_submission_found.connect(_on_pending_submission_found, CONNECT_ONE_SHOT)
  ```

  This demonstrates the breadcrumb-survives-process-death recovery path without leaking subscriptions across runs.

- **AC-1 button list (verbatim).** The main scene contains exactly these nine `Button` nodes, in this order:

  1. `[Connect]` — `accessible_name = "Connect to wallet"`
  2. `[Disconnect]` — `accessible_name = "Disconnect from wallet"`
  3. `[Reauthorize]` — `accessible_name = "Silently reauthorize cached session"`
  4. `[Deauthorize]` — `accessible_name = "Revoke wallet authorization"`
  5. `[Sign Message]` — `accessible_name = "Sign a test message"`
  6. `[Sign Transaction]` — `accessible_name = "Sign a test transaction"`
  7. `[Sign and Send]` — `accessible_name = "Sign and submit a test transaction"`
  8. `[Reset Local State]` — `accessible_name = "Clear local cache"` (calls `MWA.disconnect()` because `MWA.clear_local_cache()` is not on the GDScript facade today; see CR-5-4-A in `docs/concerns.md`).
  9. `[Forget All]` — `accessible_name = "Cancel pending op + GDPR wipe"`.

- **`Button.accessible_name` (Godot 4.3+).** This is a Control-level property landed in the Godot 4.3 accessibility track. The exact GDScript property name (`accessible_name` vs `accessibility_name`) depends on the 4.3.x point release; the implementer should confirm via `Object.get_property_list()` or the Godot release notes for their pinned version.
- **AC-6 state-transition log shape.** Every op handler logs `[<ISO timestamp>] <method>: <state>` with substates `started → wallet-prompt-launched → wallet-returned → signal-fired`. The middle two are coarse-grained — Godot has no direct hook into Android Activity transitions; log `wallet-prompt-launched` immediately after the `MWA.<op>(...)` call returns and `wallet-returned` when any terminal signal fires.

---

## API surface (consumed by the example repo)

The example app calls into the addon's public GDScript surface — 14 methods + 12 signals — exposed via the `MWA` autoload (registered by `addons/SolanaSDK/solana_sdk_plugin.gd`). Source of truth: `addons/SolanaSDK/mwa/MWA.gd:27-418`. This table is the canonical API reference until [Story 5-5](./docs/plan.md)'s `docs/api-reference.md` ships.

| Symbol | Signature / Shape | Source |
|--------|--------------------|--------|
| `MWA.connect(identity, cluster, opts={})` | `Dictionary, String, Dictionary -> String` (returns 8-hex `request_id`) | `MWA.gd:133` |
| `MWA.disconnect()` | `() -> void` (emits `disconnect_completed`) | `MWA.gd:141` |
| `MWA.reauthorize(opts={})` | `Dictionary -> void` | `MWA.gd:184` |
| `MWA.deauthorize()` | `() -> void` (emits `deauthorize_completed`) | `MWA.gd:168` |
| `MWA.sign_messages(messages, opts={})` | `Array[PackedByteArray], Dictionary -> void` | `MWA.gd:213` |
| `MWA.sign_transactions(transactions, opts={})` | `Array[PackedByteArray], Dictionary -> void` | `MWA.gd:246` |
| `MWA.sign_and_send(transactions, opts={})` | `Array[PackedByteArray], Dictionary -> void` (may also emit `pending_submission_found` after a process-death recovery) | `MWA.gd:300` |
| `MWA.forget_all()` | `() -> void` (no completion signal; emits `mwa_cancelled_lifecycle{reason: "forget_all_invoked"}` per in-flight op) | `MWA.gd:334` |
| `MWA.is_supported()` | sync; `true` on Android only (Story 5-1 NoOp on other platforms) | `MWA.gd:346` |
| `MWA.is_connected() / get_public_key() / get_cluster() / get_wallet_label() / get_diagnostics() / get_device_posture()` | sync state getters | `MWA.gd:354-408` |
| `connect_completed / reauthorize_completed / disconnect_completed / deauthorize_completed / sign_messages_completed / sign_transactions_completed / sign_and_send_completed (request_id, result)` | 2-param signal; result is a Dictionary whose shape varies per op | `MWA.gd:27-33` |
| `mwa_error(payload)` | 1-param; 9-key Dictionary `{request_id, code, user_message, developer_details, recoverable, retry_hint, layer, cause, source_method}` | `MWA.gd:34` |
| `mwa_timeout(payload)` | 1-param; 4-key `{request_id, source_method, elapsed_ms, watchdog_ms}` | `MWA.gd:35` |
| `mwa_cancelled_lifecycle(payload)` | 1-param; 3-key `{request_id, source_method, reason}`; reason ∈ `{forget_all_invoked, activity_destroyed}` | `MWA.gd:36` |
| `reauth_required(payload)` | 1-param; `{reason}` ∈ `{keystore_corrupt}` | `MWA.gd:37` |
| `pending_submission_found(payload)` | 1-param **ONE-SHOT**; 6-key `{request_id, op_type, started_at_ms, tx_count, tx_preview_hashes, recommendation}` | `MWA.gd:47` |

For full payload schemas and error-code semantics, see [`docs/architecture.md` §3.2](./docs/architecture.md) and [`docs/architecture.md` §3.3](./docs/architecture.md).

---

## CI status

- **This repo's CI:** <https://github.com/ValentinVPK/godot-solana-sdk/actions> — every PR runs build, lint, Kotlin unit tests (`./gradlew test`), pytest tooling, and the multi-platform GDExtension load matrix. Story 5-6 will add `instrumented_emulator_fakewallet.yml` (PR-blocking emulator + Fake Wallet integration), with the workflow's source-of-truth (this repo vs the external repo) decided per [CR-5-4-D](./docs/concerns.md).
- **Example repo's CI:** <https://github.com/ValentinVPK/godot-mwa-example/actions> — covers AC-1 (scene loads in Godot 4.3.x), AC-2 (devnet default + mainnet gate), AC-4 (accessibility), AC-5 (await + error race), AC-6 (state-transition log). Per [A-17](./docs/amendments.md) and [CR-5-4-E](./docs/concerns.md), grant reviewers should not trust those ACs unless the example repo's CI badge reads green at review time.

If the example repo's CI is red, that **is** the audit signal — file a finding against the example repo before forming a verdict on the SDK.
