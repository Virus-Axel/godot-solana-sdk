---
status: complete
created: 2026-04-16
feature: mwa-android
phase: 3
---
# Architecture: Godot Mobile Wallet Adapter (MWA) — Android Support

## 1. Goals & Constraints

### Architecture Drivers

| Driver | Source | Implication |
|---|---|---|
| 0 plaintext tokens at rest | NFR-1, AC-NFR-SEC-1 | All token storage via Android Keystore-backed `EncryptedSharedPreferences`; `SecretString` wrapper in every layer |
| 7/7 RN SDK parity | NFR-4, AC-D-3 | API shape mirrors RN; parity manifest drives codegen |
| 100% silent reauth vs Phantom | NFR-5, AC-NFR-3 | Cache key must be cluster-bound; Keystore recovery must be non-crashing |
| ≤16 ms method return | NFR-9, AC-NFR-1 | All public methods return `void` immediately; results arrive via signal on Godot main thread |
| 0 hand-rolled MWA protocol | NFR-10, AC-NFR-6 | Delegate protocol, WebSocket, ECDH entirely to `com.solanamobile:mobile-wallet-adapter-clientlib-ktx` |
| Cross-platform GDExtension, Android-only plugin | NFR-3, AC-A-4, AC-D-6 | Single platform-abstraction TU; non-Android returns `UNSUPPORTED_PLATFORM` |

### Non-Negotiable Boundaries

- **No reverse dependency** from `godot-solana-sdk` core (`Transaction`, `SolanaClient`, etc.) to MWA — enforced by `ISigner` abstraction (AC-A-5).
- **MWA is a sibling node** of the existing desktop/web `WalletAdapter`, not a subclass.
- **Signals are emitted on Godot main thread only.** Off-thread signal emission is a defect.
- **Auth tokens flow only through `SecretString` wrapper.** `Log.*` calls with raw token values are a CI build break (AC-D-8).
- **Single source of truth** for error codes: `error-codes.yaml` codegens Kotlin enum + C++ enum + GDScript constants (AC-A-3).

### Locked Constraints from Research / Spec

- Android only (iOS out of scope forever per DD-1)
- Godot 4.3.x (DD-9 resolved above)
- min SDK 23 / target SDK 34 (AC-NFR-8)
- MIT license (AC-NFR-7)
- Must use `com.solanamobile:mobile-wallet-adapter-clientlib-ktx` (DD-4, AC-NFR-6)

## 2. Component Decomposition

### 2.1 System View (C4 Level 1)

```
┌───────────────────────────────────────────────────────────┐
│ Consumer Godot Android App (game)                         │
│   └─ GDScript addon consumer                              │
└────────────────────┬──────────────────────────────────────┘
                     │ (signals + method calls)
                     ▼
┌───────────────────────────────────────────────────────────┐
│ godot-solana-sdk (addon)                                  │
│ ├─ GDScript layer (addons/SolanaSDK/MWA.gd)              │
│ ├─ C++ GDExtension (MobileWalletAdapter node)            │
│ └─ Android Plugin (Kotlin, AAR)                          │
└────────────────────┬──────────────────────────────────────┘
                     │ (JSON-RPC over local WebSocket, Intent)
                     ▼
┌───────────────────────────────────────────────────────────┐
│ External Wallet App (Phantom / Solflare / Backpack / …)  │
│   └─ Handles signing, holds private keys                 │
└───────────────────────────────────────────────────────────┘
                     │
                     ▼
┌───────────────────────────────────────────────────────────┐
│ Solana RPC (mainnet-beta / devnet / testnet)             │
│   └─ Only reached via sign_and_send; not directly by MWA │
└───────────────────────────────────────────────────────────┘
```

### 2.2 Container View (C4 Level 2) — the MWA module

| # | Container | Language | Responsibility | Interface |
|---|---|---|---|---|
| A | `MWA.gd` (singleton autoload) | GDScript | Public API, signal marshalling to user code, state-introspection getters, `await` ergonomics | GDScript signals + methods |
| B | `MobileWalletAdapter` node | C++ (GDExtension) | Bind GDScript calls to JNI, marshal off-thread callbacks to Godot main via `call_deferred`, delegate to `MwaAndroidBridge` | `ClassDB::bind_method` + `ADD_SIGNAL` |
| C | `MwaAndroidBridge` (C++ interface) | C++ | Abstract seam between GDExtension and JNI — platform selector; real impl on Android, no-op on other platforms | Pure virtual C++ methods |
| D | `MwaAndroidBridgeJni` | C++ | Concrete Android-only JNI impl; caches `JavaVM*`, `AttachCurrentThread` for callbacks | JNI `CallStaticObjectMethod` etc. |
| E | `MwaAndroidPlugin` (Kotlin) | Kotlin | `GodotPlugin` subclass; receives `@UsedByGodot` method calls from C++; owns `MwaClient` and `MwaSessionState`; emits signals back via `emitSignal` | Godot Android plugin ABI |
| F | `MwaClient` (Kotlin interface) | Kotlin | Abstraction over `com.solanamobile.mobilewalletadapter.clientlib.*`; `FakeMwaClient` for unit tests | Kotlin suspend funs |
| G | `MwaClientImpl` (Kotlin) | Kotlin | Concrete impl wrapping `MobileWalletAdapter`, `MobileWalletAdapterClient`, `AuthorizationResult` from clientlib-ktx | Kotlin suspend funs |
| H | `SecureTokenStore` (Kotlin) | Kotlin | Owns `EncryptedSharedPreferences` + MasterKey; CRUD for `(cluster, chain_id, wallet_package, identity_uri) → SecretString` tuples; crash-safe on Keystore corruption | `getToken`, `putToken`, `deleteToken`, `deleteAll` |
| I | `MwaSessionState` (Kotlin) | Kotlin | In-memory session: current connected pubkey, cluster, wallet label, active auth token ref; replaces scaffold's `my*` globals | Single class with private fields |
| J | `SdkLog` (Kotlin) | Kotlin | Central logger; `V/D/I/W/E` per correlation ID; stripped in release via R8 `-assumenosideeffects` | `SdkLog.v(tag, corrId, msg)` etc. |
| K | `SecretString` (Kotlin + C++) | Both | Wrapper whose `toString()` returns `"<redacted>"`; prevents accidental token leakage to `Log.*` or stack traces | `fun value(): ByteArray` (caller opts-in) |
| L | `ErrorTaxonomy` (generated) | YAML → {Kotlin, C++, GDScript} | Single manifest `error-codes.yaml` code-generated into enums; drives error payloads across layers | Generated constants |
| M | Example app (`example/MWA/`) | GDScript + `.tscn` | Minimal scene showing Connect/Disconnect/Reauthorize/Deauthorize/Sign + log + Reset Local State; defaults to devnet | Godot project |

### 2.3 Component Dependency Graph

```
                  (User's game code, GDScript)
                           │ signals, methods
                           ▼
                       A. MWA.gd ──────┐
                           │            │ (holds refs only, no logic duplication)
                           ▼            │
                 B. MobileWalletAdapter │
                 (GDExtension C++ node) │
                           │            │
                           ▼            │
                  C. MwaAndroidBridge (interface)
                           │
            ┌──────────────┴──────────────┐
            ▼                              ▼
     D. MwaAndroidBridgeJni          (no-op impl; other platforms)
            │
            ▼
     E. MwaAndroidPlugin (Kotlin)
        ┌─────────┬──────────┬──────────┐
        ▼         ▼          ▼          ▼
      F/G       H.         I.         J.  K. SecretString
    MwaClient SecureToken SessionState SdkLog   (used throughout)
      (ktx)    Store
        │
        ▼
     mobile-wallet-adapter-clientlib-ktx
        │
        ▼
     External Wallet App
```

**Upstream godot-solana-sdk coupling:**
- `ISigner` (new C++ abstract interface, **added** to `godot-solana-sdk` core as part of this work)
- Existing `Transaction` node refactored to depend on `ISigner` instead of holding a concrete signer reference (AC-A-5)
- Existing desktop `WalletAdapter` implements `ISigner`
- New `MobileWalletAdapter` (node B) implements `ISigner`
- **No reverse dependency** from core → MWA; the `ISigner` seam lives in core, implementations register themselves

### 2.3.1 `ISigner` Contract (DD-11, AC-A-14 new)

Added to `godot-solana-sdk` core as `src/isigner.hpp`. Provides the exact signer abstraction both the existing desktop `WalletAdapter` and the new `MobileWalletAdapter` implement. Threading contract: methods return immediately; results arrive via signals on the Godot main thread. Ownership via `godot::Ref<>` (engine-managed lifetime).

