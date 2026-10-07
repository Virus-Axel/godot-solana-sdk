package com.godotengine.godot_solana_sdk.mwa.client.dto

/**
 * Result of `signMessages` and `signTransactions`. One element per input
 * message/transaction, in input order. The byte-level shape depends on the
 * op that produced it:
 *
 *  - `signMessages` → each element is a **bare 64-byte ed25519 signature**
 *    (detached form). Multi-signer is explicitly unsupported by the 1-6
 *    surface: `MwaClientImpl.signMessages` rejects `addresses.size != 1`.
 *  - `signTransactions` → each element is the **fully-signed transaction
 *    bytes** (the wire-format transaction with signatures embedded, typically
 *    200–1200 bytes for a legacy/v0 transaction).
 *
 * Callers must know which op they invoked to interpret the bytes. The shared
 * DTO is intentional — consumers threading results through a generic pipe
 * treat both as opaque signed payloads.
 */
data class SignResult(
    val signedPayloads: List<ByteArray>,
)
