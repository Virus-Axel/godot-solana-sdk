# Authorization

Authorization is the entry point for every MWA session. The first time a user connects, `authorize()` opens the wallet app via an Android Intent and prompts the user to approve your game's identity. Subsequent runs can use the cached token to reconnect silently.

## `authorize()`

Opens an interactive wallet prompt. Use this for **first-time connections** or any time you need fresh user consent.

```gdscript
var result: Dictionary = await mwa.authorize(
    "My Godot Game",        # Identity name shown in the wallet
    "https://mygame.com",   # Identity URI
    mwa.Chain.DEVNET,       # Chain: DEVNET | MAINNET | TESTNET
    ""                      # auth_token — leave empty for a fresh session
)

if result.has("auth_token"):
    var token:   String = result["auth_token"]
    var address: String = result["account_address"]
    var wallet_uri_base: String = result.get("wallet_uri_base", "")
    print("Connected as: ", address)
elif result.has("error"):
    print("Auth failed: ", result["error"])
```

> **Signals emitted:**
> - `authorized(auth_token, account_address)` on success
> - `authorization_failed(error)` on failure or user cancel

---

## `reauthorize()`

Silently refreshes an existing session using a previously granted `auth_token`. The wallet may show a brief modal or approve it invisibly depending on the wallet app.

If the token is expired or revoked, `reauthorize()` returns an error and you should fall back to `authorize()`.

```gdscript
var result = await mwa.reauthorize(
    "My Godot Game",
    "https://mygame.com",
    mwa.Chain.DEVNET,
    stored_auth_token
)

if result.has("auth_token"):
    print("Session refreshed!")
else:
    # Token stale — do a full interactive authorize
    result = await mwa.authorize("My Godot Game", "https://mygame.com", mwa.Chain.DEVNET)
```

> **Tip:** You almost never need to call `reauthorize()` directly. Use `reconnect()` which automates this entire pattern.

---

## `reconnect()`

The recommended high-level reconnect helper. It:
1. Loads the cached `auth_token` from `auth_cache`.
2. Calls `reauthorize()` silently.
3. If that fails, falls back to a full interactive `authorize()`.

```gdscript
var result = await mwa.reconnect(
    "My Godot Game",
    "https://mygame.com",
    mwa.Chain.DEVNET
)

if result.has("auth_token"):
    print("Ready to go!")
```

> **Signals emitted:** same as `authorize()` — `authorized` or `authorization_failed`

---

## `deauthorize()`

Revokes an existing token on the **wallet side** only. Token is not cleared from local cache automatically here.

```gdscript
var success: bool = await mwa.deauthorize(stored_auth_token)
```

---

## `disconnect_session()`

The recommended disconnect helper. It:
1. Loads the cached token from `auth_cache`.
2. Calls `deauthorize()` to revoke it in the wallet.
3. Clears the token from `auth_cache`.
4. Emits the `disconnected()` signal.

```gdscript
await mwa.disconnect_session()
```

> See [Session Lifecycle](Session%20Lifecycle.md) for the recommended connect / play / disconnect flow.