```cpp
// src/isigner.hpp (LOCKED contract — any change requires architecture amendment)
#pragma once
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot_solana_sdk {

class ISigner : public godot::RefCounted {
    GDCLASS(ISigner, godot::RefCounted);

protected:
    static void _bind_methods();

public:
    // Synchronous state queries (≤1 ms, no I/O)
    virtual bool is_connected() const = 0;
    virtual godot::String get_public_key() const = 0;  // base58, "" if not connected

    // Async operations — return void, emit signals on Godot main thread
    // Signals defined on ISigner itself: sign_completed(request_id, signatures),
    //                                     sign_failed(request_id, error_code, message)
    virtual void sign_messages(const godot::PackedByteArray & messages_concat,
                                const godot::PackedInt32Array & lengths,
                                const godot::String & request_id) = 0;
    virtual void sign_transactions(const godot::PackedByteArray & txs_concat,
                                    const godot::PackedInt32Array & lengths,
                                    const godot::String & request_id) = 0;
};

}  // namespace godot_solana_sdk
```

**Backward compatibility shim:** existing `Transaction::sign(keypair)` continues to work unchanged in one minor version (v1.1 after MWA lands); internally it wraps the `Keypair` in a `LocalKeypairSigner : ISigner`. A deprecation warning logs at first use. Removed in v1.2. Full migration guide at `docs/migrations/isigner.md`. Upstream maintainer sign-off required before merging the refactor to `godot-solana-sdk` core — recorded in this doc via `upstream-signoff.md` commit reference.

### 2.4 Component Boundaries

| Component | Hard Boundary | Why |
|---|---|---|
| A ↔ B | GDScript ↔ C++ FFI | Crossing engine abstraction |
| B ↔ C | C++ interface (platform-agnostic) | AC-A-4 enforces single platform conditional |
| C ↔ D,E | JNI call boundary | Native / JVM split |
| E ↔ F | Kotlin interface | Enables `FakeMwaClient` (AC-D-4) without real wallet |
| H internal | Keystore MasterKey isolation | AC-NFR-SEC-1 — SecretString never crosses this boundary unwrapped |

### 2.5 Thread Contexts

| Context | Lives in | Owns |
|---|---|---|
| Godot main thread | Engine process | All signal emissions to user code, all GDScript execution |
| Android main (UI) thread | Android process (same process as Godot) | `MwaAndroidPlugin` public API; Activity lifecycle callbacks |
| clientlib-ktx coroutine IO scope | Android process | Protocol I/O, WebSocket reads, Intent launching |
| C++ JNI callback thread | Varies | Transient; only to post into Godot-main queue via `call_deferred` |

## 3. Data Models

### 3.1 Cache Record (`SecureTokenStore` entry)

**Storage:** `EncryptedSharedPreferences` backed by AES256_GCM MasterKey in Android Keystore.

**Key:** `"mwa::v1::" + SHA256(canonical(cluster, chain_id, wallet_package, identity_uri))` — a deterministic fingerprint. `canonical(a, b, c, d)` concatenates **length-prefixed** fields: `len(a):4 || a || len(b):4 || b || …` with 4-byte big-endian lengths. This prevents collisions when any field contains a literal delimiter (e.g., `|` inside a URL query string). Prefix `mwa::v1::` version-locks the schema so a future migration can detect and upgrade.

**Value:** JSON string encrypted by `EncryptedSharedPreferences` automatically.

```json
{
  "schema": 1,
  "auth_token": "<opaque base64 string from clientlib-ktx>",
  "wallet_package": "com.phantom.app",
  "wallet_label": "Phantom",
  "wallet_icon_uri": "https://…/phantom.png",
  "public_key": "<base58 pubkey>",
  "cluster": "devnet",
  "chain_id": "solana:devnet",
  "identity_uri": "https://example.com/godot-mwa-sample",
  "issued_at_ms": 1713264000000,
  "last_used_at_ms": 1713264000000
}
```

**Rationale:**
- `cluster` + `chain_id` + `wallet_package` + `identity_uri` in the cache key prevent cluster-bleed (AC-NFR-SEC-4).
- `issued_at_ms` / `last_used_at_ms` enable future TTL policies without schema bump (reserved; not enforced in v1).
- `auth_token` is stored as `SecretString` in memory; serialized to ciphertext only at the `EncryptedSharedPreferences` boundary.

### 3.2 Signal Payload Schemas

All signals emit a single `Dictionary` argument (AC-D-10 — additive keys only).

**`connect_completed`**
```gdscript
{
  "request_id": String,       # UUID v4 threaded through all 3 layers
  "auth_token_fingerprint": String,  # Short hash of token for logging (not the token itself)
  "public_key": String,       # base58-encoded
  "wallet_label": String,     # "Phantom", "Solflare", …
  "wallet_icon_uri": String,  # may be ""
  "cluster": String,          # "mainnet-beta" | "devnet" | "testnet"
}
```

**`reauthorize_completed`** — same shape as `connect_completed`.

**`sign_messages_completed`**
```gdscript
{
  "request_id": String,
  "signed_payloads": Array[PackedByteArray],  # length matches input
}
```

**`sign_transactions_completed`**
```gdscript
{
  "request_id": String,
  "signed_transactions": Array[PackedByteArray],  # length matches input
}
```

**`sign_and_send_completed`**
```gdscript
{
  "request_id": String,
  "signatures": Array[String],           # base58 tx signatures
  "submitted_at": int,                    # unix ms
  "confirmation_status": "submitted",     # explicit: NOT "confirmed"
}
```

**`deauthorize_completed`**
```gdscript
{
  "request_id": String,
  "remote_revoke_succeeded": bool,
  "local_cache_cleared": bool,  # always true (AC-NFR-SEC-5)
  "warning": String,            # "" or "remote_unreachable"
}
```

**`disconnect_completed`**
```gdscript
{ "request_id": String }
```

**`mwa_error`** (single error signal — covers all error and cancel cases; AC-U-3)
```gdscript
{
  "request_id": String,
  "code": String,                # enum from ErrorTaxonomy (string form)
  "user_message": String,        # human-readable, safe for in-game display
  "developer_details": String,   # technical detail, safe for logs
  "recoverable": bool,
  "retry_hint": String,          # "retry_now" | "reauthorize" | "connect" | "none"
  "layer": String,               # "gdscript" | "cpp" | "kotlin" | "clientlib" | "wallet"
  "cause": String,               # upstream exception message, redacted (may be "")
  "source_method": String,       # "connect" | "reauthorize" | "sign_messages" | …
}
```

**`mwa_timeout`** — emitted by watchdog (AC-A-8).
```gdscript
{
  "request_id": String,
  "source_method": String,
  "elapsed_ms": int,
  "watchdog_ms": int,  # default 60000
}
```

**`mwa_cancelled_lifecycle`** — emitted when Activity is destroyed mid-op (AC-A-2).
```gdscript
{
  "request_id": String,
  "source_method": String,
  "reason": String,  # "activity_destroyed" | "process_backgrounded" | …
}
```

**`reauth_required`** — emitted on Keystore corruption recovery (AC-NFR-SEC-3).
```gdscript
{ "reason": String }  # "keystore_corrupt" | "token_revoked_remotely"
```

### 3.3 Error Taxonomy Manifest (`error-codes.yaml`)

Single source of truth; codegen script produces three artifacts at build time (AC-A-3).

