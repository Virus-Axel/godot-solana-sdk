---
status: complete
created: 2026-04-14
feature: mwa-android
brief: docs/research.md
---
# Specification: Godot Mobile Wallet Adapter (MWA) — Android Support

## Overview

Adds Android Mobile Wallet Adapter (MWA) support to `godot-solana-sdk` so Godot 4.x developers can integrate Solana wallets on Android with GDScript only, achieving parity with the official React Native MWA SDK. See `docs/research.md` for problem/vision/users/metrics.

## Use Cases

**UC-1: First-Time Wallet Connect**
- Primary actor: End-user game player (triggered by Godot developer's code)
- Precondition: No cached auth token exists for this app; ≥1 MWA-compliant wallet installed.
- Main flow: GDScript calls connect(identity, cluster) → method returns immediately → OS wallet chooser appears → player selects wallet → wallet presents approval UI → player approves → wallet returns auth_token + public_key → game receives success signal → token stored securely.
- Alternative flows: player cancels → cancellation signal; no wallet installed → `NO_MWA_WALLET_INSTALLED` error; protocol error → typed error signal.
- Postcondition: valid auth token in encrypted storage; game holds connected public key.

**UC-2: Silent Reauthorize on App Relaunch**
- Primary actor: Godot developer (on behalf of returning player)
- Precondition: previously stored auth token exists; same wallet app installed.
- Main flow: game reads cached token → calls reauthorize → wallet validates without UI → returns same public key → success signal.
- Alternative flows: token invalid/revoked → `TOKEN_EXPIRED` (no auto-chooser; caller must invoke connect()); wallet uninstalled → `WALLET_UNINSTALLED` error.
- Postcondition: session active with no wallet UI shown.

**UC-3: Sign a Message**
- Primary actor: End-user game player
- Precondition: active authorized session.
- Main flow: GDScript calls sign_messages with byte array → wallet prompts → player approves → wallet returns signatures → success signal.
- Alternative flows: rejection → `USER_CANCELED`; expired session → `TOKEN_EXPIRED`; no session → `NOT_CONNECTED`.
- Postcondition: game holds signatures; session remains active.

**UC-4: Sign and Send Transactions**
- Primary actor: End-user game player
- Precondition: active authorized session; caller has built serialized transactions via existing godot-solana-sdk nodes.
- Main flow: GDScript calls sign_and_send with transaction array and target cluster → wallet prompts → player approves → wallet signs and dispatches to RPC → success signal with `{signatures, submitted_at, confirmation_status: "submitted"}`.
- Alternative flows: rejection → `USER_CANCELED`; RPC rejection → `RPC_FAILED` with wallet-reported reason; sign-only variant returns signed payloads without dispatch.
- Postcondition: transactions submitted (confirmation is caller's responsibility).

**UC-5: Deauthorize and Clear Cache**
- Primary actor: End-user game player
- Precondition: active authorized session and stored auth token.
- Main flow: GDScript calls deauthorize → wallet revokes token (best-effort) → cached token cleared from encrypted storage in `finally` block → success signal.
- Alternative flows: wallet unreachable → local cache still cleared (returns success with warning flag); disconnect variant closes session but retains token.
- Postcondition: no valid auth token stored; subsequent reauthorize with old token fails.

## Functional Requirements (Capability Contract)

### Install & Setup
- **FR-1**: [Godot developer] can [install MWA wallet support in a Godot 4.x Android project] [by adding a single addon without writing Java, Kotlin, or C++ code].
- **FR-11**: [Godot developer] can [provide identity metadata — name, icon URI, and identity URI — to the connect flow] [so the wallet can display the dApp identity to the end user].
- **FR-15**: [Godot developer] can [consult installation, usage, and API reference documentation] [covering all seven MWA methods and the silent-reauth flow].
- **FR-16**: [Godot developer] can [run a minimal example GDScript scene] [with buttons for Connect, Disconnect, Reauthorize, Deauthorize, and Sign Message, plus a log output].

### Connection Lifecycle
- **FR-2**: [Godot developer] can [initiate a wallet connection from GDScript] [by invoking a connect capability that triggers the OS wallet chooser and returns an auth token and public key].
- **FR-3**: [End-user game player] can [select any MWA-compliant wallet installed on their Android device] [via the operating system wallet chooser presented during the connect flow].
- **FR-4**: [Godot developer] can [silently restore a previous wallet session on app relaunch] [by invoking reauthorize with a cached token, without the wallet showing any UI prompt].
- **FR-5**: [Godot developer] can [revoke an active wallet authorization] [so that the wallet drops the session and locally cached credentials are cleared].
- **FR-6**: [Godot developer] can [close an active wallet session without revoking authorization] [so that the token remains valid for later silent reauthorization].
- **FR-14**: [Godot developer] can [persist an auth token across app process restarts] [so that a cached session survives the app being killed and relaunched].

### Signing
- **FR-7**: [Godot developer] can [request the connected wallet to sign an array of arbitrary byte-encoded messages] [and receive the signatures back in GDScript].
- **FR-8**: [Godot developer] can [request the connected wallet to sign an array of serialized transactions] [and receive the signed transactions back in GDScript].
- **FR-9**: [Godot developer] can [request the connected wallet to sign and submit an array of transactions to a Solana RPC] [and receive the resulting transaction signatures back in GDScript].

### Cross-Cutting
- **FR-10**: [Godot developer] can [select the target Solana cluster — mainnet-beta, devnet, or testnet — on each authorize call] [matching the parity of the React Native MWA SDK].
- **FR-12**: [Godot developer] can [call wallet operations as non-blocking methods that return immediately] [and await a signal to receive the result, error, or user-cancellation outcome].
- **FR-13**: [Godot developer] can [observe cancellation and failure outcomes from any wallet operation] [through a dedicated error/cancel signal path distinct from the success path].

## Non-Functional Requirements (measurable targets)

- **NFR-1 (Security)**: auth tokens at rest are encrypted via Android Keystore-backed storage; **0 bytes** of token material in plain text to disk, `user://`, logs, or unencrypted SharedPreferences.
- **NFR-2 (Platform)**: minimum Android SDK **23**; target Android SDK **34**.
- **NFR-3 (Compatibility)**: supports Godot 4.x latest stable (single minor pinned for GDExtension ABI); **0** support for Godot 3.x.
- **NFR-4 (API parity)**: **7/7** MWA methods present vs. RN SDK.
- **NFR-5 (Reliability)**: silent reauthorize succeeds on **100%** of attempts against Phantom in the example app post-restart with **0** wallet UI prompts.
- **NFR-6 (Coverage)**: end-to-end integration test coverage of **7/7** MWA methods against a reference wallet on Android.
- **NFR-7 (Cluster)**: **3/3** Solana clusters (mainnet-beta, devnet, testnet) selectable per authorize call.
- **NFR-8 (Wallet)**: works with **100%** of MWA-compliant wallets without per-wallet code paths.
- **NFR-9 (Performance)**: wallet op method calls return to GDScript in **≤ 16 ms** (1 frame @ 60 FPS); results arrive async.
- **NFR-10 (Maintainability)**: protocol-level MWA work delegated to official `com.solanamobile:mobile-wallet-adapter-clientlib-ktx`; **0** hand-rolled protocol/WebSocket/ECDH.
- **NFR-11 (Licensing)**: MIT license, **1:1** match with upstream.

## Quality Perspectives

Four persona reviews produced 46 concerns (17 P1, 17 P2, 5 P3 + 7 security-specific flagged separately).

### End User — 12 concerns
State introspection absent; no pre-flight `NOT_CONNECTED` guard; error signal untyped; missing-wallet first-run failure; no local-cache wipe without network; non-Android behavior undefined; example doesn't demo await+error race; no wallet label returned; "submitted ≠ confirmed" semantics not surfaced; token-expiry path undefined; a11y of example-scene errors unconstrained; no per-call timeout.

### Architect — 10 concerns
Thread-marshalling from Kotlin → Godot main unspecified; Activity lifecycle mid-op undefined; error taxonomy has no SSoT across 3 languages; `#ifdef __ANDROID__` will leak without platform-abstraction seam; coupling to existing `WalletAdapter` node unspecified (sibling vs subclass); ABI strategy beyond pinning one minor missing; no contract-test harness vs Fake Wallet; no observability plan; transitive dep APK-size/method-count unbudgeted; grant-reviewability artifact undefined.

### Maintainer — 12 concerns
No correlation IDs across layers; no single-source-of-truth error codes; signing pipeline duplication; no RN parity contract; Kotlin not unit-testable without device; C++ JNI glue untestable without Kotlin; non-Android paths undefined; compatibility matrix missing (scaffold already drifted: compileSdk=33 vs spec 34, minSdk=21 vs spec 23); NFR-1 plaintext guarantee uninstrumented; cross-language naming informal; no signal versioning/deprecation policy; single-command build not CI-gated for MWA. Convention drift noted: `println()` instead of `Log.v(TAG)`, `my*` global `var`s, no ktlint.

### Security Auditor — 12 concerns
`allowBackup`/`debuggable` leakage via manifest merge; Keystore invalidation (lockscreen reset) crash path; cluster bleed if cache key lacks chain/wallet context; `deauthorize` must clear local in `finally`; logcat/crash-reporter token leakage; Intent/StrandHogg hardening; dep supply-chain pinning; ProGuard/R8 consumer rules; example defaults to mainnet (unsafe); `MWA.forget_all()` for GDPR/CCPA; FLAG_SECURE + identity-URI phishing docs; optional `get_device_posture()`.

## Acceptance Criteria

**65 total** across 5 categories: AC-U (10) · AC-FR (12) · AC-A (9) · AC-D (12) · AC-NFR (9 general + 13 security = 22).

### AC-U: End User

- **AC-U-1** Synchronous state API `is_connected()`, `get_public_key()`, `get_cluster()`, `get_auth_token_fingerprint()`, `is_supported()`; each returns in ≤1 ms (benchmark test).
- **AC-U-2** Calling any signing method without active session emits error signal within 1 frame with code `NOT_CONNECTED` + `user_message`; example demos the pre-flight guard.
- **AC-U-3** Error signal payload is Dictionary `{request_id, code (enum), user_message, developer_details, recoverable, retry_hint, layer, cause?}`. Code enum: `USER_CANCELED`, `WALLET_REJECTED`, `NO_MWA_WALLET_INSTALLED`, `WALLET_UNINSTALLED`, `TOKEN_EXPIRED`, `RPC_FAILED`, `NETWORK_OFFLINE`, `TIMEOUT`, `NOT_CONNECTED`, `UNSUPPORTED_PLATFORM`, `PROTOCOL_ERROR`, `STORAGE_CORRUPT`, `REAUTH_REQUIRED`. Docs include code→copy mapping.
- **AC-U-4** SDK detects absence of MWA-capable wallets pre-chooser; emits `NO_MWA_WALLET_INSTALLED`; `get_suggested_wallet_install_links()` returns ≥1 entry; example covers empty-wallet path.
- **AC-U-5** `clear_local_cache()` synchronous, no network, distinct from `deauthorize()`; example has "Reset local state" debug button; docs have Testing & debugging section.
- **AC-U-6** Authorization success payload includes `wallet_label` and `wallet_icon_uri` when provided (empty string otherwise); example displays label post-connect.
- **AC-U-7** `sign_and_send` success payload is `{signatures: Array[String], submitted_at: int, confirmation_status: "submitted"}`; docs state confirmation is caller's responsibility; example labels state "Submitted — awaiting confirmation".
- **AC-U-8** Token expiry during silent reauth emits `TOKEN_EXPIRED` (SDK does NOT auto-launch chooser); caller must explicitly call `connect()`; behavior documented in FR-4.
- **AC-U-9** Every awaitable op accepts optional `timeout_ms` (default 120000); Activity lifecycle pause/resume without result emits `USER_CANCELED` within 1 frame of resume.
- **AC-U-10** Example scene demos `await`+error-signal race, working Cancel button, logs every state transition; docs include "Signals 101" and "Accessible error display" (text-not-color-alone, WCAG AA, labeled buttons).

### AC-FR: Functional Completeness

- **AC-FR-1** Addon installs via copy of `addons/SolanaSDK/` with 0 consumer authoring in Java/Kotlin/C++; verified by clean-project CI job loading the example scene.
- **AC-FR-2** `connect(identity: Dictionary, cluster: String) -> void` launches chooser; success emits `connect_completed` with `{auth_token, public_key, wallet_label, wallet_icon_uri, cluster}`; E2E test against Phantom asserts non-empty.
- **AC-FR-3** `reauthorize()` with valid cached token completes with 0 Activity transitions to wallet app and emits `reauthorize_completed` with existing pubkey.
- **AC-FR-4** `deauthorize()` best-effort remote + unconditional local wipe; post-call `is_connected()==false`, stored token absent from EncryptedSharedPreferences.
- **AC-FR-5** `disconnect()` clears in-memory session but retains encrypted token; subsequent `reauthorize()` succeeds silently.
- **AC-FR-6** `sign_messages(messages: PackedByteArray[]) -> void` emits `sign_messages_completed` with signed payloads of equal length; E2E round-trips ≥2 messages.
- **AC-FR-7** `sign_transactions(transactions: PackedByteArray[]) -> void` emits `sign_transactions_completed` with signed serialized transactions of equal length.
- **AC-FR-8** `sign_and_send(transactions, min_context_slot?) -> void` submits to active cluster's RPC, emits payload with `signatures`; devnet E2E resolves via `getSignatureStatuses` within 30 s.
- **AC-FR-9** All 7 public methods are `void`-returning; results via signals only; signal emission ≤1 frame from underlying completion.
- **AC-FR-10** Auth token survives process death: force-stop + relaunch + `reauthorize()` succeeds without wallet UI.
- **AC-FR-11** Docs ship install + usage + API reference covering 7 methods + silent-reauth; link-check CI fails on 404s; each method has copy-pasteable snippet.
- **AC-FR-12** `example/` contains single runnable scene with 5 buttons + scrolling log; `REVIEW.md` commands build + install + run in ≤10 min.

### AC-A: Architecture

- **AC-A-1** All signals originating off-Godot-thread traverse a single documented thread-hop primitive (MPSC inbox on `_process` OR `call_deferred`); unit test asserts signal fires on main thread within 1 frame from a background-thread callback.
- **AC-A-2** Activity destruction mid-op emits `mwa_cancelled_lifecycle` with stable code; op state NOT persisted across process death unless documented; rotation-mid-authorize instrumented test.
- **AC-A-3** Single machine-readable `error-codes.yaml` manifest; Kotlin enum + C++ enum + GDScript constants code-generated at build; CI drift check fails PR on divergence.
- **AC-A-4** Platform-agnostic C++ interface with exactly one conditional TU selecting backend; non-Android backend returns `UNSUPPORTED_PLATFORM`; grep CI bans `#ifdef __ANDROID__` outside that TU.
- **AC-A-5** MWA is sibling of `WalletAdapter` (not subclass); narrow `ISigner` abstraction implemented by both desktop `WalletAdapter` and Android MWA; `Transaction` depends only on `ISigner`; import-graph CI bans reverse dep from SDK core → MWA.
- **AC-A-6** Release artifacts include prebuilt `.so`/`.dll`/`.dylib` per Godot minor in `COMPATIBILITY.md`; CI build matrix; runtime version mismatch fails fast with named expected/actual.
- **AC-A-7** Contract-test suite exercises 7/7 methods against Fake Wallet APK; request shapes asserted vs published MWA protocol fixtures; runs on every PR and clientlib-ktx bump.
- **AC-A-8** Correlation ID logged at each layer as `GodotMWA/<layer>/<corrId>`; configurable watchdog (default 60 s) emits `mwa_timeout` signal; `debug_mode` toggle exposes full protocol traces.
- **AC-A-9** Architecture doc publishes dependency budget (max APK-size delta, max method-ref delta) vs stock Godot Android export; CI reports per-PR delta; exceeding requires amendment.

### AC-D: Developer / Maintainer

- **AC-D-1** UUID v4 `request_id` threaded GDScript → C++ → Kotlin, logged at each layer under tag `MWA` with V/D/I/W/E levels, included in every success and error signal payload; `MWA.set_verbose(true)` toggles verbose logs.
- **AC-D-2** Signing ops share `suspend fun <T> runSigningOp(op, block): Result<T, MwaError>` pipeline; unit test asserts new signing method costs ≤20 Kotlin LOC.
- **AC-D-3** `docs/rn-parity.md` machine-readable method manifest (RN method → RN version → GDScript → Kotlin bridge → C++ binding); weekly `verify_parity.yml` diffs vs pinned RN npm package and opens PR/issue on new method.
- **AC-D-4** Kotlin plugin exposes `MwaClient` interface; JVM unit-test suite with `FakeMwaClient` covers every `@UsedByGodot` method's success/cancel/failure paths WITHOUT device or Robolectric; `./gradlew test` in CI on every PR.
- **AC-D-5** C++ `MobileWalletAdapter` node delegates all JNI via injectable `MwaAndroidBridge` interface; `MockMwaAndroidBridge` drives host-platform unit tests on Linux/Windows/macOS asserting signal emission, arg marshalling, thread-hop correctness without JVM.
- **AC-D-6** Non-Android platforms return immediately and emit `UNSUPPORTED_PLATFORM` with `OS.get_name()` message; `MWA.is_supported()==false`; CI loads GDExtension in headless Godot on Linux/Windows/macOS/Web asserting no crash.
- **AC-D-7** `docs/compatibility-matrix.md` lists Godot minor + godot-cpp SHA + clientlib-ktx + rpc-core + compileSdk + targetSdk + minSdk + Kotlin + Compose + "last verified"; CI asserts `compileSdk==34` and `minSdk==23`; `docs/adr/` entry pins Godot minor (resolves OQ-1).
- **AC-D-8** All tokens and pubkeys flow through `SecretString` wrapper with `toString() == "<redacted>"`; `androidTest` asserts post-authorize→deauthorize no sandbox file and no logcat line contains token bytes; CI grep fails build on `Log.*` of `authToken`/`publicKey`/`SecretString.value`.
- **AC-D-9** `docs/naming-map.md` single table (RN method → GDScript → C++ → Kotlin `@UsedByGodot` → signal names); CI parses APIs and fails on divergence.
- **AC-D-10** Signal payloads are Dictionaries (additive keys only); `docs/api-stability.md` documents mark→warn-one-minor→remove deprecation ladder; `@since` XML tags on every public method/signal.
- **AC-D-11** `scons addon` produces `addons/SolanaSDK/` in one command; PR-blocking CI runs `scons addon` + `./gradlew test` + 4-platform GDExtension load; README has 5-line "Build the addon" section.
- **AC-D-12** Kotlin conforms to `ktlint` (CI-enforced); scaffold drift (compileSdk 33→34, minSdk 21→23, `println`→`Log.v(TAG, …)`, `my*` global vars → scoped `MwaSessionState`) resolved before 1.0; tracked as issues until closed.

### AC-NFR: Non-Functional

**General (9):**
- **AC-NFR-1** Method-call return latency ≤16 ms P99 (CI microbenchmark); results always via signal.
- **AC-NFR-2** Automated E2E suite runs 7/7 methods on real Android device (Firebase Test Lab or equivalent) per PR; blocks merge; reports per-method pass/fail.
- **AC-NFR-3** E2E against Phantom: **10 consecutive silent-reauth cycles with 0 UI prompts** (operationalizes NFR-5).
- **AC-NFR-4** E2E connect+sign on each of mainnet-beta/devnet/testnet; `cluster` parameter on `connect()` drives selection; cluster mismatch with cached token forces fresh authorize.
- **AC-NFR-5** Contract tests against ≥2 MWA wallets (Phantom + Fake Wallet) succeed without per-wallet code; source grep bans wallet-specific conditionals.
- **AC-NFR-6** SBOM lists `mobile-wallet-adapter-clientlib-ktx`; grep bans hand-rolled WebSocket/ECDH/MWA-protocol framing under `android/plugin/`.
- **AC-NFR-7** Repo `LICENSE` is MIT, byte-diff vs upstream `solana-mobile/mobile-wallet-adapter` (except copyright-holder line); CI diff check.
- **AC-NFR-8** `build.gradle` pins `compileSdk=34`, `targetSdk=34`, `minSdk=23`; `.gdextension` requires documented Godot 4.x minor; drift = CI fail.
- **AC-NFR-9** `rn-parity.md` shows 7/7 methods mapped to RN SDK at pinned version; weekly parity CI green (links to AC-D-3).

**Security (13):**
- **AC-NFR-SEC-1** 0 plaintext token bytes on disk or logcat; tokens exclusively in `EncryptedSharedPreferences` + Android Keystore MasterKey + AES256_GCM; `androidTest` scans sandbox + logcat post-authorize.
- **AC-NFR-SEC-2** Android manifest ships `android:allowBackup="false"` + `fullBackupContent` excluding prefs file; CI inspects MERGED manifest (simulating consumer merge) and fails if backup re-enabled.
- **AC-NFR-SEC-3** `EncryptedSharedPreferences` creation catches `InvalidProtocolBufferException` / `GeneralSecurityException`, wipes corrupt prefs, emits `reauth_required` signal; does NOT crash Godot runtime; unit test simulates corruption.
- **AC-NFR-SEC-4** Cache key is tuple `(cluster, chain_id, wallet_package, identity_uri)`; SDK refuses cached token when requested cluster != stored tuple; forces fresh authorize; unit test covers mismatch.
- **AC-NFR-SEC-5** `deauthorize()` clears local state in `finally` block; remote RPC failure does NOT leave token in EncryptedSharedPreferences; returns success with `warning` flag when remote unreachable; androidTest network-off path.
- **AC-NFR-SEC-6** Single `SdkLog` wrapper stripped by R8 in release (`-assumenosideeffects`); release-AAR grep test fails if token field name present; no `Log.d`/`Log.v` of tokens/ciphertext/session keys/raw tx bytes.
- **AC-NFR-SEC-7** Intent-receiving Activity: `android:exported` only where strictly necessary; verifies calling package against chosen wallet's package set; `taskAffinity=""`; `launchMode="singleTask"`; no auto-exported ContentProvider; lint/CI assertion.
- **AC-NFR-SEC-8** `gradle.lockfile` + `dependency-verification.xml` (checksum + PGP) cover all transitive deps (OkHttp, BouncyCastle, Tink, kotlinx-coroutines); reproducible-build CI rebuilds AAR, asserts byte-identical across two runners.
- **AC-NFR-SEC-9** AAR ships `consumer-rules.pro` with keeps for JNI native method names and clientlib-ktx reflection; release-variant instrumented smoke test exercises authorize E2E against mock wallet with R8 enabled.
- **AC-NFR-SEC-10** Example app defaults to `devnet`; mainnet-beta toggle requires explicit config + in-app confirmation dialog; `identityUri` is placeholder (`https://example.com/godot-mwa-sample`), not a real partner domain.
- **AC-NFR-SEC-11** `MWA.forget_all()` iterates all known wallet entries, best-effort `deauthorize` each, clears EncryptedSharedPreferences file, re-creates MasterKey; documented as GDPR/CCPA deletion entrypoint.
- **AC-NFR-SEC-12** README recommends `FLAG_SECURE` for Activities displaying pubkeys/tx details; warns `identityName`/`iconUri` are attacker-controllable and wallet signing prompt is sole trust anchor.
- **AC-NFR-SEC-13** `MWA.get_device_posture() -> Dictionary` returns `{rooted, debuggable, developer_options_on, adb_enabled}` for dApp-side mainnet gating; values are best-effort and documented as non-authoritative.

## Traceability Matrix

### FR → AC

| FR | AC Coverage |
|---|---|
| FR-1 | AC-FR-1, AC-D-11 |
| FR-2 | AC-FR-2 |
| FR-3 | AC-FR-2, AC-NFR-5 |
| FR-4 | AC-FR-3, AC-U-8, AC-NFR-3 |
| FR-5 | AC-FR-4, AC-NFR-SEC-5 |
| FR-6 | AC-FR-5 |
| FR-7 | AC-FR-6 |
| FR-8 | AC-FR-7 |
| FR-9 | AC-FR-8, AC-U-7 |
| FR-10 | AC-FR-2, AC-NFR-4, AC-NFR-SEC-4 |
| FR-11 | AC-FR-2, AC-U-6 |
| FR-12 | AC-FR-9, AC-A-1, AC-NFR-1 |
| FR-13 | AC-FR-9, AC-U-2, AC-U-3, AC-A-2, AC-A-8 |
| FR-14 | AC-FR-10, AC-NFR-SEC-1 |
| FR-15 | AC-FR-11, AC-U-10, AC-D-9 |
| FR-16 | AC-FR-12, AC-U-4, AC-U-5, AC-U-10, AC-A-10 (via REVIEW.md) |

### NFR → AC

| NFR | AC Coverage |
|---|---|
| NFR-1 | AC-NFR-SEC-1, AC-D-8, AC-NFR-SEC-6 |
| NFR-2 | AC-NFR-8, AC-D-7 |
| NFR-3 | AC-NFR-8, AC-D-7, AC-A-6 |
| NFR-4 | AC-NFR-9, AC-D-3 |
| NFR-5 | AC-NFR-3, AC-U-8 |
| NFR-6 | AC-NFR-2, AC-A-7 |
| NFR-7 | AC-NFR-4 |
| NFR-8 | AC-NFR-5, AC-A-7 |
| NFR-9 | AC-NFR-1, AC-U-1, AC-A-1 |
| NFR-10 | AC-NFR-6 |
| NFR-11 | AC-NFR-7 |

### HIGH persona concern → AC

| Concern | AC |
|---|---|
| Error taxonomy | AC-U-3, AC-A-3, AC-D-9 |
| Non-Android platform behavior | AC-U-1, AC-D-6, AC-A-4 |
| Observability / correlation ID | AC-A-8, AC-D-1 |
| Cache-key security | AC-NFR-SEC-4 |
| Thread-hop correctness | AC-A-1, AC-D-5 |
| Lifecycle / process death | AC-A-2, AC-U-9, AC-FR-10 |
| Token plaintext / logs | AC-NFR-SEC-1, AC-NFR-SEC-6, AC-D-8 |
| Silent reauth on Phantom | AC-NFR-3 |
| Testability without device | AC-D-4, AC-D-5 |
| R8 / release-variant safety | AC-NFR-SEC-6, AC-NFR-SEC-9 |
| Sibling-not-subclass boundary | AC-A-5 |
| Compatibility matrix / Godot minor pin | AC-D-7 |

## Content Quality

| Check | Status | Detail |
|---|---|---|
| CQ-1 Density | ✅ PASS | No filler phrases in any FR or NFR. |
| CQ-2 Implementation Leakage | ✅ PASS | Actors are human/domain; impl terms only in NFRs/Constraints where LOCKED. |
| CQ-3 Measurability (BLOCKING) | ✅ PASS | Every NFR has a numeric/measurable target. |
| CQ-4 Traceability (BLOCKING) | ✅ PASS | 16/16 FRs and 11/11 NFRs → ≥1 AC; 0 orphan ACs. |

## Constraints & Assumptions

### Constraints
- **C-1**: Android-only; iOS excluded — LOCKED (DD-1).
- **C-2**: Auth tokens stored via Android Keystore-backed encryption; plain-text/`user://` prohibited — LOCKED (DD-2).
- **C-3**: GDScript API mirrors RN `@solana-mobile/mobile-wallet-adapter-protocol` — LOCKED (DD-3).
- **C-4**: Min Android SDK 23, target SDK 34.
- **C-5**: Godot 4.x latest stable only.
- **C-6**: MIT license.
- **C-7**: Must use `com.solanamobile:mobile-wallet-adapter-clientlib-ktx` (DD-4).
- **C-8**: Callers build transactions via existing godot-solana-sdk nodes; MWA only signs/sends.

### Assumptions
- **A-1**: Phantom is the reference wallet for metrics (low risk).
- **A-2**: Signal-based async with `await` ergonomics (DD-6).
- **A-3**: "Silent" means zero UI on success path against Phantom.
- **A-4**: Single Godot 4.x minor pinned pre-release (DD-9 deferred → Architecture).
- **A-5**: Parity = method-surface parity, not literal signature parity.
- **A-6**: Cluster passed to sign-and-send matches authorized cluster; mismatch yields wallet-reported error.

## Risk Register

| ID | Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|---|
| R-1 | `clientlib-ktx` breaks API between Gradle releases | Medium | High | AC-D-3 weekly parity diff; AC-A-7 contract tests; pin version in compat matrix (AC-D-7) |
| R-2 | Godot minor ABI shift after release → consumer APKs crash | High (over time) | High | AC-A-6 prebuilt-per-minor + runtime version check + `COMPATIBILITY.md` |
| R-3 | Keystore invalidated (user resets lockscreen) → plugin crashes | Medium | High | AC-NFR-SEC-3 catch + wipe + `reauth_required` signal |
| R-4 | Phantom silent-reauth fails on some API level → NFR-5 miss | Low | High (grant deliverable) | AC-NFR-3 runs 10 cycles; CI on multiple API levels; docs document fallback |
| R-5 | Token leaks via logcat in release build | Medium | Critical | AC-NFR-SEC-6 R8 strip + CI grep + `SecretString` (AC-D-8) |
| R-6 | Thread-marshalling race → "signal never fired" support floods | Medium | Medium | AC-A-1 documented primitive + unit test; AC-A-8 watchdog |
| R-7 | Dep supply-chain attack (OkHttp/Tink/BC compromised) | Low | Critical | AC-NFR-SEC-8 gradle.lockfile + dependency-verification + reproducible build |
| R-8 | Scaffold convention drift bleeds into production (compileSdk=33, `println`, `my*` globals) | High (already present) | Low | AC-D-12 tracked as issues until resolved |
| R-9 | Consumer app's manifest merge re-enables `allowBackup` | Low | High | AC-NFR-SEC-2 merged-manifest CI inspection |
| R-10 | Grant reviewer can't reproduce parity claim | Low | Critical ($10k) | AC-FR-12 `REVIEW.md` + 10-min reproducible flow |

## Open Questions

- **OQ-1**: Exact Godot 4.x minor for GDExtension ABI pin — resolves in Architecture (DD-9).
- **OQ-2**: Android plugin test strategy (Firebase Test Lab vs local emulator vs CI-hosted Android runners) — resolves in Architecture/QA (DD-10).

## Out of Scope

- iOS support
- Godot 3.x support
- Non-MWA wallet flows (WalletConnect, deep links, hardware wallets)
- Custom wallet-picker UI
- Transaction-building helpers
- Polished showcase app
