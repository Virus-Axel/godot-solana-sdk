# API Reference

Full reference for the `MobileWalletAdapter` GDScript class located at:
```
addons/SolanaSDK/Optional/SolanaService/Scripts/WalletAdapter/mobile_wallet_adapter.gd
```

---

## Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `wallet_adapter` | `WalletAdapter` | `null` | **Required.** The underlying GDExtension WalletAdapter node. |
| `auth_cache` | `MWAAuthCache` | `MWAFileCache` | The cache backend for persisting auth tokens. |
| `connect_timeout_sec` | `float` | `60.0` | Seconds to wait for wallet approval before timing out. |

---

## Enums

### `Chain`
| Value | Int | Description |
|-------|-----|-------------|
| `Chain.DEVNET` | `0` | Solana Devnet |
| `Chain.MAINNET` | `1` | Solana Mainnet-Beta |
| `Chain.TESTNET` | `2` | Solana Testnet |

---

## Methods

### Session Management

#### `authorize(identity_name, identity_uri, chain, auth_token?) → Dictionary`
Opens an interactive wallet prompt to authorize a new session.
- `identity_name: String` — App name shown in the wallet
- `identity_uri: String` — App URL shown in the wallet
- `chain: Chain` — Target Solana cluster
- `auth_token: String` *(optional)* — Existing token to reuse (leave `""` for new session)
- **Returns:** `{ "auth_token", "account_address", "accounts": [String], "wallet_uri_base" }` or `{ "error" }`

#### `reauthorize(identity_name, identity_uri, chain, auth_token) → Dictionary`
Silently refreshes an existing session. Falls back to `authorize()` on failure if called via `reconnect()`.

#### `reconnect(identity_name, identity_uri, chain) → Dictionary`
High-level helper: loads cached token → `reauthorize()` → fallback to `authorize()`. **Recommended for app startup.**

#### `deauthorize(auth_token) → bool`
Revokes the given token in the wallet app. Returns `true` on success.

#### `disconnect_session() → Dictionary`
Loads the cached token, calls `deauthorize()`, clears `auth_cache`, and emits `disconnected()`.

#### `is_logged_in() → bool`
Returns `true` if `auth_cache` contains a non-empty auth token.

---

### Signing

#### `signTransactions(serialized_transactions) → Array`
- `serialized_transactions: Array[PackedByteArray]` — One or more serialized `Transaction` byte arrays
- **Returns:** `Array[PackedByteArray]` — Signed transaction bytes (same count as input)
- Emits `transactions_signed(signed_transactions)` on success, `signing_failed(error)` on failure

#### `signAndSendTransactions(serialized_transactions) → Dictionary`
- `serialized_transactions: Array[PackedByteArray]` — One or more serialized transaction byte arrays
- **Returns:** `{ "transaction_signatures": Array[String] }` or `{ "error" }`
- Internally calls `signTransaction` then broadcasts via `TransactionManager`

#### `signMessages(message_payloads) → Dictionary`
- `message_payloads: Array[PackedByteArray]` — Raw byte messages to sign
- **Returns:** `{ "signed_messages": Array[PackedByteArray], "addresses": Array[String] }` or `{ "error" }`
- Emits `messages_signed(signed_messages, addresses)` on success

---

### Capabilities

#### `getCapabilities() → Dictionary`
Queries the connected wallet for supported feature limits.
- **Returns:** `{ "max_transactions_per_request", "max_messages_per_request", "supported_transaction_versions" }` or `{ "error" }`
- Emits `capabilities_ready(capabilities)` on success, `capabilities_failed(error)` on failure

---

## Signals

| Signal | Arguments | Emitted When |
|--------|-----------|--------------|
| `authorized` | `auth_token: String, account_address: String` | `authorize()` / `reconnect()` succeeds |
| `authorization_failed` | `error: String` | Authorization fails or user cancels |
| `connected` | `auth_token: String, account_address: String` | Session is established (same as authorized) |
| `disconnected` | *(none)* | `disconnect_session()` completes |
| `deauthorized` | *(none)* | `deauthorize()` completes |
| `deauthorization_failed` | `error: String` | `deauthorize()` fails |
| `transactions_signed` | `signed_transactions: Array` | `signTransactions()` succeeds |
| `messages_signed` | `signed_messages: Array, addresses: Array` | `signMessages()` succeeds |
| `signing_failed` | `error: String` | Any signing operation fails |
| `capabilities_ready` | `capabilities: Dictionary` | `getCapabilities()` succeeds |
| `capabilities_failed` | `error: String` | `getCapabilities()` fails |

---

## Error Codes

| Error String | Meaning |
|---|---|
| `"user_cancelled"` | User dismissed or denied the wallet prompt |
| `"signing failed"` | Wallet rejected the signing request |
| `"wallet_adapter is null"` | `wallet_adapter` property was not assigned |
| `"timeout"` | Wallet did not respond within `connect_timeout_sec` |
