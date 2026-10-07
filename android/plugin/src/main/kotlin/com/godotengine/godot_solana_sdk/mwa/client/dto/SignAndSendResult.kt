package com.godotengine.godot_solana_sdk.mwa.client.dto

/**
 * Result of signAndSendTransactions — a list of base58-encoded transaction signatures
 * in the same order as the input transactions.
 */
data class SignAndSendResult(
    val signatures: List<String>,
)
