---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# Safety Guide

This document covers security-sensitive considerations when shipping a Godot game that uses the MWA SDK. Read it end-to-end before promoting `cluster = "devnet"` to `mainnet-beta`. The guidance here is canonical; cross-references point at [`../REVIEW.md`](../REVIEW.md) for design-guidance specifics that the example repo enforces, and at [`./architecture.md`](./architecture.md) and [`./spec.md`](./spec.md) for normative source.

The five topics below map to Story 5-5 `AC-7` evidence path and are required reading before mainnet:

1. `FLAG_SECURE` recommendation
2. Identity-metadata attacker-controllability
3. `sign_and_send` at-least-once semantics
4. Pending-submission recovery
5. Secure logging practices (`SecretString` + `SdkLog` R8 strip + CI grep bans)

## 1. `FLAG_SECURE` recommendation

`AC-NFR-SEC-12` ([`./spec.md`](./spec.md)) recommends adding `FLAG_SECURE` to any game Activity that displays public keys, transaction details, or signing prompts. With `FLAG_SECURE` set, Android prevents screenshots and excludes the window from the recents thumbnail and most screen-recorders. Without `FLAG_SECURE`, a hostile screen-recorder app on the same device can capture the displayed pubkey or transaction body.

To set `FLAG_SECURE` from a Godot game on Android, the recommended path is a small Android plugin hook that calls:

```kotlin
// inside a custom Godot Android plugin or Activity-lifecycle hook
activity.window.addFlags(android.view.WindowManager.LayoutParams.FLAG_SECURE)
```

Godot's stock Android Activity in the AAR template does **not** default to `FLAG_SECURE`. Game developers add the flag in one of two ways:

- A custom Android export template that overrides the Activity's `onCreate`.
- A small Godot Android plugin that hooks the Activity-lifecycle and calls `addFlags(FLAG_SECURE)` on entry into screens that show pubkeys or transaction bodies.

Refer to the Godot 4.3 Android plugin docs for the current binding shape; this SDK does not ship a `FLAG_SECURE` plugin itself (the addon is GDScript-callable only — see [`./api-reference.md`](./api-reference.md)). Toggling `FLAG_SECURE` per screen (signing screen ON, main menu OFF) is also acceptable as long as every signing prompt is covered.

## 2. Identity-metadata attacker-controllability

The `identity` Dictionary you pass to `MWA.connect(identity, cluster, opts)` ([`./api-reference.md`](./api-reference.md)) carries three fields presented to the wallet as part of the signing prompt:

```gdscript
var identity := {
    "name": "My Game",                          # identityName
    "icon_uri": "https://my-game.example.com/icon.png",
    "identity_uri": "https://my-game.example.com",
}
```

These fields are **attacker-controllable from the dApp side**. A hostile dApp cannot fake the wallet UI itself — the wallet's signing prompt is the **sole trust anchor** — but a hostile dApp **can** supply misleading `identityName` / `iconUri` / `identityUri` values. The wallet renders them verbatim. This means:

- Use a **real** `identity_uri` that matches your game's actual domain in production. The example repo's `https://example.com/godot-mwa-sample` placeholder is intentional per `AC-NFR-SEC-10` ([`./spec.md`](./spec.md)) and the Design guidance section in [`../REVIEW.md`](../REVIEW.md); production code MUST NOT inherit it.
- **Never downgrade** to `http://` or omit the scheme — the wallet UI may render the bare hostname and downgrade hides the protocol from the user.
- **Teach your players to verify the wallet prompt's domain field** before signing. The wallet is the trust anchor; the player is the verification step. The SDK cannot help here — it is a dApp-vs-player UX contract.
- The `iconUri` is fetched by the wallet over the network; serve it from the same `identity_uri` host so a domain-aware player can spot a mismatch.

The phishing threat model: a malicious dApp claims to be `https://my-game.example.com` (your real domain), supplies your real icon, and prompts the user to sign a transaction that drains their wallet. The wallet UI shows your domain and your icon. The defense is the player reading the **transaction body** in the wallet prompt — not the identity metadata. Identity metadata is for UX, not authentication.

## 3. `sign_and_send` is at-least-once semantics

Quote from [`./architecture.md`](./architecture.md) line 854: *"sign-and-send is at-least-once from the caller's perspective; treat breadcrumbs as authoritative for post-crash recovery."* This is `DD-3-3-A` LOCKED ([`./stories/3-3.md`](./stories/3-3.md)).

Why at-least-once and not exactly-once: the SDK does NOT ship an RPC endpoint config and cannot autonomously confirm whether the transactions landed on-chain. After a successful `sign_and_send_completed` signal, the SDK guarantees the wallet acknowledged submission — it does NOT guarantee the cluster confirmed the transaction.

**Caller responsibility:** after `sign_and_send_completed` fires, your game queries `getSignatureStatuses` (or `getSignaturesForAddress`) via your existing `SolanaClient` nodes for the signatures returned in the success payload's `signatures` field ([`./architecture.md`](./architecture.md) §3.2 line 252-260). Treat the SDK's success signal as "submitted, status unknown" — not "confirmed on-chain". The same caller responsibility applies after pending-submission recovery (Section 4 below).

In practice this means: never display "transaction confirmed" UX based solely on `sign_and_send_completed`. Always wait for the chain query to return `confirmed` or `finalized` before treating the transaction as durable. Showing a spinner with text "submitted; awaiting confirmation" is the correct intermediate state.

## 4. Pending-submission recovery

