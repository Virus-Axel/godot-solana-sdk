#ifndef SOLANA_SDK_MPL_TOKEN_PROGRAM
#define SOLANA_SDK_MPL_TOKEN_PROGRAM


#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>
#include <meta_data.hpp>

namespace godot{

class MplTokenMetadata : public Node{
    GDCLASS(MplTokenMetadata, Node)
private:

protected:
    static void _bind_methods();

public:
    static const std::string ID;

    static Variant create_metadata_account(const Variant& account_pubkey, const Variant& mint, const Variant& mint_authority, const Variant& payer, const Variant& update_authority, const Variant &meta_data, bool is_mutable, uint64_t collection_size);
    static Variant create_master_edition(const Variant& edition, const Variant& mint, const Variant& update_authority, const Variant& mint_authority, const Variant& metadata_account, const Variant &payer, const Variant &max_supply);
};

}

#endif