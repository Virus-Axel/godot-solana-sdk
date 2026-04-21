package com.godotengine.godot_solana_sdk.mwa.client.dto

/**
 * Result of signMessages and signTransactions — a list of signed payload byte arrays
 * in the same order as the input messages/transactions.
 */
data class SignResult(
    val signedPayloads: List<ByteArray>,
)
