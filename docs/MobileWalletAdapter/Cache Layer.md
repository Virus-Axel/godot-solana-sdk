# Cache Layer

The MWA auth token cache is a pluggable persistence layer that saves your authorization token across game sessions. Without it, the user would need to re-approve the wallet connection every time your app opens.

---

## Base Class: `MWAAuthCache`

`MWAAuthCache` is the interface all cache implementations extend. It lives at:
```
addons/SolanaSDK/Optional/SolanaService/Scripts/WalletAdapter/Cache/mwa_auth_cache.gd
```

```gdscript
class_name MWAAuthCache

## Save the token and address from a successful authorize() call.
func save_token(auth_token: String, account_address: String) -> void:
    pass

## Load the token dict. Returns {} if nothing is stored.
func load_token() -> Dictionary:
    return {}

## Returns true if a non-empty auth_token exists in storage.
func has_valid_token() -> bool:
    var data = load_token()
    return data.has("auth_token") and data["auth_token"] != ""

## Delete all cached data.
func clear_token() -> void:
    pass
```

---

## Built-in Implementations

### `MWAFileCache` *(default)*

Persists the token to disk using Godot's `ConfigFile` API, writing to the device's `user://` data directory. Token survives app restarts.

```
addons/SolanaSDK/Optional/SolanaService/Scripts/WalletAdapter/Cache/mwa_file_cache.gd
```

This is applied automatically when you instantiate `MobileWalletAdapter`:
```gdscript
var mwa = MobileWalletAdapter.new()
# auth_cache is already MWAFileCache.new() by default
```

---

### `MWAMemoryCache`

Stores the token in a plain Dictionary in memory. Wiped when the app exits. Useful for testing or when you don't want persistent sessions.

```
addons/SolanaSDK/Optional/SolanaService/Scripts/WalletAdapter/Cache/mwa_memory_cache.gd
```

```gdscript
mwa.auth_cache = MWAMemoryCache.new()
```

---

## Custom Cache Implementation

You can build your own backend by extending `MWAAuthCache`. This is useful for:
- Storing tokens in cloud saves (e.g. Apple Game Center, Google Play)
- Encrypting tokens before writing to disk
- Syncing across devices via your game's backend

```gdscript
extends MWAAuthCache
class_name MWACloudCache

func save_token(auth_token: String, account_address: String) -> void:
    MyCloudBackend.push({
        "auth_token": auth_token,
        "account_address": account_address
    })

func load_token() -> Dictionary:
    return MyCloudBackend.pull()

func clear_token() -> void:
    MyCloudBackend.delete_all()
```

Assign it to the MWA node:
```gdscript
mwa.auth_cache = MWACloudCache.new()
```

---

## Manual Cache Operations

You can interact directly with the cache at any time:

```gdscript
# Check if the user has a saved session
if mwa.auth_cache.has_valid_token():
    var data = mwa.auth_cache.load_token()
    print("Saved address: ", data["account_address"])

# Clear the cache (equivalent to a local logout)
mwa.auth_cache.clear_token()

# Or use the MWA wrapper which also deauthorizes the token in the wallet:
await mwa.disconnect_session()
```
