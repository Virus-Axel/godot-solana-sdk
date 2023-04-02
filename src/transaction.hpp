#ifndef SOLANA_SDK_TRANSACTION_HPP
#define SOLANA_SDK_TRANSACTION_HPP

#include "../wrapper/wrapper.h"

#include "pubkey.hpp"
#include "account.hpp"
#include "solana_node.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{

class Transaction : public SolanaNode {
    GDCLASS(Transaction, Node)

private:
    Array instructions;
    Variant payer;

    void _update_pointer() override;
    void _free_pointer() override;

protected:
    static void _bind_methods();

public:
    Transaction();

    void set_instructions(const Array& p_value);
    Array get_instructions();

    void set_payer(const Variant& p_value);
    Variant get_payer();

    void create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash);

    ~Transaction();
};
}

#endif