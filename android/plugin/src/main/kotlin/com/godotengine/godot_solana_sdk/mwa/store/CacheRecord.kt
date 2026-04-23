package com.godotengine.godot_solana_sdk.mwa.store

import org.json.JSONObject

/**
 * Stub for Story 2-1 T1 (D-T1-3). The 11-field shape is final (arch §3.1);
 * `toJson()` / `fromJson()` bodies are intentionally `TODO()` and land in T3.
 *
 * Schema version 1. `authToken` is the base64 opaque token string; in-memory
 * wrapping via `SecretString` happens at the SecureTokenStore / plugin-layer
 * boundary (T3 / T4). `issuedAtMs` / `lastUsedAtMs` are reserved for future
 * TTL and are not enforced in v1.
 */
data class CacheRecord(
    val schema: Int = 1,
    val authToken: String = "",
    val walletPackage: String = "",
    val walletLabel: String = "",
    val walletIconUri: String = "",
    val publicKey: String = "",
    val cluster: String = "",
    val chainId: String = "",
    val identityUri: String = "",
    val issuedAtMs: Long = 0L,
    val lastUsedAtMs: Long = 0L,
) {
    /** TODO(Story 2-1 T3): serialize to JSONObject per arch §3.1 (11 fields). */
    fun toJson(): JSONObject = TODO("Story 2-1 T3: 11-field JSON serialization")

    companion object {
        /** TODO(Story 2-1 T3): deserialize from JSONObject per arch §3.1. */
        @Suppress("UNUSED_PARAMETER")
        fun fromJson(json: JSONObject): CacheRecord = TODO("Story 2-1 T3: 11-field JSON deserialization")
    }
}