```yaml
# tools/error-codes.yaml
schema_version: 1
codes:
  - name: USER_CANCELED
    cpp_enum: USER_CANCELED
    kotlin_enum: UserCanceled
    recoverable: true
    retry_hint: retry_now
    default_user_message: "You cancelled the wallet prompt."
  - name: WALLET_REJECTED
    cpp_enum: WALLET_REJECTED
    kotlin_enum: WalletRejected
    recoverable: true
    retry_hint: retry_now
    default_user_message: "The wallet rejected the request."
  - name: NO_MWA_WALLET_INSTALLED
    cpp_enum: NO_MWA_WALLET_INSTALLED
    kotlin_enum: NoMwaWalletInstalled
    recoverable: true
    retry_hint: install_wallet
    default_user_message: "No Solana wallet installed. Install one to continue."
  - name: WALLET_UNINSTALLED
    cpp_enum: WALLET_UNINSTALLED
    kotlin_enum: WalletUninstalled
    recoverable: true
    retry_hint: connect
    default_user_message: "The previously connected wallet is no longer installed."
  - name: TOKEN_EXPIRED
    cpp_enum: TOKEN_EXPIRED
    kotlin_enum: TokenExpired
    recoverable: true
    retry_hint: connect
    default_user_message: "Your wallet session has expired. Please reconnect."
  - name: RPC_FAILED
    cpp_enum: RPC_FAILED
    kotlin_enum: RpcFailed
    recoverable: true
    retry_hint: retry_now
    default_user_message: "The Solana network rejected the transaction."
  - name: NETWORK_OFFLINE
    cpp_enum: NETWORK_OFFLINE
    kotlin_enum: NetworkOffline
    recoverable: true
    retry_hint: retry_now
    default_user_message: "No internet connection."
  - name: TIMEOUT
    cpp_enum: TIMEOUT
    kotlin_enum: Timeout
    recoverable: true
    retry_hint: retry_now
    default_user_message: "The operation timed out."
  - name: NOT_CONNECTED
    cpp_enum: NOT_CONNECTED
    kotlin_enum: NotConnected
    recoverable: true
    retry_hint: connect
    default_user_message: "Not connected to a wallet. Call MWA.connect() first."
  - name: UNSUPPORTED_PLATFORM
    cpp_enum: UNSUPPORTED_PLATFORM
    kotlin_enum: UnsupportedPlatform
    recoverable: false
    retry_hint: none
    default_user_message: "MWA is only available on Android."
  - name: PROTOCOL_ERROR
    cpp_enum: PROTOCOL_ERROR
    kotlin_enum: ProtocolError
    recoverable: false
    retry_hint: connect
    default_user_message: "Wallet communication failed."
  - name: STORAGE_CORRUPT
    cpp_enum: STORAGE_CORRUPT
    kotlin_enum: StorageCorrupt
    recoverable: true
    retry_hint: connect
    default_user_message: "Secure storage is unavailable. Please reconnect."
  - name: REAUTH_REQUIRED
    cpp_enum: REAUTH_REQUIRED
    kotlin_enum: ReauthRequired
    recoverable: true
    retry_hint: connect
    default_user_message: "Your session needs renewal."
```

**Codegen targets:**
- `android/plugin/src/generated/kotlin/…/MwaError.kt` — sealed enum
- `src/generated/mwa_error_codes.hpp` — C++ `enum class MwaErrorCode` + name-to-string table
- `addons/SolanaSDK/mwa_error_codes.gd` — GDScript `const` Dictionary / enum

CI drift check (AC-A-3): regenerate → `git diff --exit-code` fails build on drift.

### 3.4 GDScript Public API Shape (see §6 for full API)

```
MWA (autoload singleton)
 ├─ Signals:
 │   ├─ connect_completed(data: Dictionary)
 │   ├─ reauthorize_completed(data: Dictionary)
 │   ├─ disconnect_completed(data: Dictionary)
 │   ├─ deauthorize_completed(data: Dictionary)
 │   ├─ sign_messages_completed(data: Dictionary)
 │   ├─ sign_transactions_completed(data: Dictionary)
 │   ├─ sign_and_send_completed(data: Dictionary)
 │   ├─ mwa_error(data: Dictionary)
 │   ├─ mwa_timeout(data: Dictionary)
 │   ├─ mwa_cancelled_lifecycle(data: Dictionary)
 │   └─ reauth_required(data: Dictionary)
 ├─ State introspection (synchronous, ≤1ms):
 │   ├─ is_supported() -> bool
 │   ├─ is_connected() -> bool
 │   ├─ get_public_key() -> String
 │   ├─ get_cluster() -> String
 │   ├─ get_wallet_label() -> String
 │   └─ get_auth_token_fingerprint() -> String
 └─ Async methods (return void, emit signals):
     ├─ connect(identity: Dictionary, cluster: String, opts := {}) -> void
     ├─ reauthorize(opts := {}) -> void
     ├─ disconnect() -> void
     ├─ deauthorize() -> void
     ├─ sign_messages(messages: Array[PackedByteArray], opts := {}) -> void
     ├─ sign_transactions(transactions: Array[PackedByteArray], opts := {}) -> void
     ├─ sign_and_send(transactions: Array[PackedByteArray], opts := {}) -> void
     ├─ clear_local_cache() -> void             # synchronous; no network
     ├─ forget_all() -> void                    # GDPR/CCPA sign-out
     ├─ get_suggested_wallet_install_links() -> Array[Dictionary]
     ├─ get_device_posture() -> Dictionary
     └─ set_verbose(enabled: bool) -> void
```

`opts` is a common knob-bag: `{timeout_ms: int = 120000, request_id: String = "", debug_mode: bool = false}`.

## 4. Key Workflows

### 4.1 Workflow A — First-Time Connect (UC-1 / FR-2)

```
GDScript                C++ GDExt          Kotlin Plugin         clientlib-ktx       Wallet
   │                        │                    │                     │                │
   │ connect(id,cluster)    │                    │                     │                │
   │───────────────────────▶│                    │                     │                │
   │                        │ request_id=UUID    │                     │                │
   │                        │ inflight.put(rid)  │                     │                │
   │                        │ bridge.connect(…)  │                     │                │
   │                        │───────────────────▶│                     │                │
   │ return void (≤16ms)    │                    │ launch coroutine    │                │
   │◀───────────────────────│                    │ transact{…}         │                │
   │                        │                    │────────────────────▶│ start session  │
   │                        │                    │                     │───────────────▶│
   │                        │                    │                     │                │ chooser UI
   │                        │                    │                     │                │ player approves
   │                        │                    │                     │◀───────────────│ AuthorizationResult
   │                        │                    │◀────────────────────│                │
   │                        │                    │ store in            │                │
   │                        │                    │ SecureTokenStore    │                │
   │                        │                    │ (encrypted)         │                │
   │                        │                    │ updateSessionState  │                │
   │                        │                    │ emitSignal(...)     │                │
   │                        │◀───────────────────│ (on Android main)   │                │
   │                        │ call_deferred(     │                     │                │
   │                        │   emit_signal(...))│                     │                │
   │                        │ (to Godot main)    │                     │                │
   │◀───────────────────────│                    │                     │                │
   │ connect_completed{...} │                    │                     │                │
```

**Failure branches**:
- Player cancels chooser → `mwa_error{code=USER_CANCELED}`
- No MWA wallet installed → preflight check emits `mwa_error{code=NO_MWA_WALLET_INSTALLED}`
- Protocol error → `mwa_error{code=PROTOCOL_ERROR, layer=clientlib, cause=redacted}`
- Watchdog exceeds `timeout_ms` → `mwa_timeout{...}` (op considered lost; token NOT cached)

### 4.2 Workflow B — Silent Reauthorize on Relaunch (UC-2 / FR-4, NFR-5)

```
App cold start
   │
   ▼
GDScript auto-calls MWA.reauthorize() in _ready()
   │
   ▼
C++ → JNI → Kotlin plugin
   │
   ▼
SecureTokenStore.getToken(cluster, chain_id, wallet_package, identity_uri)
   │                                                         │
   │ found                                                   │ not found / corrupt
   ▼                                                         ▼
clientlib-ktx.transact { authToken = cached }           mwa_error{code=NOT_CONNECTED}
   │                                                         (caller must call connect())
   ▼
Wallet validates silently (NO UI)
   │
   ▼
AuthorizationResult (same pubkey expected)
   │
   ▼
emitSignal reauthorize_completed{...}
   │
   ▼
Godot main thread → user game ready
```

**Failure**: wallet rejects cached token → `mwa_error{code=TOKEN_EXPIRED, retry_hint=connect}`. SDK does NOT auto-launch chooser (AC-U-8).

**Critical**: NFR-5 requires 0 UI prompts on the success path. Any log of "launched Intent" or Activity transition to the wallet app is a bug. Integration test asserts Activity stack on the Godot app side remains unchanged for the duration of reauthorize.

### 4.3 Workflow C — Sign and Send Transactions (UC-4 / FR-9)

Same shape as Workflow A, but preflight adds: `if (not is_connected()) emit mwa_error{code=NOT_CONNECTED} within 1 frame` (AC-U-2). Success payload includes `confirmation_status: "submitted"`, with docs making clear confirmation is the caller's job via existing `godot-solana-sdk` RPC nodes.

### 4.4 Workflow D — Activity Lifecycle During In-Flight Op (AC-A-2)

```
MWA.connect(...)  in flight
   │
   ▼
Player rotates device / low-mem kill
   │
   ▼
Android lifecycle: onDestroy called
   │
   ▼
MwaAndroidPlugin observes Lifecycle.Event.ON_DESTROY
   │
   ▼
cancels all in-flight coroutines
   │
   ▼
For each pending request_id:
   emitSignal("mwa_cancelled_lifecycle", {request_id, source_method, reason="activity_destroyed"})
```

On process death (system kill before `onDestroy` fires), in-flight op state is LOST — the next app launch must rely on cached auth_token (FR-14) but cannot resume a half-completed signing. Documented; not a bug.

### 4.5 Workflow E — Deauthorize (UC-5 / FR-5)

