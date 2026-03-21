# Example App

The `mwa-example-app/` directory is a complete, self-contained Godot 4 Android project that showcases every MWA capability. It is the reference implementation for the grant deliverable.

## Repository Structure

```
mwa-example-app/
├── project.godot               ← Main Godot project file
├── main_scene.gd               ← Main UI logic & all button actions
├── MainScene.tscn              ← Main scene (buttons, log panel, status panel)
├── icon.svg                    ← App icon
├── android/                    ← Android Gradle build config
└── addons/SolanaSDK/           ← Full Solana + MWA SDK addon
    └── Optional/SolanaService/
        └── Scripts/WalletAdapter/
            ├── mobile_wallet_adapter.gd     ← Main MWA wrapper
            └── Cache/
                ├── mwa_auth_cache.gd        ← Base cache class
                ├── mwa_file_cache.gd        ← File-based persistence
                └── mwa_memory_cache.gd      ← In-memory (volatile)
```

---

## Features Demonstrated

| Button | Method Called | What It Shows |
|--------|--------------|---------------|
| **Connect Wallet** | `mwa.authorize()` | Full interactive wallet authorization |
| **Reconnect** | `mwa.reconnect()` | Silent reauth from cache; falls back to interactive |
| **Disconnect** | `mwa.disconnect_session()` | Token revocation + cache clear |
| **Sign Message** | `mwa.signMessages()` | Signing an arbitrary UTF-8 byte payload |
| **Sign Transaction** | `mwa.signTransactions()` | Sign without broadcast; returns signed bytes |
| **Sign & Send** | `mwa.signAndSendTransactions()` | Sign + broadcast; returns on-chain signature |
| **Get Capabilities** | `mwa.getCapabilities()` | Query wallet feature limits |
| **Clear Cache** | `mwa.auth_cache.clear_token()` | Local logout only (no wallet deauth) |

All results are printed to the on-screen **Results Log** and the Godot remote debugger console.

---

## How to Run

### Prerequisites
- Godot 4.4+
- An Android device or emulator with [Phantom](https://phantom.app/) or another MWA-compatible wallet
- Android SDK / NDK installed and configured in Godot
- ADB connected to your device

### Steps

1. Open `mwa-example-app/project.godot` in Godot Editor.
2. Configure an **Android export preset** in **Project > Export** and enable **Gradle Build**.
3. Ensure the `WalletAdapterAndroid` plugin is listed in the preset's plugin section.
4. Click **Run on Remote Device** (or export an `.apk` and install manually).

---

## Devnet Setup

The example app connects to **Solana Devnet** by default. To test transactions:

1. Get a Devnet SOL airdrop for your wallet address from [faucet.solana.com](https://faucet.solana.com/).
2. Make sure Phantom is set to **Devnet** mode in its settings.

The **Sign & Send** button transfers `1,000,000 lamports` (0.001 SOL) to a freshly generated keypair. A successful run will print a Helius explorer link like:
```
https://orb.helius.dev/tx/<signature>?cluster=devnet
```

---

## Extending the Example

You can use this project as a template:

- Swap in your own Solana program instructions instead of `SystemProgram.transfer()`
- Replace `MWAFileCache` with your own `MWAAuthCache` implementation
- Add UI elements that react to the `authorized`, `disconnected`, and `transactions_signed` signals
