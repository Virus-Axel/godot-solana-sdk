// This file is generated, do not edit.
package com.limechain.mwa.generated

sealed class MwaError(
    val code: String,
    val recoverable: Boolean,
    val retryHint: String,
    val defaultMessage: String,
) {
    object TestOne : MwaError(
        code = "TEST_ONE",
        recoverable = true,
        retryHint = "retry_now",
        defaultMessage = "Test one message.",
    )
}
