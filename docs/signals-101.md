---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# Signals 101 — the Godot signal model in MWA

This document is for developers new to Godot's signal-based async model. It explains **why** MWA uses signals (not callbacks, not promises) and how to consume them safely. For the user-facing snippet of the await + race pattern, read [`./usage.md`](./usage.md) first; this doc explains the WHY behind that pattern.

## 1. Why MWA uses signals

Two hard constraints drive the choice:

- **Non-blocking method-return** — every MWA op method returns within ≤16 ms (`AC-NFR-1`, [`./spec.md`](./spec.md)). The wallet round-trip can take seconds (the user reads the prompt, taps approve); blocking the GDScript caller would freeze the game's render loop. So op methods return immediately and complete asynchronously.
- **Results on the Godot main thread** — every result is delivered on the Godot main thread (`AC-A-1`, [`./spec.md`](./spec.md)). This means consumer code can mutate scene-tree state from the signal handler without thread-safety scaffolding. The SDK marshals the cross-thread hop internally via a single documented primitive (MPSC inbox on `_process` OR `call_deferred`).

Signals are Godot's idiomatic async-result delivery mechanism. Compared to callback-style APIs, signals decouple the producer (the SDK) from the number of consumers — multiple game systems can subscribe to `MWA.connect_completed` independently. Compared to promise-style APIs, signals fit naturally into Godot's `await` keyword and the `_process` lifecycle.

## 2. Signal payload anatomy

Every MWA signal emits a **single `Dictionary` argument** with the **exception** of the seven `*_completed` signals which emit `(String request_id, Dictionary result)` per amendment `A-12` ([`./amendments.md`](./amendments.md)).

[`./architecture.md`](./architecture.md) line 220 is the canonical statement: *"All signals emit a single `Dictionary` argument (AC-D-10 — additive keys only)."* The `*_completed` 2-arg shape is the documented exception per `A-12`; all error / lifecycle / recovery signals are strictly 1-arg Dictionary.

The full inventory:

| Signal | Arg shape | Source ([`MWA.gd`](../addons/SolanaSDK/mwa/MWA.gd)) |
|---|---|---|
| `connect_completed` / `reauthorize_completed` / `disconnect_completed` / `deauthorize_completed` / `sign_messages_completed` / `sign_transactions_completed` / `sign_and_send_completed` | `(String request_id, Dictionary result)` | lines 27-33 |
| `mwa_error` | `(Dictionary payload)` — 10 keys (per [`./amendments.md`](./amendments.md) `A-14`) | line 34 |
| `mwa_timeout` | `(Dictionary payload)` — 4 keys | line 35 |
| `mwa_cancelled_lifecycle` | `(Dictionary payload)` — 3 keys (per [`./amendments.md`](./amendments.md) `A-12`) | line 36 |
| `reauth_required` | `(Dictionary payload)` — 1 key (`reason`) | line 37 |
| `pending_submission_found` | `(Dictionary payload)` — 6 keys, **ONE-SHOT** (per `DD-3-3-D`) | line 47 |

**`AC-D-10` additive-keys-only contract:** signal payload schemas are stable in the additive direction. New keys may be added to existing payloads in any minor release; existing keys may NOT be renamed or removed without a major version bump. Consumer code that reads only the keys it cares about is forward-compatible. See [`./api-stability.md`](./api-stability.md) for the full deprecation ladder and what counts as a breaking change.

## 3. Request_id correlation

Every op-method returns or attaches an 8-hex `request_id` (Story 1-5 `D-4`) that threads through GDScript → C++ → Kotlin and is echoed in the `*_completed` payload, `mwa_error.request_id`, `mwa_timeout.request_id`, and `mwa_cancelled_lifecycle.request_id`. Use it to correlate logs across layers and to disambiguate concurrent ops:

```gdscript
func sign_two_things() -> void:
    MWA.sign_messages([msg_a])
    MWA.sign_messages([msg_b])
    # Two ops in flight; both will eventually fire sign_messages_completed.
    # Use request_id from each method's return to match results.
```

