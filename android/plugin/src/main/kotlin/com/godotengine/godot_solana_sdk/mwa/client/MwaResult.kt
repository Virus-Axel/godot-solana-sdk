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
 */
sealed class MwaResult<out T> {

    data class Success<T>(val value: T) : MwaResult<T>()

    data class Failure(val error: MwaError) : MwaResult<Nothing>()
}
