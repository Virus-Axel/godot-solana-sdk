package com.godotengine.godot_solana_sdk.mwa.client

import com.godotengine.godot_solana_sdk.mwa.generated.MwaError

/**
 * Two-parameter result type used throughout the MwaClient surface.
 *
 * Kotlin's stdlib [kotlin.Result] only parameterizes on success type — the failure
 * branch is always [Throwable]. MwaClient's contract is `(success: T, failure: MwaError)`
 * where [MwaError] is a typed sealed hierarchy, so a hand-rolled discriminated union is used.
 *
 * The `out` variance allows [Failure] (declared as `MwaResult<Nothing>`) to satisfy any
 * `MwaResult<T>` return type.
 *
 * Both variants carry an optional [corrId] — the 8-hex-char correlation id that
 * [MwaClientImpl] and [FakeMwaClient] embed in their enter/exit log lines. When
 * present, a caller can join a log trace to a specific returned result (e.g.,
 * "user reports ProtocolError; `corrId=a1b2c3d4` in log"). Backward-compatible:
 * defaults to `null` so every existing `MwaResult.Success(value)` /
 * `MwaResult.Failure(error)` call site still compiles.
 */
sealed class MwaResult<out T> {

    abstract val corrId: String?

    data class Success<T>(
        val value: T,
        override val corrId: String? = null,
    ) : MwaResult<T>()

    data class Failure(
        val error: MwaError,
        override val corrId: String? = null,
    ) : MwaResult<Nothing>()
}
