#ifndef SOLANA_SDK_SYSTEM_PROGRAM_HPP
#define SOLANA_SDK_SYSTEM_PROGRAM_HPP


#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>
#include "pubkey.hpp"

namespace godot{

class SystemProgram : public Node{
    GDCLASS(SystemProgram, Node)
private:

    enum MethodNumber{
        CREATE_ACCOUNT = 0,
        CREATE_ACCOUNT_WITH_SEED = 3,
    };

    static const int TRANSFER_DATA_SIZE = 12;
    static const int CREATE_ACCOUNT_DATA_SIZE = 52;
    static const int CREATE_ACCOUNT_WITH_SEED_DATA_SIZE = 92;

protected:
    static void _bind_methods();

public:
    static const std::string ID;

    static Variant create_account(const Variant &from_keypair, const Variant& to_keypair, const uint64_t lamports, const uint64_t space, const Variant& owner);
    static Variant create_account_with_seed(const Variant &from_keypair, const Variant& base_keypair, const String& seed, const uint64_t lamports, const uint64_t space, const Variant& owner);
    static Variant transfer(const Variant &sender, const Variant& reciever, const uint64_t lamports);
};

}

#endif