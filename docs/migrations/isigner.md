---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# Migrating to `ISigner` (v1.0 → v1.1 → v1.2)

This guide walks `godot-solana-sdk` consumers through the new `ISigner` abstraction that landed in v1.1. It explains what changed, shows before/after code for every signer flavor, lays out the deprecation timeline, and answers the questions most consumers will hit.

If you skim only one section, read the **Deprecation timeline** table — it tells you exactly when you need to act.

---

## 1. What changed

In v1.0, `Transaction.sign()` accepted three signer flavors with three different code paths:

- a `Keypair` (synchronous, in-process signing — desktop, server, tools)
- a `WalletAdapter` (async, browser-wallet bridging via JavaScript)
- nothing else

In v1.1, the SDK introduces a single, abstract C++ interface — `godot_solana_sdk::ISigner` — added to `godot-solana-sdk` core. Every signer flavor now implements this interface, and `Transaction` dispatches through it as the primary signing path:

| Signer flavor | Implements `ISigner` via | Status in v1.1 |
|---|---|---|
| `Keypair` (raw, legacy usage) | wrapped automatically by `Transaction` in a `LocalKeypairSigner` | works + emits a `push_warning` once per process |
| `LocalKeypairSigner` (explicit wrapper around `Keypair`) | implements `ISigner` directly | recommended — no warning |
| `WalletAdapter` (raw, legacy usage) | dispatched via the legacy direct branch | works unchanged, no warning |
| `WalletAdapterSigner` (explicit bridge over `WalletAdapter`) | implements `ISigner` directly | available; see Q4 in the FAQ |
| `MobileWalletAdapter` (Android MWA, Story 2-1) | implements `ISigner` directly | not yet shipped — see roadmap |

