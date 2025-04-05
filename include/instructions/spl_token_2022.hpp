#ifndef SOLANA_SDK_SPL_TOKEN_2022
#define SOLANA_SDK_SPL_TOKEN_2022

#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>
#include <spl_token.hpp>

namespace godot{

/**
 * @brief Instruction Builder for Solana Token2022 Program.
 */
class TokenProgram2022 : public TokenProgram{
    GDCLASS(TokenProgram2022, Node)
private:
    static PackedByteArray metadata_update_discriminator();
    static PackedByteArray metadata_initialize_discriminator();

    static const std::string ID;

protected:
    /**
     * @brief @bindmethods{TokenProgram2022, Node}
     */
    static void _bind_methods();

public:
    /**
     * @brief Get the Program ID of TokenProgram2022.
     * 
     * @return Variant Program ID Pubkey.
     */
    static Variant get_pid();

    /**
     * @brief Create an Instruction that initializes metadata for a mint.
     * 
     * @param payer Payer of the Transaction. @signer
     * @param mint_keypair Mint key. @writable
     * @param name Name of token.
     * @param symbol Symbol of token.
     * @param uri URI of token.
     * @return Variant Initialize metadata Instruction.
     */
    static Variant initialize_metadata(const Variant& payer, const Variant& mint_keypair, const String& name, const String& symbol, const String& uri);
    
    /**
     * @brief Create an Instruction that initializes a metadata pointer.
     * 
     * @param payer Payer of the Transaction. @signer
     * @param mint_keypair Mint key. @writable
     * @return Variant Create metadata pointer Instruction.
     */
    static Variant initialize_metadata_pointer(const Variant& payer, const Variant& mint_keypair);
    
    /**
     * @brief Create an Instruction that updates metadata.
     * 
     * @param payer Payer of the Transaction. @signer
     * @param mint_keypair Mint key. @writable
     * @param field What field to update.
     * @param value Value to set.
     * @return Variant Update metadata pointer Instruction.
     */
    static Variant update_metadata(const Variant& payer, const Variant& mint_keypair, const String& field, const String& value);

    /**
     * @brief Same as _initialize_mint but with program ID set to Spl token 2022 program ID.
     * 
     * @param mint_pubkey Mint key of mint to initialize. @writable
     * @param mint_authority Authority of mint.
     * @param freeze_authority Freeze authority of mint.
     * @param decimals Decimals to initialize with.
     * @return Variant Initialize mint instruction.
     */
    static Variant initialize_mint(const Variant& mint_pubkey, const Variant& mint_authority, const Variant& freeze_authority = nullptr, const uint32_t decimals = 9);
    
    /**
     * @brief Same as _initialize_account but with program ID set to Spl token 2022 program ID.
     * 
     * @param account_pubkey Token account to initialize. @writable
     * @param mint_pubkey Mint key.
     * @param owner_pubkey Owner of token account.
     * @return Variant Initialize account Instruction.
     */
    static Variant initialize_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey);

    /**
     * @brief Same as _mint_to but with program ID set to Spl token 2022 program ID.
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
     * @brief Same as _transfer_checked but with program ID set to Spl token 2022 program ID.
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
     * @brief Same as _freeze_account but with program ID set to Spl token 2022 program ID.
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
    static unsigned int get_mint_account_size_from_data(const String& name, const String symbol, const String uri, const Dictionary custom_data);
};

}

#endif