// This file is generated, do not edit.
package com.godotengine.godot_solana_sdk.mwa.generated

sealed class MwaError(
    val code: String,
    val recoverable: Boolean,
    val retryHint: String,
    val defaultUserMessage: String,
) {
    object TestOne : MwaError(
        code = "TEST_ONE",
        recoverable = true,
        retryHint = "retry_now",
        defaultUserMessage = "Test one message.",
    )
}
