---
status: complete
created: 2026-04-14
feature: mwa-android
---
# Brief: Godot Mobile Wallet Adapter (MWA) — Android Support

## Problem

Godot game developers who want to ship Solana-enabled mobile titles on Android currently have no supported path to integrate with the Solana Mobile Wallet Adapter (MWA) protocol. The existing `godot-solana-sdk` exposes Solana primitives (transactions, SPL tokens, Anchor) but lacks the Android-native wallet bridge — no Intent launcher for `solana-wallet://`, no local WebSocket session, no secure token cache. As a result, devs either abandon Godot for React Native/Unity, hand-roll brittle JNI code, or skip Solana entirely. This work is part of a Solana ecosystem grant with a **$10,000 reward** tied to delivering MWA parity.

## Vision

Any Godot developer can drop a single addon into their project, call `MWA.connect()` from GDScript, and receive a working Solana wallet integration that is indistinguishable from the official React Native experience — user taps "Connect", the OS wallet chooser appears, the wallet returns an auth token, and subsequent session reconnects are silent. All MWA-compliant wallets (Phantom, Solflare, Backpack, and any future entrants) are supported out of the box because the protocol is handled by Solana Mobile's official Android client library.

## Users

- **Godot game developer shipping a Solana-enabled Android title.** Intermediate Godot skills, variable Solana experience (novice → advanced). Works in GDScript day-to-day; willing to drop in a compiled addon but does not want to write Java/Kotlin/C++ themselves. Current frustration: no supported MWA path forces a rewrite in React Native or Unity, or reliance on unaudited community hacks.

## Success Metrics

- **API parity: 7/7 MWA methods** implemented and covered by an end-to-end integration test against a reference wallet (Phantom) on an Android device. Methods: `authorize`, `reauthorize`, `deauthorize`, `disconnect`, `signMessages`, `signTransactions`, `signAndSendTransactions`.
- **Silent reauthorize success**: in the example app, after a successful `authorize` → app kill → relaunch, calling `reauthorize(cached_token)` completes **without any wallet UI prompt** on Phantom, restoring the session and returning the same public key.

## Scope

### In Scope

- Java/Kotlin Godot Android Plugin wrapping Solana Mobile's official `mobile-wallet-adapter-clientlib-ktx`.
- C++ GDExtension ↔ Android plugin bridge via JNI (async callbacks back to GDScript).
- GDScript-facing API mirroring the React Native `@solana-mobile/mobile-wallet-adapter-protocol` SDK.
- Secure native token cache using **Jetpack Security `EncryptedSharedPreferences`** (Android Keystore-backed).
- All seven MWA RPC methods listed above.
- Cluster support for **mainnet-beta, devnet, and testnet** (selected per `authorize` call — same as RN SDK).
- **Minimal** GDScript example app with buttons for Connect / Disconnect / Reauthorize / Deauthorize / Sign Message, demonstrating the silent-reauth flow.
- Installation guide, usage snippets, and API reference documentation.

### Out of Scope

- **iOS support** — MWA uses local WebSockets + Android Intents; iOS background-execution rules make the protocol incompatible. No iOS work attempted.
- **Godot 3.x** — ship for latest stable Godot 4.x only (GDExtension is Godot 4+).
- **Non-MWA wallet flows** — no WalletConnect, custom deep-link schemes, or hardware-wallet integrations.
- **Custom wallet-picker UI** — the OS chooser handles wallet selection; no in-game UI for listing wallets.
- **Transaction-building helpers** — callers construct transactions using the existing `godot-solana-sdk` nodes and pass serialized bytes to `signTransactions` / `signAndSendTransactions`.
- **Polished showcase app** — example is a minimal test harness, not a grant-committee demo.

## Constraints

- **Platform**: Android only. Min SDK **23** (Jetpack Security requirement), target SDK **34**.
- **Godot**: latest stable Godot 4.x (exact minor version locked in Architecture phase).
- **License**: MIT, matching the upstream `godot-solana-sdk`.
- **Dependency**: must use Solana Mobile's official `com.solanamobile:mobile-wallet-adapter-clientlib-ktx` so that wallet-compatibility is inherited from the canonical implementation.
- **Security**: auth tokens MUST be stored via `EncryptedSharedPreferences` or equivalent Android Keystore-backed storage. Plain-text storage or Godot `user://` persistence is prohibited.
- **Grant deliverable**: $10k ecosystem grant — reviewers will evaluate API parity with the RN SDK and silent-reauth demonstration.

## Design Decisions

