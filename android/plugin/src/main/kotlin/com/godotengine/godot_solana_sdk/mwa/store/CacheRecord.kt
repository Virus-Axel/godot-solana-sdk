package com.godotengine.godot_solana_sdk.mwa.store

import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import org.json.JSONObject

/**
 * On-disk cache record for MWA auth tokens (arch §3.1). Schema version 1; an
 * 11-field shape carrying the auth-token blob, wallet identity, cluster
 * metadata, and TTL timestamps (reserved for future use; not enforced in v1).
 *
 * The [authToken] is kept wrapped in [SecretString] in memory. At the JSON
 * serialization boundary ([toJson] / [fromJson]) it crosses as a UTF-8 string
 * (the base64 opaque token the wallet returned). At-rest, the JSON is
 * AES256_GCM-encrypted by `EncryptedSharedPreferences` before reaching disk —
 * so the raw token bytes never appear in plaintext on the filesystem (AC-1,
 * AC-8 plaintext scan).
 *
 * `issuedAtMs` / `lastUsedAtMs` are reserved for TTL; callers may populate
 * them but the v1 store does not enforce expiry.
 */
data class CacheRecord(
    val schema: Int = 1,
    val authToken: SecretString = SecretString(ByteArray(0)),
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
    fun toJson(): JSONObject = JSONObject().apply {
        put("schema", schema)
        put("auth_token", String(authToken.reveal(), Charsets.UTF_8))
        put("wallet_package", walletPackage)
        put("wallet_label", walletLabel)
        put("wallet_icon_uri", walletIconUri)
        put("public_key", publicKey)
        put("cluster", cluster)
        put("chain_id", chainId)
        put("identity_uri", identityUri)
        put("issued_at_ms", issuedAtMs)
        put("last_used_at_ms", lastUsedAtMs)
    }

    /**
     * Explicit redaction (defense-in-depth). Today [SecretString.toString]
     * returns `"<redacted>"`, so the data-class default would ALSO be safe
     * — but this file should not silently depend on a distant invariant.
     * Future changes to `SecretString` cannot leak `authToken` through
     * `CacheRecord` as long as this override lives here.
     */
    override fun toString(): String = "CacheRecord(" +
        "schema=$schema, authToken=<redacted>, walletPackage='$walletPackage', " +
        "walletLabel='$walletLabel', walletIconUri='$walletIconUri', publicKey='$publicKey', " +
        "cluster='$cluster', chainId='$chainId', identityUri='$identityUri', " +
        "issuedAtMs=$issuedAtMs, lastUsedAtMs=$lastUsedAtMs)"

    companion object {
        /** Current on-disk schema version. Raise this + add a migration path for any field change. */
        const val SCHEMA_V1 = 1

        /**
         * Parses a [CacheRecord] from its on-disk JSON representation. A schema
         * mismatch throws [IllegalArgumentException] so the caller can
         * distinguish version skew from genuine corrupt-JSON (which surfaces
         * as `JSONException` and drives [StorageCorruptException]).
         */
        fun fromJson(json: JSONObject): CacheRecord {
            val schema = json.getInt("schema")
            require(schema == SCHEMA_V1) {
                "Unsupported CacheRecord schema: $schema (expected $SCHEMA_V1). " +
                    "Possible SDK downgrade or v2+ record on disk."
            }
            return CacheRecord(
                schema = schema,
                authToken = SecretString(json.getString("auth_token").toByteArray(Charsets.UTF_8)),
                walletPackage = json.getString("wallet_package"),
                walletLabel = json.getString("wallet_label"),
                walletIconUri = json.getString("wallet_icon_uri"),
                publicKey = json.getString("public_key"),
                cluster = json.getString("cluster"),
                chainId = json.getString("chain_id"),
                identityUri = json.getString("identity_uri"),
                issuedAtMs = json.getLong("issued_at_ms"),
                lastUsedAtMs = json.getLong("last_used_at_ms"),
            )
        }
    }
}
