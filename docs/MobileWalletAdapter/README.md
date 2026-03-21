# Godot Mobile Wallet Adapter SDK

This folder contains all documentation for the **Mobile Wallet Adapter (MWA)** integration in the Godot Solana SDK. The implementation brings API parity with the React Native MWA SDK so Godot developer can access Solana wallets (Phantom, Solflare, etc.) from Android games.

## Table of Contents

| Doc | Description |
|-----|-------------|
| [Getting Started](Getting%20Started.md) | Install the SDK and make your first wallet connection |
| [Authorization](Authorization.md) | Authorize, reauthorize, deauthorize and reconnect |
| [Session Lifecycle](Session%20Lifecycle.md) | Recommended connect/disconnect flow and capabilities |
| [Signing](Signing.md) | Sign messages, sign transactions, and sign + send |
| [Cache Layer](Cache%20Layer.md) | Persistent auth token storage and custom cache implementations |
| [API Reference](API%20Reference.md) | Full method and signal reference for `MobileWalletAdapter` |
| [Example App](Example%20App.md) | How to run and extend the bundled example project |

## Architecture Overview

```
Your Godot Game (GDScript)
         │
         ▼
 MobileWalletAdapter        ← GDScript wrapper (mobile_wallet_adapter.gd)
         │
         ▼
   WalletAdapter            ← GDExtension C++ bridge (godot-solana-sdk)
         │
         ▼
 WalletAdapterAndroid       ← Android JNI plugin (GDExtensionAndroidPlugin.kt)
         │
         ▼
  MWA Client Library        ← solana-mobile/mobile-wallet-adapter (Kotlin)
         │
         ▼
  Wallet App (Phantom etc.) ← Android Intent
```

## Parity Matrix — React Native vs Godot

| React Native | Godot MWA | Status |
|---|---|---|
| `transact(async session => session.authorize(...))` | `await mwa.authorize(name, uri, chain)` | ✅ |
| `session.reauthorize(...)` | `await mwa.reauthorize(name, uri, chain, token)` | ✅ |
| `session.deauthorize(...)` | `await mwa.deauthorize(token)` | ✅ |
| `reconnect` pattern | `await mwa.reconnect(name, uri, chain)` | ✅ |
| `session.signTransactions(txs)` | `await mwa.signTransactions([payload])` | ✅ |
| `session.signAndSendTransactions(txs)` | `await mwa.signAndSendTransactions([payload])` | ✅ |
| `session.signMessages(msgs)` | `await mwa.signMessages([bytes])` | ✅ |
| `session.getCapabilities()` | `await mwa.getCapabilities()` | ✅ |
| `AuthorizationResultCache` | `MWAAuthCache` / `MWAFileCache` / `MWAMemoryCache` | ✅ |
