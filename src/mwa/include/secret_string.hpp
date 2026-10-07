// ABI hygiene: this header must contain only godot:: types — header-lint enforces.
// See docs/architecture.md §7.1.1 and (operator<< dropped).
#ifndef GODOT_SOLANA_SDK_MWA_SECRET_STRING_HPP
#define GODOT_SOLANA_SDK_MWA_SECRET_STRING_HPP

#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot_solana_sdk::mwa {

/**
 * @brief Owner of secret bytes (e.g. auth tokens) with a redacted string form.
 *
 * Wraps a `godot::PackedByteArray` so the bytes never accidentally appear in
 * logs or error messages. `to_string()` returns the literal `<redacted>`;
 * `reveal_bytes` is the sole accessor for the underlying bytes.
 */
class SecretString {
public:
    /// Construct from a raw secret byte buffer. The buffer is owned by this instance.
    explicit SecretString(godot::PackedByteArray bytes) : bytes_(bytes) {}

    /// @return A copy of the underlying secret bytes. @c PackedByteArray is copy-on-write,
    /// so the copy itself is cheap; the caller is responsible for not logging the result.
    godot::PackedByteArray reveal_bytes() const { return bytes_; }

    /// @return The redaction marker @c "<redacted>". Sole stringification path —
    /// @c operator<< is intentionally absent so logs cannot leak secrets via @c std::ostream.
    godot::String to_string() const { return godot::String("<redacted>"); }

private:
    godot::PackedByteArray bytes_;
};

}  // namespace godot_solana_sdk::mwa

/// @c godot::String concatenation convenience. Forwards to @c SecretString::to_string and
/// therefore never exposes the underlying bytes.
inline godot::String operator+(const godot::String &prefix, const godot_solana_sdk::mwa::SecretString &s) {
    return prefix + s.to_string();
}

#endif  // GODOT_SOLANA_SDK_MWA_SECRET_STRING_HPP