The `Transaction` node itself no longer has a hard dependency on any one signer concrete type. Its `signers` array still accepts `Variant`s for backward compatibility (so GDScript callers don't need to touch their existing code today), but the dispatch is unified: every signer that exposes the `ISigner` shape — whether passed directly or wrapped for the legacy-Keypair path — flows through the same `_isigner_signed` / `_isigner_failed` handler pair.

This is the architecture seam that makes `MobileWalletAdapter` (Android Mobile Wallet Adapter) shippable in Story 2-1 without forcing `godot-solana-sdk` core to depend on the MWA addon.

---

## 2. Migration diff

### Keypair signing

```gdscript
# BEFORE (v1.0 — still works in v1.1 with deprecation warning)
var kp := Keypair.new_random()
tx.set_signers([kp])
tx.sign()

# AFTER (v1.1 — recommended; required in v1.2)
var kp := Keypair.new_random()
var signer := LocalKeypairSigner.new()
signer.set_keypair(kp)
tx.set_signers([signer])
tx.sign()
```

The behavior is identical: `tx.signatures[0]` is populated by the time `tx.sign()` returns (the `LocalKeypairSigner` emits its signal synchronously to preserve this guarantee). The only visible difference is that the deprecation warning stops firing.

### WalletAdapter signing (browser wallets)

```gdscript
# BEFORE (v1.0 — still works in v1.1, NO deprecation warning)
var wa: WalletAdapter = $WalletAdapter
wa.connect_wallet()
await wa.connection_established
tx.set_signers([wa])
tx.sign()

# OPTIONAL (v1.1) — wrap explicitly for the unified ISigner contract
var wa: WalletAdapter = $WalletAdapter
wa.connect_wallet()
await wa.connection_established
var signer := WalletAdapterSigner.new()
signer.set_wallet_adapter(wa)
tx.set_signers([signer])
tx.sign()
```

The legacy direct path is **not** deprecated in v1.1. See FAQ Q4 for when to use the explicit `WalletAdapterSigner` wrapper and when to stay with the direct path.

### MobileWalletAdapter signing (Android — forward-looking)

`MobileWalletAdapter` ships in Story 2-1, not Story 1-3. When it lands, the usage will look like:

```gdscript
# v1.1+ (post Story 2-1)
var mwa := MobileWalletAdapter.new()
mwa.connect({"name": "MyDApp", "icon_uri": "...", "identity_uri": "..."}, "devnet")
await mwa.connect_completed
tx.set_signers([mwa])  # MobileWalletAdapter implements ISigner directly
tx.sign()
```

No bridge or wrapper is needed — `MobileWalletAdapter` implements `ISigner` natively.

---

## 3. Deprecation timeline

| Version | Behavior of `tx.set_signers([keypair]); tx.sign()` |
|---|---|
| **v1.0** (pre-MWA) | Works silently. No warning. |
| **v1.1** (this story ships) | Works. Emits `push_warning("Transaction.sign(Keypair) is deprecated; use ISigner. See docs/migrations/isigner.md. Removed in v1.2.")` exactly once per process. The warning string is the **canonical** AC-3 text — `src/transaction/transaction.cpp` reproduces it verbatim and this doc reproduces it verbatim. If one is ever changed, the other must change in lockstep (consumers grep the warning text against this doc to find the migration path). |
| **v1.2** (MWA stable) | Returns `Error::ERR_METHOD_NOT_FOUND`. Prints `"Transaction.sign(Keypair) was removed in v1.2; use an ISigner. See docs/migrations/isigner.md."` Gated behind the `MWA_ISIGNER_REMOVE_V1_2` compile define until the v1.2 release flips it on by default. |

The compile-define gate (added in Story 1-3 Task 7) means projects that need extra runway can stay on the v1.1 deprecation behavior even after v1.2 ships. Once your codebase is migrated, the gate is invisible — your build defaults to the modern path.

The `WalletAdapter`-direct path follows a separate (currently undefined) timeline. It is **not deprecated in v1.1** and has no removal target. A future story will introduce its own warning + removal cycle if the migration to `WalletAdapterSigner` becomes the recommended pattern.

---

## 4. FAQ

### Q1. I just use `Keypair`s. What do I do?

Wrap them in `LocalKeypairSigner` and pass the wrapper into `set_signers`:

```gdscript
var signer := LocalKeypairSigner.new()
signer.set_keypair(kp)
tx.set_signers([signer])
```

The wrapper's lifetime is bound to your local variable. If you reuse the same `Keypair` across many `Transaction`s, you can reuse the same `LocalKeypairSigner` instance — it has no per-transaction state.

The synchronous-emit invariant is preserved: `tx.signatures[0]` is populated by the time `tx.sign()` returns. Your existing code that reads `tx.signatures[0]` immediately after `tx.sign()` keeps working.

### Q2. I use `WalletAdapter` for browser wallets. What do I do?

Nothing, in v1.1. The legacy direct path — `tx.set_signers([wa]); tx.sign()` — keeps working with no deprecation warning. You can optionally adopt `WalletAdapterSigner` if you want the unified `ISigner` contract for code that should be signer-flavor-agnostic, but you don't have to.

See **Q4** for one important caveat about the `WalletAdapterSigner` wrapper in v1.1.

### Q3. I want to support Android via MWA. What do I do?

Wait for Story 2-1 to ship the `MobileWalletAdapter` class, then use it like any other `ISigner`:

```gdscript
var mwa := MobileWalletAdapter.new()
# ... connect, await, etc. ...
tx.set_signers([mwa])
tx.sign()
```

The `ISigner` abstraction landed in this story (1-3) precisely so Story 2-1 can drop `MobileWalletAdapter` into the existing `set_signers` path with no changes to `Transaction`.

### Q4. Should I switch to `WalletAdapterSigner` now?

Probably not yet. The explicit wrapper exists in v1.1 to fulfill the architectural contract that "every signer flavor implements `ISigner`," but it has one v1.1 limitation worth knowing:

When a browser wallet **modifies your transaction** during signing (some wallets adjust compute budget or priority fees on your behalf), the legacy `WalletAdapter`-direct path detects the modification via `WalletAdapter::did_transaction_change()` and re-deserializes the modified bytes before splicing in the signature. The current `WalletAdapterSigner` wrapper does **not** propagate that modification back to the `Transaction`. If you sign through the wrapper and the wallet modified the transaction, you will produce a signature for the wallet's modified bytes against an unmodified `Transaction.message`, and submission will fail on-chain with an `InvalidSignature` error.

For v1.1, the safe choice is to keep using the direct `tx.set_signers([wa])` path for browser wallets that may modify transactions. The wrapper is fine for use cases that don't depend on the modification-detect semantics (most simple sign-and-send flows do not). A follow-up story will reconcile this — see `docs/concerns.md` (CR-15) for the tracked decision.

### Q5. Where do my signatures live now? Same place?

Yes — `tx.signatures` is unchanged. The dispatch path inside `Transaction::sign_at_index` changed, but the public surface (`signatures`, `set_signers`, `sign`, `partially_sign`, `signer_state_changed`, `fully_signed`, `signing_failed`) is the same. Existing code that reads `tx.signatures[i]` after a sign completes continues to work without modification.

### Q6. What happens if I mix signer flavors in the same array?

It works. `set_signers([keypair, wallet_adapter, local_keypair_signer])` is fine; each entry is dispatched through its appropriate branch. The deprecation warning fires once per process — not once per entry — the first time any `Keypair` is dispatched via the legacy path.

### Q7. Is there a way to silence the deprecation warning without migrating?

No (intentionally). The warning is the migration prompt. If you need the warning suppressed for CI noise, migrate to `LocalKeypairSigner` — that is the v1.1 way to drop the warning.

---

## 5. Reference

- Architecture: `docs/architecture.md` §2.3.1 (`ISigner` contract), AC-A-5 (no reverse dependency from core → MWA), DD-11/DD-25 (signer-abstraction decisions), DD-22 (call_deferred scope)
- Amendment: `docs/amendments.md` A-11 (`GDCLASS_CUSTOM` for `ISigner` — codebase-wide registration consistency)
- Tracked concerns: `docs/concerns.md` CR-14 (`MwaErrorCode` reuse for non-MWA `LocalKeypairSigner` — v1.2 cleanup), CR-15 (`WalletAdapter::did_transaction_change` not propagated through `WalletAdapterSigner` — see Q4 above), CR-16 (`sign_transactions` payload convention not yet aligned with MWA — Story 2-1 design), CR-17 (`isigner_request_id_to_index_` HashMap unbounded growth on hung signers)
- Story: `docs/stories/1-3.md` (this migration's source of truth)
- Source files added in v1.1:
  - `src/isigner.hpp` + `src/isigner.cpp` — abstract interface
  - `include/local_keypair_signer.hpp` + `src/local_keypair_signer.cpp` — `Keypair` compat wrapper
  - `include/wallet_adapter/wallet_adapter_signer.hpp` + `src/wallet_adapter/wallet_adapter_signer.cpp` — `WalletAdapter` bridge

  > **Layout note.** Two header conventions appear in the list above. `src/isigner.hpp` lives in `src/` because the LOCKED §2.3.1 architecture contract pins that path verbatim (the contract is the source of truth for the interface; moving the header would be an architecture amendment). `LocalKeypairSigner` and `WalletAdapterSigner` headers live under `include/` because that is the established convention for this codebase (`include/keypair.hpp`, `include/wallet_adapter/wallet_adapter.hpp`, etc.). Both `src/` and `include/` are on `CPPPATH` (`SConstruct` lines 270-272), so `#include "isigner.hpp"` and `#include "local_keypair_signer.hpp"` both resolve regardless of which directory the header lives in. Consumers writing C++ extensions should not need to care about the split.
