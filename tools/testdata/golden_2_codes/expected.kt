// This file is generated, do not edit.
package com.godotengine.godot_solana_sdk.mwa.generated

sealed class MwaError(
    val code: String,
    val recoverable: Boolean,
    val retryHint: String,
    val defaultMessage: String,
) {
    object TestAlpha : MwaError(
        code = "TEST_ALPHA",
        recoverable = true,
        retryHint = "retry_now",
        defaultMessage = "Alpha failed.",
    )
    object TestBeta : MwaError(
        code = "TEST_BETA",
        recoverable = false,
        retryHint = "none",
        defaultMessage = "Beta is fatal.",
    )
}
