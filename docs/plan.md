---
status: complete
feature: mwa-android
created: 2026-04-16
phase: 4
---
# Plan: Godot Mobile Wallet Adapter (MWA) — Android

## Decomposition Strategy

**Selected:** Option C — Hybrid Interface-First Foundation + Vertical Slices + Polish.

**Rationale:** The architecture has four seams (`ISigner`, `MwaAndroidBridge`, `MwaClient`, `GodotMainDispatcher`) plus an error-codes codegen toolchain. Bottom-up (A) delays any user-visible progress until very late; pure vertical slicing (B) forces the first operation story to invent every seam inline, making it enormous and risking rework as later operations revisit design choices. Option C pays the foundation cost upfront (6 stories of interface-first scaffolding — all 3 languages compile and load with `UNSUPPORTED_PLATFORM` stubs), then each subsequent vertical slice codes against concrete interfaces. After Epic 1, each MWA method is a clean vertical slice.

---

## Epic 1: Foundation — Interface-First Scaffolding

Goal: stand up the entire 3-language layering with compiling, loading no-op implementations. After this epic, any story in Epics 2–5 can code against concrete interfaces.

### Story 1-1: Error-Codes Manifest + 3-Language Codegen + Generator Tests [M]

**User Story:** As an SDK contributor, I want a single source of truth for the 13 MWA error codes that regenerates Kotlin + C++ + GDScript constants, so that enum drift across layers is impossible.

**Dependencies:** none
**Wave:** 1
**FR Coverage:** — (enabling)
**NFR Coverage:** NFR-4 (parity foundation)
**Architecture Ref:** DD-13, DD-33, §3.3, §10.3.2

**Acceptance Criteria:**
- **AC-1 (happy):** *Given* `tools/error-codes.yaml` defines 13 codes with `cpp_enum`, `kotlin_enum`, `recoverable`, `retry_hint`, `default_user_message`, *when* `python tools/gen_error_codes.py` runs, *then* `android/plugin/src/generated/.../MwaError.kt` contains a sealed class with 13 subclasses, `src/generated/mwa_error_codes.hpp` contains `enum class MwaErrorCode` with 13 members + `code_name()` function, and `addons/SolanaSDK/mwa_error_codes.gd` contains 13 string constants; all 3 files regenerate byte-identically on repeat invocation.
- **AC-2 (drift check):** *Given* the generated files are committed and `error-codes.yaml` is modified, *when* the CI `codegen_drift_check` job runs, *then* the job fails the build with a diff showing the drift.
- **AC-3 (generator tests):** *Given* the generator Python script, *when* `pytest tools/test_gen_error_codes.py` runs, *then* 3 golden-file fixtures assert byte-identical output; property-based tests round-trip random valid manifests and cross-verify `recoverable` / `retry_hint` across all 3 artifacts; malformed YAML produces a specific error message; test suite is PR-blocking independent of drift check.

**Interface Contract:**
- **Defines:** `tools/error-codes.yaml` schema (v1), `enum class MwaErrorCode` in C++, `sealed class MwaError` in Kotlin, `const MWA_ERR_*` in GDScript.
- **Consumed by:** every story in Epics 2–5 that emits `mwa_error` signals.

---

### Story 1-2: SecretString + SdkLog + Scaffold Cleanup [M]

**User Story:** As an SDK maintainer, I want a redacted-by-default secret wrapper, a release-stripped logger with lambda-safe signatures, and a cleaned-up Android plugin scaffold, so that token leakage is impossible by construction and the codebase follows the host repo's conventions.

**Dependencies:** none
**Wave:** 1
**FR Coverage:** — (enabling / cross-cutting)
**NFR Coverage:** NFR-1 (defines SecretString; cross-cutting thereafter), NFR-2 (fixes compileSdk/minSdk drift)
**Architecture Ref:** DD-14, DD-30, §5.5, §8, AC-D-8, AC-D-12

**Acceptance Criteria:**
- **AC-1 (happy):** *Given* `SecretString` Kotlin class (wrapping `ByteArray`) and C++ POD-like `SecretString` wrapping `godot::PackedByteArray`, *when* either is stringified via `toString()` / `operator<<`, *then* the literal string `"<redacted>"` is returned; explicit `.reveal(): ByteArray` / `.reveal_bytes()` methods are the only way to access contents.
- **AC-2 (SdkLog lambda):** *Given* `SdkLog.v/d/i/w/e` accept `(tag: String, corrId: String, msgFactory: () -> String)` signatures, *when* a release build runs with R8 `-assumenosideeffects` on `SdkLog.v/d`, *then* both the call AND the lambda body are stripped (argument evaluation does NOT run); verified by decompiling release AAR.
- **AC-3 (scaffold cleanup):** *Given* the existing `android/plugin/` scaffold has compileSdk=33, minSdk=21, `println()` calls, module-global `var myResult`/`myAction`/`myConnectedKey`/etc., and no ktlint config, *when* this story merges, *then* compileSdk=34, minSdk=23, `println` replaced with `SdkLog.d`, module globals replaced by a scoped `MwaSessionState` class, ktlint config added and `./gradlew ktlintCheck` green.
- **AC-4 (CI grep):** *Given* CI runs a grep lint, *when* any source file calls `Log.*(authToken)`, `Log.*(publicKey)`, `Log.*(SecretString.reveal())`, or `SdkLog.[vdiwe]\(.*\$\{` (string interpolation without lambda), *then* the build fails with a named violation line.

**Interface Contract:**
- **Defines:** `class SecretString { fun reveal(): ByteArray; override fun toString() = "<redacted>" }` Kotlin; `class SecretString { PackedByteArray reveal_bytes() const; String to_string() const { return "<redacted>"; } }` C++; `object SdkLog { fun v(tag: String, corrId: String, msg: () -> String); ... }`.
- **Consumed by:** every story that handles tokens (2-1, 2-2, 3-*, 4-*).

---

### Story 1-3: ISigner in godot-solana-sdk Core + Transaction Refactor + Compat Shim [M]

**User Story:** As an SDK consumer, I want `Transaction.sign(keypair)` to keep working after MWA lands, so that my existing godot-solana-sdk integration doesn't break.

**Dependencies:** none
**Wave:** 1
**FR Coverage:** — (enabling)
**NFR Coverage:** — (prerequisite for NFR-4 parity path)
**Architecture Ref:** DD-11, DD-25, §2.3.1

**Acceptance Criteria:**
- **AC-1 (interface defined):** *Given* `src/isigner.hpp` is added to core, *when* it compiles, *then* `ISigner : public godot::RefCounted` declares `virtual bool is_connected() const`, `virtual godot::String get_public_key() const`, `virtual void sign_messages(PackedByteArray, PackedInt32Array, String request_id)`, `virtual void sign_transactions(PackedByteArray, PackedInt32Array, String request_id)` with signals `sign_completed(String, Array)` and `sign_failed(String, String, String)`.
- **AC-2 (Transaction refactor):** *Given* `Transaction` node previously held a concrete signer reference, *when* the refactor merges, *then* `Transaction` depends only on `Ref<ISigner>` and the existing desktop `WalletAdapter` class `implements ISigner` via a thin bridging impl.
- **AC-3 (compat shim):** *Given* existing consumer code calls `Transaction.sign(keypair)`, *when* the code runs against the post-refactor SDK, *then* it works unchanged (wrapped internally by `LocalKeypairSigner : ISigner`) AND emits a one-time deprecation warning `"Transaction.sign(Keypair) is deprecated; use ISigner. See docs/migrations/isigner.md. Removed in v1.2."` via Godot `push_warning()`.
- **AC-4 (migration doc):** *Given* `docs/migrations/isigner.md` is authored, *when* a consumer reads it, *then* they find: what changed, a migration diff (before/after GDScript), the deprecation timeline (v1.1 warn → v1.2 remove), and an FAQ covering the common 3 use cases.
- **AC-5 (error case):** *Given* a post-v1.2 build (flag simulated via compile define), *when* `Transaction.sign(keypair)` is called, *then* it errors with `ERR_METHOD_NOT_FOUND` and a clear message pointing at `ISigner`.

**Interface Contract:**
- **Defines:** `ISigner` (C++ pure-virtual interface in `src/isigner.hpp`) — load-bearing for DD-11/DD-25.
- **Consumed by:** Story 2-1 (MWA `MobileWalletAdapter` implements it).

---

