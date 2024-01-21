#ifndef SOLANA_SDK_SPL_TOKEN
#define SOLANA_SDK_SPL_TOKEN

#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>

namespace godot{

class TokenProgram : public Node{
    GDCLASS(TokenProgram, Node)
private:

protected:
    static void _bind_methods();

public:
    enum MetaDataDelegateRole{
        AUTHORITY = 0,
        COLLECTION = 1,
        USE = 2,
        UPDATE = 3,
    };

    static const std::string ID;
    static Variant new_token_record_address(const Variant &owner_account, const Variant &mint);
    static Variant new_delegate_record_address(const Variant& update_authority, const Variant &mint, const Variant& delegate_address, const MetaDataDelegateRole role);

    static Variant initialize_mint(const Variant& mint_pubkey, const Variant& mint_authority, const Variant& freeze_authority = nullptr, const uint32_t decimals = 9);
    static Variant initialize_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey);

    // TODO(Virax): Write a multisignature mint_to with Array as mint_authority.
    static Variant mint_to(const Variant& mint_pubkey, const Variant& account_pubkey, const Variant& owner_pubkey, const Variant& mint_authority, uint64_t amount);
    static Variant transfer_checked(const Variant& source_pubkey, const Variant& mint_pubkey, const Variant& destination_pubkey, const Variant& source_authority, uint64_t amount, uint32_t decimals);
    static Variant freeze_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey, const Variant& freeze_authority); 

    static Variant get_pid();
};

}

#endif