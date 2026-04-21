package com.godotengine.godot_solana_sdk.mwa.client.dto

import com.godotengine.godot_solana_sdk.mwa.util.SecretString

/**
 * Result of authorize / reauthorize. Mirrors the subset of clientlib-ktx
 * `AuthorizationResult` needed by the plugin.
 *
 * **[authToken] is opaque.** The wallet returns the auth token as a base64
 * string; this field stores the UTF-8 bytes of that string inside a
 * [SecretString]. Consumers assign it back to the adapter via an identity
 * transform — `adapter.authToken = String(authToken.reveal(), Charsets.UTF_8)`
 * — with no decoding or re-encoding. This sidesteps the ambiguity of which
 * base64 variant (standard vs URL-safe, padded vs unpadded) the wallet used,
 * and keeps the `SecretString` contract intact: the raw bytes never leak into
 * logs or `toString()`.
 *
 * Default data-class equals/hashCode compare [publicKey] by reference.
 * Callers must not use `AuthResult` equality as a semantic test — compare
 * individual fields with `ByteArray.contentEquals` where bitwise equality is
 * required.
 */
data class AuthResult(
    val authToken: SecretString,
    val publicKey: ByteArray,
    val accountLabel: String?,
    val walletUriBase: String?,
    val walletPackage: String?,
    val cluster: String,
    val chainId: String,
)