### Story 1-4: MwaAndroidBridge Interface + Platform Selector TU + MockMwaAndroidBridge + GodotMainDispatcher [M]

**User Story:** As an SDK contributor, I want a platform-agnostic C++ bridge that returns `UNSUPPORTED_PLATFORM` on non-Android and delegates to JNI on Android, and a main-thread dispatcher that guarantees all signals cross to Godot main, so that non-Android builds don't need `#ifdef __ANDROID__` sprawl and signal marshalling is verifiable.

**Dependencies:** 1-1 (error codes), 1-2 (SecretString)
**Wave:** 2
**FR Coverage:** FR-12 (non-blocking)
**NFR Coverage:** NFR-9 (≤16 ms return — dispatcher enforces main-thread marshalling)
**Architecture Ref:** DD-12, DD-22, DD-26, §2.3, §7.1, §7.1.1

**Acceptance Criteria:**
- **AC-1 (interface):** *Given* `src/mwa/include/mwa_android_bridge.hpp` declares `class MwaAndroidBridge` with pure-virtual methods for each of the 7 MWA ops + lifecycle, *when* compiled, *then* all public headers under `src/mwa/include/` reference only `godot::` types (no `std::`); verified by CI header-lint.
- **AC-2 (platform selector):** *Given* `src/mwa/platform_selector.cpp` is the only conditional-compilation TU, *when* built for non-Android, *then* `MwaAndroidBridge::create()` returns a no-op impl that emits `mwa_error{code=UNSUPPORTED_PLATFORM, message="MWA is Android-only; OS.get_name()=<name>"}` within 1 frame for every method; *when* built for Android, returns `MwaAndroidBridgeJni` stub (no JNI calls yet — just logs and returns `NOT_CONNECTED`).
- **AC-3 (MockMwaAndroidBridge):** *Given* `MockMwaAndroidBridge` test double, *when* a host-platform unit test (`MWA.is_supported == false` on Linux etc.) invokes a method, *then* it records the call + args + lets the test drive the callback on a named thread.
- **AC-4 (GodotMainDispatcher):** *Given* `class GodotMainDispatcher` wraps a `Ref<MobileWalletAdapter>` node and exposes `post(signal_name: String, payload: Dictionary)`, *when* invoked from any thread, *then* the emission runs on Godot main thread via `call_deferred("emit_signal", signal_name, payload)`.
- **AC-5 (architectural check):** *Given* CI greps source, *when* any C++ TU outside `platform_selector.cpp` contains `#ifdef __ANDROID__`, *then* the build fails with a named file and line.
- **AC-6 (error case):** *Given* `GodotMainDispatcher` called before the node is registered (e.g., during engine teardown), *when* `post()` runs, *then* the call is silently dropped with `SdkLog.w("dispatcher post after destroy")` — no crash.

**Interface Contract:**
- **Defines:** `MwaAndroidBridge` (C++), `GodotMainDispatcher` (C++), `MockMwaAndroidBridge` (C++ test helper).
- **Consumed by:** Story 1-5 (node skeleton), 2-1, all operation stories.

---

### Story 1-5: MobileWalletAdapter C++ Node Skeleton + Thread-Hop Unit Test [S]

**User Story:** As an SDK contributor, I want a `MobileWalletAdapter` C++ GDExtension node that registers all 11 signals and delegates every method to `MwaAndroidBridge` via `GodotMainDispatcher`, so that the Godot engine sees the full signal surface from day one and our thread-hop rule is testable.

**Dependencies:** 1-4
**Wave:** 3
**FR Coverage:** FR-12, FR-13 (signal skeleton)
**NFR Coverage:** NFR-9
**Architecture Ref:** DD-22, §2.2 (component B), §7.1

**Acceptance Criteria:**
- **AC-1 (happy):** *Given* `MobileWalletAdapter` GDExtension node is registered, *when* the engine loads on Android, *then* all 11 signals (`connect_completed`, `reauthorize_completed`, `disconnect_completed`, `deauthorize_completed`, `sign_messages_completed`, `sign_transactions_completed`, `sign_and_send_completed`, `mwa_error`, `mwa_timeout`, `mwa_cancelled_lifecycle`, `reauth_required`) and 13 methods (7 ops + 4 state getters + 2 utility) bind successfully; calling any op method returns within ≤16 ms.
- **AC-2 (thread-hop test):** *Given* a host-platform unit test (Linux/Windows/macOS) with `MockMwaAndroidBridge`, *when* the test invokes the mock's callback from a `std::thread` (not Godot main), *then* the corresponding signal fires on Godot main thread within 1 frame AND the count of direct `emit_signal` calls not routed through `GodotMainDispatcher` is 0 (verified by source grep).
- **AC-3 (pre-op error case):** *Given* the node on Android but without the Kotlin plugin registered (simulated), *when* any op method is invoked, *then* `mwa_error{code=UNSUPPORTED_PLATFORM, message="Kotlin plugin not loaded"}` fires within 1 frame.

