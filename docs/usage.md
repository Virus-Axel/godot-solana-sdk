---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# Usage Guide

This guide is for Godot game developers integrating MWA into a game **outside** the bundled example. For the example app's design constraints (PASS markers, mainnet gate, button accessibility), see [`REVIEW.md`](../REVIEW.md) at the repo root — that document is the canonical home for example-app patterns per [`docs/amendments.md`](./amendments.md) `A-17`. This guide describes the patterns; REVIEW.md hosts the reference implementation.

## Prerequisites

You have completed the steps in [`docs/install.md`](./install.md) and the `MWA` autoload is available from any GDScript.

## Hello, MWA — connect and disconnect

The minimum viable flow: connect to a wallet, confirm the connection, disconnect.

```gdscript
extends Node

func _ready() -> void:
    if not MWA.is_supported():
        print("MWA only runs on Android; this build is unsupported")
        return
    MWA.connect_completed.connect(_on_connect_completed)
    MWA.mwa_error.connect(_on_mwa_error)
    var identity := {
        "name": "My Game",
        "icon_uri": "",
        "identity_uri": "https://my-game.example.com",
    }
    MWA.connect(identity, "devnet")

func _on_connect_completed(_request_id: String, result: Dictionary) -> void:
    print("Connected to ", result["wallet_label"], " on ", result["cluster"])

func _on_mwa_error(payload: Dictionary) -> void:
    push_error("MWA error: %s — %s" % [payload["code"], payload["user_message"]])
```

That is the full happy-path shape: an op method (`MWA.connect`) returns immediately; the result arrives later via a signal.

## The await + signal pattern

MWA op methods are non-blocking. They return within ≤16ms (per `AC-NFR-1` and `AC-A-1` in [`docs/spec.md`](./spec.md)) and complete asynchronously by emitting a terminal signal — one of `<op>_completed`, `mwa_error`, `mwa_timeout`, or `mwa_cancelled_lifecycle`. Per `AC-FR-12` in [`docs/spec.md`](./spec.md), this is the universal pattern across every op.

The simplest way to consume an op as if it were synchronous is the **await + race** pattern: await whichever of the success / error signals fires first, then react. Godot 4.3 does not ship a built-in `Signal.race()`, so the pattern is implemented as a small helper.

The example repo ships a reference implementation of this helper at `<example-repo>/scripts/await_helpers.gd`, signature `static func _race_signals(success_sig: Signal, error_sig: Signal) -> Dictionary`. See [`REVIEW.md`](../REVIEW.md) Design Guidance section — that file is the canonical home for the helper. Below is the same shape adapted into a single-file game:

```gdscript
# Inline a minimal race helper. The example repo's _race_signals at
# <example-repo>/scripts/await_helpers.gd is the canonical reference impl;
# see REVIEW.md Design Guidance for the full design rationale.
func _await_op(success_sig: Signal, error_sig: Signal) -> Dictionary:
    var success_received := [false, null]
    var error_received := [false, null]
    var success_cb := func(_rid: String, result: Dictionary):
        success_received[0] = true
        success_received[1] = result
    var error_cb := func(payload: Dictionary):
        error_received[0] = true
        error_received[1] = payload
    success_sig.connect(success_cb, CONNECT_ONE_SHOT)
    error_sig.connect(error_cb, CONNECT_ONE_SHOT)
    while not success_received[0] and not error_received[0]:
        await get_tree().process_frame
    if success_received[0]:
        return {"ok": true, "value": success_received[1]}
    return {"ok": false, "error": error_received[1]}
```

Use it like this:

```gdscript
func sign_a_message() -> void:
    MWA.sign_messages([PackedByteArray("hello".to_utf8_buffer())])
    var outcome := await _await_op(MWA.sign_messages_completed, MWA.mwa_error)
    if outcome["ok"]:
        var signed: Array = outcome["value"]["signed_payloads"]
        print("got %d signed payload(s)" % signed.size())
    else:
        push_error("sign_messages failed: %s" % outcome["error"]["user_message"])
```

The race pattern is correct as long as you connect both signals **before** invoking the op (otherwise an instant error response — e.g., `NOT_CONNECTED` — could fire before the listener is attached). The example repo's `_race_signals` enforces this ordering. See [`docs/signals-101.md`](./signals-101.md) for a deeper explainer of Godot's signal model.

## Cluster selection

`MWA.connect(identity, cluster, opts={})` takes a cluster string. Three values are supported:

| Cluster | When to use |
|---|---|
| `"devnet"` | Default for development. Free SOL via faucet; does not affect real funds. |
| `"testnet"` | Validator-development network; rarely needed for game devs. |
| `"mainnet-beta"` | Real money. Treat every `mainnet-beta` connect as a security event. |

Per `AC-NFR-SEC-10` in [`docs/spec.md`](./spec.md), production code MUST default to `devnet` and require an explicit attention-checkpoint to switch to `mainnet-beta`. The example repo demonstrates the canonical pattern: a `const` edit + an in-app `ConfirmationDialog` whose body text reads `"I understand — use mainnet"`, confirmed **per app launch** (not persisted to `user://`). See [`REVIEW.md`](../REVIEW.md) Design Guidance section for the mainnet-gate semantics — it is a security control, not a UX nicety.

