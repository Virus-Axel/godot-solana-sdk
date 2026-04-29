---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# Install Guide

This guide takes a Godot game developer from a clean machine to a working `MWA` autoload in a fresh Godot 4.3.x project. For the grant-reviewer ≤10-minute verification path, see [`REVIEW.md`](../REVIEW.md) at the repo root — that document is the canonical reviewer surface and is intentionally kept short. This guide is the developer in-depth reference.

## What this SDK is

`godot-solana-sdk` ships a single Godot addon (`addons/SolanaSDK/`) that exposes Solana Mobile Wallet Adapter (MWA) operations to GDScript via an autoload singleton named `MWA`. Per [`docs/spec.md`](./spec.md) FR-1 the install path is intentionally one-step — drop the addon, register the autoload, ship. There is no separate Kotlin / C++ / Gradle project for game developers to integrate.

## Prerequisites

- **Godot 4.3.x editor.** The addon's `compatibility_minimum` is `"4.3"` (set in `addons/SolanaSDK/bin/godot-solana-sdk.gdextension`). Godot 4.4+ refuses to load the addon today; see Troubleshooting below.
- **Android device or emulator** with API level 23 or higher. The fastest path is `reactivecircus/android-emulator-runner@v2`'s default API 33 image, but any API 23+ image works. iOS / desktop / web are NOT supported targets — `MWA.is_supported()` returns `false` and every op emits `mwa_error{code=UNSUPPORTED_PLATFORM}` on those platforms. See [`docs/api-reference.md`](./api-reference.md) for the platform-gating pattern.
- **`adb`** on PATH. Ships with the Android SDK platform-tools.
- **An MWA-compatible wallet** installed on the device. For development, the canonical reference is `solana-mobile/mobile-wallet-adapter`'s **Fake Wallet** APK (`./gradlew :fakewallet:assembleDebug` from the upstream repo, then `adb install`). For production, a real wallet such as Phantom / Solflare / Backpack — `MWA.get_suggested_wallet_install_links()` returns the canonical install-prompt list.

You do **not** need: Java, Kotlin, C++, scons, the Android NDK, or any familiarity with the Android Gradle build, **unless** you want to build the addon from source (see "Build from source" below).

## Install via tagged release (preferred)

> **Status (Story 5-5, 2026-04-29):** Tagged releases are not yet available on this repo. Story 5-6 will land release-on-tag CI per `CR-5-4-F` MANDATED in [`docs/concerns.md`](./concerns.md); when that workflow ships, the steps below become the canonical path. Until then, follow "Build from source" below.

When releases are available:

1. Navigate to <https://github.com/ValentinVPK/godot-solana-sdk/releases>.
2. Download `addons-bundle.zip` from the latest stable release's assets.
3. Extract into your Godot project's `addons/` directory:

   ```bash
   # from your Godot project root
   mkdir -p addons
   unzip -o ~/Downloads/addons-bundle.zip -d addons/
   # you should now see addons/SolanaSDK/ with GDExtension binaries + AAR + GDScript
   ```

4. Verify the addon's compatibility line:

   ```bash
   grep '^compatibility_minimum' addons/SolanaSDK/bin/godot-solana-sdk.gdextension
   # expected: compatibility_minimum = "4.3"
   ```

If the line reads `"4.4"`, the release artifact predates `DD-5-4-1` (see [`docs/stories/5-4.md`](./stories/5-4.md)) — pull a newer release.

## Build from source

This is the path you take while tagged releases are not yet available, or if you want to track `master` for unreleased fixes.

1. Clone this repo with submodules:

   ```bash
   git clone --recurse-submodules https://github.com/ValentinVPK/godot-solana-sdk.git
   cd godot-solana-sdk
   ```

