// This file is generated, do not edit.
#ifndef GODOT_SOLANA_SDK_MWA_ERROR_CODES_HPP
#define GODOT_SOLANA_SDK_MWA_ERROR_CODES_HPP

namespace godot_solana_sdk::mwa {

enum class MwaErrorCode : int {
    USER_CANCELED = 0,
    WALLET_REJECTED = 1,
    NO_MWA_WALLET_INSTALLED = 2,
    WALLET_UNINSTALLED = 3,
    TOKEN_EXPIRED = 4,
    RPC_FAILED = 5,
    NETWORK_OFFLINE = 6,
    TIMEOUT = 7,
    NOT_CONNECTED = 8,
    UNSUPPORTED_PLATFORM = 9,
    PROTOCOL_ERROR = 10,
    STORAGE_CORRUPT = 11,
    REAUTH_REQUIRED = 12,
};

inline const char* code_name(MwaErrorCode e) {
    switch (e) {
        case MwaErrorCode::USER_CANCELED: return "USER_CANCELED";
        case MwaErrorCode::WALLET_REJECTED: return "WALLET_REJECTED";
        case MwaErrorCode::NO_MWA_WALLET_INSTALLED: return "NO_MWA_WALLET_INSTALLED";
        case MwaErrorCode::WALLET_UNINSTALLED: return "WALLET_UNINSTALLED";
        case MwaErrorCode::TOKEN_EXPIRED: return "TOKEN_EXPIRED";
        case MwaErrorCode::RPC_FAILED: return "RPC_FAILED";
        case MwaErrorCode::NETWORK_OFFLINE: return "NETWORK_OFFLINE";
        case MwaErrorCode::TIMEOUT: return "TIMEOUT";
        case MwaErrorCode::NOT_CONNECTED: return "NOT_CONNECTED";
        case MwaErrorCode::UNSUPPORTED_PLATFORM: return "UNSUPPORTED_PLATFORM";
        case MwaErrorCode::PROTOCOL_ERROR: return "PROTOCOL_ERROR";
        case MwaErrorCode::STORAGE_CORRUPT: return "STORAGE_CORRUPT";
        case MwaErrorCode::REAUTH_REQUIRED: return "REAUTH_REQUIRED";
    }
    return "UNKNOWN";
}

}  // namespace godot_solana_sdk::mwa

#endif // GODOT_SOLANA_SDK_MWA_ERROR_CODES_HPP