In practice, the await + race pattern (Section 4 below) keeps you from ever needing manual request_id matching — the helper subscribes once per op and disconnects after the first terminal signal fires. Manual request_id matching is for advanced consumers running multiple concurrent ops of the same type.

## 4. The await + race pattern

Godot 4.3 has no built-in `Signal.race()`. The SDK ships no helper inside `addons/SolanaSDK/` (FR-1 single-addon-install discipline keeps the addon's public surface minimal). The example repo at `<example-repo>/scripts/await_helpers.gd` ships a reference implementation called `_race_signals(success_sig, error_sig) -> Dictionary` per [`../REVIEW.md`](../REVIEW.md) Design guidance (line 149).

The shape:

1. Connect to the success signal AND the error signal **before** invoking the op (so an instant error response — e.g., `NOT_CONNECTED` — doesn't fire before listeners attach).
2. Await whichever fires first.
3. Disconnect the other to prevent leaked subscriptions.
4. Return a `{ok: true, value: <success>}` or `{ok: false, error: <error>}` Dictionary.

[`./usage.md`](./usage.md) provides the user-facing snippet — you do not need to duplicate the helper inside the addon. The pattern is correct because:

- All terminal signals fire on the main thread (`AC-A-1`), so the await-and-disconnect race is sequentially consistent.
- The seven `*_completed` signals are 2-arg `(request_id, result)`; error/lifecycle signals are 1-arg `(payload)`. The race helper must accept both shapes — the example repo's `_race_signals` does this with two small lambda wrappers.

## 5. One-shot vs persistent subscriptions

Most signal subscriptions are **persistent** — `MWA.mwa_error.connect(my_handler)` stays connected for the lifetime of the connecting node. The handler fires every time `mwa_error` is emitted.

**One-shot subscriptions** auto-disconnect after the first emission. Use the `CONNECT_ONE_SHOT` flag:

```gdscript
MWA.pending_submission_found.connect(_on_pending_submission_found, CONNECT_ONE_SHOT)
```

`pending_submission_found` is the canonical one-shot signal in MWA per `DD-3-3-D` ([`./stories/3-3.md`](./stories/3-3.md)): each pending breadcrumb produces exactly one emission and is then cleared. Subscribing without `CONNECT_ONE_SHOT` would leak the subscription across every `_ready()` call if the player restarts the game.

The `_race_signals` helper from Section 4 also uses `CONNECT_ONE_SHOT` internally for both the success and error legs — that is what makes the race deterministic. See [`../REVIEW.md`](../REVIEW.md) Design guidance (lines 151-157) for the canonical pattern.

## 6. Signal vs callback comparison

If you come from JavaScript / Kotlin / Swift, the closest analog to Godot signals is **typed event emitters with named topics**:

- A signal is a named topic (e.g., `connect_completed`); multiple consumers can subscribe.
- The argument shape is part of the signal definition — Godot enforces it at signal-emit time. There is no "the callback receives whatever you pass in" laxity.
- Signals are not callbacks. There is no return value the producer reads back. Signals are one-way, fan-out broadcasts.
- Signals integrate with Godot's `await` keyword: `await MWA.connect_completed` suspends the calling coroutine until the signal fires (returns the args as a tuple).

This typed, fan-out, await-friendly shape is what makes signals the right async primitive for MWA. You get non-blocking method-return for free, type discipline at the signal definition, and easy multi-consumer subscription — all while staying inside Godot's idiomatic patterns.

## See also

- [`./usage.md`](./usage.md) — user-facing snippets including the await + race helper.
- [`./api-reference.md`](./api-reference.md) — every method's "Emits" section names the signals it can fire.
- [`./architecture.md`](./architecture.md) §3.2 — full signal payload schemas (line 218).
- [`../REVIEW.md`](../REVIEW.md) Design guidance — canonical example-repo patterns including `_race_signals` and `CONNECT_ONE_SHOT`.
- [`./api-stability.md`](./api-stability.md) — additive-keys-only contract and deprecation ladder.
