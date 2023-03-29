#ifndef SOLANA_SDK_ACCOUNT_HPP
#define SOLANA_SDK_ACCOUNT_HPP

#include "../wrapper/wrapper.h"

#include "pubkey.hpp"
#include "account.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{
class Account : public Node {
    GDCLASS(Account, Node)

private:
    void *data_pointer;

protected:
    static void _bind_methods();

public:
    Account();
    Error create(uint64_t lamports, uint64_t size, const Pubkey& owner);

    ~Account();
};
}

#endif