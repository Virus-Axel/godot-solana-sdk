#ifndef SOLANA_SDK_PUBKEY_HPP
#define SOLANA_SDK_PUBKEY_HPP

#include <godot_cpp/classes/node.hpp>

namespace godot{
class Pubkey : public Node {
    GDCLASS(Pubkey, Node)

private:
    static const int PUBKEY_LENGTH = 32;
    PackedByteArray data;

protected:
    static void _bind_methods();

public:
    Pubkey();
    Error from_encoded_string(const String& encoded_string);
    Error from_bytes(const PackedByteArray& bytes);

    ~Pubkey();
};
}

#endif