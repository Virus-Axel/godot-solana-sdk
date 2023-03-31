#ifndef SOLANA_SDK_PUBKEY_HPP
#define SOLANA_SDK_PUBKEY_HPP

#include <godot_cpp/classes/node.hpp>
#include "../wrapper/wrapper.h"

namespace godot{
class Pubkey : public Node {
    GDCLASS(Pubkey, Node)

private:
    void *data_pointer;

protected:
    static void _bind_methods();

public:
    Pubkey();
    const void* to_ptr() const;

    ~Pubkey();
};
}

#endif