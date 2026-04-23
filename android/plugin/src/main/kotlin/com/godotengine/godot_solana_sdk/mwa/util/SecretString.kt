package com.godotengine.godot_solana_sdk.mwa.util

import java.security.MessageDigest

/**
 * Opaque wrapper around a byte-level secret (auth tokens, keys, etc). Provides:
 *  - `toString()` redaction so logs / debugger stringification never leak bytes.
 *  - Constant-time content equality via `MessageDigest.isEqual`.
 *  - [clear] — deterministic best-effort wipe (Story 2-1 T4, D-T4-3).
 *
 * **Lifetime discipline.** `val bytes` holds the reference-immutable handle to
 * the secret's backing array. The backing bytes ARE mutable via [clear], which
 * overwrites every byte with `0x00`. After [clear], subsequent [reveal] calls
 * return the zeroed view (constant-time observable by a caller racing a live
 * reference — see T4 `MwaSessionState.clearOnLogout` kdoc for the
 * caller-race caveat).
 *
 * Best-effort only: a secret copied out via [reveal] is outside the instance's
 * lifetime control. Callers minimize reveal scope, never retain the byte[]
 * handed out by reveal, and prefer to pass `SecretString` across boundaries.
 */
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

    /**
     * Overwrite every byte with `0x00`. Closes the Story 2-1 T4 `MwaSessionState`
     * logout wipe chain — without this, nulling the `SecretString?` reference
     * only makes the instance GC-eligible; the bytes linger until the GC runs.
     * After [clear], [reveal] returns an all-zero array of the original length,
     * and `equals` comparisons against live tokens return `false` (constant-time).
     *
     * Thread-safety: [clear] mutates in place WITHOUT synchronization. Callers
     * that share a `SecretString` across threads (e.g., via
     * [com.godotengine.godot_solana_sdk.mwa.session.MwaSessionState]) MUST
     * ensure no concurrent [reveal] is in flight — typically by holding the
     * session lock across clear + null-out.
     */
    fun clear() {
        bytes.fill(0)
    }
}
