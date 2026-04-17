// This file is generated, do not edit.
#ifndef GODOT_SOLANA_SDK_MWA_ERROR_CODES_HPP
#define GODOT_SOLANA_SDK_MWA_ERROR_CODES_HPP

namespace godot_solana_sdk::mwa {

enum class MwaErrorCode : int {
    TEST_ONE = 0,
};

inline const char* code_name(MwaErrorCode e) {
    switch (e) {
        case MwaErrorCode::TEST_ONE: return "TEST_ONE";
    }
    return "UNKNOWN";
}

}  // namespace godot_solana_sdk::mwa

#endif // GODOT_SOLANA_SDK_MWA_ERROR_CODES_HPP