```
MWA.deauthorize()
   │
   ▼
Kotlin plugin:
   try {
     clientlib.revoke(auth_token)  // remote best-effort
     remote_succeeded = true
   } catch (e) {
     remote_succeeded = false
     SdkLog.w(...)
   } finally {
     SecureTokenStore.deleteToken(currentKey())  // MUST always run
     MwaSessionState.clear()
   }
   emitSignal("deauthorize_completed", {
     remote_revoke_succeeded: remote_succeeded,
     local_cache_cleared: true,
     warning: if (!remote_succeeded) "remote_unreachable" else ""
   })
```

**Invariant**: AC-NFR-SEC-5 — local cache MUST be cleared regardless of remote outcome. `finally` block enforces this; unit test simulates network-off.

### 4.6 Workflow F — Error Propagation Across 3 Layers

```
clientlib-ktx throws MobileWalletAdapterException (Kotlin)
   │
   ▼
MwaAndroidPlugin catches, maps via ErrorTaxonomy:
   MwaError.WalletRejected, developer_details = exception.message
   │
   ▼
emitSignal("mwa_error", {...serialized dictionary...})
   │  (via Godot-provided JNI helper; lands on Android main)
   ▼
C++ GDExtension receives on JNI callback thread
   │
   ▼
MobileWalletAdapter::on_mwa_error(java_dict):
   Dictionary data = to_gd_dictionary(java_dict);
   call_deferred("emit_signal", "mwa_error", data);
   │
   ▼
Godot main thread drains deferred queue:
   emit_signal("mwa_error", data)
   │
   ▼
User's GDScript connected handler fires
```

## 5. Error Handling Strategy

### 5.1 Error Classes

| Class | Layer Origin | Example | Transport |
|---|---|---|---|
| User action | Wallet | User rejects signing | `mwa_error{code=USER_CANCELED, recoverable=true}` |
| Protocol | clientlib-ktx | Malformed JSON-RPC | `mwa_error{code=PROTOCOL_ERROR, recoverable=false}` |
| Network | OS | No connectivity | `mwa_error{code=NETWORK_OFFLINE, recoverable=true}` |
| Expected lifecycle | Android | Activity destroyed | `mwa_cancelled_lifecycle{...}` |
| Storage | Android Keystore | Corrupt ciphertext | `reauth_required{reason="keystore_corrupt"}` + wipe |
| Developer error | GDScript | Sign without connect | `mwa_error{code=NOT_CONNECTED, recoverable=true}` |
| Platform | Non-Android | iOS/editor/desktop | `mwa_error{code=UNSUPPORTED_PLATFORM, recoverable=false}` |
| Timeout | Watchdog | No response in 60s | `mwa_timeout{...}` |

### 5.2 Propagation Rules

1. **Every error has a `request_id`** tied to the originating call (AC-D-1 / AC-A-8).
2. **Every error has a `layer` field** identifying where it originated (gdscript / cpp / kotlin / clientlib / wallet).
3. **No stack traces cross the FFI boundary unfiltered** — `developer_details` is a single-line message, no file paths or proprietary wallet info.
4. **Logging at each layer** uses the request_id as correlation key: `MWA/{layer}/{corrId}`.
5. **Release builds strip V/D logs** via R8 `-assumenosideeffects` on `SdkLog.v/d/i`.
6. **Never log token bytes** — `SecretString.toString() = "<redacted>"`; CI grep bans direct `Log.*(authToken)` (AC-D-8).

### 5.3 Recovery Patterns

| Code | Recovery |
|---|---|
| `USER_CANCELED` / `WALLET_REJECTED` | Show friendly message; caller may retry with a new prompt |
| `NO_MWA_WALLET_INSTALLED` | Present `get_suggested_wallet_install_links()` UI in-game |
| `TOKEN_EXPIRED` / `REAUTH_REQUIRED` | Caller invokes `connect()`; do NOT auto-launch chooser |
| `STORAGE_CORRUPT` | SDK auto-wipes and emits `reauth_required`; caller invokes `connect()` |
| `TIMEOUT` | Caller retries or shows "wallet not responding" |
| `UNSUPPORTED_PLATFORM` | Caller renders feature-gated UI (desktop/editor) |

### 5.4 Watchdog

See §7.3 for the full watchdog + terminal-signal invariant. Summary: effective watchdog = `min(60s internal default, opts.timeout_ms default 120s)`; on expiry emit `mwa_timeout{...}` as the terminal signal for that `request_id`; late results are dropped and counted.

### 5.5 Logging Signatures (AC-A-17 new)

`SdkLog.v/d/i/w/e` accept `() -> String` lambdas, not concrete strings:

```kotlin
SdkLog.d(TAG, corrId) { "token = $token" }  // argument evaluation stripped by R8 in release
// NOT: SdkLog.d(TAG, corrId, "token = $token")  // banned by CI lint
```

A CI grep rejects `SdkLog\.(v|d|i|w|e)\([^{]*\$\{` (string interpolation without lambda). This prevents the classic R8 foot-gun where `-assumenosideeffects` strips the call but argument evaluation still runs.

## 6. Library Public API Surface (GDScript)

Full public surface is the GDScript facade `MWA.gd` — an autoload singleton added by the addon. The C++ `MobileWalletAdapter` node is also exposed as a class but most users will use the singleton.

### 6.1 Signals

| Signal | Payload | Emitted by |
|---|---|---|
| `connect_completed` | {request_id, auth_token_fingerprint, public_key, wallet_label, wallet_icon_uri, cluster} | Success path of `connect()` |
| `reauthorize_completed` | same as `connect_completed` | Success path of `reauthorize()` |
| `disconnect_completed` | {request_id} | Success path of `disconnect()` |
| `deauthorize_completed` | {request_id, remote_revoke_succeeded, local_cache_cleared, warning} | Always fires for `deauthorize()` (cache always cleared) |
| `sign_messages_completed` | {request_id, signed_payloads} | Success path of `sign_messages()` |
| `sign_transactions_completed` | {request_id, signed_transactions} | Success path of `sign_transactions()` |
| `sign_and_send_completed` | {request_id, signatures, submitted_at, confirmation_status} | Success path of `sign_and_send()` |
| `mwa_error` | {request_id, code, user_message, developer_details, recoverable, retry_hint, layer, cause, source_method} | Any error or cancellation |
| `mwa_timeout` | {request_id, source_method, elapsed_ms, watchdog_ms} | Watchdog fires |
| `mwa_cancelled_lifecycle` | {request_id, source_method, reason} | Activity destroyed mid-op |
| `reauth_required` | {reason} | Keystore corruption or remote revocation detected |

### 6.2 Methods — Public Shape

**State introspection (synchronous, ≤1 ms):**
```gdscript
MWA.is_supported() -> bool
MWA.is_connected() -> bool
MWA.get_public_key() -> String      # "" if not connected
MWA.get_cluster() -> String         # "" if not connected
MWA.get_wallet_label() -> String    # "" if not connected
MWA.get_auth_token_fingerprint() -> String  # non-sensitive id for debug/logs
```

**Async operations (return void, fire signals):**
```gdscript
MWA.connect(identity: Dictionary, cluster: String, opts := {}) -> void
# identity = {name: String, icon_uri: String, identity_uri: String}
# cluster ∈ {"mainnet-beta", "devnet", "testnet"}
# opts = {timeout_ms: int = 120000, request_id: String = "", debug_mode: bool = false}

MWA.reauthorize(opts := {}) -> void
MWA.disconnect() -> void
MWA.deauthorize() -> void

MWA.sign_messages(messages: Array[PackedByteArray], opts := {}) -> void
MWA.sign_transactions(transactions: Array[PackedByteArray], opts := {}) -> void
MWA.sign_and_send(transactions: Array[PackedByteArray], opts := {}) -> void
```

**Utility:**
```gdscript
MWA.clear_local_cache() -> void                           # synchronous; no network
MWA.forget_all() -> void                                   # GDPR/CCPA wipe all cached entries + rotate MasterKey
MWA.get_suggested_wallet_install_links() -> Array[Dictionary]  # [{name, package_id, play_store_url}]
MWA.get_device_posture() -> Dictionary                     # {rooted, debuggable, developer_options_on, adb_enabled}
MWA.set_verbose(enabled: bool) -> void
MWA.get_diagnostics() -> Dictionary                        # debuggability surface (AC-D-22 new)
```

**`get_diagnostics()` payload** (non-sensitive; safe to attach to bug reports):
```gdscript
{
  "sdk_version": "1.0.0",
  "clientlib_ktx_version": "2.x.y",
  "godot_version": "4.3.x",
  "android_api_level": 34,
  "session_state": "connected" | "disconnected" | "reauthorizing" | "signing" | "error",
  "wallet_package": "com.phantom.app",
  "wallet_version": "25.3.0",
  "auth_token_fingerprint": "ab3f9c21",
  "cluster": "devnet",
  "last_n_correlation_trace": [
    {"request_id": "...", "source_method": "connect", "terminal_signal": "connect_completed", "elapsed_ms": 2340, "timestamp_ms": ...},
    ...
  ],  # ring buffer, N=20
  "late_result_count": 0,
  "pending_submissions_count": 0
}
```

