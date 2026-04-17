# Architecture Amendments

Amendments to `docs/architecture.md` (frozen after Gate 3). Story-creator-agent reads
BOTH architecture.md and this file, using amended values where they conflict.

## A-1: §3.2 retry_hint enum — add install_wallet
- **Story:** 1-1 | **Date:** 2026-04-17
- **Original:** `retry_hint: "retry_now" | "reauthorize" | "connect" | "none"`
- **Actual:** `retry_hint: "retry_now" | "reauthorize" | "install_wallet" | "connect" | "none"`
- **Rationale:** `install_wallet` was defined in §3.3 (error-codes manifest, used by NO_MWA_WALLET_INSTALLED) but missing from the §3.2 signal payload schema. Additive; no downstream breakage.

## A-2: §3.3 TOKEN_EXPIRED retry_hint — connect → reauthorize
- **Story:** 1-1 | **Date:** 2026-04-17
- **Original:** `retry_hint: connect`
- **Actual:** `retry_hint: reauthorize`
- **Rationale:** The correct recovery for an expired token is to attempt `MWA.reauthorize()` first (silent, no UI). Only if reauthorize itself fails should the caller fall back to `MWA.connect()`. Using `connect` as the hint skips the cheaper recovery path.

## A-3: §3.3 REAUTH_REQUIRED retry_hint — connect → reauthorize
- **Story:** 1-1 | **Date:** 2026-04-17
- **Original:** `retry_hint: connect`
- **Actual:** `retry_hint: reauthorize`
- **Rationale:** Same logic as A-2. REAUTH_REQUIRED signals that the session needs renewal — `reauthorize()` is the intended first-attempt recovery.

## A-4: §3.3 PROTOCOL_ERROR recoverable — false → true
- **Story:** 1-1 | **Date:** 2026-04-17
- **Original:** `recoverable: false, retry_hint: connect`
- **Actual:** `recoverable: true, retry_hint: connect`
- **Rationale:** `recoverable: false` with `retry_hint: connect` is contradictory — a non-`none` hint implies a recovery path exists. Protocol errors are transient (bad WebSocket frame, interrupted session); a fresh `connect()` can recover. `UNSUPPORTED_PLATFORM` remains the only `recoverable: false, retry_hint: none` entry.

## A-5: §3.3 / §8.5 Kotlin package namespace — com.limechain.mwa.generated → com.godotengine.godot_solana_sdk.mwa.generated
- **Story:** 1-1 | **Date:** 2026-04-17
- **Original:** story-creator agent chose `com.limechain.mwa.generated`
- **Actual:** `com.godotengine.godot_solana_sdk.mwa.generated`
- **Rationale:** This is a contribution to `Virus-Axel/godot-solana-sdk`, not a standalone LimeChain product. Using a contributor-branded namespace (`com.limechain.*`) in someone else's OSS repo is inappropriate. The architecture doc §8.5 ProGuard rules reference `com.godotengine.godot_solana_sdk.mwa.**` — the generated code must live under this namespace or R8 will strip the sealed enum in release builds, causing `ClassNotFoundException`. Aligning with the architecture doc's ProGuard rules requires zero additional amendments to §8.5.
