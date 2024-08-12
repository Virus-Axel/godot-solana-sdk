#ifndef SOLANA_SDK_ASSOCIATED_TOKEN_ACCOUNT_HPP
#define SOLANA_SDK_ASSOCIATED_TOKEN_ACCOUNT_HPP

#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>

namespace godot{

class AssociatedTokenAccountProgram : public Node{
    GDCLASS(AssociatedTokenAccountProgram, Node)
private:

protected:
    static void _bind_methods();

public:
    static const std::string ID;

    static Variant create_associated_token_account(const Variant &payer, const Variant &wallet_address, const Variant& mint_address, const Variant& token_program_id);
    static Variant get_pid();
};

}

#endif