**Auth token fingerprint algorithm** (AC-D-26 new; salt size aligned by A-16 2026-04-29): `HKDF-SHA256(ikm = auth_token_bytes, salt = per_install_salt, info = "mwa-fingerprint-v1")` → first 4 bytes → 8 hex chars. `per_install_salt` is a 32-byte random value generated at first addon init (per `SecureTokenStore.FINGERPRINT_SALT_BYTES = 32` — aligned to NIST SP 800-108 salt-length ≥ hash-output recommendation for SHA256), stored under Keystore alias `godot-sdk-mwa-fingerprint-salt-v1`, stable for the app install, rotated only on `forget_all`. Guarantees: (a) 0 token entropy leaked, (b) same token → same fingerprint for the life of the install, (c) different installs of the same app generate different fingerprints for the same token (privacy-preserving), (d) 32-bit output → ~2^16 collision after birthday bound — acceptable for debug correlation, explicitly not a security identifier.

### 6.3 Usage Example (copy-pasteable for docs FR-15)

```gdscript
extends Node

func _ready() -> void:
    if not MWA.is_supported():
        $ConnectButton.disabled = true
        $Status.text = "MWA is Android-only"
        return

    MWA.mwa_error.connect(_on_error)
    MWA.connect_completed.connect(_on_connected)

    # Try silent reauth first
    if MWA.get_auth_token_fingerprint() != "":
        MWA.reauthorize()
        var result = await MWA.reauthorize_completed
        if result.public_key != "":
            $Status.text = "Reconnected: %s" % result.wallet_label
            return

func _on_connect_pressed() -> void:
    MWA.connect(
        {
            "name": "My Godot Game",
            "icon_uri": "https://example.com/icon.png",
            "identity_uri": "https://example.com/my-game",
        },
        "devnet",
    )

func _on_connected(data: Dictionary) -> void:
    $Status.text = "Connected: %s (%s)" % [data.public_key, data.wallet_label]

func _on_error(data: Dictionary) -> void:
    $Status.text = "%s: %s" % [data.code, data.user_message]
```

## 7. Event-Driven / Async Architecture

### 7.1 Thread Marshalling Primitive (AC-A-1, AC-A-11 new)

**Problem:** Kotlin callbacks land on Android main thread or clientlib-ktx coroutine IO; `emit_signal` can only run on Godot main thread.

**Single rule (no fallback path):** *Every* signal to GDScript crosses to the Godot main thread via `call_deferred` in the C++ `MobileWalletAdapter` node. Kotlin **never** emits signals directly to GDScript. The `GodotAndroidPlugin.emitSignal` helper is **not** used for user-facing signals in this SDK — we use our own dispatcher to keep the thread contract verifiable.

**Flow:**
1. Kotlin plugin completes (or fails) an op on its coroutine scope.
2. Kotlin invokes a JNI callback `onMwaResult(requestId, payloadJson)` on whatever thread the coroutine resumed on.
3. C++ JNI callback — on that arbitrary thread — calls `GodotMainDispatcher::post(...)` which wraps `call_deferred("emit_signal", signal_name, data)` on the singleton `MobileWalletAdapter` node.
4. Godot main thread drains the deferred queue next frame; signal fires on main; GDScript handler runs.

**`GodotMainDispatcher`** is a named component added to §2 — a tiny C++ class holding a reference to the singleton node and exposing `post(signal_name, data)`. All Kotlin → GDScript signal paths MUST route through it. An architectural test greps Kotlin source for any direct `emitSignal` to the host Godot plugin (bans), and greps C++ for any `emit_signal` outside the dispatcher (bans).

**Test (AC-A-1):** unit test with `MockMwaAndroidBridge` invokes a callback from a `std::thread` not on Godot main; asserts signal is observed within 1 frame on main thread AND that the count of direct `emit_signal` calls (not via dispatcher) is 0.

**Caching JNIEnv:** the JNI layer caches `JavaVM*` (not `JNIEnv*`) at load time and uses `AttachCurrentThread` when invoked from non-JVM threads. `DetachCurrentThread` on drop. This prevents the classic "invalid JNIEnv" crash when callbacks arrive on unknown threads.

### 7.1.1 ABI Hygiene (AC-A-16 new)

Public C++ headers of the GDExtension boundary use **only** `godot::` types — `godot::Variant`, `godot::String`, `godot::Dictionary`, `godot::PackedByteArray`, `godot::Ref<T>`. No `std::string`, `std::vector`, `std::unique_ptr`, or other STL across the GDExtension ABI. This is the only discipline that survives multiple toolchains (MSVC vs MinGW vs Clang; libc++ vs libstdc++; NDK r23 vs r25) without silent binary-incompatibility.

`SecretString` on the C++ side is a POD-like wrapper holding engine-allocated `PackedByteArray` storage, with `to_string()` returning the literal `"<redacted>"` (see A-7 — `operator<<` dropped because it would require `<ostream>` and violate DD-26). No STL internals.

A CI header-lint job scans public headers under `src/mwa/include/` and fails the build on any STL type reference.

### 7.2 Correlation IDs

- Generated at the GDScript layer (or passed in `opts.request_id`); UUID v4.
- Threaded through all 3 layers: GDScript → C++ (`request_id` param on bridge method) → Kotlin (param on `@UsedByGodot` method).
- Logged by `SdkLog` at each layer: `tag="MWA", format="[corrId=xxxx] message"`.
- Echoed in every success and error signal payload — developer can match logs ↔ signals.

### 7.3 Watchdog & Terminal-Signal Invariant (AC-A-12 new)

**Rules:**
1. Every op has exactly one **effective watchdog** = `min(internal_default=60000, opts.timeout_ms=120000)` ms. There is no nested-timeout scheme — a single `kotlinx.coroutines.withTimeoutOrNull(effective)` wraps `clientlib.transact{}`.
2. On timeout expiry: cancel coroutine, emit `mwa_timeout{request_id, source_method, elapsed_ms, watchdog_ms}`, mark the `request_id` as **terminated** in an internal inflight map.
3. **Terminal-signal invariant**: for any given `request_id`, at most one terminal signal ever fires across `*_completed`, `mwa_error`, `mwa_timeout`, `mwa_cancelled_lifecycle`. Late results arriving after a terminal signal are logged via `SdkLog.w("late_result", corrId)`, incremented in a `late_result_count` metric surfaced by `get_diagnostics()`, and **dropped**.
4. Caller treats `mwa_timeout` as a lost op; state not updated.

### 7.4 Pending-Submission Breadcrumb (AC-A-13 new)

For `sign_and_send` only — other ops do not need this protection.

**Write-ahead breadcrumb:** *Before* `clientlib.transact{}` is invoked, Kotlin writes a `PendingSubmission` record to `SecureTokenStore` under key `"pending::" + request_id`:

```json
{
  "request_id": "uuid",
  "op_type": "sign_and_send",
  "cluster": "devnet",
  "identity_uri": "...",
  "started_at_ms": 1713264000000,
  "tx_count": 3,
  "tx_preview_hashes": ["sha256-first-32B", ...]
}
```

**Cleanup:** on success OR terminal error OR watchdog timeout, the breadcrumb is deleted.

**Surface on next launch:** on each `connect`/`reauthorize` success, `SecureTokenStore.listStalePendingSubmissions(older_than_ms=10_000)` is scanned. For each stale entry, emit:

```gdscript
pending_submission_found({
  request_id: String,
  op_type: "sign_and_send",
  started_at_ms: int,
  tx_count: int,
  tx_preview_hashes: Array[String],
  recommendation: "check_chain_for_signatures"
})
```

The GDScript caller is responsible for querying `getSignatureStatuses` via existing `SolanaClient` nodes to determine whether the transactions landed. This is a deliberate trade-off — the SDK cannot autonomously confirm on-chain state without requiring an RPC endpoint config. Documented in user-facing `docs/safety.md` as "sign-and-send is at-least-once from the caller's perspective; treat breadcrumbs as authoritative for post-crash recovery."

### 7.5 Lifecycle Observer

### 7.5 Lifecycle Observer

- Kotlin plugin registers `DefaultLifecycleObserver` on Godot's Activity.
- On `ON_DESTROY`: cancel all in-flight coroutines; for each `request_id`, emit `mwa_cancelled_lifecycle` (terminal per 7.3).
- On process death (no `ON_DESTROY` fires), in-flight op state is lost — only the auth token AND `sign_and_send` breadcrumbs (per 7.4) survive.

