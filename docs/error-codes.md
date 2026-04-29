---
Owner: @ValentinVPK
Drift-check: codegen_drift_check.yml
---

# Error Codes Reference

The `mwa_error` signal carries a 10-key payload (per [`docs/amendments.md`](./amendments.md) `A-14`); the `code` field is one of the 13 enum values in this reference. The single source of truth for this taxonomy is the YAML manifest at [`docs/architecture.md`](./architecture.md) §3.3 (lines 316-409); CI drift is enforced by the `codegen_drift_check.yml` workflow per `AC-A-3`.

## How to read this reference

For each code below:

- **Recoverable** — whether the SDK considers this transient. `recoverable: true` means a retry has a reasonable chance of succeeding; `recoverable: false` means the caller should not retry (it is a permanent / configuration / platform issue).
- **Retry hint** — one of `retry_now`, `connect`, `install_wallet`, `none`. The `mwa_error` payload's `retry_hint` field carries this verbatim.
- **Default user message** — verbatim from the manifest. Safe to display in-game.
- **Source methods** — which op-method emit-sites can fire this code.
- **What to do** — recovery pattern (cross-references [`docs/architecture.md`](./architecture.md) §5.3, lines 632-640).

The full 10-key `mwa_error` payload also carries `request_id`, `code`, `message`, `user_message`, `developer_details`, `recoverable`, `retry_hint`, `layer`, `cause`, `source_method` — see [`docs/api-reference.md`](./api-reference.md) and [`docs/architecture.md`](./architecture.md) §3.2 for full details.

## USER_CANCELED

- **Recoverable:** `true`
- **Retry hint:** `retry_now`
- **Default user message:** *"You cancelled the wallet prompt."*
- **Source methods:** `connect`, `sign_messages`, `sign_transactions`, `sign_and_send`.
- **What to do:** show a friendly toast; the caller may retry with a new prompt. Per [`docs/architecture.md`](./architecture.md) §5.3.

## WALLET_REJECTED

- **Recoverable:** `true`
- **Retry hint:** `retry_now`
- **Default user message:** *"The wallet rejected the request."*
- **Source methods:** `sign_messages`, `sign_transactions`, `sign_and_send` (the wallet user explicitly rejects the signing prompt).
- **What to do:** show a friendly message; caller may retry. Same recovery class as `USER_CANCELED` — both are user-initiated dismissals from the wallet's side. Per [`docs/architecture.md`](./architecture.md) §5.3.

## NO_MWA_WALLET_INSTALLED

- **Recoverable:** `true`
- **Retry hint:** `install_wallet`
- **Default user message:** *"No Solana wallet installed. Install one to continue."*
- **Source methods:** `connect` (no MWA-compatible wallet detected on the device).
- **What to do:** present `MWA.get_suggested_wallet_install_links()` UI in-game — the SDK ships a 3-wallet reference list (Phantom / Solflare / Backpack). Per [`docs/architecture.md`](./architecture.md) §5.3.

## WALLET_UNINSTALLED

- **Recoverable:** `true`
- **Retry hint:** `connect`
- **Default user message:** *"The previously connected wallet is no longer installed."*
- **Source methods:** `reauthorize`, `sign_messages`, `sign_transactions`, `sign_and_send` (the wallet that issued the cached auth_token is no longer on the device).
- **What to do:** clear the cached session (`MWA.disconnect()` or `MWA.deauthorize()`) and prompt the user to reconnect via `MWA.connect()`.

## TOKEN_EXPIRED

- **Recoverable:** `true`
- **Retry hint:** `connect`
- **Default user message:** *"Your wallet session has expired. Please reconnect."*
- **Source methods:** `reauthorize` (the cached token was rejected by the wallet — `AC-3` graceful wipe). Also possible from `sign_*` ops if the wallet revokes mid-op.
- **What to do:** the SDK auto-wipes the cached token on this code; caller invokes `MWA.connect()`. **Do NOT auto-launch the wallet chooser** without user intent. Per [`docs/architecture.md`](./architecture.md) §5.3.

## RPC_FAILED

- **Recoverable:** `true`
- **Retry hint:** `retry_now`
- **Default user message:** *"The Solana network rejected the transaction."*
- **Source methods:** `sign_and_send` (the wallet forwarded the transaction to the RPC endpoint and the cluster rejected it — could be insufficient funds, invalid blockhash, or program-level rejection).
- **What to do:** inspect `developer_details` for the cluster's error message; rebuild the transaction and retry. Caller surfaces the user_message verbatim.

## NETWORK_OFFLINE

