# Technical Specification: Godot Mobile Wallet Adapter (MWA) SDK

## 1. Project Objective

Upgrade the existing `godot-solana-sdk` to fully support the Solana Mobile Wallet Adapter (MWA) protocol on Android. The implementation must achieve exact API and functional parity with the official `@solana-mobile/mobile-wallet-adapter-protocol` React Native SDK.

The target platform is strictly **Android** (MWA uses local WebSockets and Android Intents, which are incompatible with iOS background execution limits).

---

## 2. Architecture & Tech Stack

The integration requires a three-tier architecture to bridge the Godot Engine, the Solana blockchain, and the native Android OS.

- **Top Layer (GDScript):** User-facing API for Godot game developers. Must expose signals and methods for wallet connection, signing, and state management.
- **Middle Layer (C++ / GDExtension / JNI):** The core of the `godot-solana-sdk`. Requires writing JNI (Java Native Interface) wrappers to bridge Godot C++ with native Android Java methods and handle asynchronous callbacks.
- **Bottom Layer (Java / Kotlin):** A Godot Android Plugin. This layer interacts directly with the Android OS to launch Intents (`solana-wallet://`), establish the local WebSocket server, and securely store the session tokens.

---

## 3. Required Deliverables & Implementation Details

### Deliverable 1: API Parity with React Native SDK

Implement the core MWA JSON-RPC methods and lifecycle events. The GDScript API must mirror the capabilities of the React Native SDK.

- **Methods to Implement via JNI -> Java:**
  - `authorize(identityName, iconUri, identityUri)`: Initiates the connection, ECDH key exchange, and retrieves the `auth_token`.
  - `reauthorize(auth_token)`: Silently re-establishes a session using a cached token without requiring user UI interaction.
  - `deauthorize()`: Revokes the current token on the wallet side and clears the local cache.
  - `disconnect()`: Closes the current WebSocket session without revoking the authorization token.
  - `signMessages(messages_array)`: Prompts the wallet to sign an array of byte-encoded messages.
  - `signTransactions(transactions_array)`: Prompts the wallet to sign an array of transactions.
  - `signAndSendTransactions(transactions_array)`: Prompts the wallet to sign and dispatch transactions to the RPC.

### Deliverable 2: Extensible Cache Layer for Authorization Token

To enable seamless reconnects (`reauthorize`), the `auth_token` received from the wallet must be persisted securely on the device.

- **Technical Requirement:** The C++ layer must delegate caching to the Android Java layer.
- **Implementation:** Use Android's `EncryptedSharedPreferences` or the Android Keystore system. The token must _not_ be stored in plain text or standard Godot `user://` directories, as it acts as a sensitive session key.
- **Flow:** `authorize` (Save to Cache) -> App Restart -> Read from Cache -> `reauthorize` (Silent Login) -> `deauthorize` (Clear Cache).

### Deliverable 3: Example Godot Application

Create a basic, open-source Godot Android project that imports the compiled SDK to serve as a functional integration test and developer reference.

- **Requirements:** \* Built using GDScript.
  - UI with buttons to test: Connect, Disconnect, Reauthorize, Deauthorize, Sign Message.
  - Must successfully demonstrate the Cache Layer (closing the app and reopening it should allow a silent `reauthorize` without Phantom prompting for permission).

### Deliverable 4: Comprehensive Documentation

- **Installation Guide:** Step-by-step instructions on configuring the Godot Android build template and enabling the MWA Android plugin.
- **Usage Examples:** GDScript copy-paste snippets mirroring the React Native docs.
- **API Reference:** Detailed breakdown of all methods, signals, inputs, and expected outputs.

---

## 4. Reference Repositories & Specifications

- **MWA Protocol Specification:** https://docs.solanamobile.com/developers/mobile-wallet-adapter
- **MWA React Native SDK (Parity Target):** https://github.com/solana-mobile/mobile-wallet-adapter/tree/main/js/packages/mobile-wallet-adapter-protocol
- **Godot Solana SDK Base (C++ GDExtension):** https://github.com/Virus-Axel/godot-solana-sdk