`pending_submission_found` is a **one-shot** signal that fires AFTER a successful `connect_completed` or `reauthorize_completed` if a prior `sign_and_send` was interrupted by process death (Story 3-3 `DD-3-3-D` and `DD-3-3-E`). The SDK writes a breadcrumb to encrypted storage **before** the wallet round-trip starts; if the OS kills the process mid-op, the breadcrumb survives and surfaces on the next successful auth.

**Payload — 6 keys** ([`MWA.gd:47`](../addons/SolanaSDK/mwa/MWA.gd) line 47):

```gdscript
{
    "request_id": String,                   # 8-hex correlation id
    "op_type": String,                      # "sign_and_send"
    "started_at_ms": int,                   # unix ms when breadcrumb was written
    "tx_count": int,                        # number of transactions in the batch
    "tx_preview_hashes": Array[String],     # SHA256 prefixes of each tx
    "recommendation": String,               # always "check_chain_for_signatures"
}
```

**Caller pattern** — subscribe with `CONNECT_ONE_SHOT` in `_ready()`:

```gdscript
func _ready() -> void:
    MWA.pending_submission_found.connect(_on_pending_submission_found, CONNECT_ONE_SHOT)

func _on_pending_submission_found(payload: Dictionary) -> void:
    var hashes: Array = payload["tx_preview_hashes"]
    # Query getSignatureStatuses for each hash via your SolanaClient.
    # If ALL hashes are NOT found on-chain, resubmit. If ANY hash IS found,
    # treat the prior submission as authoritative — do NOT resubmit.
    print("pending submission recovered, %d hashes to verify" % hashes.size())
```

The `CONNECT_ONE_SHOT` flag is critical: each pending breadcrumb produces exactly one emission and is then cleared. Subscribing without `CONNECT_ONE_SHOT` leaks subscriptions across runs. See [`../REVIEW.md`](../REVIEW.md) Design guidance (line 151-157) for the canonical pattern shipped by the example repo.

This recovery path is the practical face of the at-least-once guarantee from Section 3 above: the breadcrumb is **authoritative** for post-crash recovery; the chain query disambiguates "did it land or not."

## 5. Secure logging practices

Three layered defenses prevent token / pubkey / signing-key material from reaching disk or logcat:

### `SecretString` wrapper (`AC-NFR-SEC-1`)

Every token field flows through the `SecretString` wrapper whose `toString()` returns the literal string `"<redacted>"`. Logging a `SecretString` directly — `Log.d(TAG, "token=$token")` — produces `token=<redacted>` in logcat. The unredacted bytes are accessible only via `SecretString.reveal()`, which is forbidden in non-test code and grep-banned in CI per Story 5-6 `AC-6`. See [`./spec.md`](./spec.md) `AC-NFR-SEC-1` for the normative requirement and the `androidTest` sandbox + logcat scan that closes the AC.

Never log `authToken`, `publicKey`, or `SecretString.reveal()` output directly. Always pipe through `SecretString` and let the wrapper handle redaction.

### R8-strip via `SdkLog.v/d` (`AC-NFR-SEC-6`)

The `SdkLog` wrapper centralizes all SDK logging. Verbose / debug levels (`SdkLog.v` / `SdkLog.d`) are stripped by R8 in release builds via `-assumenosideeffects`. This means:

- Development builds: full verbose logging available for debugging.
- Release builds: zero verbose / debug bytes. `SdkLog.i/w/e` are preserved for support purposes.

The release-AAR grep test (per `AC-NFR-SEC-6`, [`./spec.md`](./spec.md)) fails the build if any token field name appears in the release AAR's bytecode. Use `SdkLog.v` / `SdkLog.d` for fields you would otherwise be tempted to redact at runtime — let R8 redact at link time.

`SdkLog.v` / `SdkLog.d` MUST use the lambda form to allow R8 to strip the call sites entirely. String interpolation (`"...${x}..."`) eagerly constructs the message even when the level is disabled and survives R8 stripping; the lambda form does not.

### CI grep bans (Story 5-6 `AC-6` — secure logging)

Story 5-6 wires PR-blocking grep gates that reject:

- `Log.*\(.*authToken` and `Log.*\(.*publicKey` — direct logging of secrets bypassing `SecretString`.
- `SecretString.reveal\(\)` outside of test code.
- `SdkLog.v/d/i/w/e\(.*"\$\{` — string interpolation passed to `SdkLog`. The lambda form is required so R8 can drop the call.

Together these three layers — `SecretString` for runtime redaction, `SdkLog.v/d` for R8 link-time stripping, and CI grep bans for code-review enforcement — satisfy the spec's "0 plaintext token bytes on disk or logcat" guarantee. Secure logging is non-negotiable: a single `Log.d(TAG, "token=$authToken")` lands the entire SDK release in violation of `AC-NFR-SEC-1`.

## See also

- [`../REVIEW.md`](../REVIEW.md) Design guidance — canonical home for example-app patterns referenced from this safety guide (mainnet gate, `_race_signals`, `pending_submission_found` `CONNECT_ONE_SHOT`).
- [`./error-codes.md`](./error-codes.md) — the 13-code error taxonomy. `STORAGE_CORRUPT` and `REAUTH_REQUIRED` are the security-relevant codes.
- [`./api-reference.md`](./api-reference.md) — per-method contracts including `MWA.get_device_posture()` for non-authoritative mainnet gating heuristics.
- [`./architecture.md`](./architecture.md) §7.4 (sign-and-send breadcrumb), §8 (security model).
- [`./spec.md`](./spec.md) `AC-NFR-SEC-1` / `AC-NFR-SEC-6` / `AC-NFR-SEC-10` / `AC-NFR-SEC-12` — normative source for this guide's recommendations.