- **DD-1 [LOCKED]** Target Android platform only; iOS explicitly out of scope. *Source: MWA protocol mechanics.*
- **DD-2 [LOCKED]** Cache layer uses `EncryptedSharedPreferences` (Jetpack Security `MasterKey` API). *Source: task spec.*
- **DD-3 [LOCKED]** API surface mirrors the official RN `@solana-mobile/mobile-wallet-adapter-protocol` SDK. *Source: grant requirement.*
- **DD-4 [DISCRETION]** Use Solana Mobile's official `mobile-wallet-adapter-clientlib-ktx` for protocol-level work rather than reimplementing JSON-RPC/WebSocket handshake. *Rationale: inherits wallet compatibility, reduces audit surface, matches RN SDK's own strategy of wrapping the native lib.*
- **DD-5 [DISCRETION]** Bridge C++ ↔ Android via Godot 4's `GodotAndroidPlugin` singleton pattern (Godot-provided JNI helpers) rather than raw `JNIEnv*` calls from GDExtension. Async results returned via `emit_signal` from the plugin → GDExtension listener → GDScript signal. *Rationale: cleaner lifecycle management, no manual JNIEnv attach/detach, works with Godot's main-thread guarantees.*
- **DD-6 [DISCRETION]** Async model in GDScript: **signals**, with method calls returning immediately. Devs can `await` the signal for promise-like ergonomics (`var result = await MWA.authorize_completed`). *Rationale: signals are Godot-idiomatic and `await` on them gives parity with RN promises.*
- **DD-7 [DISCRETION]** Support all three Solana clusters (mainnet-beta, devnet, testnet) from launch. Cluster is a string parameter on `authorize`, matching the RN SDK; protocol itself is chain-agnostic so cost of supporting all three is zero. *Rationale: removes a dev-UX footgun at negligible implementation cost.*
- **DD-8 [DISCRETION]** Example app scope is **minimal** — a single scene with the required buttons and a log output. Not a polished demo. *Source: user directive.*
- **DD-9 [DEFERRED]** Exact Godot 4.x minor version (4.2 vs. 4.3 vs. latest) to pin against for GDExtension ABI. Resolve in Architecture phase after checking what the existing `godot-solana-sdk` targets in its `godot-cpp` submodule.
- **DD-10 [DEFERRED]** Test strategy for the Android plugin (instrumented `androidTest` with a mock wallet, vs. unit tests only, vs. manual device testing). Resolve in Architecture / QA phase.

## Additional Context

The following technical details were extracted from the user-supplied `docs/Godot_MWA_Task_Overview.md` and preserved verbatim where practical.

### Three-Tier Architecture (from spec)

- **Top Layer (GDScript):** User-facing API for Godot game developers. Exposes signals and methods for wallet connection, signing, and state management.
- **Middle Layer (C++ / GDExtension / JNI):** Core of `godot-solana-sdk`. JNI wrappers bridge Godot C++ with native Android Java methods and handle asynchronous callbacks.
- **Bottom Layer (Java / Kotlin):** A Godot Android Plugin. Interacts directly with the Android OS to launch Intents (`solana-wallet://`), establish the local WebSocket session, and securely store session tokens.

### MWA Methods to Implement (from spec)

| Method | Purpose |
|---|---|
| `authorize(identityName, iconUri, identityUri)` | Initiates connection, ECDH key exchange, returns `auth_token`. |
| `reauthorize(auth_token)` | Silently re-establishes a session using a cached token without user UI. |
| `deauthorize()` | Revokes the current token on the wallet side and clears the local cache. |
| `disconnect()` | Closes the current WebSocket session without revoking the authorization token. |
| `signMessages(messages_array)` | Prompts the wallet to sign an array of byte-encoded messages. |
| `signTransactions(transactions_array)` | Prompts the wallet to sign an array of transactions. |
| `signAndSendTransactions(transactions_array)` | Prompts the wallet to sign and dispatch transactions to the RPC. |

### Cache Flow (from spec)

`authorize` (save to cache) → app restart → read from cache → `reauthorize` (silent login) → `deauthorize` (clear cache).

### Upstream SDK Context

The base `godot-solana-sdk` (MIT) already ships:

- **Nodes**: `SolanaUtils`, `SolanaClient`, `Account`, `Transaction`, `WalletAdapter`, `ComputeBudget`, `SystemProgram`, `TokenProgram`, `MplTokenMetadata`, `MplCandyMachine`, `AnchorProgram`, etc.
- **Resources**: `Pubkey`, `Keypair`, `CompiledInstruction`, `Message`, `Hash`, `AccountMeta`, `Instruction`.
- **Android scaffold** at `android/` with Gradle build files and an empty `plugin/` module — the MWA plugin will live here.
- **Build**: SCons `SConstruct` for the GDExtension C++ target; Gradle wrapper for the Android plugin.

The new `WalletAdapter` node exists in the upstream SDK for desktop/web flows; MWA Android will either extend it or introduce a sibling `MobileWalletAdapter` node — decision deferred to Architecture phase.

### Reference Documents

- **MWA Protocol Specification**: https://docs.solanamobile.com/developers/mobile-wallet-adapter
- **MWA React Native SDK (parity target)**: https://github.com/solana-mobile/mobile-wallet-adapter/tree/main/js/packages/mobile-wallet-adapter-protocol
- **Base repo**: https://github.com/Virus-Axel/godot-solana-sdk
