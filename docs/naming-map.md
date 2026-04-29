---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# Naming Map

This document is the canonical RN ↔ GDScript ↔ C++ ↔ Kotlin method-name map
for the 7 MWA op-method families (AC-D-9). Story 5-6's CI parses public APIs
across the three layers and fails the build if any of the four name columns
diverges from the row below without a matching update to this document.

The row count (7) matches `docs/rn-parity.md`'s `methods` array length and
is fixed: each row corresponds to exactly one of the 7 MWA op-method
stories (Stories 2-1 / 2-2 / 2-3 / 3-1 / 3-2 / 3-3 / 4-1) shipped through
Wave 6. `MWA.forget_all()` (Story 4-2) is intentionally excluded — it is a
godot-mwa-native GDPR/CCPA deletion entrypoint per `docs/spec.md`
AC-NFR-SEC-11 and has no RN/clientlib-ktx counterpart. See
`docs/api-reference.md` for the full method documentation and
`docs/api-stability.md` for the breaking-change semantics that govern any
edit to this map.

## Layer mapping

The four name columns correspond to the four containers in
`docs/architecture.md` §2.2 (lines 71-85): the GDScript surface lives in
container A (`MWA.gd`), the C++ binding in container B
(`MobileWalletAdapter` GDExtension node), and the Kotlin `@UsedByGodot`
methods in container E (`MwaAndroidPlugin`). RN naming follows
`solana-mobile/mobile-wallet-adapter-clientlib-ktx` 2.0.3 (the
`MobileWalletAdapterClient.kt` API surface).

## The 7 op rows

| RN method (clientlib-ktx 2.0.3) | GDScript (`MWA.gd`) | C++ (`MobileWalletAdapter`) | Kotlin `@UsedByGodot` (`MwaAndroidPlugin`) | Success signal | Error / extra signals |
|---|---|---|---|---|---|
| `authorize` | `connect(identity, cluster, opts)` (`MWA.gd:133`) | `MobileWalletAdapter::connect` (declared in `include/mwa/mobile_wallet_adapter.hpp`) | `mwaAuthorize` (Kotlin plugin line 505) | `connect_completed(request_id, result)` | `mwa_error`, `mwa_timeout`, `mwa_cancelled_lifecycle` |
| `<transport-only>` (no named API method; WebSocket teardown) | `disconnect()` (`MWA.gd:141`) | `MobileWalletAdapter::disconnect` | `mwaDisconnect` (Kotlin plugin line 597) | `disconnect_completed(request_id, result)` | `mwa_error` |
| `reauthorize` | `reauthorize(opts)` (`MWA.gd:184`) | `MobileWalletAdapter::reauthorize` | `mwaReauthorize` (Kotlin plugin line 694) | `reauthorize_completed(request_id, result)` | `mwa_error`, `mwa_timeout`, `reauth_required` |
| `deauthorize` | `deauthorize()` (`MWA.gd:168`) | `MobileWalletAdapter::deauthorize` | `mwaDeauthorize` (Kotlin plugin line 1286) | `deauthorize_completed(request_id, result)` | `mwa_error` |
| `signMessages` | `sign_messages(messages, opts)` (`MWA.gd:213`) | `MobileWalletAdapter::sign_messages` | `mwaSignMessages` (Kotlin plugin line 1072) | `sign_messages_completed(request_id, result)` | `mwa_error`, `mwa_timeout`, `reauth_required` |
| `signTransactions` | `sign_transactions(transactions, opts)` (`MWA.gd:246`) | `MobileWalletAdapter::sign_transactions` | `mwaSignTransactions` (Kotlin plugin line 1121) | `sign_transactions_completed(request_id, result)` | `mwa_error`, `mwa_timeout`, `reauth_required` |
| `signAndSendTransactions` | `sign_and_send(transactions, opts)` (`MWA.gd:300`) | `MobileWalletAdapter::sign_and_send` | `mwaSignAndSendTransactions` (Kotlin plugin line 1183) | `sign_and_send_completed(request_id, result)` | `mwa_error`, `mwa_timeout`, `reauth_required`, `pending_submission_found` (one-shot, fired on next `connect`/`reauthorize` per Story 3-3 DD-3-3-D) |

## Notes on the `disconnect` row

`disconnect` has no named RN API method in clientlib-ktx 2.0.3 — the
upstream protocol terminates the local association by closing the WebSocket
transport rather than invoking a named JSON-RPC method. The
`<transport-only>` marker in the RN column reflects that asymmetry. The
GDScript / C++ / Kotlin layers still expose a named `disconnect`
op-method because the godot-mwa-android SDK guarantees a uniform
"every op emits a `*_completed` signal with a `request_id`" contract
(AC-NFR-1, see `docs/architecture.md` §3.2). The CR-5-5-B concern
(filed in Story 5-5) tracks the open question of whether clientlib-ktx
2.0.3 exposes a higher-level `disconnect` helper that would let us replace
the `<transport-only>` marker with a real RN method name; do not file a
new CR.

## Signal-payload anatomy

All 7 success signals emit a 2-arg payload `(request_id: String, result:
Dictionary)` per Amendment A-12 and `docs/architecture.md` §3.2
(lines 218-230) "All signals emit a single `Dictionary` argument
(AC-D-10 — additive keys only)" — the `request_id` is hoisted to a
positional parameter and the remaining keys live inside `result`. See
`docs/api-reference.md` for the per-method `result` Dictionary schema and
`docs/signals-101.md` for the request_id correlation pattern.

## Drift-check contract

The Story 5-6 AC-D-9 workflow parses the 7-row table above and validates,
for each row, that:

1. The GDScript symbol exists in `addons/SolanaSDK/mwa/MWA.gd`.
2. The C++ method is declared in `include/mwa/mobile_wallet_adapter.hpp`.
3. The Kotlin `@UsedByGodot` method exists in
   `android/plugin/src/main/java/plugin/walletadapterandroid/GDExtensionAndroidPlugin.kt`.
4. The success-signal name appears in `MWA.gd`'s signal block (lines 27-47).

The `request_id` formatting (8-hex per Story 1-5 D-4) is enforced by a
separate workflow and is out of scope for this drift check.

## Editing this file

Edits to the table above are inherently breaking changes per
`docs/api-stability.md` (method removal / signal removal). Add a new column
or row only via the additive-keys process documented there. Removing a
column or row requires the full mark → warn-one-minor → remove deprecation
ladder per `docs/architecture.md` DD-25.

## Cross-references

- AC-D-9 in `docs/spec.md` line 160.
- `docs/architecture.md` §2.2 (Container View) for the four-container
  origin of the columns; §3.2 for signal payload anatomy.
- `docs/amendments.md` A-12 for the 2-arg `*_completed` signal shape.
- `docs/rn-parity.md` for the machine-readable RN-parity manifest (same 7
  rows, YAML-encoded for the weekly drift cron).
- `docs/api-reference.md` for per-method semantics and `result` Dictionary
  schemas.
