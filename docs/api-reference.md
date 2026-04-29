---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# MWA API Reference

Per-method reference for the `MWA` autoload. The single source of truth for the API surface is `addons/SolanaSDK/mwa/MWA.gd` lines 27-418 (12 signals + 14 methods). This document expands each entry with signature, parameters, return type, signals emitted, possible error codes, and at least one copy-pasteable GDScript snippet.

The summary table in [`REVIEW.md`](../REVIEW.md) `## API surface` lists all 17 symbols at a glance — start there if you want the bird's-eye view. This document provides the per-symbol depth.

## Conventions

- All op methods are **non-blocking** and return within ≤16ms (per `AC-NFR-1` and `AC-A-1` in [`docs/spec.md`](./spec.md)). Results arrive via signals on the Godot main thread.
- Every op mints an 8-lowercase-hex `request_id` correlating the call site to its terminal signal. `MWA.connect` returns the `request_id` directly; the other op methods return `void` per the LOCKED Story 2-1 T7 convention — read the `request_id` from the eventual signal payload.
- Every op terminates with exactly one signal: a success `<op>_completed`, an `mwa_error`, an `mwa_timeout`, or an `mwa_cancelled_lifecycle`. There are no per-op error signals — all errors funnel through `mwa_error` per `DD-15`.
- For the full 10-key `mwa_error` payload schema see [`docs/architecture.md`](./architecture.md) §3.2 and [`docs/amendments.md`](./amendments.md) `A-14`. For the 13 error codes see [`docs/error-codes.md`](./error-codes.md).

## Signals (overview)

`addons/SolanaSDK/mwa/MWA.gd:27-47` declares 12 signals. The `<op>_completed` family takes two parameters per [`docs/amendments.md`](./amendments.md) `A-12`; the error / lifecycle family takes a single Dictionary payload.

| Signal | Shape | Notes |
|---|---|---|
| `connect_completed(request_id, result)` | 2-param | success of `MWA.connect` |
| `reauthorize_completed(request_id, result)` | 2-param | success of `MWA.reauthorize` |
| `disconnect_completed(request_id, result)` | 2-param | success of `MWA.disconnect` |
| `deauthorize_completed(request_id, result)` | 2-param | success of `MWA.deauthorize`; always emitted |
| `sign_messages_completed(request_id, result)` | 2-param | success of `MWA.sign_messages` |
| `sign_transactions_completed(request_id, result)` | 2-param | success of `MWA.sign_transactions` |
| `sign_and_send_completed(request_id, result)` | 2-param | success of `MWA.sign_and_send` |
| `mwa_error(payload)` | 1-param | universal error signal; 10-key payload (`A-14`) |
| `mwa_timeout(payload)` | 1-param | watchdog expiry; 4-key payload |
| `mwa_cancelled_lifecycle(payload)` | 1-param | activity destroyed or `forget_all` invoked; 3-key payload |
| `reauth_required(payload)` | 1-param | keystore corruption recovery; `{reason}` |
| `pending_submission_found(payload)` | 1-param **ONE-SHOT** | sign_and_send breadcrumb survived process death; 6-key payload |

For full payload schemas (per-key types and semantics), see [`docs/architecture.md`](./architecture.md) §3.2 (lines 218-298).

## Methods

### MWA.connect

Initiate a wallet connect (authorize). Returns the generated 8-lowercase-hex `request_id` so callers can correlate the eventual terminal signal.

**Signature** (`MWA.gd:133`):

```gdscript
func connect(identity: Dictionary, cluster: String, opts: Dictionary = {}) -> String
```

**Parameters:**
- `identity: Dictionary` — `{"name": String, "icon_uri": String, "identity_uri": String}`. The wallet renders these in the signing prompt.
- `cluster: String` — `"devnet"` / `"testnet"` / `"mainnet-beta"`. See [`docs/usage.md`](./usage.md) for cluster-selection guidance.
- `opts: Dictionary` (optional) — `{"timeout_ms": int, "chain_id": String}`. Omitted keys fall to internal defaults.

**Returns:** `String` — the 8-hex `request_id`.

