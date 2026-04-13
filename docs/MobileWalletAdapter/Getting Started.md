# Getting Started

The Godot MWA SDK connects your Godot Engine Android games to any [Solana Mobile Wallet Adapter](https://github.com/solana-mobile/mobile-wallet-adapter) enabled wallet (Phantom, Solflare, etc.).

## Prerequisites

- **Godot Engine 4.x**
- **Android SDK** with an Android export preset configured in Godot
- **SolanaSDK** addon installed in your project
- An Android device or emulator with a compatible wallet app (e.g. [Phantom](https://phantom.app/))

## Installation

1. Copy the `addons/SolanaSDK` folder into your Godot project's root directory.
2. In Godot, go to **Project > Project Settings > Plugins** and enable the `SolanaSDK` plugin.
3. Add the following **Autoloads** in **Project Settings > Autoload**:

| Singleton Name | Path |
|---|---|
| `SolanaService` | `res://addons/SolanaSDK/Optional/SolanaService/Autoload/SolanaService.tscn` |

4. In your Android export preset, make sure the **WalletAdapterAndroid** plugin is listed under *Gradle Build > Plugins*.

## Quick Setup in GDScript

Add a `MobileWalletAdapter` node to your scene, or create one from code:

```gdscript
extends Node

var mwa: MobileWalletAdapter

func _ready() -> void:
    # Attach the wrapper to the scene
    mwa = load("res://addons/SolanaSDK/Optional/SolanaService/Scripts/WalletAdapter/mobile_wallet_adapter.gd").new()
    mwa.name = "MobileWalletAdapter"
    add_child(mwa)

    # Required: point it at the WalletAdapter GDExtension node
    mwa.wallet_adapter = SolanaService.wallet.get_node("WalletAdapter")

    # Optional: choose a cache backend (default is MWAFileCache)
    mwa.auth_cache = MWAFileCache.new()

    # Connect signals
    mwa.authorized.connect(_on_authorized)
    mwa.authorization_failed.connect(_on_auth_failed)

func _on_authorized(token: String, address: String) -> void:
    print("Connected as: ", address)

func _on_auth_failed(error: String) -> void:
    print("Auth failed: ", error)
```

## First Connection

```gdscript
func connect_wallet() -> void:
    var result = await mwa.authorize(
        "My Godot Game",          # App name shown in the wallet
        "https://mygame.com",     # App URI
        mwa.Chain.DEVNET          # Chain (DEVNET, MAINNET, TESTNET)
    )

    if result.has("auth_token"):
        print("Token: ", result["auth_token"])
        print("Address: ", result["account_address"])
    else:
        print("Failed: ", result.get("error", "unknown"))
```

> **Tip:** After the first successful `authorize()`, the `auth_token` is automatically saved by `MWAFileCache`. On subsequent app launches, use `mwa.reconnect()` instead of `authorize()` — it tries the cache first without showing a prompt.

## Next Steps

- [Authorization](Authorization.md) — detailed token lifecycle
- [Signing](Signing.md) — how to sign transactions and messages
- [Session Lifecycle](Session%20Lifecycle.md) — recommended connect/play/logout flow
- [Cache Layer](Cache%20Layer.md) — persist tokens across sessions
- [API Reference](API%20Reference.md) — full method listing
