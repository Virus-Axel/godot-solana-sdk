#ifndef SOLANA_SDK_MPL_TOKEN_PROGRAM
#define SOLANA_SDK_MPL_TOKEN_PROGRAM


#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>
#include <meta_data.hpp>
#include <solana_client.hpp>

namespace godot{

class MplTokenMetadata : public Node{
    GDCLASS(MplTokenMetadata, Node)
private:
    bool pending_fetch = false;
    SolanaClient *metadata_client = nullptr;

    void metadata_callback(const Dictionary& rpc_result);

protected:
    static void _bind_methods();

public:
    void _process(double delta) override;
    MplTokenMetadata();

    void set_url_override(const String& url_override);
    String get_url_override();

    static const std::string ID;

    static Variant new_associated_metadata_pubkey(const Variant& mint);
    static Variant new_associated_metadata_pubkey_master_edition(const Variant& mint);

    Variant get_mint_metadata(const Variant& mint);

    static Variant create_metadata_account(const Variant& account_pubkey, const Variant& mint, const Variant& mint_authority, const Variant& payer, const Variant& update_authority, const Variant &meta_data, bool is_mutable, uint64_t collection_size);
    static Variant create_master_edition(const Variant& edition, const Variant& mint, const Variant& update_authority, const Variant& mint_authority, const Variant& metadata_account, const Variant &payer, const Variant &max_supply);
    static Variant get_pid();
};

}

#endif