## 8. Security

### 8.1 Trust Boundaries

```
┌─────────────────────────────────────────────────────┐
│ UNTRUSTED: User's device, network, OS               │
│                                                     │
│   ┌───────────────────────────────────────────┐    │
│   │ Godot App Process (Android)               │    │
│   │                                           │    │
│   │   ┌─────────────────────────────────┐     │    │
│   │   │ Android Keystore (TEE-backed)   │     │    │
│   │   │   MasterKey (AES256_GCM)        │     │    │
│   │   └─────────────────────────────────┘     │    │
│   │            │ wraps                         │    │
│   │            ▼                               │    │
│   │   EncryptedSharedPreferences               │    │
│   │   (AES256_GCM ciphertext on disk)          │    │
│   │                                            │    │
│   │   In-memory SecretString (ephemeral)       │    │
│   └───────────────────────────────────────────┘    │
│                                                     │
│   Wallet App (separate process, OS-isolated)        │
│   └─ Private keys never leave wallet process        │
└─────────────────────────────────────────────────────┘
```

### 8.2 Threat Model (MASVS-aligned)

| Threat | Control | AC Ref |
|---|---|---|
| Device backup extracts token | `android:allowBackup=false` + `fullBackupContent` excludes prefs; merged-manifest CI check | AC-NFR-SEC-2 |
| `adb backup` / `run-as` on debuggable device | `android:debuggable=false` in release; `MWA.get_device_posture()` for app-side gating | AC-NFR-SEC-13 |
| Keystore invalidation (lockscreen reset) | Catch `GeneralSecurityException` + `InvalidProtocolBufferException`; wipe + `reauth_required` signal | AC-NFR-SEC-3 |
| Cluster bleed (devnet token replay on mainnet) | Cache key = `(cluster, chain_id, wallet_package, identity_uri)`; SDK refuses mismatched cluster | AC-NFR-SEC-4 |
| Deauth leaves stale local token | `finally` block in deauthorize; network-off unit test | AC-NFR-SEC-5 |
| Logcat leaks token | `SdkLog` wrapper + R8 `-assumenosideeffects` + CI grep + `SecretString` | AC-NFR-SEC-6, AC-D-8 |
| Intent redirection / StrandHogg 2.0 | Activity flags: `taskAffinity=""`, `launchMode="singleTask"`, `android:exported` narrowed; verify calling package | AC-NFR-SEC-7 |
| Malicious dependency in transitive graph | `gradle.lockfile` + `dependency-verification.xml`; reproducible-build CI | AC-NFR-SEC-8 |
| R8 strips required symbols in release | `consumer-rules.pro` shipped in AAR; release-variant smoke test | AC-NFR-SEC-9 |
| New dev signs real-value tx in example | Example defaults to devnet; mainnet toggle requires explicit opt-in | AC-NFR-SEC-10 |
| Player can't fully sign out | `MWA.forget_all()` clears every cached entry + rotates MasterKey | AC-NFR-SEC-11 |
| Identity-metadata phishing | Docs warn `identityName`/`iconUri` are attacker-controllable; signing prompt is sole trust anchor | AC-NFR-SEC-12 |
| Screenshot / Recents leakage | Docs recommend `FLAG_SECURE`; SDK does not enforce (game-specific decision) | AC-NFR-SEC-12 |

### 8.3 Secure Storage Implementation

```kotlin
class SecureTokenStore(private val context: Context) {
    private val masterKey: MasterKey by lazy {
        MasterKey.Builder(context, MASTER_KEY_ALIAS)
            .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
            .build()
    }

    private val prefs: SharedPreferences by lazy {
        try {
            EncryptedSharedPreferences.create(
                context,
                PREFS_FILE_NAME,
                masterKey,
                EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
                EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM,
            )
        } catch (e: GeneralSecurityException) {
            wipeCorruptPrefs()
            throw StorageCorruptException(e)
        } catch (e: InvalidProtocolBufferException) {
            wipeCorruptPrefs()
            throw StorageCorruptException(e)
        }
    }

    fun getToken(key: CacheKey): SecretString? = …
    fun putToken(key: CacheKey, record: CacheRecord) = …
    fun deleteToken(key: CacheKey) = …
    fun deleteAll() = …

    private fun wipeCorruptPrefs() {
        context.deleteSharedPreferences(PREFS_FILE_NAME)
        // MasterKey entry remains; next create will regenerate ciphertext
    }

    companion object {
        const val MASTER_KEY_ALIAS = "godot-sdk-mwa-master-key-v1"
        const val PREFS_FILE_NAME = "godot-sdk-mwa-tokens-v1"
    }
}
```

`StorageCorruptException` is caught at the plugin boundary → emits `reauth_required{reason=keystore_corrupt}` → never crashes Godot.

### 8.3.1 `forget_all` Semantics (concern from review)

`forget_all()` executes under a coroutine mutex held by `MwaAndroidPlugin`:

```kotlin
suspend fun forgetAll() = mutex.withLock {
    // 1. Cancel all in-flight ops; emit mwa_cancelled_lifecycle per request_id
    inflight.cancelAll(reason = "forget_all_invoked")
    // 2. Best-effort remote deauthorize for every stored token
    store.listAllKeys().forEach { key -> runCatching { clientlib.revoke(store.getToken(key)) } }
    // 3. Wipe all ciphertext entries
    store.deleteAll()
    // 4. Rotate MasterKey alias v1 → v1 (delete + recreate)
    store.rotateMasterKey()
    // 5. Clear session state
    sessionState.clear()
}
```

Mutex prevents mid-rotation `EncryptedSharedPreferences` access exceptions. MasterKey alias stays as `godot-sdk-mwa-master-key-v1`; true alias bump (v1 → v2) is reserved for a future schema migration and gated by a `docs/adr/` entry.

### 8.4 Manifest Hardening

```xml
<!-- android/plugin/src/main/AndroidManifest.xml -->
<manifest>
    <application
        android:allowBackup="false"
        tools:replace="android:allowBackup"
        android:fullBackupContent="@xml/backup_rules"
        android:dataExtractionRules="@xml/data_extraction_rules">

        <!-- No exported activities introduced by this plugin -->
        <!-- No exported ContentProviders -->

    </application>
</manifest>
```

`backup_rules.xml`:
```xml
<full-backup-content>
    <exclude domain="sharedpref" path="godot-sdk-mwa-tokens-v1.xml"/>
</full-backup-content>
```

CI step (AC-NFR-SEC-2): build a synthetic consumer app that includes this plugin + its own `android:allowBackup="true"`; run `aapt2 dump` on the merged manifest and assert the backup rule for our prefs file is preserved.

### 8.5 ProGuard / R8 Rules

`android/plugin/consumer-rules.pro`:
```
# Keep JNI native method names
-keepclassmembers class com.godotengine.godot_solana_sdk.mwa.** {
    native <methods>;
}

# Keep @UsedByGodot methods (for reflection from GDExtension)
-keep @org.godotengine.godot.plugin.UsedByGodot class * {
    @org.godotengine.godot.plugin.UsedByGodot *;
}

# Keep clientlib-ktx entry points
-keep class com.solanamobile.mobilewalletadapter.** { *; }
-dontwarn com.solanamobile.mobilewalletadapter.**

# Strip verbose/debug logs in release (package corrected — see A-6)
-assumenosideeffects class com.godotengine.godot_solana_sdk.mwa.util.SdkLog {
    public static void v(...);
    public static void d(...);
}

# Keep Story 1-2 Task 6 R8-strip evidence sentinel from DCE (see A-6)
-keep class com.godotengine.godot_solana_sdk.mwa.util.R8Sentinel { *; }
```

## 9. Infrastructure & Deployment

**Scope:** this is a *library* — we do not operate backend infra. "Deployment" means:

1. **Distribution channels**
   - GitHub Releases: prebuilt `.so` / `.dll` / `.dylib` / `.aar` + source, per Godot minor supported
   - Godot Asset Library: post-release upload
   - Source: MIT on GitHub

2. **CI/CD (inherits upstream `.github/workflows/`, extended):**
   - `build.yml` — already builds GDExtension per platform; extend matrix to also build Android plugin AAR via `./gradlew :plugin:assembleRelease`
   - `clang_tools.yml` — existing; format + tidy C++ changes
   - New: `kotlin_lint.yml` — `./gradlew ktlintCheck`
   - New: `verify_parity.yml` — weekly cron diff vs RN SDK npm package (AC-D-3)
   - New: `instrumented_tests.yml` — macos-latest runner + emulator + Fake Wallet APK + `./gradlew connectedAndroidTest` (AC-A-7)
   - Existing `build.yml` android matrix extended to run `./gradlew test` (AC-D-4)
   - Existing `verify_docs.yml` extended to link-check new docs (AC-FR-11)
   - New check: `error-codes.yaml` codegen drift (AC-A-3)
   - New check: `manifest-merge-audit.yml` — consumer-merge simulation (AC-NFR-SEC-2)
   - New check: R8 release-variant smoke (AC-NFR-SEC-9)

