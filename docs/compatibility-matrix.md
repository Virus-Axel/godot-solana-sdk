---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# Compatibility Matrix

This document is the canonical source-of-truth for the godot-mwa-android SDK's
runtime/build dependency pins (AC-D-7). The Story 5-6 AC-8 CI drift check
(see `docs/plan.md` Story 5-6 task list) parses the table below and fails the
build if any of `compileSdk`, `targetSdk`, `minSdk`, the clientlib-ktx
version, the Android Gradle Plugin version, or the pinned Godot minor in
`godot-solana-sdk.gdextension` drifts from these values without an
accompanying matrix update.

The Godot minor pin (`compatibility_minimum = "4.3"` in
`godot-solana-sdk.gdextension:4`) is locked per design decision DD-5-4-1
(Story 5-4 T0 pinning commit `dd05abc5`). Bumping the Godot minor is a
breaking change per `docs/api-stability.md` and requires the full
deprecation ladder there.

## Pinned values (last verified 2026-04-29)

| Component | Pinned value | Source of truth | Notes |
|---|---|---|---|
| Godot minor | `4.3.x` | `godot-solana-sdk.gdextension:4` (`compatibility_minimum = "4.3"`) | DD-5-4-1 LOCKED; ADR pending per spec OQ-1. |
| godot-cpp SHA | `714c9e2c` (`godot-3.3.3-stable-1101-g714c9e2c`) | `git submodule status godot-cpp` | Tracks Godot 4.3.x branch. |
| `mobile-wallet-adapter-clientlib-ktx` | `2.0.3` | `android/plugin/build.gradle.kts:225` | Same version is the `pinned_clientlib_ktx_version` in `docs/rn-parity.md`. |
| `compileSdk` | `34` | `android/plugin/build.gradle.kts` | CI asserts `compileSdk == 34` per AC-D-7. |
| `targetSdk` | `34` | `android/plugin/build.gradle.kts` | Aligned with `compileSdk` per AGP 7.4 guidance. |
| `minSdk` | `23` | `android/plugin/build.gradle.kts` | CI asserts `minSdk == 23` per AC-D-7. |
| Kotlin plugin | `1.8.0` | `android/build.gradle.kts:29` (`id("org.jetbrains.kotlin.android") version "1.8.0"`) | Compiler/stdlib version. |
| JDK (compile/runtime) | `17` | Project scaffold expectation | The Gradle build is launched with JDK 17; bytecode targets Java 11 (see next row). |
| `jvmTarget` (bytecode) | `11` | `android/plugin/build.gradle.kts:138` (`kotlinOptions { jvmTarget = "11" }`) | Bytecode target — NOT the JDK version. |
| Android Gradle Plugin | `7.4.1` | `android/build.gradle.kts:28` (`id("com.android.library") version "7.4.1"`) | Pairs with Gradle wrapper 7.5. |
| Gradle wrapper | `7.5` | `android/gradle/wrapper/gradle-wrapper.properties` (`distributionUrl=...gradle-7.5-bin.zip`) | Bumping requires re-verifying AGP compat. |
| `last verified` | `2026-04-29` | This document (Story 5-5 T2 commit time) | Refresh on every story that touches build files. |

## Drift-check contract

The Story 5-6 AC-8 workflow (`compat_matrix_drift.yml`, lands in 5-6) reads
the rows above as a key/value map and compares each entry to the live value
in the corresponding source-of-truth file. A divergence fails the build with
an actionable message naming the row, the expected value, and the actual
value. The workflow is read-only with respect to this document — it does
not auto-update the matrix; the implementer who changes a build pin must
update this file in the same PR.

## Bumping a pin

1. Open a PR that updates BOTH the source-of-truth file (e.g.
   `android/plugin/build.gradle.kts`) AND the matching row in this document.
2. Update the `last verified` row to the PR's commit date.
3. If the bump crosses a Godot minor (e.g. 4.3.x → 4.4.x), file an ADR per
   `docs/architecture.md` and follow `docs/api-stability.md`'s breaking-change
   process — the Godot minor pin is part of the public API surface.
4. Re-run the full verification chain (`format → lint → build → test → security`)
   on the bumped version before merging.

## Cross-references

- AC-D-7 in `docs/spec.md` (line 158): "lists Godot minor + godot-cpp SHA +
  clientlib-ktx + rpc-core + compileSdk + targetSdk + minSdk + Kotlin +
  Compose + 'last verified'; CI asserts `compileSdk==34` and `minSdk==23`".
- DD-5-4-1 (Story 5-4 T0): Godot minor pin enforcement.
- `docs/api-stability.md`: bumping the Godot minor is a breaking change.
- `docs/rn-parity.md`: `pinned_clientlib_ktx_version` mirrors the
  `mobile-wallet-adapter-clientlib-ktx` row above.
