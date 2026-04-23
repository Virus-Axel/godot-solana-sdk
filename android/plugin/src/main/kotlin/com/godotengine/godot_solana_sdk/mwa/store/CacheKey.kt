package com.godotengine.godot_solana_sdk.mwa.store

/**
 * Stub for Story 2-1 T1 (D-T1-3). The 4-field shape is final (DD-17 cache-key
 * tuple); the `toHash()` body is intentionally `TODO()` and lands in T2 with
 * length-prefixed canonicalization + SHA256 per DD-17 / DD-27 and AC-1 / AC-2
 * delimiter-collision coverage. A raw `"|".join(...)` joiner would collide on
 * `identityUri` containing a literal `|` in a URL query.
 */
data class CacheKey(
    val cluster: String,
    val chainId: String,
    val walletPackage: String,
    val identityUri: String,
) {
    /** TODO(Story 2-1 T2): `"mwa::v1::" + sha256Hex(canonical())`. */
    fun toHash(): String = TODO("Story 2-1 T2: length-prefixed canonicalization + SHA256")
}
