# Lockfile + Verification-Metadata Regeneration

Story 4-3 T5 (AC-6 / DD-4-3-4). `android/gradle.lockfile` pins resolved
versions; `android/gradle/verification-metadata.xml` pins per-artifact
SHA-256 + PGP (where Maven Central publishes one). Both are committed.

## Regeneration

Run after any dep-version bump in `android/plugin/build.gradle.kts`, Gradle
wrapper bump, or AGP bump:

```bash
cd android
./gradlew :plugin:assembleRelease :plugin:dependencies \
    --write-locks --write-verification-metadata pgp,sha256 --refresh-dependencies
git diff -- gradle.lockfile gradle/verification-metadata.xml
```

`assembleRelease` is included so transitive POMs (which `:plugin:dependencies`
alone does not resolve) get checksums — without it, later `assembleRelease`
runs fail on unrecorded `.pom` artifacts.

## SHA-256 mismatch in CI

Strong supply-chain-tampering signal. Investigate the failing artifact (report
under `android/plugin/build/reports/dependency-verification/`) against upstream
Maven Central BEFORE re-running `--write-verification-metadata`. If upstream
legitimately changed (rare — MC is immutable) or the dep was bumped, regenerate
and audit the diff. Otherwise HALT the merge.

## PGP best-effort (DD-4-3-4.a)

`<verify-signatures>true</verify-signatures>` is global. Long-tail jars without
PGP get auto-recorded under `<ignored-keys>` (file header). SHA-256 is
mandatory; PGP gaps warn, not block.
