#ifndef SOLANA_SDK_SPL_TOKEN
#define SOLANA_SDK_SPL_TOKEN

#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>

namespace godot{

/**
 * @brief Instruction Builder for Solana Token Program.
 */
class TokenProgram : public Node{
    GDCLASS(TokenProgram, Node)
private:
    static const std::string ID;

protected:

    /**
     * @brief Create an Instruction that initializes a mint.
     * 
     * @param token_program_pid Token program to initialize with.
     * @param mint_pubkey Mint key of mint to initialize. @writable
     * @param mint_authority Authority of mint.
     * @param freeze_authority Freeze authority of mint.
     * @param decimals Decimals to initialize with.
     * @return Variant Initialize mint instruction.
     */
    static Variant _initialize_mint(const Variant& token_program_pid, const Variant& mint_pubkey, const Variant& mint_authority, const Variant& freeze_authority = nullptr, const uint32_t decimals = 9);

    /**
     * @brief Create an instruction that initializes a token account.
     * 
     * @param token_program_pid Token program to initialize with.
     * @param account_pubkey Token account to initialize. @writable
     * @param mint_pubkey Mint key.
     * @param owner_pubkey Owner of token account.
     * @return Variant Initialize account Instruction.
     */
    static Variant _initialize_account(const Variant& token_program_pid, const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey);

    /**
     * @brief Create an instruction that mints a token to account.
     * 
     * @param token_program_pid Token program to mint with.
     * @param mint_pubkey Mint key. @writable
     * @param account_pubkey Token account key. @writable
     * @param owner_pubkey Owner of token account. @writable
     * @param mint_authority Authority of mint. @signer
     * @param amount Amount of tokens to mint.
     * @return Variant Mint to Instruction.
     */
    static Variant _mint_to(const Variant& token_program_pid, const Variant& mint_pubkey, const Variant& account_pubkey, const Variant& owner_pubkey, const Variant& mint_authority, uint64_t amount);

    /**
     * @brief Create an Instruction that transfers a token to another account.
     * 
     * @param token_program_pid Token program to mint with.
     * @param source_pubkey Account to send from. @writable
     * @param mint_pubkey Mint key.
     * @param destination_pubkey Account to send to. @writable
     * @param source_authority Sender account authority. @signer
     * @param amount Amount of fractions of a token to send.
     * @param decimals Decimals of one fraction.
     * @return Variant Transfer checked Instruction.
     */
    static Variant _transfer_checked(const Variant& token_program_pid, const Variant& source_pubkey, const Variant& mint_pubkey, const Variant& destination_pubkey, const Variant& source_authority, uint64_t amount, uint32_t decimals);

    /**
     * @brief Create an Instruction that freezes a token account.
     * 
     * @param token_program_pid Token program to mint with.
     * @param account_pubkey Account to freeze. @writable
     * @param mint_pubkey Mint key.
     * @param owner_pubkey Unused TODO(Virax): remove 
     * @param freeze_authority Freeze authority of account. @signer
     * @return Variant Freeze account instruction.
     */
    static Variant _freeze_account(const Variant& token_program_pid, const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey, const Variant& freeze_authority); 

    /**
     * @bindmethods{TokenProgram, Node}
     */
    static void _bind_methods();

public:
    /**
     * @brief Role of delegate in metadata.
     */
    enum MetaDataDelegateRole{
        AUTHORITY = 0, ///< Authority role.
        COLLECTION = 1, ///< Collection role.
        USE = 2, ///< Use role.
        UPDATE = 3, ///< Update role.
    };

    /**
     * @brief Create a new token record Pubkey.
     * 
     * @pda{"metadata", token_metadata_pid, mint, "token_record", token}
     * 
     * @param token Token account address.
     * @param mint Mint address.
     * @return Variant Token record Pubkey.
     */
    static Variant new_token_record_address(const Variant &token, const Variant &mint);

    /**
     * @brief Create a new delegate record address.
     * 
     * @warning Setting role does not have an effect. Collection role will always be used.
     * 
     * @param update_authority Update authority.
     * @param mint Mint key.
     * @param delegate_address Delegate address.
     * @param role Role of delegate account.
     * @return Variant Delegate record address.
     */
    static Variant new_delegate_record_address(const Variant& update_authority, const Variant &mint, const Variant& delegate_address, const MetaDataDelegateRole role);

    /**
     * @brief Same as _initialize_mint but with program ID set to Spl token program ID.
     * 
     * @param mint_pubkey Mint key of mint to initialize. @writable
     * @param mint_authority Authority of mint.
     * @param freeze_authority Freeze authority of mint.
     * @param decimals Decimals to initialize with.
     * @return Variant Initialize mint instruction.
     */
    static Variant initialize_mint(const Variant& mint_pubkey, const Variant& mint_authority, const Variant& freeze_authority = nullptr, const uint32_t decimals = 9);

    /**
     * @brief Same as _initialize_account but with program ID set to Spl token program ID.
     * 
     * @param account_pubkey Token account to initialize. @writable
     * @param mint_pubkey Mint key.
     * @param owner_pubkey Owner of token account.
     * @return Variant Initialize account Instruction.
     */
    static Variant initialize_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey);

    // TODO(Virax): Write a multisignature mint_to with Array as mint_authority.
    /**
     * @brief Same as _mint_to but with program ID set to Spl token program ID.
     * 
     * @param mint_pubkey Mint key. @writable
     * @param account_pubkey Token account key. @writable
     * @param owner_pubkey Owner of token account. @writable
     * @param mint_authority Authority of mint. @signer
     * @param amount Amount of tokens to mint.
     * @return Variant Mint to Instruction.
     */
    static Variant mint_to(const Variant& mint_pubkey, const Variant& account_pubkey, const Variant& owner_pubkey, const Variant& mint_authority, uint64_t amount);
    
    /**
     * @brief Same as _transfer_checked but with program ID set to Spl token program ID.
     * 
     * @param source_pubkey Account to send from. @writable
     * @param mint_pubkey Mint key.
     * @param destination_pubkey Account to send to. @writable
     * @param source_authority Sender account authority. @signer
     * @param amount Amount of fractions of a token to send.
     * @param decimals Decimals of one fraction.
     * @return Variant Transfer checked Instruction.
     */
    static Variant transfer_checked(const Variant& source_pubkey, const Variant& mint_pubkey, const Variant& destination_pubkey, const Variant& source_authority, uint64_t amount, uint32_t decimals);
    
    /**
     * @brief Same as _freeze_account but with program ID set to Spl token program ID.
     * 
     * @param account_pubkey Account to freeze. @writable
     * @param mint_pubkey Mint key.
     * @param owner_pubkey Unused TODO(Virax): remove 
     * @param freeze_authority Freeze authority of account. @signer
     * @return Variant Freeze account instruction.
     */
    static Variant freeze_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey, const Variant& freeze_authority); 

    /**
     * @brief Get the program ID of TokenProgram.
     * 
     * @return Variant Program ID Pubkey.
     */
    static Variant get_pid();
};

}

#endif