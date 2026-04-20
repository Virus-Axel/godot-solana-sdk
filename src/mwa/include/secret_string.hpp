// ABI hygiene: this header must contain only godot:: types — header-lint (Story 1-4) enforces.
// See docs/architecture.md §7.1.1 (DD-26) and amendment A-7 (operator<< dropped).
#ifndef GODOT_SOLANA_SDK_MWA_SECRET_STRING_HPP
#define GODOT_SOLANA_SDK_MWA_SECRET_STRING_HPP

#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot_solana_sdk::mwa {

class SecretString {
public:
    explicit SecretString(godot::PackedByteArray bytes) : bytes_(bytes) {}

    // Godot PackedByteArray is copy-on-write — a copy here is cheap.
    godot::PackedByteArray reveal_bytes() const { return bytes_; }

    // Sole stringification path (DD-26 / A-7): operator<< is intentionally absent.
    godot::String to_string() const { return godot::String("<redacted>"); }

private:
    godot::PackedByteArray bytes_;
};

}  // namespace godot_solana_sdk::mwa

// godot::String concatenation convenience (engine types only, DD-26-compliant). Never exposes bytes_.
inline godot::String operator+(const godot::String &prefix, const godot_solana_sdk::mwa::SecretString &s) {
    return prefix + s.to_string();
}

#endif  // GODOT_SOLANA_SDK_MWA_SECRET_STRING_HPP
