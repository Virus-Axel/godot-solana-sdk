// This file is generated, do not edit.
package com.godotengine.godot_solana_sdk.mwa.generated

sealed class MwaError(
    val code: String,
    val recoverable: Boolean,
    val retryHint: String,
    val defaultMessage: String,
) {
    object UserCanceled : MwaError(
        code = "USER_CANCELED",
        recoverable = true,
        retryHint = "retry_now",
        defaultMessage = "You cancelled the wallet prompt.",
    )
    object WalletRejected : MwaError(
        code = "WALLET_REJECTED",
        recoverable = true,
        retryHint = "retry_now",
        defaultMessage = "The wallet rejected the request.",
    )
    object NoMwaWalletInstalled : MwaError(
        code = "NO_MWA_WALLET_INSTALLED",
        recoverable = true,
        retryHint = "install_wallet",
        defaultMessage = "No Solana wallet installed. Install one to continue.",
    )
    object WalletUninstalled : MwaError(
        code = "WALLET_UNINSTALLED",
        recoverable = true,
        retryHint = "connect",
        defaultMessage = "The previously connected wallet is no longer installed.",
    )
    object TokenExpired : MwaError(
        code = "TOKEN_EXPIRED",
        recoverable = true,
        retryHint = "reauthorize",
        defaultMessage = "Your wallet session has expired. Please reconnect.",
    )
    object RpcFailed : MwaError(
        code = "RPC_FAILED",
        recoverable = true,
        retryHint = "retry_now",
        defaultMessage = "The Solana network rejected the transaction.",
    )
    object NetworkOffline : MwaError(
        code = "NETWORK_OFFLINE",
        recoverable = true,
        retryHint = "retry_now",
        defaultMessage = "No internet connection.",
    )
    object Timeout : MwaError(
        code = "TIMEOUT",
        recoverable = true,
        retryHint = "retry_now",
        defaultMessage = "The operation timed out.",
    )
    object NotConnected : MwaError(
        code = "NOT_CONNECTED",
        recoverable = true,
        retryHint = "connect",
        defaultMessage = "Not connected to a wallet. Call MWA.connect() first.",
    )
    object UnsupportedPlatform : MwaError(
        code = "UNSUPPORTED_PLATFORM",
        recoverable = false,
        retryHint = "none",
        defaultMessage = "MWA is only available on Android.",
    )
    object ProtocolError : MwaError(
        code = "PROTOCOL_ERROR",
        recoverable = true,
        retryHint = "connect",
        defaultMessage = "Wallet communication failed.",
    )
    object StorageCorrupt : MwaError(
        code = "STORAGE_CORRUPT",
        recoverable = true,
        retryHint = "connect",
        defaultMessage = "Secure storage is unavailable. Please reconnect.",
    )
    object ReauthRequired : MwaError(
        code = "REAUTH_REQUIRED",
        recoverable = true,
        retryHint = "reauthorize",
        defaultMessage = "Your session needs renewal.",
    )
}
