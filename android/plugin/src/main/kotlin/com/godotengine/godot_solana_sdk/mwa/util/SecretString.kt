package com.godotengine.godot_solana_sdk.mwa.util

import java.security.MessageDigest

class SecretString(source: ByteArray) {

    private val bytes: ByteArray = source.copyOf()

    fun reveal(): ByteArray = bytes.copyOf()

    override fun toString(): String = "<redacted>"

    // Constant-time equality: `ByteArray.contentEquals` short-circuits on
    // the first mismatched byte, which leaks the common-prefix length
    // through timing. `MessageDigest.isEqual` is documented constant-time
    // on modern Android runtimes and is the standard primitive for
    // comparing secret byte arrays. Code-review Finding #3 (Story 1-2).
    override fun equals(other: Any?): Boolean {
        return other is SecretString && MessageDigest.isEqual(bytes, other.bytes)
    }

    override fun hashCode(): Int = bytes.contentHashCode()
}