2. Install build dependencies:
   - **scons** (`pip install scons` or your distro's package manager).
   - **JDK 17** (required by the Android Gradle Plugin pinned in `android/build.gradle.kts`).
   - **Android SDK + NDK.** Set `ANDROID_HOME` and `ANDROID_NDK_HOME` so Gradle and scons can find them.

3. Build the addon:

   ```bash
   scons addon
   ```

   This produces `addons/SolanaSDK/` with the compiled GDExtension binaries (per-platform `.so` / `.dylib` / `.dll`) plus the Android plugin AAR.

4. Copy `addons/SolanaSDK/` into your Godot project's `addons/` directory (or symlink it during development).

> **Known issue (`CR-5-4-G` HIGH).** Pre-existing C++ compile errors in `src/wallet_adapter/wallet_adapter_signer.cpp` and `src/transaction/transaction.cpp` (godot-cpp API drift) currently block local source builds. Story 5-6 confronts this. Track progress in [`docs/concerns.md`](./concerns.md). Until resolved, the only working path is the tagged-release flow above (once Story 5-6 ships it).

## Project setup

The addon ships an editor plugin script (`addons/SolanaSDK/solana_sdk_plugin.gd`) that registers the `MWA` autoload automatically when you enable the plugin in Godot's Project Settings → Plugins panel. After enabling:

1. Confirm `MWA` is registered as an autoload: open Project Settings → Globals → Autoload. You should see `MWA` pointing at `res://addons/SolanaSDK/mwa/MWA.gd`.
2. From any GDScript, you can now call `MWA.is_supported()`, `MWA.connect(...)`, etc. — see [`docs/usage.md`](./usage.md) for the recommended call patterns and [`docs/api-reference.md`](./api-reference.md) for the per-method API.

A minimal smoke test (paste into any scene's `_ready()`):

```gdscript
func _ready() -> void:
    print("MWA supported on this platform: ", MWA.is_supported())
    if MWA.is_supported():
        print("ready to call MWA.connect(...)")
    else:
        print("desktop/editor build — MWA ops will emit UNSUPPORTED_PLATFORM")
```

## Quick path for grant reviewers

If you are reviewing this SDK for a grant audit and want the ≤10-minute end-to-end verification path (clone the demo repo → install Fake Wallet → run the scripted demo → assert 7/7 PASS markers), see [`REVIEW.md`](../REVIEW.md) at the repo root. That document is the canonical reviewer surface per [`docs/amendments.md`](./amendments.md) `A-17`. This install guide is the developer in-depth complement; the two are intentionally non-duplicating.

## Troubleshooting

### `incompatible godot version` on editor load

The addon targets Godot 4.3.x. If the editor reports `incompatible godot version` or `addon requires Godot 4.4`, you are on the wrong Godot release. Either downgrade to 4.3.x or, if you must use 4.4, file a finding against `DD-5-4-1` in [`docs/stories/5-4.md`](./stories/5-4.md) — the compatibility floor is intentionally pinned and changing it requires a story.

### `MwaAndroidPlugin not loaded` on Android

The Android plugin AAR did not register at runtime. Common causes:

- The AAR is missing from `addons/SolanaSDK/bin/android/`. Re-extract the release zip or re-run `scons addon`.
- Your Godot Android export template is too old. Update to Godot 4.3.x's matching export template.
- Your APK's `AndroidManifest.xml` does not include the plugin's `<meta-data>` entry. Godot's editor plugin registration step normally handles this; if you exported via CLI, confirm the export preset has the SolanaSDK plugin enabled.

### Emulator architecture mismatch

The emulator image's CPU architecture must match a `.so` shipped in the AAR. The AAR builds for `arm64-v8a` (real devices and Apple Silicon emulators), `armeabi-v7a` (older devices), and `x86_64` (Intel-host emulators). If you see `dlopen failed: library "libsolana_sdk.so" not found`, your emulator is on an architecture the AAR did not build for; switch to a supported emulator image.

### `MWA.is_supported()` returns `false` on a real Android device

Check `OS.get_name()` — `MWA.is_supported()` is implemented as `OS.get_name() == "Android"` (see `addons/SolanaSDK/mwa/MWA.gd:346`). If you are running the editor remotely or under an Android-on-host shim, `OS.get_name()` may not report `"Android"`. The check is intentionally strict; running in an environment where it returns false means the underlying JNI bridge is not available.

### `NO_MWA_WALLET_INSTALLED` on every connect attempt

No MWA-compatible wallet is installed on the device. Display `MWA.get_suggested_wallet_install_links()` to the user (Phantom / Solflare / Backpack) or install the Fake Wallet APK from upstream for development. See [`docs/error-codes.md`](./error-codes.md) for the full error-code reference.

## Next steps

- Read [`docs/usage.md`](./usage.md) for the recommended call-site patterns (await + signal pattern, cluster selection, error handling).
- Read [`docs/api-reference.md`](./api-reference.md) for the per-method API with copy-pasteable snippets.
- Read [`docs/safety.md`](./safety.md) before shipping mainnet support.
- For grant reviewers, [`REVIEW.md`](../REVIEW.md) is the ≤10-minute verification path.