**Interface Contract:**
- **Defines:** 11 GDScript-visible signals + 13 GDScript-visible methods bound via `ClassDB::bind_method`.
- **Consumed by:** Story 2-1 (real `connect` impl replaces skeleton's `NOT_CONNECTED` stub).

---

### Story 1-6: MwaClient Kotlin Interface + FakeMwaClient + testdata/mwa-fixtures [M]

**User Story:** As an SDK contributor, I want a Kotlin `MwaClient` interface wrapping clientlib-ktx with a `FakeMwaClient` reading JSON fixtures, so that the Kotlin plugin is unit-testable without Android device and Fake Wallet APK behavior is reproducible.

**Dependencies:** 1-1 (error codes), 1-2 (SecretString)
**Wave:** 2
**FR Coverage:** — (enabling)
**NFR Coverage:** NFR-4 (parity surface), NFR-10 (0 hand-rolled protocol)
**Architecture Ref:** DD-19, DD-28, §2.2 (components F/G), §10.3.1

**Acceptance Criteria:**
- **AC-1 (interface):** *Given* `MwaClient` interface declares suspend funs for each of the 7 MWA ops + lifecycle returning `Result<T, MwaError>`, *when* `MwaClientImpl` (wrapping `com.solanamobile.mobilewalletadapter.clientlib.MobileWalletAdapter`) is instantiated, *then* each method invokes `clientlib.transact { ... }` with the canonical identity/cluster config.
- **AC-2 (FakeMwaClient):** *Given* `FakeMwaClient` reads `testdata/mwa-fixtures/{op}_{scenario}.json` (e.g., `authorize_success.json`, `authorize_user_canceled.json`, `sign_messages_wallet_rejected.json`, …), *when* a test configures the fake with a scenario name, *then* the next op returns the canned response (success or typed `MwaError`) without any network.
- **AC-3 (fixture format):** *Given* `testdata/mwa-fixtures/` contains 20+ JSON files, *when* the schema-check script `tools/validate_fixtures.py` runs in CI, *then* all files validate against the documented fixture schema (scenario name, expected input shape, response type, response payload); PR-blocking.
- **AC-4 (error case):** *Given* a test configures `FakeMwaClient` with a non-existent scenario name, *when* the op runs, *then* an assertion error fires immediately with message `"no fixture for scenario '<name>'"`.

**Interface Contract:**
- **Defines:** `interface MwaClient { suspend fun authorize(...); suspend fun reauthorize(...); ... }`; `class FakeMwaClient(fixtureDir: File) : MwaClient`; fixture JSON schema in `testdata/mwa-fixtures/SCHEMA.md`.
- **Consumed by:** Story 2-1, 2-2, 2-3, 3-1, 3-2, 3-3, 4-1, 4-2 (all plugin-side logic); T4 contract tier.

---

## Epic 2: Authorize Path

### Story 2-1: connect + SecureTokenStore + EncryptedSharedPreferences + Cache Key + Identity + Cluster [L]

**User Story:** As a Godot game developer, I want to call `MWA.connect(identity, cluster)` and receive a `connect_completed` signal with the player's public key, auth token fingerprint, and wallet label, so that my game is authenticated against the wallet.

**Dependencies:** 1-1, 1-2, 1-3, 1-4, 1-5, 1-6
**Wave:** 4
**FR Coverage:** FR-2, FR-3, FR-10, FR-11, FR-14
**NFR Coverage:** NFR-1 (direct — first token persisted encrypted), NFR-7 (cluster param), NFR-8 (no per-wallet code), NFR-9 (≤16 ms return)
**Architecture Ref:** §2, §3.1, §4.1, §7.2, §7.3, §8.3, DD-2, DD-17, DD-23, DD-27

**Why L (not splittable):** This is the first end-to-end vertical slice. It must stand up secure storage (EncryptedSharedPreferences + MasterKey + cache key canonicalization + CacheRecord schema), the authorize clientlib-ktx call path, the watchdog, the terminal-signal invariant, identity metadata plumbing, cluster parameter plumbing, and the GDScript state getters (`is_connected`, `get_public_key`, `get_cluster`, `get_wallet_label`, `get_auth_token_fingerprint`). Splitting would require a "no-op authorize returning a fake token" intermediate, which cannot be tested E2E and creates a pointless merge-and-rewrite cycle.

**Acceptance Criteria:**
- **AC-1 (SecureTokenStore):** *Given* `SecureTokenStore` is constructed with a `Context`, *when* `putToken(key, record)` and later `getToken(key)` run, *then* the record round-trips; the underlying `.xml` file on disk contains no plaintext `auth_token` bytes (verified by `androidTest` that scans the file post-put); key is `"mwa::v1::" + SHA256(length-prefixed canonical(cluster, chain_id, wallet_package, identity_uri))`.
- **AC-2 (canonical key):** *Given* two tuples `(devnet, solana:devnet, com.p.app, "https://ex.com/?a=1|2")` and `(devnet|solana:devnet, com.p.app, "https://ex.com/?a=1", 2)`, *when* the cache key is computed, *then* the two SHA256 outputs differ (delimiter collision prevented by length-prefixing).
- **AC-3 (connect E2E happy):** *Given* Fake Wallet APK is installed on the emulator, a `MwaClient` is authorize-ready, *when* GDScript calls `MWA.connect({name:"G",icon_uri:"",identity_uri:"https://ex.com"}, "devnet")`, *then* `connect` returns in ≤16 ms; within ≤3 s `connect_completed` fires on Godot main thread with `{request_id, auth_token_fingerprint, public_key, wallet_label, wallet_icon_uri, cluster}` all populated; `MWA.is_connected()` returns true; `SecureTokenStore.getToken(key)` returns the CacheRecord.
- **AC-4 (no-wallet error):** *Given* no MWA-compliant wallet installed, *when* `MWA.connect(...)` is called, *then* within 1 frame `mwa_error{code=NO_MWA_WALLET_INSTALLED}` fires (NOT `USER_CANCELED` from an empty chooser); `get_suggested_wallet_install_links()` returns at least 1 entry.
- **AC-5 (user canceled):** *Given* Fake Wallet configured with `authorize_user_canceled.json` fixture, *when* `MWA.connect(...)` is called, *then* `mwa_error{code=USER_CANCELED, recoverable=true, retry_hint="retry_now"}` fires; `is_connected()` returns false; no token stored.
- **AC-6 (watchdog timeout):** *Given* Fake Wallet configured to delay 180000 ms, *when* `MWA.connect(..., {timeout_ms: 5000})` is called, *then* `mwa_timeout{request_id, source_method:"connect", elapsed_ms≈5000, watchdog_ms:5000}` fires within 1 frame of the elapsed time; late response arriving at 180 s is dropped with `SdkLog.w("late_result")`; `late_result_count` in diagnostics increments by 1.
- **AC-7 (fingerprint):** *Given* `get_auth_token_fingerprint()` after successful connect, *when* called twice, *then* both returns are the same 8-char hex; the value is `HKDF-SHA256(ikm=auth_token_bytes, salt=per_install_salt, info="mwa-fingerprint-v1")` first 4 bytes hex; per-install salt is stored under Keystore alias `godot-sdk-mwa-fingerprint-salt-v1`.
- **AC-8 (plaintext scan):** *Given* after `MWA.connect(...)` succeeds, *when* `androidTest` scans every file under the app sandbox and the full `adb logcat` since test start, *then* 0 occurrences of the raw token bytes are found.

**Interface Contract:**
- **Defines:** `class SecureTokenStore(Context) { fun getToken(key: CacheKey): CacheRecord?; fun putToken(key, record); fun deleteToken(key); fun deleteAll() }`; `data class CacheKey(cluster, chainId, walletPackage, identityUri) { fun toHash(): String }`; `data class CacheRecord` per §3.1.
- **Consumed by:** 2-2, 2-3, 3-*, 4-*.

---

### Story 2-2: reauthorize + TOKEN_EXPIRED + 10-Cycle Silent Reauth Test [M]

**User Story:** As a Godot game developer, I want to call `MWA.reauthorize()` on app relaunch and have the session silently restore without any wallet UI, so that returning players skip the chooser.

**Dependencies:** 2-1
**Wave:** 5
**FR Coverage:** FR-4, FR-14
**NFR Coverage:** NFR-5 (direct — 10-cycle Phantom test; release-gate), NFR-6 (E2E coverage of reauthorize)
**Architecture Ref:** §4.2, DD-23

**Acceptance Criteria:**
- **AC-1 (silent happy):** *Given* a cached auth token from prior `connect`, *when* `MWA.reauthorize()` is called, *then* `reauthorize_completed` fires within ≤3 s on Godot main thread with the same `public_key` as before; `androidTest` asserts the Activity task stack contains zero transitions to the wallet app (no UI surface).
- **AC-2 (no cached token):** *Given* no token present (post-`deauthorize` or first launch), *when* `MWA.reauthorize()` is called, *then* `mwa_error{code=NOT_CONNECTED, retry_hint="connect"}` fires within 1 frame.
- **AC-3 (token expired / revoked):** *Given* Fake Wallet configured with `reauthorize_token_expired.json`, *when* `MWA.reauthorize()` is called, *then* `mwa_error{code=TOKEN_EXPIRED, retry_hint="connect"}` fires; the SDK does NOT launch a chooser; the cached token is removed from `SecureTokenStore` (graceful wipe).
- **AC-4 (cluster mismatch):** *Given* cached token for cluster=devnet and a subsequent `MWA.reauthorize()` call where active cluster was swapped to mainnet via config, *when* the mismatch is detected, *then* `mwa_error{code=NOT_CONNECTED, retry_hint="connect"}` fires (cache key mismatch forces fresh authorize per DD-27).
- **AC-5 (10-cycle silent — release-gate):** *Given* a real Phantom wallet authorized on-device (T6 release-gate manual/FTL run), *when* the E2E script force-stops the app, relaunches, calls reauthorize, and repeats 10 times, *then* 10/10 cycles succeed with 0 wallet-app Activity transitions AND `get_public_key()` returns the same value every time.
- **AC-6 (preserves fingerprint):** *Given* reauthorize succeeds, *when* `get_auth_token_fingerprint()` is called before and after, *then* it returns the same 8-char hex (proves token identity per AC-1 / DD-34).

**Interface Contract:** N/A (consumes `SecureTokenStore` from 2-1).

---

### Story 2-3: disconnect [S]

**User Story:** As a Godot game developer, I want to call `MWA.disconnect()` to close the session while keeping the auth token cached, so that a later `reauthorize()` still works silently.

**Dependencies:** 2-1
**Wave:** 5
**FR Coverage:** FR-6
**NFR Coverage:** —
**Architecture Ref:** §4 UC-5 (alt flow)

**Acceptance Criteria:**
- **AC-1 (happy):** *Given* `is_connected()==true`, *when* `MWA.disconnect()` is called, *then* `disconnect_completed` fires within ≤200 ms; `is_connected()==false`; `get_public_key()==""`; BUT `SecureTokenStore.getToken(key)` still returns the CacheRecord (token retained).
- **AC-2 (reconnect after disconnect):** *Given* a post-disconnect state, *when* `MWA.reauthorize()` is called, *then* it succeeds silently (per story 2-2 AC-1).
- **AC-3 (double disconnect):** *Given* already disconnected, *when* `MWA.disconnect()` is called again, *then* `disconnect_completed` still fires (idempotent); no error.

---

## Epic 3: Signing Path

### Story 3-1: sign_messages + runSigningOp Shared Pipeline + Contract Test [M]

**User Story:** As a Godot game developer, I want to call `MWA.sign_messages([bytes1, bytes2])` and receive back an array of signatures, so that my game can prove ownership or sign off-chain data.

**Dependencies:** 2-1
**Wave:** 5
**FR Coverage:** FR-7
**NFR Coverage:** NFR-4 (1/3 of signing parity), NFR-6 (E2E coverage)
**Architecture Ref:** AC-D-2 (shared pipeline), §10.3.1 (contract tests), DD-15

**Acceptance Criteria:**
- **AC-1 (shared pipeline):** *Given* a Kotlin `suspend fun <T> runSigningOp(op: SigningOp, request_id: String, block: suspend MobileWalletAdapterClient.() -> T): Result<T, MwaError>` helper handling connection-identity restore, cluster selection, auth-token restore, watchdog, and error translation via ErrorTaxonomy, *when* `signMessages` uses it, *then* the method body is ≤20 Kotlin LOC; unit test asserts LOC budget with a source-line count.
- **AC-2 (happy E2E):** *Given* connected session + Fake Wallet configured with `sign_messages_success.json`, *when* GDScript calls `MWA.sign_messages([PackedByteArray(["hello"]), PackedByteArray(["world"])])`, *then* `sign_messages_completed{signed_payloads: Array[PackedByteArray]}` fires with `signed_payloads.size()==2` and each payload has length ≥ input length + 64 (signature appended).
- **AC-3 (not connected preflight):** *Given* `is_connected()==false`, *when* `MWA.sign_messages([bytes])` is called, *then* `mwa_error{code=NOT_CONNECTED, user_message:"Not connected to a wallet. Call MWA.connect() first.", retry_hint:"connect"}` fires within 1 frame (no wallet UI).
- **AC-4 (user rejects):** *Given* Fake Wallet configured with `sign_messages_user_canceled.json`, *when* called, *then* `mwa_error{code=USER_CANCELED, recoverable=true}` fires.
- **AC-5 (contract test):** *Given* fixtures in `testdata/mwa-fixtures/sign_messages_*.json`, *when* the T4 contract test tier runs, *then* actual JSON-RPC request shapes sent by both `FakeMwaClient` AND the recorded-fixture real-clientlib adapter are asserted byte-identical against the published MWA protocol fixtures; divergence is PR-blocking.

**Interface Contract:**
- **Defines:** `suspend fun <T> MwaAndroidPlugin.runSigningOp(...)` helper.
- **Consumed by:** 3-2, 3-3.

---

### Story 3-2: sign_transactions + Contract Test [M]

**User Story:** As a Godot game developer, I want to call `MWA.sign_transactions([tx_bytes])` and receive back the signed transactions, so that I can broadcast them myself via existing `SolanaClient` nodes.

**Dependencies:** 3-1 (uses `runSigningOp`)
**Wave:** 6
**FR Coverage:** FR-8
**NFR Coverage:** NFR-4 (2/3 signing parity), NFR-6
**Architecture Ref:** AC-D-2, DD-15

**Acceptance Criteria:**
- **AC-1 (happy):** *Given* connected session + fixture `sign_transactions_success.json`, *when* `MWA.sign_transactions([tx1, tx2])` is called, *then* `sign_transactions_completed{signed_transactions: Array[PackedByteArray]}` fires with `.size()==2`; each signed tx deserializes as a valid Solana Transaction (signature present).
- **AC-2 (minimum new LOC):** *Given* `runSigningOp` exists (from 3-1), *when* `signTransactions` is added to `MwaAndroidPlugin.kt`, *then* the new Kotlin body is ≤20 LOC; unit test asserts.
- **AC-3 (preflight):** *Given* `is_connected()==false`, *when* called, *then* `mwa_error{code=NOT_CONNECTED}` within 1 frame.
- **AC-4 (wallet rejects):** *Given* fixture `sign_transactions_wallet_rejected.json`, *when* called, *then* `mwa_error{code=WALLET_REJECTED}` fires.
- **AC-5 (contract test):** JSON-RPC request shape asserted against MWA fixture set (T4 tier), PR-blocking.

---

### Story 3-3: sign_and_send + Pending-Submission Breadcrumb + pending_submission_found Signal [L]

**User Story:** As a Godot game developer, I want to call `MWA.sign_and_send([tx_bytes])` and receive tx signatures after the wallet submits to the cluster, AND be notified on next app launch if my last sign_and_send might have landed after a process death, so that players never silently lose transactions.

**Dependencies:** 3-1 (uses `runSigningOp`)
**Wave:** 6
**FR Coverage:** FR-9
**NFR Coverage:** NFR-4 (3/3 signing parity), NFR-6, NFR-7 (cluster bleed prevention)
**Architecture Ref:** §7.4, DD-24, AC-A-13

**Why L (not splittable):** sign_and_send + the pending-submission breadcrumb are one mental unit. Splitting would leave the breadcrumb in a separate story requiring a rewrite of the `sign_and_send` code path that just landed. Breadcrumb write-ahead MUST be part of the initial implementation, not grafted on later — otherwise a version ships without it and real money is lost during the gap.

**Acceptance Criteria:**
- **AC-1 (happy):** *Given* connected session + fixture `sign_and_send_success.json`, *when* `MWA.sign_and_send([tx1, tx2])` is called, *then* `sign_and_send_completed{signatures: Array[String], submitted_at: int, confirmation_status: "submitted"}` fires with 2 base58 signatures; example app labels the state `"Submitted — awaiting confirmation"`.
- **AC-2 (breadcrumb write-ahead):** *Given* the op starts, *when* clientlib.transact begins, *then* `SecureTokenStore` contains an entry `"pending::<request_id>"` with `{request_id, op_type:"sign_and_send", cluster, identity_uri, started_at_ms, tx_count, tx_preview_hashes:[sha256(tx)[:32]]}`.
- **AC-3 (breadcrumb cleared on success):** *Given* the op completes successfully, *then* the `"pending::<request_id>"` entry is removed from SecureTokenStore before `sign_and_send_completed` fires.
- **AC-4 (breadcrumb cleared on error):** *Given* any terminal signal (`mwa_error`, `mwa_timeout`, `mwa_cancelled_lifecycle`), *then* the pending entry is removed.
- **AC-5 (pending_submission_found on next launch):** *Given* process was killed mid-op (simulated by skipping the cleanup step), *when* the next successful `connect` or `reauthorize` completes, *then* `pending_submission_found{request_id, op_type, started_at_ms, tx_count, tx_preview_hashes, recommendation:"check_chain_for_signatures"}` fires AND the stale pending entry is then removed (one-shot).
- **AC-6 (cluster-bleed refuse):** *Given* connected session for cluster=devnet + cached token for devnet, *when* `MWA.sign_and_send([tx])` is called but the active session's cluster has been swapped to mainnet-beta, *then* `mwa_error{code=NOT_CONNECTED}` fires (cluster-key mismatch per DD-27 / AC-NFR-SEC-4).
- **AC-7 (contract test):** JSON-RPC request shape asserted (T4 tier).

---

## Epic 4: Token Lifecycle & Cache Hardening

### Story 4-1: deauthorize + finally-block Semantics + Cache Wipe Independent of Remote [M]

**User Story:** As a Godot game developer, I want `MWA.deauthorize()` to ALWAYS clear the local token even if the wallet is unreachable, so that a returned-and-stolen device never retains a stale connected state.

**Dependencies:** 2-1
**Wave:** 5
**FR Coverage:** FR-5
**NFR Coverage:** AC-NFR-SEC-5 (local wipe unconditional)
**Architecture Ref:** §4.5, AC-NFR-SEC-5, DD-15

**Acceptance Criteria:**
- **AC-1 (happy remote + local):** *Given* connected session, wallet online, *when* `MWA.deauthorize()` is called, *then* within ≤3 s `deauthorize_completed{remote_revoke_succeeded: true, local_cache_cleared: true, warning: ""}` fires; `SecureTokenStore.getToken(key)` returns null; `is_connected()==false`.
- **AC-2 (network-off):** *Given* network disabled, *when* `MWA.deauthorize()` is called, *then* `deauthorize_completed{remote_revoke_succeeded: false, local_cache_cleared: true, warning: "remote_unreachable"}` fires; token STILL removed from SecureTokenStore; androidTest covers this path with `setNetworkEnabled(false)`.
- **AC-3 (wallet uninstalled):** *Given* wallet app uninstalled between connect and deauth, *when* `MWA.deauthorize()` is called, *then* same shape as AC-2 (local wipe always).
- **AC-4 (idempotent):** *Given* already deauthorized, *when* called again, *then* `deauthorize_completed{remote_revoke_succeeded: true|false, local_cache_cleared: true}` fires (no error).
- **AC-5 (finally-block enforcement):** *Given* Kotlin code in `MwaAndroidPlugin.deauthorize`, *when* reviewed, *then* `SecureTokenStore.deleteToken()` + `MwaSessionState.clear()` are inside a `finally` block; a CI structure test (AST or source regex) verifies this.

---

### Story 4-2: forget_all + MasterKey Rotation + Mutex + Cancel In-Flight [M]

**User Story:** As a Godot game developer, I want `MWA.forget_all()` to wipe every cached wallet token, rotate the encryption key, and cancel any in-flight ops, so that I can implement a compliant "Sign out everywhere" flow for privacy regulation.

**Dependencies:** 4-1
**Wave:** 6
**FR Coverage:** — (AC-NFR-SEC-11, GDPR/CCPA)
**NFR Coverage:** AC-NFR-SEC-11
**Architecture Ref:** §8.3.1, DD-29

**Acceptance Criteria:**
- **AC-1 (happy):** *Given* 3 cached tokens from 3 different wallets + 0 in-flight ops, *when* `MWA.forget_all()` is called, *then* within ≤10 s: best-effort deauthorize is attempted on each, `SecureTokenStore.listAllKeys().isEmpty()==true`, `MasterKey` alias `godot-sdk-mwa-master-key-v1` was deleted and recreated (verified by `KeyStore.containsAlias` before/after), `MwaSessionState` is clear, `is_connected()==false`.
- **AC-2 (cancels in-flight):** *Given* an in-flight `sign_messages` op, *when* `forget_all()` is called, *then* `mwa_cancelled_lifecycle{request_id, source_method:"sign_messages", reason:"forget_all_invoked"}` fires BEFORE the wipe starts.
- **AC-3 (mutex correctness):** *Given* concurrent callers invoke `forget_all()` and `connect()` simultaneously, *when* both run, *then* one completes first, the other observes clean state (connect may fail with `UNSUPPORTED_PLATFORM` or `PROTOCOL_ERROR` but does NOT see partial state); unit test uses two coroutines.
- **AC-4 (wallet offline):** *Given* some wallets unreachable, *when* forget_all runs, *then* local wipe completes regardless; unreachable wallets are logged but do not block.

---

### Story 4-3: Keystore Corruption Recovery + Manifest Hardening + R8 Rules + Dep Lockfile + Reproducible Build [L]

**User Story:** As an SDK auditor, I want the plugin to (a) recover without crashing when Android Keystore entries are invalidated by a lockscreen reset, (b) ship with hardened manifest + R8 rules that consumer apps cannot silently weaken, and (c) have reproducible builds with verified dependencies, so that the plugin passes a mobile security audit.

**Dependencies:** 2-1
**Wave:** 5
**FR Coverage:** — (AC-NFR-SEC-1..9)
**NFR Coverage:** AC-NFR-SEC-2, -3, -6, -7, -8, -9
**Architecture Ref:** §8.2, §8.3, §8.4, §8.5, DD-14

**Why L (not splittable):** Keystore recovery, manifest hardening, R8 rules, dep-lockfile, and reproducible-build CI form one coherent security hardening unit. Splitting them creates interim merges with partial protection (e.g., hardened manifest but missing R8 rules → release builds broken in a specific way). Each sub-item is small alone but demands that the others exist to be meaningful. Combined verification is tractable in one audited PR.

**Acceptance Criteria:**
- **AC-1 (Keystore recovery):** *Given* `EncryptedSharedPreferences.create(...)` throws `GeneralSecurityException` or `InvalidProtocolBufferException` (simulated by pre-populating a ciphertext file then wiping MasterKey alias), *when* the plugin boots, *then* `wipeCorruptPrefs()` deletes the `.xml` file AND `reauth_required{reason:"keystore_corrupt"}` fires AND the plugin is usable for a fresh `connect` (no crash, no stuck state).
- **AC-2 (manifest merge):** *Given* the plugin's `AndroidManifest.xml` declares `android:allowBackup="false"` with `tools:replace` + `fullBackupContent="@xml/backup_rules"` excluding the prefs file, *when* CI builds a synthetic consumer app that itself declares `android:allowBackup="true"`, *then* the merged manifest (`aapt2 dump badging` output) shows backup rule for our prefs file is preserved; CI asserts.
- **AC-3 (Intent hardening):** *Given* manifest inspection, *when* CI runs, *then* no Activity introduced by this plugin has `android:exported="true"` without justification comment; any Activity that DOES set `exported="true"` has `taskAffinity=""`, `launchMode="singleTask"`, and an intent filter that verifies calling package; no auto-exported `ContentProvider` appears.
- **AC-4 (consumer-rules.pro):** *Given* the AAR is unpacked, *when* inspecting `META-INF/proguard/consumer-rules.pro`, *then* it contains `-keep` rules for JNI native method names, `@UsedByGodot` annotated methods, `clientlib-ktx` reflection entry points, and `-assumenosideeffects` for `SdkLog.v/d`.
- **AC-5 (R8 release smoke):** *Given* a release-variant APK built with R8 enabled, *when* an instrumented test invokes `authorize` E2E against Fake Wallet, *then* the test passes; CI job `r8_release_smoke` runs this on every PR.
- **AC-6 (dep lockfile + verification):** *Given* `gradle.lockfile` and `dependency-verification.xml` are committed, *when* CI runs `./gradlew --write-verification-metadata pgp,sha256 dependencies`, *then* no drift is detected; a reproducible-build job rebuilds the AAR on two separate macos-latest runners and asserts byte-identical SHA256.
- **AC-7 (error case — manifest re-enabled):** *Given* a consumer app removes `tools:replace="android:allowBackup"` and sets `allowBackup="true"`, *when* the CI manifest-merge audit runs, *then* the audit emits a clear warning naming the consumer-side line; the plugin itself does not fail but the warning is visible in the merged manifest report.

---

## Epic 5: Polish — Observability, Lifecycle, Example, Docs, CI

### Story 5-1: Non-Android Platform Behavior + 4-Platform GDExtension Load CI [S]

**User Story:** As a Godot game developer testing on desktop before deploying to Android, I want MWA methods to return a typed `UNSUPPORTED_PLATFORM` error (not crash the editor) when I'm on non-Android, so that my cross-platform code compiles and runs with a clear error signal.

**Dependencies:** 1-4, 1-5
**Wave:** 7
**FR Coverage:** — (AC-U-6, AC-D-6)
**NFR Coverage:** NFR-3 (by platform load)
**Architecture Ref:** AC-A-4, AC-D-6, §2.3

**Acceptance Criteria:**
- **AC-1 (UNSUPPORTED on desktop):** *Given* GDExtension is loaded on Linux/Windows/macOS/Web in headless Godot, *when* any MWA method is called, *then* `mwa_error{code=UNSUPPORTED_PLATFORM, user_message:"MWA is only available on Android.", developer_details:"OS.get_name()=<name>"}` fires within 1 frame; `MWA.is_supported()==false`; `is_connected()==false`.
- **AC-2 (no crash):** *Given* GDExtension load in headless Godot on all 4 non-Android platforms, *when* the engine initializes, *then* 0 crashes, 0 segfaults; the plugin symbol table resolves cleanly.
- **AC-3 (CI matrix):** Existing `.github/workflows/build.yml` matrix is extended; a new step loads the built GDExtension via a headless Godot test scene and asserts UNSUPPORTED_PLATFORM emission on each of Linux/Windows/macOS/Web.

---

### Story 5-2: get_diagnostics() + Correlation Ring Buffer + HKDF Fingerprint + device_posture + get_suggested_wallet_install_links [M]

**User Story:** As a support engineer triaging a user's "signal never fired" bug report, I want `MWA.get_diagnostics()` to return the last 20 correlation IDs, session state, versions, and a non-sensitive auth_token fingerprint, so that I can match logcat traces to user-reported symptoms without needing the device.

**Dependencies:** 2-1 (relies on connect's plumbing)
**Wave:** 7
**FR Coverage:** — (AC-D-22)
**NFR Coverage:** —
**Architecture Ref:** DD-32, DD-34, §6.2, §11

**Acceptance Criteria:**
- **AC-1 (diagnostics shape):** *Given* any session state, *when* `MWA.get_diagnostics()` is called, *then* a Dictionary is returned containing exactly these top-level keys: `sdk_version`, `clientlib_ktx_version`, `godot_version`, `android_api_level`, `session_state`, `wallet_package`, `wallet_version`, `auth_token_fingerprint`, `cluster`, `last_n_correlation_trace` (Array, length ≤ 20), `late_result_count`, `pending_submissions_count`.
- **AC-2 (ring buffer):** *Given* 25 op invocations, *when* `get_diagnostics()` is called, *then* `last_n_correlation_trace.size()==20` (oldest 5 evicted); each entry has `{request_id, source_method, terminal_signal, elapsed_ms, timestamp_ms}`.
- **AC-3 (non-sensitive):** *Given* the full diagnostics payload, *when* inspected, *then* 0 occurrences of auth token bytes, signing key material, or PII; safe to attach to a bug report.
- **AC-4 (device_posture):** *Given* `MWA.get_device_posture()`, *when* called, *then* returns `{rooted: bool, debuggable: bool, developer_options_on: bool, adb_enabled: bool}`; values sourced via `Build.TAGS`, `ApplicationInfo.FLAG_DEBUGGABLE`, `Settings.Secure.DEVELOPMENT_SETTINGS_ENABLED`, `Settings.Global.ADB_ENABLED`; documented as non-authoritative.
- **AC-5 (wallet install links):** *Given* `MWA.get_suggested_wallet_install_links()`, *when* called, *then* returns an Array of ≥3 Dictionaries `{name, package_id, play_store_url}` covering Phantom, Solflare, Backpack.

---

### Story 5-3: Lifecycle Observer + mwa_cancelled_lifecycle + Rotation Test [S]

**User Story:** As a Godot game developer, I want in-flight MWA ops to emit a deterministic cancellation signal when Android destroys my Activity (e.g., on device rotation), so that my UI doesn't hang waiting for a signal that will never arrive.

**Dependencies:** 1-5
**Wave:** 7
**FR Coverage:** FR-13 (error/cancel path)
**NFR Coverage:** AC-A-2
**Architecture Ref:** §4.4, §7.5, DD-23

**Acceptance Criteria:**
- **AC-1 (happy):** *Given* `MwaAndroidPlugin` registers `DefaultLifecycleObserver` on the Godot `Activity`, *when* the Activity emits `Lifecycle.Event.ON_DESTROY` with an in-flight `connect` op, *then* the op's coroutine is cancelled AND `mwa_cancelled_lifecycle{request_id, source_method:"connect", reason:"activity_destroyed"}` fires (terminal signal, per DD-23).
- **AC-2 (rotation instrumented test):** *Given* an `androidTest` that calls `MWA.connect(...)` and triggers a configuration change (rotation) mid-flight via `ActivityScenario.recreate()`, *when* the test runs, *then* the test observes `mwa_cancelled_lifecycle` within 1 s AND no further signal fires for that `request_id`.
- **AC-3 (no double terminal):** *Given* a connect already completed successfully before rotation, *when* rotation fires ON_DESTROY, *then* NO `mwa_cancelled_lifecycle` is emitted for the already-terminated `request_id`.

---

### Story 5-4: Minimal Example App + REVIEW.md + devnet Default + Scripted 7/7 Demo [M]

**User Story:** As a grant reviewer, I want a minimal example Godot Android app demonstrating all 7 MWA methods on devnet, with a `REVIEW.md` that takes me from clone to running demo in ≤10 minutes, so that I can verify API parity without being an Android/Godot expert.

**Dependencies:** 2-1, 2-2, 2-3, 3-1, 3-2, 3-3, 4-1 (needs core ops working)
**Wave:** 7
**FR Coverage:** FR-1 (demonstrates single-addon install), FR-16 (the example itself)
**NFR Coverage:** AC-FR-12, AC-A-10
**Architecture Ref:** §2.2 (component M), AC-NFR-SEC-10

**Acceptance Criteria:**
- **AC-1 (scene):** *Given* the new example at `example/MWA/`, *when* opened in Godot 4.3.x, *then* it contains a single scene `main.tscn` with labeled buttons `[Connect] [Disconnect] [Reauthorize] [Deauthorize] [Sign Message] [Sign Transaction] [Sign and Send] [Reset Local State] [Forget All]`, a `RichTextLabel` log area, and a "Cluster: devnet" label; scene runs without errors in the editor.
- **AC-2 (devnet default):** *Given* the example's config script, *when* inspected, *then* `cluster = "devnet"` is the hard-coded default; switching to `"mainnet-beta"` requires editing a const AND clicking an in-app "I understand — use mainnet" confirm dialog on first use; `identity_uri = "https://example.com/godot-mwa-sample"` is the placeholder (not a real domain).
- **AC-3 (REVIEW.md):** *Given* `REVIEW.md` at repo root, *when* a reviewer follows it, *then* they can: (1) install Android SDK + NDK via documented commands, (2) run `scons addon && cd android && ./gradlew :plugin:assembleRelease`, (3) install the Fake Wallet APK + example APK to an emulator via `adb install`, (4) run a scripted flow (`tools/review-demo.sh`) that drives the example through all 7 methods, printing `PASS: <method>` for each; total wall-clock ≤10 min on a modern laptop with warm caches.
- **AC-4 (accessible error display):** *Given* any `mwa_error` signal, *when* the example app renders it, *then* the log area shows the `user_message` as TEXT (not color alone); buttons have accessible names (`AccessibleName` property set); contrast passes WCAG AA against the default Godot theme.
- **AC-5 (await+error race):** *Given* the example's connect button handler, *when* inspected, *then* the code uses the `await` + error-signal race pattern (helper function races success vs error, caller awaits only the helper); a cancel button invokes a documented "cancel pending op" path.
- **AC-6 (state transition log):** *Given* any operation, *when* it runs, *then* the example logs `[<timestamp>] <method>: <state>` for state transitions (started → wallet-prompt-open → wallet-returned → signal-fired); reviewer can follow the flow linearly.

---

### Story 5-5: Documentation Bundle [M]

**User Story:** As a Godot developer adopting the SDK, I want comprehensive docs covering install, usage, API reference, error codes, compatibility matrix, naming map, RN parity, API stability, ISigner migration, safety guidance, triage runbook, "Signals 101", and accessibility, so that I can ship a Solana-enabled game without reading source code.

**Dependencies:** all implementation stories (docs reference final API)
**Wave:** 7
**FR Coverage:** FR-15
**NFR Coverage:** NFR-11 (MIT verification), AC-NFR-SEC-12 (FLAG_SECURE + phishing guidance)
**Architecture Ref:** AC-FR-11, AC-D-3, AC-D-7, AC-D-9, AC-D-10, AC-NFR-SEC-12

**Acceptance Criteria:**
- **AC-1 (docs complete):** *Given* the doc tree, *when* listing, *then* the following files exist with non-empty content: `docs/install.md`, `docs/usage.md`, `docs/api-reference.md`, `docs/error-codes.md`, `docs/compatibility-matrix.md`, `docs/naming-map.md`, `docs/rn-parity.md`, `docs/api-stability.md`, `docs/migrations/isigner.md`, `docs/safety.md`, `docs/triage.md`, `docs/signals-101.md`, `docs/accessibility.md`.
- **AC-2 (ownership headers):** *Given* each new doc, *when* its first 10 lines are inspected, *then* they contain `Owner:` and `Drift-check:` frontmatter identifying the maintainer and which CI job enforces it; CODEOWNERS entry for `docs/` exists.
- **AC-3 (7/7 API reference):** *Given* `docs/api-reference.md`, *when* a Godot dev reads it, *then* each of 7 MWA methods + state getters + utility methods has: signature, parameters, return, signals emitted (success + all error codes), copy-pasteable GDScript snippet.
- **AC-4 (link check):** *Given* CI `verify_docs.yml` is extended, *when* it runs on every PR, *then* any broken markdown link to a local path or external URL fails the build.
- **AC-5 (LICENSE byte-diff):** *Given* repo root `LICENSE`, *when* CI compares against upstream `solana-mobile/mobile-wallet-adapter` `LICENSE` (MIT text), *then* they differ only on the copyright-holder line (byte-identical otherwise).
- **AC-6 (rn-parity machine-readable):** *Given* `docs/rn-parity.md`, *when* a parser reads a YAML table of 7 rows, *then* each row has `rn_method`, `rn_version`, `gdscript_method`, `kotlin_method`, `cpp_method`, `signal_names`; consumed by the weekly `verify_parity.yml` cron job.
- **AC-7 (safety doc):** *Given* `docs/safety.md`, *when* a dev reads it, *then* it covers: `FLAG_SECURE` recommendation, identity-metadata attacker-controllability, sign_and_send is at-least-once semantics, pending-submission recovery, and secure logging practices.

---

### Story 5-6: CI/Build Integration [L]

**User Story:** As an SDK maintainer, I want all PR-blocking CI jobs wired up — one-command addon build, kotlin lint, codegen drift, generator tests, instrumented tests, manifest audit, R8 smoke, parity cron, wall-clock SLO monitor — so that every merge to master is verified to architecture + spec invariants without manual intervention.

**Dependencies:** 1-1, 1-2, 2-1, 4-3 (needs codegen, ktlint config, E2E setup, and manifest hardening)
**Wave:** 7
**FR Coverage:** — (enabling / cross-cutting)
**NFR Coverage:** NFR-2 (drift check), NFR-3 (Godot version check), NFR-4 (parity cron), NFR-6 (E2E coverage), NFR-9 (microbenchmark), NFR-10 (grep for hand-rolled protocol)
**Architecture Ref:** §9, §10.4, DD-20, DD-33

**Why L (not splittable):** CI configuration is one coherent surface area (≈12 GitHub Actions workflow changes + new/extended jobs). Splitting leaves CI in an inconsistent state (some jobs running, others not) for the duration of interim merges, which blocks every other story's CI feedback. Combined verification in one PR is tractable; green-on-merge is the guarantee.

**Acceptance Criteria:**
- **AC-1 (one-command build):** *Given* a fresh clone, *when* `scons addon` runs, *then* `addons/SolanaSDK/` is produced with GDExtension binaries + AAR + GDScript + docs in one invocation; existing `SConstruct addon` alias is extended/validated; README has a 5-line "Build the addon" section.
- **AC-2 (PR-blocking CI):** *Given* every PR to `master`, *when* CI runs, *then* these jobs run and all must pass before merge: `build.yml` (Android + multi-platform GDExtension — existing), `clang_tools.yml` (existing), `kotlin_lint.yml` (new: ktlint + Android Lint), `kotlin_unit.yml` (new: `./gradlew test`), `cpp_host_unit_tests.yml` (new: GoogleTest on Linux/Windows/macOS), `gdextension_load_multi_platform.yml` (reuse of existing 4-platform matrix, extended to assert UNSUPPORTED_PLATFORM), `instrumented_emulator_fakewallet.yml` (new: macos-latest + emulator + Fake Wallet APK), `codegen_drift_check.yml` (new), `generator_tests.yml` (new: PR-blocking), `manifest_merge_audit.yml` (new), `r8_release_smoke.yml` (new), `reproducible_build.yml` (new: 2-runner byte-diff), `fixture_schema_validation.yml` (new), `header_lint.yml` (new: bans STL types in public C++ headers), `since_tags_check.yml` (new).
- **AC-3 (weekly cron):** *Given* `verify_parity.yml` cron, *when* it runs weekly Sunday 03:00 UTC, *then* it diffs the pinned RN SDK npm package against `docs/rn-parity.md` method list; on new method found, opens an issue tagged `parity-drift`; PR-blocking only during new-method follow-up PR.
- **AC-4 (CI SLO monitor):** *Given* the last 30 PRs, *when* `ci_slo_audit` (monthly cron) runs, *then* it reports P95 wall-clock, T3 flake rate, and number of retries; emits a dashboard comment on the tracking issue if P95 > 35 min or T3 flake > 2%.
- **AC-5 (retry policy):** *Given* T3 instrumented_emulator_fakewallet fails due to emulator boot failure (detected via log pattern `emulator boot timed out`), *when* the job runs, *then* it auto-retries exactly once; no retries on T1/T2/T5.
- **AC-6 (grep bans):** *Given* CI greps run, *then* the build fails if any of these patterns appear: (a) `WebSocket` or `ECDH` in hand-rolled form under `android/plugin/`, (b) `Log\.(d|v|i|w|e)\(` with `authToken|publicKey|SecretString.reveal` args, (c) `SdkLog\.(v|d|i|w|e)\([^{]*\$\{` (string interpolation without lambda), (d) any wallet package name string literal in a conditional (e.g., `if (wallet == "com.phantom.app")`), (e) `#ifdef __ANDROID__` outside `src/mwa/platform_selector.cpp`.
- **AC-7 (@since check):** *Given* `scripts/check_since_tags.py` runs on every PR, *when* a new public method/signal is added without an `@since` XML tag, *then* the build fails with the offending line.
- **AC-8 (compat-matrix enforcement):** *Given* CI validates `docs/compatibility-matrix.md`, *when* `build.gradle.kts` or `.gdextension` drifts from the matrix values (compileSdk, minSdk, clientlib-ktx version, Godot minor), *then* the build fails with a named drift.

---

## FR Coverage Map

| FR | Requirement | Stories | Status |
|---|---|---|---|
| FR-1 | Install single addon, no Java/Kotlin/C++ required | 5-4, 5-5, 5-6 (scons addon one-command) | ✅ Covered |
| FR-2 | Initiate connect, returns token + pubkey | 2-1 | ✅ Covered |
| FR-3 | Wallet selection via OS chooser | 2-1 (integration test with Fake Wallet chooser UI) | ✅ Covered |
| FR-4 | Silent reauth with cached token | 2-2 | ✅ Covered |
| FR-5 | Deauthorize drops session + clears cache | 4-1 | ✅ Covered |
| FR-6 | Disconnect without revoke | 2-3 | ✅ Covered |
| FR-7 | Sign array of byte messages | 3-1 | ✅ Covered |
| FR-8 | Sign array of transactions | 3-2 | ✅ Covered |
| FR-9 | Sign and send transactions to RPC | 3-3 | ✅ Covered |
| FR-10 | Cluster select per authorize | 2-1 (cluster param), cluster-bleed tests in 3-3 | ✅ Covered |
| FR-11 | Identity metadata on connect | 2-1 | ✅ Covered |
| FR-12 | Non-blocking, awaitable signals | 1-4, 1-5 (defines the contract), all op stories | ✅ Covered |
| FR-13 | Error signal distinct from success | 1-1, 1-5 (signal skeleton), 5-3 (lifecycle cancel) | ✅ Covered |
| FR-14 | Persist auth token across process restarts | 2-1 (SecureTokenStore), 2-2 (reauth) | ✅ Covered |
| FR-15 | Docs bundle | 5-5 | ✅ Covered |
| FR-16 | Minimal example scene | 5-4 | ✅ Covered |

**Coverage: 16/16 (100%)** — zero gaps.

## NFR Coverage Strategy

| NFR | Path | Story/Note |
|---|---|---|
| NFR-1 (0 plaintext tokens) | Cross-cutting — SecretString wrapper defined in 1-2 | Enforced across all token-handling stories; verified by 2-1 AC-8 (sandbox+logcat scan), 4-3 AC-1 (Keystore recovery path), and 5-6 AC-6 (CI grep) |
| NFR-2 (min SDK 23, target 34) | Direct + Cross-cutting | Fixed in 1-2 (scaffold cleanup); verified by 5-6 AC-8 (compat-matrix drift CI) |
| NFR-3 (Godot 4.x single minor) | Direct | DD-9 locked 4.3.x; verified by 5-6 AC-8 |
| NFR-4 (7/7 RN API parity) | Direct | Operation stories 2-1, 2-2, 2-3, 3-1, 3-2, 3-3, 4-1 each cover one method; weekly verify_parity cron (5-6 AC-3); rn-parity.md (5-5 AC-6) |
| NFR-5 (100% silent reauth vs Phantom) | Direct | 2-2 AC-5 (10-cycle release-gate test) |
| NFR-6 (E2E 7/7 on Android) | Direct | Each op story ships an E2E test in the T3 instrumented suite; aggregated by 5-6 AC-2 (instrumented_emulator_fakewallet.yml) |
| NFR-7 (3/3 cluster support) | Direct | 2-1 (cluster param), 3-3 AC-6 (cluster-bleed refuse), 5-4 example exercises all 3 via config toggle |
| NFR-8 (100% MWA wallets, no per-wallet code) | Direct + Cross-cutting | 1-6 (FakeMwaClient + fixtures = deterministic parity base); 5-6 AC-6 CI grep bans wallet-specific conditionals |
| NFR-9 (≤16 ms method return) | Cross-cutting | Thread dispatcher in 1-4 enforces; microbenchmark job in 5-6 verifies P99 ≤ 16 ms |
| NFR-10 (0 hand-rolled protocol) | Direct + Cross-cutting | 1-6 (MwaClient wraps clientlib-ktx); 5-6 AC-6 CI grep bans WebSocket/ECDH |
| NFR-11 (MIT license 1:1) | Direct | 5-5 AC-5 (LICENSE byte-diff) |

All 11 NFRs have a coverage path — zero gaps.

## Dependency Graph

```
Wave 1:
  1-1 ─┐
  1-2 ─┤
  1-3 ─┘  (all independent — foundation)

Wave 2:
  1-4 ← 1-1, 1-2
  1-6 ← 1-1, 1-2

Wave 3:
  1-5 ← 1-4

Wave 4:
  2-1 ← 1-1, 1-2, 1-3, 1-4, 1-5, 1-6 (vertical slice L)

Wave 5:
  2-2 ← 2-1
  2-3 ← 2-1
  3-1 ← 2-1 (defines runSigningOp helper)
  4-1 ← 2-1
  4-3 ← 2-1

Wave 6:
  3-2 ← 3-1 (consumes runSigningOp)
  3-3 ← 3-1 (consumes runSigningOp + adds breadcrumb)
  4-2 ← 4-1 (forget_all calls deauthorize internals)

Wave 7:
  5-1 ← 1-4, 1-5
  5-2 ← 2-1
  5-3 ← 1-5
  5-4 ← 2-1, 2-2, 2-3, 3-1, 3-2, 3-3, 4-1 (demos all ops)
  5-5 ← all (docs reference final API)
  5-6 ← 1-1, 1-2, 2-1, 4-3 (wires up full CI surface)
```

DAG validated — no cycles.

## Wave Assignments

| Wave | Stories | Rationale |
|---|---|---|
| 1 | 1-1, 1-2, 1-3 | Foundation; all independent |
| 2 | 1-4, 1-6 | Both consume 1-1 and 1-2; independent of each other (different files/layers) |
| 3 | 1-5 | Depends on 1-4's bridge interface |
| 4 | 2-1 | First vertical slice (L); depends on all of Epic 1 |
| 5 | 2-2, 2-3, 3-1, 4-1, 4-3 | All depend only on 2-1; independent of each other (2-2 reauth, 2-3 disconnect, 3-1 signing pipeline, 4-1 deauth, 4-3 security hardening — each in distinct primary files plus additive methods to `MwaAndroidPlugin.kt`; runs sequentially in default mode or parallel with Agent Teams) |
| 6 | 3-2, 3-3, 4-2 | Depend on 3-1 (runSigningOp) or 4-1 (deauthorize primitive) |
| 7 | 5-1, 5-2, 5-3, 5-4, 5-5, 5-6 | All Polish; depend on core ops being in place |

**Wave count: 7.** Over the advisory "wave depth >5" threshold by 2. Trade-off: collapsing waves would require merging Epic 3 signing stories into a single L (loses granular PR review) or running shared-file stories concurrently (risks merge conflicts). The chosen 7-wave structure preserves per-story granularity and sequential wave ordering cleanly. Agent Teams parallel mode absorbs Wave 5's 5 parallel stories and Wave 7's 6 parallel stories, so wall-clock is not 7× a single-story duration.

## Interface Contracts

### `ISigner` (C++ abstract interface)
- **Defined by:** Story 1-3 (`src/isigner.hpp` in godot-solana-sdk core)
- **Consumed by:** Story 2-1 (`MobileWalletAdapter` implements it), existing `WalletAdapter` (refactored in 1-3)
- **Signature:** see architecture §2.3.1

### `MwaAndroidBridge` (C++ interface)
- **Defined by:** Story 1-4 (`src/mwa/include/mwa_android_bridge.hpp`)
- **Consumed by:** Story 1-5, 2-1, all op stories via the `MobileWalletAdapter` node
- **Signature:** pure-virtual methods for 7 ops + lifecycle; `godot::` types only (no STL)

### `GodotMainDispatcher` (C++)
- **Defined by:** Story 1-4
- **Consumed by:** Story 1-5 (all signal emissions), all op stories
- **Signature:** `void post(const godot::String& signal_name, const godot::Dictionary& payload)` — wraps `call_deferred("emit_signal", ...)` on the singleton node

### `MwaClient` (Kotlin interface)
- **Defined by:** Story 1-6 (`android/plugin/src/main/kotlin/.../MwaClient.kt`)
- **Consumed by:** Story 2-1 (`MwaClientImpl`), all op stories, T4 contract tier
- **Signature:** `suspend fun authorize(...): Result<AuthResult, MwaError>` and 6 others mirroring MWA methods

### `SecretString` (Kotlin + C++)
- **Defined by:** Story 1-2
- **Consumed by:** every token-handling story
- **Signature:** Kotlin `class SecretString(private val bytes: ByteArray) { fun reveal(): ByteArray; override fun toString() = "<redacted>" }`; C++ parallel

### `SdkLog` (Kotlin)
- **Defined by:** Story 1-2
- **Consumed by:** every Kotlin source file
- **Signature:** `object SdkLog { fun v(tag: String, corrId: String, msg: () -> String); fun d(...); fun i(...); fun w(...); fun e(...) }`

### `ErrorTaxonomy` (generated YAML → 3 artifacts)
- **Defined by:** Story 1-1
- **Consumed by:** every story that emits `mwa_error`
- **Signature:** see architecture §3.3 (13 codes, fields per code)

### `runSigningOp` (Kotlin helper)
- **Defined by:** Story 3-1
- **Consumed by:** Story 3-2, 3-3
- **Signature:** `suspend fun <T> MwaAndroidPlugin.runSigningOp(op: SigningOp, requestId: String, block: suspend MobileWalletAdapterClient.() -> T): Result<T, MwaError>`

### `SecureTokenStore` (Kotlin)
- **Defined by:** Story 2-1
- **Consumed by:** Story 2-2, 3-3 (breadcrumb), 4-1, 4-2
- **Signature:** `class SecureTokenStore(context: Context) { fun getToken(key: CacheKey): CacheRecord?; fun putToken(key: CacheKey, record: CacheRecord); fun deleteToken(key: CacheKey); fun deleteAll(); fun listAllKeys(): List<CacheKey>; fun rotateMasterKey() }`

## Plan Decisions

- **PD-1 [LOCKED]** Decomposition: Option C — Hybrid Interface-First Foundation + Vertical Slices + Polish.
  - *Alternatives considered:* A (Layered bottom-up) — rejected for no-E2E-until-late; B (Pure vertical) — rejected for connect-story bloat.
  - *Rationale:* matches architecture's 4 seams + codegen pattern; interface-first minimizes cross-story churn.

- **PD-2 [LOCKED]** 7 waves accepted over the "wave depth > 5" advisory.
  - *Alternatives considered:* Collapse Epic 3 signing into one L story (rejected — loses PR granularity); Allow shared-file stories in same wave (rejected — merge-conflict risk in default sequential mode).
  - *Rationale:* preserves atomicity; Agent Teams parallel mode absorbs Wave 5 and Wave 7 breadth.

- **PD-3 [LOCKED]** Acknowledged persona concerns absorbed into stories rather than as separate stories.
  - *Concerns acknowledged:* docs ownership / CODEOWNERS (→ 5-5 AC-2 + 5-6); `@since` CI check (→ 5-6 AC-7); `scripts/new_method.py` scaffolder (deferred post-launch; not in sprint).
  - *Rationale:* each is a small cross-cutting concern; separate stories would be too small and would create unnecessary wave dependency.

- **PD-4 [LOCKED]** Story sizes S=4, M=13, L=4. Three L stories justified individually: 2-1 (vertical slice standing up all 4 seams + secure storage), 3-3 (sign_and_send + breadcrumb is one mental unit), 4-3 (security hardening is one audited unit), 5-6 (CI configuration is one coherent surface).
  - *Alternatives considered:* split 2-1 into "stub connect → real connect" — rejected, stub cannot be tested E2E; split 5-6 into 3-4 smaller CI stories — rejected, interim CI state is broken.
  - *Rationale:* atomicity trumps size in these cases.