- **Recoverable:** `true`
- **Retry hint:** `retry_now`
- **Default user message:** *"No internet connection."*
- **Source methods:** `sign_and_send` (wallet cannot reach the RPC endpoint), and indirectly any op if the wallet's MWA association call fails offline.
- **What to do:** prompt the user to verify connectivity and retry.

## TIMEOUT

- **Recoverable:** `true`
- **Retry hint:** `retry_now`
- **Default user message:** *"The operation timed out."*
- **Source methods:** any op subject to the watchdog (`connect`, `reauthorize`, `sign_messages`, `sign_transactions`, `sign_and_send`).
- **What to do:** caller retries or shows "wallet not responding". Note: watchdog expiry typically fires as `mwa_timeout(payload)` — a separate signal — with a 4-key payload `{request_id, source_method, elapsed_ms, watchdog_ms}`. The `TIMEOUT` code on `mwa_error` is the canonical fallback when an op upstream of the watchdog reports a timeout. Per [`docs/architecture.md`](./architecture.md) §5.3.

## NOT_CONNECTED

- **Recoverable:** `true`
- **Retry hint:** `connect`
- **Default user message:** *"Not connected to a wallet. Call MWA.connect() first."*
- **Source methods:** `disconnect`, `reauthorize` (no cached token present, or cluster mismatch — `AC-2` / `AC-4`), `sign_messages`, `sign_transactions`, `sign_and_send` (synchronous preflight per `DD-3-1-6`). Also the cluster-bleed defensive guard on `sign_and_send` per `DD-27` / `AC-NFR-SEC-4` / `AC-6`.
- **What to do:** call `MWA.connect(identity, cluster)` and await `connect_completed` before retrying the failed op.

## UNSUPPORTED_PLATFORM

- **Recoverable:** `false`
- **Retry hint:** `none`
- **Default user message:** *"MWA is only available on Android."*
- **Source methods:** every op-method on non-Android builds (desktop / web / iOS / editor). Routed through the NoOp bridge per Story 5-1's `DD-A-4` LOCKED single-platform-abstraction TU.
- **What to do:** caller renders feature-gated UI (e.g., hide the wallet panel; show a desktop fallback). Use `MWA.is_supported()` as the synchronous preflight rather than discovering this via an error. Per [`docs/architecture.md`](./architecture.md) §5.3.

## PROTOCOL_ERROR

- **Recoverable:** `false`
- **Retry hint:** `connect`
- **Default user message:** *"Wallet communication failed."*
- **Source methods:** any op (the catch-all for wire-protocol failures — malformed JSON, unexpected wallet responses, JNI-layer exceptions). Also fires on `deauthorize` for catastrophic local-wipe failures (per `DD-4-1-3`).
- **What to do:** inspect `developer_details` for the upstream cause; the recommended path is a clean reconnect (`MWA.disconnect()` → `MWA.connect()`).

## STORAGE_CORRUPT

- **Recoverable:** `true`
- **Retry hint:** `connect`
- **Default user message:** *"Secure storage is unavailable. Please reconnect."*
- **Source methods:** any op that touches the encrypted token store (most often surfaces on `connect` / `reauthorize` / `sign_and_send` breadcrumb writes).
- **What to do:** the SDK auto-wipes the corrupt store and emits `reauth_required{reason: "keystore_corrupt"}`; the caller invokes `MWA.connect()` to mint a fresh token. Per [`docs/architecture.md`](./architecture.md) §5.3.

## REAUTH_REQUIRED

- **Recoverable:** `true`
- **Retry hint:** `connect`
- **Default user message:** *"Your session needs renewal."*
- **Source methods:** ops that require a valid session but find one missing/invalid in a way distinct from `TOKEN_EXPIRED` — typically `sign_and_send` after a Tink corruption event (per `DD-3-3-G` fail-closed wrapper).
- **What to do:** prompt the user to reconnect. Same recovery class as `TOKEN_EXPIRED`. Per [`docs/architecture.md`](./architecture.md) §5.3.

## See also

- [`docs/architecture.md`](./architecture.md) §3.3 (lines 316-409) — the YAML manifest that drives this reference.
- [`docs/architecture.md`](./architecture.md) §3.2 (lines 218-298) — the full 10-key `mwa_error` payload schema.
- [`docs/architecture.md`](./architecture.md) §5.3 (lines 632-640) — the 6-row recovery table.
- [`docs/api-reference.md`](./api-reference.md) — per-method list of which codes each op can emit.
- [`docs/triage.md`](./triage.md) — runbook for "my MWA call is failing, what now?"
- [`docs/safety.md`](./safety.md) — security-sensitive considerations for token-related codes.