## Identity metadata

The `identity` dictionary passed to `MWA.connect(identity, cluster, opts)` is presented to the wallet as part of the signing prompt:

```gdscript
var identity := {
    "name": "My Game",                          # human-readable, shown to user
    "icon_uri": "https://my-game.example.com/icon.png",
    "identity_uri": "https://my-game.example.com",
}
```

`identity_uri` is the user-visible domain in the wallet's signing prompt. Per `AC-NFR-SEC-12` and the security-auditor concern list at [`docs/spec.md`](./spec.md), this field is **attacker-controllable from the dApp side** — the wallet's UI is the trust anchor, but a malicious dApp can supply a misleading `identity_uri`. The example repo uses the placeholder `"https://example.com/godot-mwa-sample"` intentionally; production code MUST use a real, HTTPS identity URI matching the game's actual domain. See [`docs/safety.md`](./safety.md) for the full identity-metadata threat model.

## Sign-and-send recovery

`MWA.sign_and_send(transactions, opts)` writes a breadcrumb to encrypted storage **before** the wallet round-trip starts (per Story 3-3 `DD-3-3-A/B/F`). If the process is killed mid-op, the breadcrumb survives and the next successful `connect` or `reauthorize` emits a one-shot `pending_submission_found` signal so your game can recover.

Subscribe **once** in `_ready()` with `CONNECT_ONE_SHOT`:

```gdscript
func _ready() -> void:
    MWA.pending_submission_found.connect(_on_pending_submission_found, CONNECT_ONE_SHOT)

func _on_pending_submission_found(payload: Dictionary) -> void:
    # 6-key payload: {request_id, op_type, started_at_ms, tx_count,
    # tx_preview_hashes, recommendation: "check_chain_for_signatures"}
    var hashes: Array = payload["tx_preview_hashes"]
    print("recovered pending submission, %d hash(es) to verify" % hashes.size())
    # Query the chain for these hashes via your existing SolanaClient.
    # Resubmit only if NONE of them are present on-chain.
```

Per [`docs/safety.md`](./safety.md), `sign_and_send` is **at-least-once** from the caller's perspective: the SDK cannot autonomously confirm on-chain landing without an RPC config. Treat the breadcrumb as authoritative for crash recovery; the caller queries `getSignatureStatuses` to determine final state.

## Cancel an in-flight op

There is no per-op `cancel()` method. The supported cancel-in-flight path is `MWA.forget_all()` — it emits `mwa_cancelled_lifecycle{reason: "forget_all_invoked"}` for every in-flight slot, then performs the GDPR/CCPA local wipe + best-effort remote deauthorize. See [`docs/api-reference.md`](./api-reference.md) for the full `forget_all` contract.

Lifecycle cancellation (the user rotates the screen mid-op or backgrounds the activity) is observed automatically and emits `mwa_cancelled_lifecycle{reason: "activity_destroyed"}`. Your game retries on the next user interaction.

## Error handling

Every error routes through one signal: `mwa_error(payload: Dictionary)`. The 10-key payload carries `code`, `user_message`, `developer_details`, `recoverable`, `retry_hint`, `layer`, `cause`, `source_method`, `request_id`, and `message` (per [`docs/amendments.md`](./amendments.md) `A-14`). Common patterns:

```gdscript
func _on_mwa_error(payload: Dictionary) -> void:
    var code: String = payload["code"]
    var user_message: String = payload["user_message"]
    match code:
        "USER_CANCELED":
            # User dismissed the wallet prompt — show a friendly message
            show_toast("Cancelled.")
        "NO_MWA_WALLET_INSTALLED":
            # Present install links via MWA.get_suggested_wallet_install_links()
            show_install_prompt(MWA.get_suggested_wallet_install_links())
        "TOKEN_EXPIRED", "REAUTH_REQUIRED":
            # Caller invokes connect() — do NOT auto-launch the wallet chooser
            await_user_action_then_reconnect()
        "UNSUPPORTED_PLATFORM":
            # Desktop / editor build — gate the feature in UI
            hide_wallet_buttons()
        _:
            # Surface the user_message verbatim — it is safe for in-game display
            show_toast(user_message)
```

For the full 13-code taxonomy and recovery patterns, see [`docs/error-codes.md`](./error-codes.md). For triage workflow when something goes wrong, see [`docs/triage.md`](./triage.md). For the signal model fundamentals, see [`docs/signals-101.md`](./signals-101.md).

## Next steps

- [`docs/api-reference.md`](./api-reference.md) — per-method reference with snippets.
- [`docs/error-codes.md`](./error-codes.md) — the 13-code taxonomy.
- [`docs/safety.md`](./safety.md) — security guidance before shipping mainnet.
- [`docs/accessibility.md`](./accessibility.md) — keyboard, screen-reader, contrast.
