package com.godotengine.godot_solana_sdk.mwa.util

import android.util.Log

/**
 * Release-stripped logger with lambda-safe signatures.
 *
 * All five methods take a `() -> String` message factory (NOT a concrete `String`). In release
 * builds, R8's `-assumenosideeffects` rule in `consumer-rules.pro` (§8.5) strips both the call
 * AND the lambda body for `v` and `d`, so the token/pubkey values referenced inside the lambda
 * never materialize on the device. Argument evaluation of a concrete `String` argument would
 * happen BEFORE R8 could elide the call — the lambda interposition is what makes the strip safe.
 *
 * Two R8-rule alignment invariants that must not drift:
 * 1. Package path is `com.godotengine.godot_solana_sdk.mwa.util.SdkLog`. The
 *      rule matches exact class names; any rename silently matches nothing.
 *   2. `object` + `@JvmStatic` on every member. The rule descriptor is `public static void v(...)`,
 *      which only matches JVM static dispatch. Without `@JvmStatic`, Kotlin compiles to
 *      `SdkLog.INSTANCE.v(...)` (instance dispatch on the singleton) and R8 cannot match.
 *
 * Log line format: `"[corrId=<corrId>] <rendered-message>"` (§7.2 Correlation IDs,
 * `docs/architecture.md:810`).
 */
object SdkLog {

    @JvmStatic
    fun v(tag: String, corrId: String, msg: () -> String) {
        Log.v(tag, "[corrId=$corrId] ${msg()}")
    }

    @JvmStatic
    fun d(tag: String, corrId: String, msg: () -> String) {
        Log.d(tag, "[corrId=$corrId] ${msg()}")
    }

    @JvmStatic
    fun i(tag: String, corrId: String, msg: () -> String) {
        Log.i(tag, "[corrId=$corrId] ${msg()}")
    }

    @JvmStatic
    fun w(tag: String, corrId: String, msg: () -> String) {
        Log.w(tag, "[corrId=$corrId] ${msg()}")
    }

    @JvmStatic
    fun e(tag: String, corrId: String, msg: () -> String) {
        Log.e(tag, "[corrId=$corrId] ${msg()}")
    }
}
