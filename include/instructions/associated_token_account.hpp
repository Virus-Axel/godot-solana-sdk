#ifndef SOLANA_SDK_ASSOCIATED_TOKEN_ACCOUNT_HPP
#define SOLANA_SDK_ASSOCIATED_TOKEN_ACCOUNT_HPP

#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>

namespace godot{

/**
 * @brief Instruction builder for the Associated Token Account Program.
 */
class AssociatedTokenAccountProgram : public Node{
    GDCLASS(AssociatedTokenAccountProgram, Node)
private:

protected:
    /**
     * @brief Bind methods to AssociatedTokenAccountProgram Node.
     */
    static void _bind_methods();

public:
    static const std::string ID; ///< Base 58 encoded address string.

    /**
     * @brief Create a associated token account Instruction.
     * 
     * @param payer Payer of the account creation
     * @signer
     * @writable
     * @param wallet_address Wallet that owns the token account.
     * @param mint_address Mint address.
     * @param token_program_id Program ID of token program to use.
     * @return Variant Create associated token account instruction.
     */
    static Variant create_associated_token_account(const Variant &payer, const Variant &wallet_address, const Variant& mint_address, const Variant& token_program_id);
    
    /**
     * @brief Get the pid of Associated Token Account Program.
     * 
     * @return Variant Pubkey of program ID.
     */
    static Variant get_pid();
};

}

#endif