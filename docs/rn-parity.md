---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# RN Parity Manifest

This document is the machine-readable RN-parity manifest for the
godot-mwa-android SDK (AC-D-3, AC-NFR-9). The YAML block below is consumed
by `tools/validate_rn_parity.py` (Story 5-5 T2 deliverable) and by the
Story 5-6 `verify_parity.yml` weekly cron, which diffs the
`mobile-wallet-adapter-clientlib-ktx` package's published API surface
against the `methods` array and opens a PR or issue when upstream adds or
renames a method.

The schema is locked per design decision DD-5-5-4 (see Story 5-5 file):
top-level `schema_version: 1`, top-level `pinned_clientlib_ktx_version` as
a string, and a `methods` array of exactly 7 entries (one per Wave-6 op
story). Each entry has the 6 keys `rn_method`, `rn_version`,
`gdscript_method`, `kotlin_method`, `cpp_method`, `signal_names`. Adding a
key is a schema_version bump per `docs/api-stability.md`.

`MWA.forget_all()` is intentionally NOT in the `methods` array — it is a
godot-mwa-native GDPR/CCPA deletion entrypoint per AC-NFR-SEC-11 and has no
counterpart in clientlib-ktx 2.0.3. If a future clientlib-ktx version
adds an equivalent method, file an amendment proposing an 8th entry rather
than silently extending the array.

## Schema

```yaml
schema_version: 1
pinned_clientlib_ktx_version: "2.0.3"
methods:
  - rn_method: authorize
    rn_version: "2.0.3"
    gdscript_method: MWA.connect
    kotlin_method: mwaAuthorize
    cpp_method: MobileWalletAdapter::connect
    signal_names: [connect_completed, mwa_error]
  - rn_method: "<transport-only>"
    rn_version: "2.0.3"
    gdscript_method: MWA.disconnect
    kotlin_method: mwaDisconnect
    cpp_method: MobileWalletAdapter::disconnect
    signal_names: [disconnect_completed, mwa_error]
  - rn_method: reauthorize
    rn_version: "2.0.3"
    gdscript_method: MWA.reauthorize
    kotlin_method: mwaReauthorize
    cpp_method: MobileWalletAdapter::reauthorize
    signal_names: [reauthorize_completed, mwa_error, reauth_required]
  - rn_method: deauthorize
    rn_version: "2.0.3"
    gdscript_method: MWA.deauthorize
    kotlin_method: mwaDeauthorize
    cpp_method: MobileWalletAdapter::deauthorize
    signal_names: [deauthorize_completed, mwa_error]
  - rn_method: signMessages
    rn_version: "2.0.3"
    gdscript_method: MWA.sign_messages
    kotlin_method: mwaSignMessages
    cpp_method: MobileWalletAdapter::sign_messages
    signal_names: [sign_messages_completed, mwa_error, reauth_required]
  - rn_method: signTransactions
    rn_version: "2.0.3"
    gdscript_method: MWA.sign_transactions
    kotlin_method: mwaSignTransactions
    cpp_method: MobileWalletAdapter::sign_transactions
    signal_names: [sign_transactions_completed, mwa_error, reauth_required]
  - rn_method: signAndSendTransactions
    rn_version: "2.0.3"
    gdscript_method: MWA.sign_and_send
    kotlin_method: mwaSignAndSendTransactions
    cpp_method: MobileWalletAdapter::sign_and_send
    signal_names: [sign_and_send_completed, mwa_error, reauth_required, pending_submission_found]
```

## Notes

- The `disconnect` row uses `"<transport-only>"` as its `rn_method` because
  clientlib-ktx 2.0.3 closes the WebSocket transport rather than invoking
  a named JSON-RPC method. CR-5-5-B is on file to track whether a
  higher-level RN helper exists; the `<transport-only>` marker is the
  documented placeholder per `docs/naming-map.md`'s "Notes on the
  `disconnect` row" section. Do not file a new CR for this.
- `pinned_clientlib_ktx_version` mirrors the
  `mobile-wallet-adapter-clientlib-ktx` row in
  `docs/compatibility-matrix.md`. Bumping requires updating BOTH this
  document AND the matrix in the same PR.

## Cross-references

- AC-D-3 in `docs/spec.md` line 154 — RN-parity drift CI contract.
- AC-NFR-9 in `docs/spec.md` line 176 — "7/7 methods mapped".
- `docs/naming-map.md` — human-readable counterpart with C++ + signal columns.
- `docs/api-stability.md` — adding a method or schema-version bump rules.
- `tools/validate_rn_parity.py` — schema validator (exits 0 on conform).