3. **Reproducible build**
   - `gradle.lockfile` + `dependency-verification.xml` committed to repo
   - CI rebuilds AAR on two runners, asserts byte-identical output (AC-NFR-SEC-8)

4. **Secrets** — no CI secrets required for the free-tier flow. Firebase Test Lab (if used for release Phantom verification) is out-of-PR-CI and manually triggered with a workflow_dispatch input from a maintainer.

Production infra generation (Terraform etc.) is N/A for this project — `/devops` Pass 2 will still run but will document "library: no deployed infra; CI/CD + signing + release process only".

## 10. Testing Strategy

Resolves OQ-2 / DD-10 via the three-tier hybrid decided in Step 2.

### 10.1 Test Tiers

| Tier | Scope | Tools | Runs | AC Ref |
|---|---|---|---|---|
| T1 Unit (Kotlin) | `MwaAndroidPlugin`, `SecureTokenStore`, `MwaSessionState`, `SdkLog`, error-mapper | JUnit 5 + MockK + `FakeMwaClient` | Every PR (`./gradlew test`) | AC-D-4 |
| T2 Unit (C++) | `MobileWalletAdapter` node, thread-hop, signal emission, `MwaAndroidBridge` consumer side | GoogleTest + `MockMwaAndroidBridge` on Linux/Windows/macOS | Every PR (host-platform build) | AC-D-5 |
| T3 Instrumented (Android emulator + Fake Wallet) | Full stack end-to-end, all 7 MWA methods | `androidTest` + `reactivecircus/android-emulator-runner` + Fake Wallet APK | Every PR (macos-latest) | AC-A-7, AC-NFR-2, AC-NFR-5 |
| T4 Contract (MWA protocol fixtures) | JSON-RPC request shapes vs published MWA spec fixtures | T3 suite with fixture assertions | Every PR + on clientlib-ktx version bump | AC-A-7 |
| T5 GDScript integration | Signal contract, state introspection on non-Android | GUT (Godot Unit Test) in headless Godot | Every PR (multi-platform load) | AC-D-6 |
| T6 Release-gate Phantom | AC-NFR-3 10 silent-reauth cycles, 0 UI | Manual or Firebase Test Lab workflow_dispatch | Pre-release only | AC-NFR-3, AC-A-10 |

### 10.2 Patterns

- **AAA** (Arrange-Act-Assert) for unit tests.
- **Every test is deterministic** — no real network, no real wallet in T1/T2/T3/T5. Fake Wallet APK and `FakeMwaClient` provide canned responses.
- **TDD at the seams** — `ErrorTaxonomy` codegen, `SecureTokenStore` CRUD, thread-hop, manifest-merge assertion are test-first.
- **Golden files** — MWA protocol fixture JSONs checked into repo; contract tests diff against them.

### 10.3 Coverage Targets

- Kotlin: 90%+ line coverage on `MwaAndroidPlugin` + `SecureTokenStore` (hard targets; everything else opportunistic)
- C++: 80%+ on `MobileWalletAdapter` node and bridge interface consumer side
- GDScript: 70%+ on `MWA.gd` (harder to measure; key functions tested)
- **Integration**: 7/7 MWA methods covered by at least one T3 test (AC-NFR-2)

### 10.3.1 Fake/Real Contract Parity (AC-D-24 new)

Both `FakeMwaClient` (Kotlin unit tier T1) and the emulated Fake Wallet APK (instrumented tier T3) are exercised by the **same** JSON-RPC fixture set stored at `testdata/mwa-fixtures/*.json`. T4 contract tier runs an identical test suite twice: once against `FakeMwaClient`, once against a thin adapter that replays recorded clientlib-ktx traffic. Divergence between the two is PR-blocking. The compatibility matrix (AC-D-7) adds a row `"FakeMwaClient last verified against clientlib-ktx X.Y.Z @ commit <sha> on <date>"`.

### 10.3.2 Codegen Generator Tests (AC-D-23 new)

The `error-codes.yaml` → {Kotlin, C++, GDScript} generator script ships with:
- **Golden-file tests**: 3 minimal YAML inputs → expected 3-artifact output, asserted byte-for-byte.
- **Property tests**: randomly generated valid manifests round-trip through the generator; emitted Kotlin enum, C++ enum, and GDScript constants are parsed back and `recoverable` / `retry_hint` fields cross-verified for consistency.
- **Failure tests**: malformed YAML produces specific error messages.
- PR-blocking: generator test suite failure blocks merge **independent** of the drift-check job.

### 10.3.3 CI SLOs (AC-D-30 new)

- **Wall-clock P95**: ≤ 35 min per PR
- **Wall-clock hard ceiling**: 45 min — exceeding triggers a monthly audit issue
- **Flake rate T3**: ≤ 2% — auto-retry emulator-boot failure exactly once; no retries on T1/T2/T5
- **Quarterly CI audit**: maintainer-owned; results committed under `docs/ci-audit/YYYY-QN.md`

### 10.4 CI Matrix (additions to existing `build.yml`)

| Job | Runner | Time budget |
|---|---|---|
| `kotlin_unit` | ubuntu-latest | ≤2 min |
| `kotlin_ktlint` | ubuntu-latest | ≤1 min |
| `cpp_host_unit_tests` | ubuntu-latest + win + macos | ≤3 min each |
| `gdextension_load_multi_platform` | existing matrix | +0 (reuse) |
| `instrumented_emulator_fakewallet` | macos-latest | ≤15 min |
| `codegen_drift_check` | ubuntu-latest | ≤30 s |
| `manifest_merge_audit` | ubuntu-latest | ≤1 min |
| `reproducible_build` | ubuntu-latest (2 passes) | ≤10 min |
| `verify_parity` (weekly cron) | ubuntu-latest | ≤1 min |

Total per-PR CI overhead: ~30 min wall-clock (most jobs run in parallel).

## 11. Design Rationale — Full DD-* Log

Decisions from research.md are carried forward; new decisions are added here.

### Carried from Research

- **DD-1 [LOCKED]** Android-only; iOS out forever. *Rationale:* MWA protocol relies on local WebSocket + Intent — incompatible with iOS.
- **DD-2 [LOCKED]** Cache layer = `EncryptedSharedPreferences`. *Rationale:* spec-mandated; Android Keystore-backed; only sanctioned option.
- **DD-3 [LOCKED]** API mirrors RN SDK. *Rationale:* grant requirement; parity is the headline metric.
- **DD-4 [DISCRETION → LOCKED]** Use `com.solanamobile:mobile-wallet-adapter-clientlib-ktx`. *Rationale:* NFR-10; inherits protocol correctness from canonical impl.
- **DD-5 [DISCRETION → LOCKED]** Bridge via Godot 4 `GodotAndroidPlugin` + `emitSignal` (primary path) with MPSC+`call_deferred` fallback for JNI-only callbacks. *Rationale:* lifecycle-safe, main-thread-correct.
- **DD-6 [DISCRETION → LOCKED]** GDScript signals with `await` ergonomics. *Rationale:* idiomatic Godot; mirrors RN promises via `await`.
- **DD-7 [DISCRETION → LOCKED]** Support mainnet-beta + devnet + testnet from launch. *Rationale:* chain-agnostic protocol; zero marginal cost.
- **DD-8 [DISCRETION → LOCKED]** Minimal example app scope. *Rationale:* user directive.

### Resolved in this Phase

- **DD-9 [DEFERRED → LOCKED]** Godot 4.3.x pin.
  - **Alternatives considered:**
    - 4.4 / 4.5 latest stable — rejected; requires godot-cpp submodule rebase, Kotlin/Gradle bumps, out-of-MWA-scope churn.
    - Multi-minor support — rejected at launch; AC-A-6 leaves room to expand later with prebuilt-per-minor binaries.
  - **Rationale:** matches existing `godot-cpp` submodule branch and Android gradle dependency; patch releases picked up via branch tracking; `compatibility_minimum = "4.4"` in the gdextension file is an upstream inconsistency we will correct to `"4.3"` as part of the scaffold drift cleanup (AC-D-12).

- **DD-10 [DEFERRED → LOCKED]** Three-tier test strategy: unit (JUnit/GoogleTest) + instrumented-emulator (Fake Wallet) + release-gate Phantom.
  - **Alternatives considered:**
    - Firebase Test Lab for every PR — rejected; cost + Phantom requires human approval tap.
    - Manual only — rejected; fails AC-NFR-2.
    - Skip instrumented tests — rejected; breaks AC-A-7 contract-test harness.
  - **Rationale:** satisfies NFR-6, AC-NFR-2, AC-NFR-3, AC-A-7 with zero paid infrastructure; Phantom verification reserved for release-gate manual/FTL run.

