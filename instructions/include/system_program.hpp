#ifndef SOLANA_SDK_SYSTEM_PROGRAM_HPP
#define SOLANA_SDK_SYSTEM_PROGRAM_HPP


#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>
#include "pubkey.hpp"

namespace godot{

class SystemProgram : public Node{
    GDCLASS(SystemProgram, Node)
private:

    static const int TRANSFER_DATA_SIZE = 12;

protected:
    static void _bind_methods();

public:
    static const std::string ID;

    static Variant transfer(const Variant &sender, const Variant& reciever, const uint32_t lamports);
};

}

#endif