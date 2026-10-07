# Mobile Wallet Adapter (Android)

This PR adds Solana **Mobile Wallet Adapter (MWA)** support to `godot-solana-sdk`, exposing the eight MWA wallet operations to GDScript through a single autoload singleton named `MWA`. It targets **Android** (API 23+, `arm64-v8a` / `armeabi-v7a` / `x86_64`); on every other platform the singleton is a no-op that surfaces a clear `UNSUPPORTED_PLATFORM` error so cross-platform game code stays branch-free.

The implementation follows the published [MWA spec](https://github.com/solana-mobile/mobile-wallet-adapter) and uses `solana-mobile/mobile-wallet-adapter-clientlib-ktx` under the hood. Cryptographic material (auth tokens) is stored in `EncryptedSharedPreferences` backed by the Android Keystore.

---

## What's new

| Layer | What ships |
|---|---|
| GDScript autoload | `addons/SolanaSDK/mwa/MWA.gd` — singleton facade with 12 signals + 17 methods |
| C++ GDExtension node | `MobileWalletAdapter` (`src/mwa/`) — owns the JNI bridge, mints request IDs, runs the watchdog, marshals payloads to/from the autoload |
| Android plugin | `android/plugin/` — Kotlin plugin around `mobile-wallet-adapter-clientlib-ktx`, secure token store, session state, lifecycle observer, diagnostics |
| Example | `example/WalletAdapterAndroid/` — minimal Godot project demonstrating the connect / sign / sign-and-send flow |

Eight wallet ops: `mwa_connect`, `mwa_disconnect`, `deauthorize`, `reauthorize`, `sign_messages`, `sign_transactions`, `sign_and_send`, `forget_all`. Nine read-only getters for state, diagnostics, and UX prompts.

---

## Integration — three steps

1. **Drop the addon into your project.**

   Copy `addons/SolanaSDK/` from this repo into your Godot project's `addons/` directory (or build from source — see below).

2. **Enable the plugin.**

   In the Godot editor: **Project → Project Settings → Plugins → SolanaSDK → Enable**.

   The plugin's `solana_sdk_plugin.gd` registers an autoload named `MWA` pointing at `res://addons/SolanaSDK/mwa/MWA.gd`. Verify under **Project Settings → Autoload**.

3. **Use it from any script.**

   ```gdscript
   func _ready() -> void:
       if not MWA.is_supported():
           return  # editor / iOS / desktop / web — MWA is a no-op here
       MWA.connect_completed.connect(_on_connected)
       MWA.mwa_error.connect(_on_mwa_error)
       MWA.mwa_connect(
           {"name": "My Game", "icon_uri": "", "identity_uri": "https://my-game.example.com"},
           "devnet",
       )

   func _on_connected(_request_id: String, result: Dictionary) -> void:
       print("Connected: ", result["wallet_label"], " @ ", result["public_key"])

   func _on_mwa_error(payload: Dictionary) -> void:
       push_error("MWA %s: %s" % [payload["code"], payload["user_message"]])
   ```

That's a working MWA integration. The rest of this document explains why some method names are prefixed with `mwa_`, how to consume async results idiomatically, and what each error code means.

### Building from source

If you can't take the prebuilt `addons/SolanaSDK/`:

```bash
git clone --recurse-submodules https://github.com/ValentinVPK/godot-solana-sdk.git
cd godot-solana-sdk
pip install scons
# Set ANDROID_HOME and ANDROID_NDK_HOME, then:
scons addon
```

`scons addon` produces `addons/SolanaSDK/` containing the GDExtension binaries plus the Android plugin AAR. Requires **JDK 17** (Android Gradle Plugin requirement) and the Android SDK + NDK.

---

## The async pattern

Every wallet op is **non-blocking**. The op method returns within ~16 ms after queuing the request; the result arrives later via exactly one of four signal types:

- `<op>_completed(request_id: String, result: Dictionary)` — success
- `mwa_error(payload: Dictionary)` — domain error (user-canceled, network, protocol, etc.)
- `mwa_timeout(payload: Dictionary)` — watchdog expired
- `mwa_cancelled_lifecycle(payload: Dictionary)` — preempted by `forget_all` or app shutdown

Each op mints an 8-hex `request_id` that appears on the terminal signal so you can correlate concurrent requests.

The canonical consumption pattern is **connect-then-call**: subscribe to the success and error signals *before* invoking the op, then `await` whichever fires first. A tiny helper:

```gdscript
func await_op(success_sig: Signal, error_sig: Signal) -> Dictionary:
    var done := [false, null, null]  # [resolved, ok, payload]
    success_sig.connect(func(_rid, result):
        if not done[0]: done[0] = true; done[1] = true; done[2] = result, CONNECT_ONE_SHOT)
    error_sig.connect(func(payload):
        if not done[0]: done[0] = true; done[1] = false; done[2] = payload, CONNECT_ONE_SHOT)
    while not done[0]:
        await get_tree().process_frame
    return {"ok": done[1], "value": done[2]}

# Usage:
MWA.sign_messages([msg_bytes])
var out := await await_op(MWA.sign_messages_completed, MWA.mwa_error)
if out["ok"]:
    var signed: Array = out["value"]["signed_payloads"]
```

`CONNECT_ONE_SHOT` is important — without it, listeners accumulate across calls.

---

## API reference

### Method names

Three method names are prefixed with `mwa_` because they would otherwise shadow inherited methods on `Node`:

| GDScript API | Reason |
|---|---|
| `MWA.mwa_connect(identity, cluster, opts)` | `Node.connect(signal, callable)` |
| `MWA.mwa_disconnect()` | `Node.disconnect(signal, callable)` |
| `MWA.mwa_is_connected()` | `Node.is_connected(signal, callable)` |

All other ops keep their ergonomic names (`sign_messages`, `sign_transactions`, etc.).

### Operations

| Method | Returns | Terminal signals |
|---|---|---|
| `mwa_connect(identity: Dictionary, cluster: String, opts := {}) -> String` | request_id | `connect_completed` / `mwa_error` / `mwa_timeout` |
| `mwa_disconnect() -> void` | — | `disconnect_completed` / `mwa_error` |
| `deauthorize() -> void` | — | `deauthorize_completed` (always; carries `remote_revoke_succeeded` flag) / `mwa_error` (catastrophic only) |
| `reauthorize(opts := {}) -> void` | — | `reauthorize_completed` / `mwa_error` (`NOT_CONNECTED` / `TOKEN_EXPIRED`) / `mwa_timeout` |
| `sign_messages(messages: Array[PackedByteArray], opts := {}) -> void` | — | `sign_messages_completed` / `mwa_error` / `mwa_timeout` |
| `sign_transactions(transactions: Array[PackedByteArray], opts := {}) -> void` | — | `sign_transactions_completed` / `mwa_error` / `mwa_timeout` |
| `sign_and_send(transactions: Array[PackedByteArray], opts := {}) -> void` | — | `sign_and_send_completed` / `mwa_error` / `mwa_timeout` / `reauth_required` (keystore corruption) |
| `forget_all() -> void` | — | **No completion signal** (fire-and-forget). Best-effort remote deauth + full local wipe + key rotation. |

`identity` shape: `{"name": String, "icon_uri": String, "identity_uri": String}` — this is what the wallet UI shows the user.

`opts` accepts `{"timeout_ms": int, "chain_id": String}`. Omit it for sensible defaults (watchdog ≈ 90 s; `chain_id = "solana:<cluster>"`).

### Getters

| Method | Returns | Pre-connect value |
|---|---|---|
| `is_supported() -> bool` | `true` on Android, `false` elsewhere | n/a |
| `mwa_is_connected() -> bool` | `true` after successful connect | `false` |
| `get_public_key() -> String` | Base58 public key | `""` |
| `get_cluster() -> String` | e.g. `"devnet"` | `""` |
| `get_wallet_label() -> String` | wallet-reported label | `""` |
| `get_auth_token_fingerprint() -> String` | 8-hex HKDF-SHA256 fingerprint (NOT the token) | `""` |
| `get_diagnostics() -> Dictionary` | 12-key support snapshot (no PII, no token bytes) | shape preserved, empty values |
| `get_device_posture() -> Dictionary` | `{rooted, debuggable, developer_options_on, adb_enabled}` — heuristic, not attestation | all `false` |
| `get_suggested_wallet_install_links() -> Array[Dictionary]` | Phantom / Solflare / Backpack install URLs for `NO_MWA_WALLET_INSTALLED` UX | static |

### Signals

12 signals on the `MWA` autoload. The seven `*_completed` signals all share the shape `(request_id: String, result: Dictionary)`; the five lifecycle / error signals carry a single `Dictionary` payload.

| Signal | Payload |
|---|---|
| `connect_completed` | `(request_id, {request_id, public_key, cluster, wallet_label})` |
| `reauthorize_completed` | `(request_id, {request_id, public_key, cluster, wallet_label})` |
| `disconnect_completed` | `(request_id, {request_id})` |
| `deauthorize_completed` | `(request_id, {request_id, remote_revoke_succeeded, local_cache_cleared, warning})` |
| `sign_messages_completed` | `(request_id, {request_id, signed_payloads: Array[PackedByteArray]})` |
| `sign_transactions_completed` | `(request_id, {request_id, signed_transactions: Array[PackedByteArray]})` |
| `sign_and_send_completed` | `(request_id, {request_id, signatures: Array[String] (base58), submitted_at: int, confirmation_status: "submitted"})` |
| `mwa_error` | `{request_id, code, source_method, user_message, retry_hint, ...}` |
| `mwa_timeout` | `{request_id, source_method, elapsed_ms, ...}` |
| `mwa_cancelled_lifecycle` | `{request_id, source_method, reason}` (e.g. `reason: "forget_all_invoked"`) |
| `reauth_required` | `{reason}` — e.g. `"keystore_corrupt"`; game should call `mwa_connect` again |
| `pending_submission_found` | `{request_id, op_type, started_at_ms, tx_count, tx_preview_hashes, recommendation}` — fired once after a reconnect if a sign-and-send was interrupted by process death |

---

## Error handling

`mwa_error.payload.code` is one of a small enum (full list in the generated `mwa_error_codes.gd`):

| Code | Meaning | Typical handling |
|---|---|---|
| `UNSUPPORTED_PLATFORM` | Op called on non-Android build | Gate UI on `MWA.is_supported()` |
| `NO_MWA_WALLET_INSTALLED` | No compatible wallet on device | Show `get_suggested_wallet_install_links()` |
| `USER_CANCELED` | User dismissed wallet prompt | Restore game UI, no retry |
| `WALLET_REJECTED` | Wallet refused (policy / bad tx) | Show the wallet's reason, no automatic retry |
| `NOT_CONNECTED` | Op requires an active session | Call `mwa_connect` first |
| `TOKEN_EXPIRED` | Cached token rejected; cache wiped | Call `mwa_connect` (full re-auth) |
| `NETWORK_OFFLINE` / `RPC_FAILED` | `sign_and_send` couldn't reach RPC | Retry with backoff |
| `PROTOCOL_ERROR` | Catastrophic local failure | Rare; report a bug |

`mwa_timeout` is distinct from `mwa_error` so games can offer "retry" UX without parsing strings.

### Sign-and-send crash recovery

`sign_and_send` writes a small encrypted breadcrumb **before** sending the transaction to the wallet. If the game process is killed mid-flight (e.g. Android OOM), the breadcrumb survives. On the next successful `mwa_connect` or `reauthorize`, a one-shot `pending_submission_found` signal fires with the transaction's preview hashes so the game can query the chain and reconcile state before showing the user any "transaction failed" UX.

Subscribe once in `_ready()` with `CONNECT_ONE_SHOT`:

```gdscript
MWA.pending_submission_found.connect(func(payload):
    for tx_hash in payload["tx_preview_hashes"]:
        # query the chain for tx_hash before assuming the previous send failed
        ...
, CONNECT_ONE_SHOT)
```

---

## Security notes

- **Default to `devnet`.** Production code should require an explicit user-visible confirmation before switching to `mainnet-beta`. The bundled example uses a per-session attention checkpoint (not a `user://`-persisted flag) — see `example/WalletAdapterAndroid/`.
- **`identity_uri` is attacker-controllable from the dApp side.** The wallet UI is the trust anchor. Set `identity_uri` to your real HTTPS domain; don't ship the placeholder used in the example.
- **`get_device_posture()` is informational, not authoritative.** A determined attacker can mask `rooted` / `debuggable` / `adb_enabled` at the OS level. Use it for support triage, not for security gates.
- **`forget_all` is a hard reset.** It best-effort remote-deauthorizes every cached wallet, wipes local secure storage, rotates the Keystore master key alias, and cancels every in-flight op with `mwa_cancelled_lifecycle{reason: "forget_all_invoked"}`. No completion signal — verify via `mwa_is_connected() == false`.
- **Auth tokens never appear in `get_diagnostics()`.** Only an 8-hex HKDF-SHA256 fingerprint is exposed, safe to attach to bug reports.

---

## Platform support

| Platform | Status |
|---|---|
| Android (API 23+) | Supported |
| iOS / Windows / macOS / Linux / Web | `is_supported()` returns `false`; every op emits `mwa_error{code=UNSUPPORTED_PLATFORM}`. Safe to leave the autoload enabled. |

Supported Android ABIs: `arm64-v8a`, `armeabi-v7a`, `x86_64`. Other ABIs will fail to load the native library at runtime.

Godot compatibility: **4.3.x** (`compatibility_minimum = "4.3"` in the `.gdextension` manifest).

---

## File map

New top-level paths introduced by this PR:

```
addons/SolanaSDK/mwa/MWA.gd                # autoload singleton (GDScript facade)
addons/SolanaSDK/mwa_error_codes.gd        # generated error-code enum

include/mwa/mobile_wallet_adapter.hpp      # C++ GDExtension node public header
src/mwa/                                   # node impl, JNI bridge, platform selector,
                                           # no-op fallback for non-Android builds

android/plugin/                            # Kotlin Android plugin (AAR)
  src/main/kotlin/.../mwa/client/          # MwaClient (clientlib-ktx wrapper) + DTOs
  src/main/kotlin/.../mwa/plugin/          # plugin entry, InflightMap, diagnostics,
                                           # lifecycle observer, native bridge
  src/main/kotlin/.../mwa/store/           # SecureTokenStore + key management
  src/main/kotlin/.../mwa/session/         # MwaSessionState
  src/main/kotlin/.../mwa/util/            # Base58, SdkLog, SecretString

example/WalletAdapterAndroid/              # reference Godot project
```

The C++ node uses internal names prefixed with `mwa_` for the three shadowed methods (`mwa_connect`, `mwa_disconnect`, `mwa_is_connected`) to avoid collision at the GDExtension binding layer. The GDScript facade carries those same names so the public API is consistent.

---

## Acknowledgements

Built on `solana-mobile/mobile-wallet-adapter-clientlib-ktx`. The Solana Mobile team's [reference Fake Wallet](https://github.com/solana-mobile/mobile-wallet-adapter/tree/main/android/fakewallet) is the recommended development target — install it on the emulator before running the example.
