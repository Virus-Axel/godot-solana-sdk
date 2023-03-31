#ifndef SOLANA_SDK_TRANSACTION_HPP
#define SOLANA_SDK_TRANSACTION_HPP

#include "../wrapper/wrapper.h"

#include "pubkey.hpp"
#include "account.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{

class Transaction : public Node {
    GDCLASS(Transaction, Node)

private:
    void *data_pointer;

protected:
    static void _bind_methods();

public:
    Transaction();

    void create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash);

    ~Transaction();
};
}

#endif