**Signals emitted:**
- `connect_completed(request_id, result)` on success. `result` keys: `request_id`, `auth_token_fingerprint`, `public_key`, `wallet_label`, `wallet_icon_uri`, `cluster`.
- `mwa_error(payload)` on failure. Possible codes: `USER_CANCELED`, `NO_MWA_WALLET_INSTALLED`, `WALLET_REJECTED`, `UNSUPPORTED_PLATFORM`, `PROTOCOL_ERROR`.
- `mwa_timeout(payload)` on watchdog expiry.

**Snippet:**

```gdscript
func _ready() -> void:
    MWA.connect_completed.connect(_on_connect_completed)
    MWA.mwa_error.connect(_on_mwa_error)
    var identity := {
        "name": "My Game",
        "icon_uri": "https://my-game.example.com/icon.png",
        "identity_uri": "https://my-game.example.com",
    }
    var request_id := MWA.connect(identity, "devnet")
    print("dispatched connect with request_id=%s" % request_id)

func _on_connect_completed(request_id: String, result: Dictionary) -> void:
    print("connected: ", result["wallet_label"], " on ", result["cluster"])
```

### MWA.disconnect

Tear down the active session. Idempotent on a non-connected facade.

**Signature** (`MWA.gd:141`):

```gdscript
func disconnect() -> void
```

**Parameters:** none.

**Returns:** `void`. Read the `request_id` from the `disconnect_completed` payload.

**Signals emitted:**
- `disconnect_completed(request_id, result)` on success. `result` is `{request_id}`.
- `mwa_error(payload)` on failure. Possible codes: `NOT_CONNECTED`, `PROTOCOL_ERROR`, `UNSUPPORTED_PLATFORM`.

**Snippet:**

```gdscript
func sign_out() -> void:
    MWA.disconnect_completed.connect(_on_disconnect_completed, CONNECT_ONE_SHOT)
    MWA.disconnect()

func _on_disconnect_completed(request_id: String, _result: Dictionary) -> void:
    print("disconnected (request_id=%s)" % request_id)
```

### MWA.reauthorize

Silent re-authorization using the cached auth token. No wallet UI is shown (per `NFR-5`).

**Signature** (`MWA.gd:184`):

```gdscript
func reauthorize(opts: Dictionary = {}) -> void
```

**Parameters:**
- `opts: Dictionary` (optional) — `{"timeout_ms": int}`. Clamped to internal default per `DD-23` watchdog.

**Returns:** `void`.

**Signals emitted:**
- `reauthorize_completed(request_id, result)` on success — same shape as `connect_completed`.
- `mwa_error(payload)` on failure. Possible codes: `NOT_CONNECTED` (no cached token / cluster mismatch — `AC-2` / `AC-4`), `TOKEN_EXPIRED` (wallet rejected token — `AC-3` graceful wipe), `UNSUPPORTED_PLATFORM`, `PROTOCOL_ERROR`.
- `mwa_timeout(payload)` on watchdog expiry per `DD-2-2-3`.

**Snippet:**

```gdscript
func resume_session() -> void:
    MWA.reauthorize_completed.connect(_on_reauthorize_completed, CONNECT_ONE_SHOT)
    MWA.mwa_error.connect(_on_mwa_error)
    MWA.reauthorize()

func _on_reauthorize_completed(_request_id: String, result: Dictionary) -> void:
    print("session resumed for ", result["wallet_label"])
```

### MWA.deauthorize

Revoke the wallet's auth_token AND wipe ALL local cache records keyed under this caller's identity (per `FR-5` / `AC-NFR-SEC-5`). Always clears local state regardless of remote outcome — the threat model is a returned/stolen device that must not retain a working auth_token.

**Signature** (`MWA.gd:168`):

```gdscript
func deauthorize() -> void
```

**Parameters:** none.

**Returns:** `void`.

**Signals emitted:**
- `deauthorize_completed(request_id, result)` on **every** call. `result` keys: `request_id`, `remote_revoke_succeeded`, `local_cache_cleared` (always `true`), `warning` (`""` or `"remote_unreachable"`).
- `mwa_error{code=PROTOCOL_ERROR}` only on catastrophic local failures (e.g., corrupted-keystore wipe crash — see `DD-4-1-3` in [`docs/stories/4-1.md`](./stories/4-1.md)). Remote-revoke failure does NOT route here per `DD-4-1-1`; it surfaces as `deauthorize_completed{remote_revoke_succeeded: false, warning: "remote_unreachable"}`.

