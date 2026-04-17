// This file is generated, do not edit.
#ifndef GODOT_SOLANA_SDK_MWA_ERROR_CODES_HPP
#define GODOT_SOLANA_SDK_MWA_ERROR_CODES_HPP

namespace godot_solana_sdk::mwa {

enum class MwaErrorCode : int {
    TEST_ALPHA = 0,
    TEST_BETA = 1,
};

inline const char* code_name(MwaErrorCode e) {
    switch (e) {
        case MwaErrorCode::TEST_ALPHA: return "TEST_ALPHA";
        case MwaErrorCode::TEST_BETA: return "TEST_BETA";
    }
    return "UNKNOWN";
}

}  // namespace godot_solana_sdk::mwa

#endif // GODOT_SOLANA_SDK_MWA_ERROR_CODES_HPP
