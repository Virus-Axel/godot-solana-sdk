# Session Lifecycle

This page explains the recommended flow for connecting, using, and disconnecting a wallet in your Godot game. The MWA protocol requires a fresh `auth_token` for every physical session, but with the built-in cache layer you can reconnect silently on subsequent launches.

---

## Recommended Flow

```
─── App Launch ───────────────────────────────────────────────────────────
│
├── is_logged_in()?
│     ├── YES → reconnect()  ──→ [Silent reauthorize via cached token]
│     │                               │
│     │                               ├── Success → Ready ✅
│     │                               └── Fail    → authorize() (interactive)
│     └── NO  → User taps "Connect"
│                   └── authorize() (interactive) → Ready ✅
│
─── In Game ──────────────────────────────────────────────────────────────
│
├── signAndSendTransactions([tx_payload])   ← most common
├── signTransactions([tx_payload])          ← sign only
├── signMessages([msg_bytes])               ← off-chain signing
│
─── Logout ───────────────────────────────────────────────────────────────
│
└── disconnect_session()   ← deauthorizes + clears cache
```

---

## Code Example — Full Lifecycle

```gdscript
extends Node

@onready var mwa: MobileWalletAdapter = $MobileWalletAdapter

func _ready() -> void:
    mwa.authorized.connect(_on_authorized)
    mwa.disconnected.connect(_on_disconnected)
    mwa.signing_failed.connect(_on_signing_failed)

    # Auto-reconnect if we have a saved token
    if mwa.is_logged_in():
        await mwa.reconnect("My Game", "https://mygame.com", mwa.Chain.DEVNET)

func _on_authorized(token: String, address: String) -> void:
    print("Session active for: ", address)
    # Now safe to sign transactions

func connect_wallet() -> void:
    _set_ui_busy(true)
    var res = await mwa.authorize("My Game", "https://mygame.com", mwa.Chain.DEVNET)
    _set_ui_busy(false)
    if res.has("error"):
        print("Connect cancelled: ", res["error"])

func send_sol(to: Pubkey, amount: int) -> void:
    var sender = Pubkey.new_from_string(mwa.auth_cache.load_token()["account_address"])
    var ix     = SystemProgram.transfer(sender, to, amount)
    var tx     = Transaction.new()
    tx.add_instruction(ix)
    tx.set_payer(sender)
    await SolanaService.transaction_manager.update_blockhash(tx)

    var result = await mwa.signAndSendTransactions([tx.serialize()])
    if result.has("transaction_signatures"):
        print("Sent! Sig: ", result["transaction_signatures"][0])

func logout() -> void:
    await mwa.disconnect_session()

func _on_disconnected() -> void:
    print("Logged out.")

func _on_signing_failed(error: String) -> void:
    print("Signing error: ", error)
```

---

## `getCapabilities()`

Query the connected wallet for its feature limits and supported versions. Useful to check if the wallet supports batching multiple transactions in one request.

```gdscript
var caps: Dictionary = await mwa.getCapabilities()

print("Max transactions per request: ", caps.get("max_transactions_per_request", "N/A"))
print("Max messages per request: ",     caps.get("max_messages_per_request", "N/A"))
print("Supported tx versions: ",        caps.get("supported_transaction_versions", []))
```

---

## Timeout Behaviour

The `connect_timeout_sec` property (default `60.0`) controls how long `authorize()` waits before timing out if the user does not interact with the wallet prompt.

```gdscript
mwa.connect_timeout_sec = 30.0  # shorter timeout for impatient flows
```

If the user backgrounds the game or dismisses the wallet without approving, a `user_cancelled` error is returned.
