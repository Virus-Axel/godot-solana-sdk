![Godot Version](https://img.shields.io/badge/Godot-v4.0%2B-blue?logo=godot-engine)
![Platform](https://img.shields.io/badge/platform-Win%20%7C%20Linux%20%7C%20Mac%20%7C%20Web%20%7C%20Android%20%7C%20iOS-brightgreen)


# Godot Solana SDK

A GDExtention that gives Godot engine Solana support. This plugin provides the possibility to create Solana games for any platform supported by Godot. Utilize NFTs and Solana smart contracts in your game. The SDK has interface nodes for Anchor programs and instruction builders for Candy Machine, Candy Guard, System programs, SPL tokens and more.

![godot_solana_sdk4](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/1d4c4ff8-0763-4ad2-82f6-4ea3d66cde96)

# How to use

Download the binaries in the release section [here](https://github.com/Virus-Axel/godot-solana-sdk/releases). place the binaries anywhere in your godot project directory under *res://bin/*. Reload your project and the SDK should be loaded.

![setup](https://github.com/user-attachments/assets/1307ebe4-9630-4d0a-baa8-ecfbeea99759)

See examples for an up to date usage example.

> [!CAUTION]
> It might be dangerous to use mainnet in you applications. Crypto can be stolen. Security is not yet evaluated for the godot-solana-sdk. I can audit security if there is demand.

When the plugin is loaded, the engine will have an additional set of nodes and resources.

## Nodes

These are some of the nodes provided by this plugin:

- SolanaUtils
- SolanaClient
- Account
- Transaction
- WalletAdapter
- ComputeBudget
- SystemProgram
- TokenProgram
- MplTokenMetadata
- AssociatedTokenAccountProgram
- MplCandyMachine
- MplCandyGuard
- AnchorProgram

![Screenshot from 2023-05-20 17-41-43](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/56464477-dafc-41c6-8c6c-cfe9cd89ca6d)

## Resources
These are some of the resources provided by this plugin:

- Pubkey
- Keypair
- CompiledInstruction
- Message
- Hash
- AccountMeta
- Instruction
- CreateMetaDataArgs
- MetaDataCreator
- MetaDataCollection
- MetaDataUses
- MetaData
- CreateMetaDataArgs
- CandyGuardAccessList
- CandyMachineData

![Screenshot from 2023-05-20 17-44-00](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/7ad0212f-f897-43a9-82e5-d0b5351772de)

## Examples

There are example projects under the [example folder](https://github.com/Virus-Axel/godot-solana-sdk/tree/master/example). These are used to test the plugin whenever changes are made to the code. There might be some good references on how to get started. There are also a [demo project on Github](https://github.com/ZenRepublic/GodotSolanaSDKDemos) showcasing the plugin. The source can also be used for reference. The demo project is deployed on [Github pages](https://zenrepublic.github.io/GodotSolanaSDKDemos/), go check it out. Here is how it looks.

![Screenshot from 2024-05-01 18-13-29](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/1f77ce05-0a42-4892-8bc5-c8059d900316)


# Contributing

Feel free to open issues or feature requests. Contributions are very welcome. There is a [contribution guideline](https://github.com/Virus-Axel/godot-solana-sdk/blob/master/CONTRIBUTING.md) but get involved by heading to [discord](https://discord.gg/9aFDCvqPgt) or [starting a discussion](https://github.com/Virus-Axel/godot-solana-sdk/discussions).

## Setting up Build Environment

The plugin builds easily for your host platform with [SCons](https://www.scons.org/).

```
git clone --recurse-submodules git@github.com:Virus-Axel/godot-solana-sdk.git
cd godot-solana-sdk
scons
```

This repository also includes means to build all the binaries in containers. This requires [Podman](https://podman.io/) to be installed.

```
scons --container_build
```

## Build the addon (MWA-bundled distribution)

The `scons addon` alias produces a **single redistributable `addons/SolanaSDK/` tree** containing the GDExtension binaries, the MWA Android plugin AAR, the GDScript facade, and docs — exactly what consumers drop into their Godot 4.3.x project's `addons/` directory (per FR-1: "install single addon, no Java/Kotlin/C++ required"):

```
git clone --recurse-submodules https://github.com/<owner>/godot-solana-sdk.git
cd godot-solana-sdk
scons addon                       # produces addons/SolanaSDK/{bin, WalletAdapterAndroid, mwa, ...}
```

> ⚠️ **Local-build status (2026-04-30):** `scons addon` from a fresh clone currently trips on a known godot-cpp API drift in `src/wallet_adapter/wallet_adapter_signer.cpp` + `src/transaction/transaction.cpp` (tracked as CR-5-4-G HIGH). Multi-platform GitHub Actions builders dodge this by pinning a different toolchain. **The supported install path is the published GitHub Release** (see "CI status" below). Local-build fix is tracked as a follow-up; see `docs/triage/CR-5-4-G-cpp-compile-drift.md` (lands at Story 5-6 T14).

## CI status

Every PR to `master` is gated by the following CI workflows (PR-blocking unless noted):

| Workflow | Tier / Purpose | AC ref |
|---|---|---|
| [`build.yml`](.github/workflows/build.yml) | 16-target native compile matrix (linux/windows/macos/android/web/ios × debug/release) | AC-2 |
| [`clang_tools.yml`](.github/workflows/clang_tools.yml) | clang-format + clang-tidy on C++ changes | AC-2 |
| [`kotlin_lint.yml`](.github/workflows/kotlin_lint.yml) | `:plugin:ktlintCheck` + `:plugin:lint` (Android Lint with baseline) | AC-2 |
| [`kotlin_unit.yml`](.github/workflows/kotlin_unit.yml) | `:plugin:testDebugUnitTest` (JUnit + MockK + FakeMwaClient) | AC-2 |
| [`gdscript_tests.yml`](.github/workflows/gdscript_tests.yml) | 4-platform GDScript / GDExtension load + UNSUPPORTED_PLATFORM matrix (also serves as AC-2's `gdextension_load_multi_platform`) | AC-2 |
| [`generator_tests.yml`](.github/workflows/generator_tests.yml) | error-codes codegen golden + property + failure tests | AC-2 |
| [`codegen_drift_check.yml`](.github/workflows/codegen_drift_check.yml) | error-codes manifest vs generated artifacts drift | AC-2 |
| [`fixture_schema_validation.yml`](.github/workflows/fixture_schema_validation.yml) | testdata/mwa-fixtures/ JSON schema | AC-2 |
| [`grep_bans.yml`](.github/workflows/grep_bans.yml) | source-tree banned patterns (12 rules including AC-6 (a) WebSocket/ECDH + (d) wallet-conditional) | AC-2 + AC-6 |
| [`header_lint.yml`](.github/workflows/header_lint.yml) | DD-26 STL-ban allow-list for `src/mwa/include/*.hpp` | AC-2 |
| [`since_tags_check.yml`](.github/workflows/since_tags_check.yml) | `@since v<X.Y.Z>` doc-comment gate on new public symbols | AC-7 |
| [`compat_matrix_drift.yml`](.github/workflows/compat_matrix_drift.yml) | `docs/compatibility-matrix.md` vs source-of-truth files (compileSdk / minSdk / clientlib-ktx / AGP / Godot minor) | AC-8 |
| [`byte_diff_license.yml`](.github/workflows/byte_diff_license.yml) | LICENSE byte-match against OSI canonical MIT (per A-18) | AC-10 |
| [`instrumented_tests.yml`](.github/workflows/instrumented_tests.yml) | `:plugin:connectedAndroidTest` on API 33 emulator + Fake Wallet (T3 tier) — retry-once on `emulator boot timed out` per AC-5 | AC-2 + AC-5 |
| [`r8_release_smoke.yml`](.github/workflows/r8_release_smoke.yml) | R8-minified release AAR — verifies `@UsedByGodot` method-name survival + AC3ConnectE2ETest E2E | AC-2 |
| [`manifest_merge_audit.yml`](.github/workflows/manifest_merge_audit.yml) | merged-AndroidManifest consumer simulation (allowBackup=false, dataExtractionRules) | AC-2 |
| [`reproducible_build.yml`](.github/workflows/reproducible_build.yml) | 2-runner byte-identical AAR build | AC-2 |
| [`verify_docs.yml`](.github/workflows/verify_docs.yml) | doxygen + markdown link-check | AC-2 |

**Cron-driven (informational):**

| Workflow | Schedule | Purpose |
|---|---|---|
| [`verify_parity.yml`](.github/workflows/verify_parity.yml) | weekly Sun 03:00 UTC | RN parity manifest schema + clientlib-ktx upstream-drift detection (AC-3) |
| [`ci_slo_audit.yml`](.github/workflows/ci_slo_audit.yml) | monthly 1st 03:00 UTC | P95 wallclock + T3 retry rate vs arch §10.3.3 budgets (AC-4) |

**Tag-driven release pipeline:**

| Workflow | Trigger | Purpose |
|---|---|---|
| [`release_on_tag.yml`](.github/workflows/release_on_tag.yml) | `push: tags: ['v*.*.*', 'v*.*.*-rc*']` | Multi-platform native build + AAR + bundle → `gh release create` with `addons-bundle.zip` (AC-9 / A-17 / CR-5-4-F MANDATED) |

# Projects using the Plugin

Hurry up, create some games and list them here!
