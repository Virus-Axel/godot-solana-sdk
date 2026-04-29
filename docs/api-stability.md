---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# API Stability

This document defines the public-API stability contract for the
godot-mwa-android SDK (AC-D-10). It is the source-of-truth for what counts
as a breaking change, the deprecation ladder used to retire any breaking
change, and the versioning policy that governs releases.

The "public API" is the surface listed in `docs/api-reference.md`,
`docs/naming-map.md`, and the 7 RN-parity rows of `docs/rn-parity.md`,
plus the signal-payload schemas in `docs/architecture.md` §3.2 and the
error-code taxonomy in `docs/error-codes.md`. The example app under
`example/MWA/` is NOT part of the public API surface — its design is
documented in `REVIEW.md` per amendment A-17 and may change without notice.

## Versioning policy

The SDK follows semantic versioning (semver — `MAJOR.MINOR.PATCH`):

- **Pre-`v0.1.0` (current state, experimental):** the SDK is pre-stable.
  Breaking changes may land in any release without a deprecation window.
  Consumers pinning to a pre-`v0.1.0` tag accept that minor bumps may
  reshape the public API. Story 5-6's release-on-tag CI publishes the
  first stable cut.
- **`v0.1.0` (first stable cut):** the first release that honors the
  deprecation ladder below. After `v0.1.0` ships, every breaking change
  follows the mark → warn-one-minor → remove process.
- **`v0.x.y` minor bumps (post-`v0.1.0`):** additive only. New methods,
  new signals, new optional Dictionary keys, and new error codes added to
  the existing taxonomy ship in minor versions. The deprecation MARK step
  may also land in a minor version.
- **`v0.x.y` patch bumps:** bug fixes that do not touch the public API.
  Documentation-only changes also ship as patches.
- **Major bumps (`v1.0.0`+):** reserved for the post-experimental
  stabilization milestone; the deprecation ladder still applies, but the
  bar for breaking changes is lower because callers have explicitly opted
  into the new major.

Concern CR-5-4-F (RECLASSIFIED MANDATED, see `docs/concerns.md` line 955)
locks Story 5-6's release-on-tag CI as the canonical mechanism for cutting
`v0.1.0` and every release thereafter. Manual `gh release create` is no
longer an acceptable release path.

## Deprecation ladder

Every breaking change to the public API follows the three-step ladder
locked by DD-25 in `docs/architecture.md` (line 1201):

1. **Mark.** The deprecated symbol stays functional and gains a `@deprecated`
   tag in its XML doc-comment plus a runtime log warning at first use
   (release builds emit the warning via `SdkLog.w`; debug builds escalate to
   `SdkLog.e`). The replacement symbol ships in the same release.
2. **Warn one minor.** The deprecated symbol must remain functional for at
   least one minor version after the mark step. The release notes for
   that minor version explicitly call out the deprecation and link to the
   migration guide.
3. **Remove.** The deprecated symbol is deleted no earlier than the next
   minor after the warn step. The removal release ships a migration note
   and bumps the version per the policy above.

The reference example for this ladder is the `ISigner` migration in
`docs/migrations/isigner.md` — `Transaction::sign(keypair)` was MARKED in
v1.0, kept WARN-functional through v1.1, and REMOVED in v1.2. Any future
breaking change should follow the same shape.

## What counts as a breaking change

The following edits are breaking and require the full deprecation ladder:

- Removing a method from `MWA.gd`, `MobileWalletAdapter` (C++), or
  `MwaAndroidPlugin` (`@UsedByGodot` Kotlin).
- Removing a signal from `MWA.gd` or changing its argument arity (the
  2-arg `*_completed(request_id, result)` shape from amendment A-12 is
  load-bearing).
- Removing a key from a signal's `result` Dictionary or any other
  signal-payload Dictionary (additive-only contract per AC-D-10).
- Changing the `request_id` format (the 8-hex shape per Story 1-5 D-4).
- Changing the semantic meaning of an error code in
  `docs/error-codes.md` (renaming a code is also breaking — error codes
  are part of the public API).
- Bumping the `compatibility_minimum` value in
  `godot-solana-sdk.gdextension` to a new Godot minor (the Godot minor
  pin is part of the public API surface per
  `docs/compatibility-matrix.md`).
- Bumping the `minSdk` value in `android/plugin/build.gradle.kts`.
- Schema-version bumps in `docs/rn-parity.md` (existing consumers of the
  YAML manifest must opt in).

## What does NOT count as breaking

The following edits are additive and ship in minor versions without a
deprecation window:

- Adding a new method to `MWA.gd` (and its C++/Kotlin counterparts).
- Adding a new signal.
- Adding a new optional argument with a default value to an existing
  method.
- Adding a new key to a signal's `result` Dictionary (the
  additive-keys-only contract in AC-D-10).
- Adding a new error code to the existing taxonomy in
  `docs/error-codes.md`.
- Tightening internal behavior in a way that does not change the documented
  public contract (e.g. reducing latency, fixing a bug whose previous
  behavior was undefined per the spec).
- Bumping the `mobile-wallet-adapter-clientlib-ktx` patch version when the
  upstream changelog is bug-fix-only.

## `@since` tags

Every public method, signal, and error code carries an `@since v<X.Y.Z>`
XML doc-comment tag identifying the first release that exposed it. The
`scripts/check_since_tags.py` PR-blocking workflow lands in Story 5-6
AC-7 (see `docs/architecture.md` line 1216 — maintainer review #8) and
fails the build if a new public symbol is added without a `@since` tag.
The first `v0.1.0` release seeds the tag baseline; subsequent additions
use the release version that ships them.

## Cross-references

- AC-D-10 in `docs/spec.md` line 161 — additive-keys-only signal payloads
  + deprecation ladder.
- DD-25 LOCKED in `docs/architecture.md` line 1201 — deprecation ladder
  origin.
- Maintainer review #8 in `docs/architecture.md` line 1216 — `@since`
  tag automation.
- `docs/migrations/isigner.md` — reference example for the ladder.
- CR-5-4-F MANDATED in `docs/concerns.md` line 955 — release-on-tag CI
  as the canonical release path.
- `REVIEW.md` (project root) — example-app-internal API surface
  (NOT part of the public API per amendment A-17).
- `docs/architecture.md` §3.2 — signal-payload schemas (additive-only).
- `docs/error-codes.md` — error-code taxonomy.
