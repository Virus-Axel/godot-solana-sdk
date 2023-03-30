#ifndef SOLANA_SDK_KEYPAIR_HPP
#define SOLANA_SDK_KEYPAIR_HPP

#include <godot_cpp/classes/node.hpp>
#include "../wrapper/wrapper.h"

namespace godot{
class Keypair : public Node {
    GDCLASS(Keypair, Node)

private:
    void *data_pointer;

protected:
    static void _bind_methods();

public:
    Keypair();
    void* to_ptr() const;

    ~Keypair();
};
}

#endif