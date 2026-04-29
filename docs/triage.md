---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# Triage Guide

This is a runbook for "my MWA call is failing — what now?" It covers common symptoms, the signal-by-signal decision tree, the 13-code error-to-diagnosis mapping, and when to reach for `MWA.get_diagnostics()`. For the canonical error-code reference see [`./error-codes.md`](./error-codes.md); for the signal model fundamentals see [`./signals-101.md`](./signals-101.md).

## 1. Decision tree by symptom

Match the symptom to the row, then follow the action.

| Symptom | What it means | First action |
|---|---|---|
| **No signal fires after `MWA.<op>(...)`** | Either you are on a non-Android build OR you forgot to subscribe to the result signal before invoking the op | Check `MWA.is_supported()` returns `true`. If `false`, the build is desktop / web / iOS / editor — gate the feature in UI. If `true`, verify your `_completed` and `mwa_error` listeners are connected BEFORE the op call. |
| **`mwa_error` fires** | An error path was hit; payload carries the code | Read `payload["code"]`. Look it up in Section 2 below. |
| **`mwa_timeout` fires** | The watchdog (default 60s) expired before any other terminal signal arrived. The wallet is hung, backgrounded, or the user walked away | Optionally retry; consider a smaller in-game timeout for UX feedback. The 4-key payload is `{request_id, source_method, elapsed_ms, watchdog_ms}` per [`./architecture.md`](./architecture.md) §3.2. |
| **`mwa_cancelled_lifecycle` fires** | The op was cancelled because the Activity was destroyed (screen rotation, process backgrounded) OR `MWA.forget_all()` was invoked mid-op | Inspect `payload["reason"]`: `"activity_destroyed"` → retry on next user interaction; `"forget_all_invoked"` → user-initiated wipe, do not retry. See Story 5-3 `mwa_cancelled_lifecycle` payload in [`./amendments.md`](./amendments.md) `A-12`. |
| **`reauth_required` fires** | The Keystore-backed token store is corrupt; the SDK auto-wiped it | Prompt the user to call `MWA.connect()` again. The session is gone but no transactions were lost. |
| **`pending_submission_found` fires** | A prior `sign_and_send` was interrupted by process death; breadcrumb survived | Query `getSignatureStatuses` for `payload["tx_preview_hashes"]`. Resubmit only if NONE landed. See [`./safety.md`](./safety.md) §4. |
| **The op succeeds but the chain doesn't show the tx** | `sign_and_send` is at-least-once; submitted ≠ confirmed | Continue polling `getSignatureStatuses`. See [`./safety.md`](./safety.md) §3. |

## 2. Error-code-to-diagnosis table

Maps the 13 codes from [`./error-codes.md`](./error-codes.md) to "what likely happened" and "what to do." For the full per-code reference (recoverable, retry_hint, source methods, default user_message), read [`./error-codes.md`](./error-codes.md).

| Code | What likely happened | What to do |
|---|---|---|
| `USER_CANCELED` | The user dismissed the wallet prompt. | Show a friendly toast. The caller may retry. |
| `WALLET_REJECTED` | The wallet user explicitly rejected the signing prompt. | Same recovery as `USER_CANCELED`. Both are user-initiated dismissals. |
| `NO_MWA_WALLET_INSTALLED` | No MWA-compatible wallet APK is installed on the device. | Present `MWA.get_suggested_wallet_install_links()` UI. **Common false-positive on emulators without Fake Wallet** — see Section 5. |
| `WALLET_UNINSTALLED` | The wallet that issued the cached auth_token is no longer on the device. | Clear the cached session (`MWA.disconnect()` or `MWA.deauthorize()`) and prompt the user to reconnect. |
| `TOKEN_EXPIRED` | Cached token rejected by the wallet. | The SDK auto-wipes the cached token on this code; caller invokes `MWA.connect()`. Do NOT auto-launch the wallet chooser without user intent. |
| `RPC_FAILED` | The wallet forwarded the transaction to RPC and the cluster rejected it. | Inspect `payload["developer_details"]` for the cluster error message; rebuild the transaction and retry. |
| `NETWORK_OFFLINE` | The wallet cannot reach the RPC endpoint, OR the wallet's MWA association call failed offline. | Prompt the user to verify connectivity and retry. |
| `TIMEOUT` | An op upstream of the watchdog reported a timeout (the watchdog itself fires `mwa_timeout`, not `mwa_error.code=TIMEOUT`). | Retry, or show "wallet not responding" UX. |
| `NOT_CONNECTED` | No cached token present, OR cluster mismatch in the cluster-bleed defensive guard (`AC-6` / `AC-NFR-SEC-4`). | Call `MWA.connect(identity, cluster)` and await `connect_completed` before retrying the failed op. |
| `UNSUPPORTED_PLATFORM` | The current build is not Android (desktop / web / iOS / editor). | Render feature-gated UI. Use `MWA.is_supported()` as the synchronous preflight. |
| `PROTOCOL_ERROR` | Wire-protocol failure (malformed JSON, unexpected wallet response, JNI exception), OR catastrophic local-wipe failure on `deauthorize` (per `DD-4-1-3`). | Inspect `payload["developer_details"]`; recommended path is a clean reconnect (`MWA.disconnect()` → `MWA.connect()`). |
| `STORAGE_CORRUPT` | The encrypted token store cannot be read (Tink corruption — typical post-Android-backup-restore). | The SDK auto-wipes the corrupt store and emits `reauth_required{reason: "keystore_corrupt"}`. Caller invokes `MWA.connect()` to mint a fresh token. |
| `REAUTH_REQUIRED` | Session-required op found a missing/invalid session in a way distinct from `TOKEN_EXPIRED` — typically `sign_and_send` after Tink corruption (`DD-3-3-G` fail-closed wrapper). | Same recovery class as `TOKEN_EXPIRED`. Prompt the user to reconnect. |

