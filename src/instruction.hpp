#ifndef SOLANA_SDK_INSTRUCTION_HPP
#define SOLANA_SDK_INSTRUCTION_HPP

#include "../wrapper/wrapper.h"
#include "account_meta.hpp"
#include "pubkey.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{

class Instruction : public Node {
    GDCLASS(Instruction, Node)

private:
    void *data_pointer;

protected:
    static void _bind_methods();

public:
    Instruction();

    void create_new(const Variant& program_id, PackedByteArray data, Array account_metas);

    ~Instruction();
};
}

#endif