#ifndef SOLANA_SDK_MPL_CANDY_MACHINE
#define SOLANA_SDK_MPL_CANDY_MACHINE

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>

namespace godot{

class CandyMachineData : public Resource{
    GDCLASS(CandyMachineData, Resource)
private:
protected:
    static void _bind_methods();
public:
};

class MplCandyMachine : public Node{
    GDCLASS(MplCandyMachine, Node)
private:
    PackedByteArray mint_discriminator();

protected:
    static void _bind_methods();

public:
    static const std::string ID;

    Variant initialize(
        const Variant &candy_machine,
        const Variant &authority,
        const Variant &payer,
        const Variant &rule_set,
        const Variant &collection_metadata,
        const Variant &collection_mint,
        const Variant &collection_master_edition,
        const Variant &collection_update_authority,
        const Variant &collection_delegate_record,
        const Variant &authorization_rules);

    Variant mint(
        const Variant &candy_machine,
        const Variant &mint_authority,
        const Variant &payer,
        const Variant &nft_owner,
        const Variant &nft_mint,
        const Variant &nft_mint_authority,
        const Variant &nft_metadata,
        const Variant &nft_master_edition,
        const Variant &token,
        const Variant &token_record,
        const Variant &collection_delegate_record,
        const Variant &collection_mint,
        const Variant &collection_metadata,
        const Variant &collection_master_edition,
        const Variant &collection_update_authority,
        const Variant &authorization_rules);

    static Variant get_pid();
};

}

#endif