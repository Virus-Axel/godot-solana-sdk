package com.godotengine.godot_solana_sdk.mwa.store

/**
 * Thrown when `SecureTokenStore`'s `EncryptedSharedPreferences` file cannot be
 * opened — tampered ciphertext, `MasterKey` rotation mismatch, or corrupted
 * Tink protobuf. The store auto-wipes the prefs file before rethrowing (the
 * `MasterKey` Keystore entry stays), so the caller retries from a clean slate.
 *
 * The plugin layer catches this at the authorize-path boundary and emits
 * `reauth_required{reason=keystore_corrupt}` — never crashes Godot
 * (arch §8.3).
 */
class StorageCorruptException(cause: Throwable) : RuntimeException(cause)
