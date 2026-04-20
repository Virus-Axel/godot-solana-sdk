package com.godotengine.godot_solana_sdk.mwa.util

class SecretString(source: ByteArray) {

    private val bytes: ByteArray = source.copyOf()

    fun reveal(): ByteArray = bytes.copyOf()

    override fun toString(): String = "<redacted>"

    override fun equals(other: Any?): Boolean =
        other is SecretString && bytes.contentEquals(other.bytes)

    override fun hashCode(): Int = bytes.contentHashCode()
}