## 3. When to read `MWA.get_diagnostics()`

`MWA.get_diagnostics()` returns a 12-key Dictionary safe to attach to bug reports (Story 5-2 `AC-1`; see [`./api-reference.md`](./api-reference.md) `MWA.get_diagnostics`). Reach for it when:

- You file a support ticket — attach the full 12-key snapshot.
- Repeated `mwa_error` events fire and the `code` field doesn't match any pattern in Section 2 — `last_n_correlation_trace` shows the recent op chain.
- A `late_result_count` greater than 0 indicates ops whose result arrived after the watchdog fired — this is informational; not an error in itself.
- A `pending_submissions_count` greater than 0 indicates breadcrumbs awaiting recovery — usually the next `connect_completed` clears them via `pending_submission_found`.

The 12 keys are: `sdk_version`, `clientlib_ktx_version`, `godot_version`, `android_api_level`, `session_state`, `wallet_package`, `wallet_version`, `auth_token_fingerprint`, `cluster`, `last_n_correlation_trace`, `late_result_count`, `pending_submissions_count`. Per `AC-3`, zero auth-token bytes / signing key material / PII — the only token-derived field is the 8-hex `auth_token_fingerprint` (HKDF-SHA256 truncated, NOT the token).

`MWA.get_device_posture()` ([`./api-reference.md`](./api-reference.md)) returns 4 best-effort heuristic booleans (`rooted`, `debuggable`, `developer_options_on`, `adb_enabled`) intended for dApp-side mainnet gating UX. **Do NOT use as a security gate** — a determined attacker can mask each signal at the OS level.

## 4. When to enable verbose logging

Per `AC-D-1` ([`./spec.md`](./spec.md) line 152), the spec describes `MWA.set_verbose(true)` as the verbose-toggle entrypoint. **As of this writing, `MWA.set_verbose` does NOT exist on the GDScript facade** ([`addons/SolanaSDK/mwa/MWA.gd`](../addons/SolanaSDK/mwa/MWA.gd) — no `set_verbose` symbol). This is a known facade gap tracked as `CR-5-5-A` ([`./concerns.md`](./concerns.md)). Until it lands:

- `SdkLog.v` / `SdkLog.d` are R8-stripped in release per `AC-NFR-SEC-6` and unavailable to runtime toggles in production.
- For debugging in development builds, the existing tag `MWA` ([`./spec.md`](./spec.md) `AC-D-1`) carries V/D/I/W/E levels via Android `Log.*`. Filter logcat by `adb logcat MWA:V *:S` to see all SDK logs from a development APK.
- `MWA.get_diagnostics()`'s `last_n_correlation_trace` provides post-hoc tracing without needing verbose logs enabled at runtime.

In a future minor release, `MWA.set_verbose(true)` lands on the facade; this section will document the runtime toggle. For now, the workflow is: develop with a debuggable APK + `adb logcat MWA:V`, ship with R8-stripped release builds, and rely on `get_diagnostics()` for in-the-wild triage.

## 5. Common false-positives

Three failure-shaped events that are NOT bugs in the SDK:

- **Emulator without Fake Wallet → `NO_MWA_WALLET_INSTALLED`.** A vanilla Android emulator image has no MWA-compatible wallet pre-installed. Install `fakewallet-debug.apk` from the upstream `solana-mobile/mobile-wallet-adapter` repo per [`../REVIEW.md`](../REVIEW.md) Step 3. This is the most common "is the SDK broken?" report — answer: no, install Fake Wallet.
- **Process killed by OOM mid-op → no terminal signal.** If the OS reaps the process during a wallet round-trip and the op is `sign_and_send`, the breadcrumb survives and `pending_submission_found` fires on the next `connect_completed`. For other ops, the in-flight slot is lost — no signal, no breadcrumb. Caller retries on the next user interaction. This is by design per [`./architecture.md`](./architecture.md) §7.5.
- **Screen rotation mid-prompt → `mwa_cancelled_lifecycle{reason: "activity_destroyed"}`.** Godot's Activity is destroyed and recreated on rotation; in-flight ops are cancelled with this lifecycle signal. Retry on the next user interaction. Locking the orientation in `project.godot` prevents this entirely.

## See also

- [`./error-codes.md`](./error-codes.md) — full per-code reference (recoverable, retry_hint, source methods, default messages).
- [`./signals-101.md`](./signals-101.md) — explainer for Godot's signal model (1-arg Dictionary, additive keys, request_id correlation, await + race).
- [`./api-reference.md`](./api-reference.md) — `MWA.get_diagnostics()`, `MWA.get_device_posture()`, `MWA.get_auth_token_fingerprint()` for triage tooling.
- [`./safety.md`](./safety.md) — at-least-once `sign_and_send` semantics and `pending_submission_found` recovery.
- [`./amendments.md`](./amendments.md) `A-12` — `mwa_cancelled_lifecycle` 1-param Dictionary contract; `A-14` — 10-key `mwa_error` payload.