### New (Architecture Phase)

- **DD-11 [LOCKED]** `ISigner` abstract C++ interface added to `godot-solana-sdk` core; `Transaction` depends on `ISigner`; desktop `WalletAdapter` and Android `MobileWalletAdapter` both implement it.
  - **Alternatives considered:** subclass — rejected, desktop/web lifecycle doesn't fit Android. Singleton — rejected, makes multi-adapter usage awkward.
  - **Rationale:** AC-A-5; prevents reverse dep core → MWA; enables signer swap for tests.

- **DD-12 [LOCKED]** `MwaAndroidBridge` C++ interface + concrete `MwaAndroidBridgeJni` implementation; single platform-selector TU.
  - **Rationale:** AC-A-4; prevents `#ifdef __ANDROID__` sprawl; enables `MockMwaAndroidBridge` for host-platform testing.

- **DD-13 [LOCKED]** Single `error-codes.yaml` manifest → codegen for Kotlin enum + C++ enum + GDScript constants.
  - **Alternatives considered:** hand-maintain 3 enums (rejected — drift invisible); use clientlib-ktx's exception hierarchy directly (rejected — leaks implementation detail, breaks layer encapsulation).
  - **Rationale:** AC-A-3, AC-D-2; single source of truth; CI drift check.

- **DD-14 [LOCKED]** `SecretString` wrapper type in both Kotlin and C++ with `toString()` (Kotlin) / `to_string()` (C++) → `"<redacted>"`; tokens never unwrap into `String`-typed fields. (See A-7 — `operator<<` dropped per DD-26.)
  - **Rationale:** AC-D-8, AC-NFR-SEC-6; defense in depth against accidental `Log.*` leakage.

- **DD-15 [LOCKED]** Single `mwa_error` signal (not per-operation error signals).
  - **Alternatives considered:** per-op error signals (rejected — doubles signal count, no new info); untyped error as second arg on each success signal (rejected — couples failure path to success payload shape).
  - **Rationale:** AC-U-3; consumer pattern is `await race(success_signal, mwa_error)`; enumerated `source_method` field disambiguates.

- **DD-16 [LOCKED]** Watchdog default 60 s, overridable via `opts.timeout_ms`.
  - **Rationale:** AC-A-8; safety net against hung wallets; UX research (persona 5.1) noted 120 s default for user-visible op timeouts vs. 60 s for internal network calls — we use 60 s for clientlib-level watchdog and 120 s as the UI-call default via `opts.timeout_ms`.

- **DD-17 [LOCKED]** Cache key = `SHA256(cluster "|" chain_id "|" wallet_package "|" identity_uri)`.
  - **Alternatives considered:** key on `wallet_package` only (rejected — cluster bleed, AC-NFR-SEC-4); key on `auth_token` directly (rejected — tokens are opaque, can rotate).
  - **Rationale:** prevents cluster bleed; stable across token refresh; SHA256 prefix for forward-compat migration (`mwa::v1::`).

- **DD-18 [LOCKED]** MWA is an **autoload singleton** (`MWA`) in GDScript, not a manually instantiated node.
  - **Alternatives considered:** manual node instantiation (rejected — users will forget lifecycle, and we need a single session-state holder). Static class on `SolanaUtils` (rejected — breaks existing SDK convention).
  - **Rationale:** Simplest ergonomics; matches RN SDK's single-module shape; one session per app is the correct model.

- **DD-19 [LOCKED]** `MwaClient` Kotlin interface separates plugin from clientlib-ktx concrete types.
  - **Rationale:** AC-D-4; enables `FakeMwaClient` for JVM unit tests without Android device.

- **DD-20 [LOCKED]** GitHub Actions macos-latest + android-emulator-runner + Fake Wallet APK for per-PR instrumented tests.
  - **Rationale:** free, hardware-accelerated on macOS runners, deterministic; see DD-10 rationale.

- **DD-21 [LOCKED]** Example app defaults to devnet; mainnet-beta requires explicit config + confirmation.
  - **Rationale:** AC-NFR-SEC-10; new-evaluator safety.

### Added During Persona Review

- **DD-22 [LOCKED]** Single thread-marshalling rule: all signals cross to Godot main via `call_deferred` in the C++ `GodotMainDispatcher`; no Kotlin-direct `emitSignal` for user-facing signals. *Rationale:* eliminates the "two paths" ambiguity flagged in architect review #1; makes AC-A-1 testable.
- **DD-23 [LOCKED]** Single effective watchdog = `min(internal_default=60000, opts.timeout_ms=120000)`; terminal-signal invariant (one terminal signal per request_id; late results dropped + counted). *Rationale:* removes nested-timeout race flagged in architect review #2.
- **DD-24 [LOCKED]** Pending-submission breadcrumb for `sign_and_send` only, surfaced via `pending_submission_found` signal on next connect/reauth; caller confirms via existing SolanaClient RPC. *Rationale:* real-money UX hazard flagged in architect review #3 is mitigated without requiring an RPC endpoint config in the plugin.
- **DD-25 [LOCKED]** `ISigner` contract inlined in §2.3.1 with full C++ signature + threading contract; backward-compat shim retains `Transaction::sign(keypair)` for one minor with deprecation warning; removed v1.2. `docs/migrations/isigner.md` + upstream signoff required. *Rationale:* resolves architect review #4 and maintainer review #6.
- **DD-26 [LOCKED]** Public C++ GDExtension boundary uses only `godot::` types; no STL. `SecretString` C++ holds `PackedByteArray` storage. CI header-lint enforces. *Rationale:* ABI stability flagged in architect review #5.
- **DD-27 [LOCKED]** Cache key uses length-prefixed canonical encoding before SHA256; prevents delimiter collision. *Rationale:* architect review #6.
- **DD-28 [LOCKED]** `FakeMwaClient` and Fake Wallet APK driven by shared `testdata/mwa-fixtures/` JSON set; T4 contract tier runs identical suite against both; compat matrix records `clientlib-ktx` version-tagged line. *Rationale:* architect review #7, maintainer review #3.
- **DD-29 [LOCKED]** `forget_all` acquires coroutine mutex, cancels in-flight, then revokes/wipes/rotates; MasterKey alias stays `-v1` until formal migration ADR. *Rationale:* architect review #8.
- **DD-30 [LOCKED]** `SdkLog.v/d/i/w/e` signatures take `() -> String` lambdas; CI lint bans string-interpolated calls. *Rationale:* architect review #9 (R8 argument-eval foot-gun).
- **DD-31 [LOCKED]** Payload `schema_version` field + semver policy: new error codes = MINOR bump; rename/remove = MAJOR. *Rationale:* architect review #10.
- **DD-32 [LOCKED]** `MWA.get_diagnostics()` + `docs/triage.md` runbook added; ring buffer N=20 correlation traces; non-sensitive fingerprint surface. *Rationale:* maintainer review #1 (assembled triage path).
- **DD-33 [LOCKED]** Error-codes generator has golden-file + property tests + failure tests; PR-blocking independent of drift check. *Rationale:* maintainer review #2.
- **DD-34 [LOCKED]** `get_auth_token_fingerprint()` uses HKDF-SHA256 with per-install **32-byte random** salt stored under Keystore alias `godot-sdk-mwa-fingerprint-salt-v1`; 8 hex chars output; stable across token refresh, rotated only on `forget_all`. *Rationale:* maintainer review #5. *Wording aligned by amendment A-16 (2026-04-29) to match `SecureTokenStore.FINGERPRINT_SALT_BYTES = 32` actual impl.*

### Acknowledged (documented trade-offs; no amendment)

- Maintainer review #4 (docs ownership): each new doc will carry `Owner:` + `Drift-check:` header; CODEOWNERS entry for `docs/` required; monthly docs-health review. Captured as a plan-phase story.
- Maintainer review #7 (shotgun surgery for new methods): ship `scripts/new_method.py` scaffolder + `docs/contributing-new-method.md` worked example. Captured as a plan-phase story (post-launch extension work).
- Maintainer review #8 (`@since` automation): `scripts/check_since_tags.py` in PR-blocking CI; runtime deprecation warning is docs-only for v1.x. Captured as a plan-phase story.

## 12. Open Questions — Carried to Plan Phase

None. Both original open questions (OQ-1 Godot minor, OQ-2 test strategy) are resolved. All DEFERRED DD-* are now LOCKED. All 19 persona concerns Addressed (16) or Acknowledged with documented trade-off (3). Total DDs: 34.