There is **no** watchdog for this op — `AC-2` requires remote failure to surface as `remote_unreachable`, NOT `mwa_timeout`.

**Snippet:**

```gdscript
func revoke_wallet() -> void:
    MWA.deauthorize_completed.connect(_on_deauthorize_completed, CONNECT_ONE_SHOT)
    MWA.deauthorize()

func _on_deauthorize_completed(_request_id: String, result: Dictionary) -> void:
    if result["remote_revoke_succeeded"]:
        print("wallet revoke confirmed; local cache cleared")
    else:
        print("local cache cleared; remote revoke deferred (warning: %s)" % result["warning"])
```

### MWA.sign_messages

Sign one or more byte-array messages with the connected wallet. The wallet UI is shown for confirmation.

**Signature** (`MWA.gd:213`):

```gdscript
func sign_messages(messages: Array[PackedByteArray], opts: Dictionary = {}) -> void
```

**Parameters:**
- `messages: Array[PackedByteArray]` — list of message byte arrays, 1+ entries.
- `opts: Dictionary` (optional) — `{"timeout_ms": int}`. Clamped to internal default per `DD-3-1-3`.

**Returns:** `void`.

**Signals emitted:**
- `sign_messages_completed(request_id, result)` on success. `result` keys: `request_id`, `signed_payloads` (`Array[PackedByteArray]`, length matches input; each is the message bytes prefixed with the wallet's 64-byte ed25519 signature, decoded length ≥ input length + 64).
- `mwa_error(payload)` on failure. Possible codes: `NOT_CONNECTED` (synchronous preflight per `DD-3-1-6` / `AC-3`), `USER_CANCELED`, `WALLET_REJECTED`, `UNSUPPORTED_PLATFORM`, `PROTOCOL_ERROR`.
- `mwa_timeout(payload)` on watchdog expiry.

**Snippet:**

```gdscript
func sign_login_challenge() -> void:
    MWA.sign_messages_completed.connect(_on_sign_messages_completed, CONNECT_ONE_SHOT)
    MWA.mwa_error.connect(_on_mwa_error)
    var msg := PackedByteArray("login-challenge-12345".to_utf8_buffer())
    MWA.sign_messages([msg])

func _on_sign_messages_completed(_request_id: String, result: Dictionary) -> void:
    var signed: Array = result["signed_payloads"]
    print("got %d signed payload(s); first is %d bytes" % [signed.size(), signed[0].size()])
```

### MWA.sign_transactions

Sign one or more serialized Solana transactions with the connected wallet. The wallet UI is shown for confirmation. Caller submits to RPC separately.

**Signature** (`MWA.gd:246`):

```gdscript
func sign_transactions(transactions: Array[PackedByteArray], opts: Dictionary = {}) -> void
```

**Parameters:**
- `transactions: Array[PackedByteArray]` — list of serialized transaction byte arrays, 1+ entries.
- `opts: Dictionary` (optional) — `{"timeout_ms": int}`.

**Returns:** `void`.

**Signals emitted:**
- `sign_transactions_completed(request_id, result)` on success. `result` keys: `request_id`, `signed_transactions` (`Array[PackedByteArray]`, length matches input; each is the caller's serialized transaction with the wallet's signature(s) populated). The payload-array key is `signed_transactions` (renamed from the op-agnostic `signed_payloads` per `DD-3-2-3` + `D-3-2-1`).
- `mwa_error(payload)` on failure. Possible codes: `NOT_CONNECTED`, `USER_CANCELED`, `WALLET_REJECTED`, `UNSUPPORTED_PLATFORM`, `PROTOCOL_ERROR`.
- `mwa_timeout(payload)` on watchdog expiry.

**Snippet:**

```gdscript
func sign_and_self_submit(serialized_tx: PackedByteArray) -> void:
    MWA.sign_transactions_completed.connect(_on_sign_transactions_completed, CONNECT_ONE_SHOT)
    MWA.mwa_error.connect(_on_mwa_error)
    MWA.sign_transactions([serialized_tx])

func _on_sign_transactions_completed(_request_id: String, result: Dictionary) -> void:
    var signed: Array = result["signed_transactions"]
    print("signed transaction is %d bytes; submit to RPC next" % signed[0].size())
```

### MWA.sign_and_send

Sign one or more serialized Solana transactions AND submit them to the connected cluster's RPC endpoint via the wallet (single round-trip).

**Signature** (`MWA.gd:300`):

```gdscript
func sign_and_send(transactions: Array[PackedByteArray], opts: Dictionary = {}) -> void
```

**Parameters:**
- `transactions: Array[PackedByteArray]` — list of serialized transaction byte arrays, 1+ entries.
- `opts: Dictionary` (optional) — `{"timeout_ms": int}`.

**Returns:** `void`.

**Signals emitted:**
- `sign_and_send_completed(request_id, result)` on success. `result` keys: `request_id`, `signatures` (`Array[String]`, base58-encoded transaction signatures — distinct from the base64 wire format used in `sign_messages` / `sign_transactions`), `submitted_at` (wall-clock ms-epoch the wallet acknowledged forwarding the txs to RPC), `confirmation_status` (literal `"submitted"`).
- `mwa_error(payload)` on failure. Possible codes: `NOT_CONNECTED` (also fires on cluster-bleed defensive guard per `DD-27` / `AC-NFR-SEC-4` / `AC-6`), `USER_CANCELED`, `WALLET_REJECTED`, `NETWORK_OFFLINE`, `RPC_FAILED`, `UNSUPPORTED_PLATFORM`, `PROTOCOL_ERROR`.
- `mwa_timeout(payload)` on watchdog expiry.
- `reauth_required{reason: "keystore_corrupt"}` if a Tink corruption event is detected during the breadcrumb-write site (per `DD-3-3-G` fail-closed wrapper).

**Pending-submission breadcrumb (`AC-2` / `AC-5` of Story 3-3):** before the wallet round-trip starts, this op writes a 7-field breadcrumb to encrypted storage. The breadcrumb is removed in ALL terminal paths (success/error/timeout) per `DD-3-3-C`. If the process is killed mid-op, the breadcrumb survives and the next successful `connect_completed` / `reauthorize_completed` will be followed by a one-shot `pending_submission_found` signal. **`sign_and_send` is at-least-once from the caller's perspective** — see [`docs/safety.md`](./safety.md) for the recovery pattern.

**Snippet:**

```gdscript
func submit_payment(serialized_tx: PackedByteArray) -> void:
    MWA.sign_and_send_completed.connect(_on_sign_and_send_completed, CONNECT_ONE_SHOT)
    MWA.mwa_error.connect(_on_mwa_error)
    MWA.sign_and_send([serialized_tx])

func _on_sign_and_send_completed(_request_id: String, result: Dictionary) -> void:
    var sigs: Array = result["signatures"]
    print("submitted at %d; signature %s" % [result["submitted_at"], sigs[0]])
    # Caller queries getSignatureStatuses on `sigs` to confirm landing.
```

### MWA.forget_all

Story 4-2 — GDPR/CCPA "Sign out everywhere" reset. Wipes EVERY cached wallet token across all identities, rotates the encryption MasterKey (Android Keystore alias), cancels any in-flight ops by emitting `mwa_cancelled_lifecycle{reason: "forget_all_invoked"}` per slot BEFORE the wipe, and best-effort remote-deauthorizes each cached wallet (offline wallets do not block local wipe per `AC-4`).

**Signature** (`MWA.gd:334`):

```gdscript
func forget_all() -> void
```

**Parameters:** none. No `opts` per `DD-4-2-7` — `forget_all` is a hard reset; per-call tunables would only obscure the security intent.

**Returns:** `void`.

**Signals emitted:**
- **No completion signal** per `DD-4-2-8` — `forget_all` is fire-and-forget. Verify completion via state inspection: `MWA.is_connected()` returns `false`; the next `MWA.connect()` triggers a fresh wallet handshake (no cache hit).
- `mwa_cancelled_lifecycle{request_id, source_method, reason: "forget_all_invoked"}` for every in-flight op slot, BEFORE the wipe.

Mutex serialization (per `DD-4-2-2` + `AC-3`): concurrent op-method calls block on a plugin-instance Mutex; either complete BEFORE the wipe (and have their in-flight slots cancelled) or run AFTER the wipe completes (observing clean state). NO partial state observable.

**Snippet:**

```gdscript
func panic_reset() -> void:
    MWA.mwa_cancelled_lifecycle.connect(_on_lifecycle_cancel)
    MWA.forget_all()
    # Wait one frame, then verify clean state.
    await get_tree().process_frame
    assert(not MWA.is_connected())
    print("forget_all complete; SecureTokenStore wiped, MasterKey rotated")

func _on_lifecycle_cancel(payload: Dictionary) -> void:
    if payload["reason"] == "forget_all_invoked":
        print("cancelled in-flight %s (request_id=%s)" % [payload["source_method"], payload["request_id"]])
```

### MWA.is_supported

Synchronous platform check. Returns `true` only on Android, where the JNI bridge to the MWA Android plugin is available.

**Signature** (`MWA.gd:346`):

```gdscript
func is_supported() -> bool
```

**Parameters:** none.

**Returns:** `bool`. `true` iff `OS.get_name() == "Android"`.

**Snippet:**

```gdscript
func _ready() -> void:
    if MWA.is_supported():
        $WalletPanel.visible = true
    else:
        $WalletPanel.visible = false
        $DesktopFallbackLabel.text = "Sign-in via wallet not supported in editor / desktop builds."
```

### MWA.is_connected

Synchronous state read — `true` after a successful `connect`, `false` otherwise.

**Signature** (`MWA.gd:354`):

```gdscript
func is_connected() -> bool
```

**Parameters:** none.

**Returns:** `bool`. Backed by `MwaSessionState.authToken != null` (per [`docs/architecture.md`](./architecture.md) §7.1).

**Snippet:**

```gdscript
func update_ui() -> void:
    if MWA.is_connected():
        $StatusLabel.text = "Connected to %s" % MWA.get_wallet_label()
    else:
        $StatusLabel.text = "Not connected"
```

### MWA.get_public_key

Base58 public key of the connected wallet, or empty string pre-connect / on non-Android.

**Signature** (`MWA.gd:360`):

```gdscript
func get_public_key() -> String
```

**Parameters:** none.

**Returns:** `String`. Base58 encoded; empty pre-connect.

**Snippet:**

```gdscript
func display_account() -> void:
    var pk := MWA.get_public_key()
    if pk.is_empty():
        $AccountLabel.text = "(not connected)"
    else:
        $AccountLabel.text = "%s...%s" % [pk.substr(0, 4), pk.substr(pk.length() - 4)]
```

### MWA.get_cluster

Cluster name (e.g., `"devnet"`) of the active session, or empty string pre-connect.

**Signature** (`MWA.gd:366`):

```gdscript
func get_cluster() -> String
```

**Parameters:** none.

**Returns:** `String`. One of `"devnet"`, `"testnet"`, `"mainnet-beta"`, or `""` pre-connect.

**Snippet:**

```gdscript
func warn_if_mainnet() -> void:
    if MWA.get_cluster() == "mainnet-beta":
        $MainnetWarningBanner.visible = true
```

### MWA.get_wallet_label

Wallet-reported human-readable label, or empty string pre-connect.

**Signature** (`MWA.gd:371`):

```gdscript
func get_wallet_label() -> String
```

**Parameters:** none.

**Returns:** `String`. E.g., `"Phantom"`, `"Solflare"`, `"Backpack"`. Empty pre-connect.

**Snippet:**

```gdscript
func _on_connect_completed(_request_id: String, _result: Dictionary) -> void:
    print("connected via ", MWA.get_wallet_label())
```

### MWA.get_diagnostics

Story 5-2 `AC-1` — synchronous diagnostics snapshot. Returns a 12-key Dictionary safe to attach to bug reports. `AC-3` invariant: zero auth-token bytes / signing key material / PII; the only token-derived field is the 8-hex `auth_token_fingerprint` (HKDF-SHA256 truncated, NOT the token).

**Signature** (`MWA.gd:393`):

```gdscript
func get_diagnostics() -> Dictionary
```

**Parameters:** none.

**Returns:** `Dictionary` with 12 keys: `sdk_version`, `clientlib_ktx_version`, `godot_version`, `android_api_level`, `session_state`, `wallet_package`, `wallet_version`, `auth_token_fingerprint`, `cluster`, `last_n_correlation_trace`, `late_result_count`, `pending_submissions_count`. On non-Android exports returns the same shape with empty values per `DD-5-2-3` — callers do NOT need to branch on `is_supported()`.

**Snippet:**

```gdscript
func attach_diagnostics_to_bug_report() -> void:
    var diag := MWA.get_diagnostics()
    var report := "MWA diagnostics:\n"
    for key in diag.keys():
        report += "  %s: %s\n" % [key, diag[key]]
    $BugReportTextArea.text += report
```

### MWA.get_device_posture

Story 5-2 `AC-4` — device posture snapshot. **Non-authoritative** heuristics for support-team triage; do NOT use as a security gate.

**Signature** (`MWA.gd:408`):

```gdscript
func get_device_posture() -> Dictionary
```

**Parameters:** none.

**Returns:** `Dictionary` with 4 keys, all `bool`: `rooted`, `debuggable`, `developer_options_on`, `adb_enabled`. Sourced from `Build.TAGS`, `ApplicationInfo.FLAG_DEBUGGABLE`, `Settings.Secure.DEVELOPMENT_SETTINGS_ENABLED`, `Settings.Global.ADB_ENABLED`. A determined attacker can mask each signal at the OS level. On non-Android exports returns 4-key all-false per `DD-5-2-3`.

**Snippet:**

```gdscript
func log_device_posture() -> void:
    var posture := MWA.get_device_posture()
    if posture["rooted"] or posture["debuggable"]:
        print("(advisory) device shows developer-mode signals: ", posture)
```

### MWA.get_auth_token_fingerprint

8-lowercase-hex fingerprint of the current auth_token (`AC-7`), or empty string pre-connect. The fingerprint is HKDF-SHA256(auth_token, per-install salt) — salt rotates on `forget_all` (Story 4-2), so the fingerprint is stable within a single install lifecycle and changes after deauth+rotate. **Safe to log** — it is not the token itself.

**Signature** (`MWA.gd:379`):

```gdscript
func get_auth_token_fingerprint() -> String
```

**Parameters:** none.

**Returns:** `String`. 8-hex fingerprint or empty pre-connect.

**Snippet:**

```gdscript
func tag_log_lines() -> String:
    var fp := MWA.get_auth_token_fingerprint()
    return "[mwa-fp=%s]" % (fp if not fp.is_empty() else "none")
```

### MWA.get_suggested_wallet_install_links

Suggested wallet install links for `AC-4` UX — display when `mwa_error` fires with `code=NO_MWA_WALLET_INSTALLED`. Static data, no network call.

**Signature** (`MWA.gd:417`):

```gdscript
func get_suggested_wallet_install_links() -> Array[Dictionary]
```

**Parameters:** none.

**Returns:** `Array[Dictionary]`. Three reference wallets — Phantom / Solflare / Backpack. Each entry has 4 keys per `DD-5-2-4` superset: `name`, `package_id`, `play_store_url`, `website_url`.

**Snippet:**

```gdscript
func _on_mwa_error(payload: Dictionary) -> void:
    if payload["code"] == "NO_MWA_WALLET_INSTALLED":
        var wallets := MWA.get_suggested_wallet_install_links()
        for wallet in wallets:
            var btn := Button.new()
            btn.text = "Install %s" % wallet["name"]
            btn.pressed.connect(func(): OS.shell_open(wallet["play_store_url"]))
            $InstallPromptVBox.add_child(btn)
```

## See also

- [`docs/usage.md`](./usage.md) — end-to-end usage patterns.
- [`docs/error-codes.md`](./error-codes.md) — full 13-code error taxonomy.
- [`docs/signals-101.md`](./signals-101.md) — signal-model fundamentals for devs new to Godot.
- [`docs/architecture.md`](./architecture.md) §3.2 — full signal payload schemas.
- [`REVIEW.md`](../REVIEW.md) — grant-reviewer ≤10-minute verification path with the 17-row API summary.
