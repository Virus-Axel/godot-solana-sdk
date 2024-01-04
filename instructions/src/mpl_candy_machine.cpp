#include <mpl_candy_machine.hpp>

#include <pubkey.hpp>
#include <instruction.hpp>
#include "mpl_token_metadata.hpp"
#include "system_program.hpp"
#include "spl_token.hpp"
#include "associated_token_account.hpp"

namespace godot{

void CandyMachineData::_bind_methods(){

}


const std::string MplCandyMachine::ID = "CndyV3LdqHUfDLmE5naZjVN8rBZz4tqhdefbAnjHG3JR";
const std::string MplCandyGuard::ID = "Guard1JwRhJkVH6XZhzoYxeBVQe872VH6QggF4BWmS9g";

PackedByteArray MplCandyMachine::mint_discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {51, 57, 225, 47, 182, 146, 137, 166};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

void MplCandyMachine::_bind_methods(){
    ClassDB::bind_method(D_METHOD("initialize", "candy_machine", "authority", "payer", "rule_set", "collection_metadata", "collection_mint", "collection_master_edition", "collection_update_authority", "collection_delegate_record", "authorization_rules"), &MplCandyMachine::initialize);
    ClassDB::bind_method(D_METHOD("mint", "candy_machine", "mint_authority", "payer", "nft_owner", "nft_mint", "nft_mint_authority", "nft_metadata", "nft_master_edition", "token", "token_record", "collection_delegate_record", "collection_mint", "collection_metadata", "collection_master_edition", "collection_update_authority", "authorization_rules"), &MplCandyMachine::mint);

    ClassDB::bind_static_method("MplCandyMachine", D_METHOD("get_pid"), &MplCandyMachine::get_pid);
}

Variant MplCandyMachine::initialize(
    const Variant &candy_machine,
    const Variant &authority,
    const Variant &payer,
    const Variant &rule_set,
    const Variant &collection_metadata,
    const Variant &collection_mint,
    const Variant &collection_master_edition,
    const Variant &collection_update_authority,
    const Variant &collection_delegate_record,
    const Variant &authorization_rules
){
    Instruction *result = memnew(Instruction);

    PackedByteArray data = mint_discriminator();

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(candy_machine, false, true));

    Array seeds;
    seeds.append(String("candy_machine").to_ascii_buffer());
    seeds.append(Pubkey(candy_machine).get_bytes());

    result->append_meta(AccountMeta(Pubkey::new_pda_bytes(seeds, get_pid()), false, true));
    result->append_meta(AccountMeta(payer, true, true));
    if(rule_set.get_type() == Variant::OBJECT){
        result->append_meta(AccountMeta(rule_set, false, false));
    }
    result->append_meta(AccountMeta(collection_metadata, false, false));
    result->append_meta(AccountMeta(collection_mint, false, false));
    result->append_meta(AccountMeta(collection_master_edition, false, false));
    result->append_meta(AccountMeta(collection_update_authority, true, true));
    result->append_meta(AccountMeta(collection_delegate_record, false, true));

    result->append_meta(AccountMeta(MplTokenMetadata::get_pid(), false, false));
    result->append_meta(AccountMeta(SystemProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111"), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("auth9SigNpDKz4sJJ1DfCTuZrZNSAgh9sFD3rboVmgg"), false, false));

    result->append_meta(AccountMeta(authorization_rules, false, false));

    return result;
}

Variant MplCandyMachine::mint(
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
    const Variant &authorization_rules
){
    Instruction *result = memnew(Instruction);

    PackedByteArray data = mint_discriminator();

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(candy_machine, false, true));

    Array seeds;
    seeds.append(String("candy_machine").to_ascii_buffer());
    seeds.append(Pubkey(candy_machine).get_bytes());

    result->append_meta(AccountMeta(Pubkey::new_pda_bytes(seeds, get_pid()), false, true));
    result->append_meta(AccountMeta(mint_authority, true, false));
    result->append_meta(AccountMeta(payer, true, true));
    result->append_meta(AccountMeta(nft_owner, false, false));
    result->append_meta(AccountMeta(nft_mint, false, true));
    result->append_meta(AccountMeta(nft_mint_authority, true, false));
    if(token.get_type() == Variant::OBJECT){
        result->append_meta(AccountMeta(token, false, false));
    }
    if(token_record.get_type() == Variant::OBJECT){
        result->append_meta(AccountMeta(token_record, false, false));
    }

    result->append_meta(AccountMeta(collection_delegate_record, false, false));

    result->append_meta(AccountMeta(collection_mint, false, false));
    result->append_meta(AccountMeta(collection_metadata, false, true));
    result->append_meta(AccountMeta(collection_master_edition, false, false));
    result->append_meta(AccountMeta(collection_update_authority, true, true));

    result->append_meta(AccountMeta(MplTokenMetadata::get_pid(), false, false));
    result->append_meta(AccountMeta(TokenProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(AssociatedTokenAccountProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(SystemProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111"), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("SysvarS1otHashes111111111111111111111111111"), false, false));

    if(authorization_rules.get_type() == Variant::OBJECT){
        result->append_meta(AccountMeta(Pubkey::new_from_string("auth9SigNpDKz4sJJ1DfCTuZrZNSAgh9sFD3rboVmgg"), false, false));
        result->append_meta(AccountMeta(authorization_rules, false, false));
    }

    return result;
}

Variant MplCandyMachine::new_candy_machine_authority_pda(const Variant& candy_machine_key){
    Array seeds;
    seeds.append(String("candy_machine").to_ascii_buffer());
    seeds.append(Pubkey(candy_machine_key).get_bytes());

    return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant MplCandyMachine::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}

void MplCandyGuard::_bind_methods(){
    ClassDB::bind_method(D_METHOD("mint", "candy_machine", "payer", "minter", "nft_mint", "nft_mint_authority", "nft_metadata", "nft_master_edition", "token", "token_record", "collection_delegate_record", "collection_mint", "collection_metadata", "collection_master_edition", "collection_update_authority", "authorization_rules", "label"), &MplCandyGuard::mint);

    ClassDB::bind_static_method("MplCandyGuard", D_METHOD("get_pid"), &MplCandyGuard::get_pid);
}

Variant MplCandyGuard::mint(
    const Variant &candy_machine,
    const Variant &payer,
    const Variant &minter,
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
    const Variant &authorization_rules,
    const String &label
){
    if(label.length() > 6){
         internal::gdextension_interface_print_warning("Label size must be 6 or less", "mint", __FILE__, __LINE__, true);
         return nullptr;
    }

    Instruction *result = memnew(Instruction);

    PackedByteArray data = MplCandyMachine::mint_discriminator();

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(new_associated_candy_guard_key(candy_machine), false, false));
    result->append_meta(AccountMeta(MplCandyMachine::get_pid(), false, false));
    result->append_meta(AccountMeta(candy_machine, false, true));
    result->append_meta(AccountMeta(MplCandyMachine::new_candy_machine_authority_pda(candy_machine), false, true));

    result->append_meta(AccountMeta(payer, true, true));
    result->append_meta(AccountMeta(minter, true, true));
    result->append_meta(AccountMeta(nft_mint, false, true));
    result->append_meta(AccountMeta(nft_mint_authority, true, false));
    result->append_meta(AccountMeta(nft_metadata, false, true));
    result->append_meta(AccountMeta(nft_master_edition, false, true));

    if(token.get_type() == Variant::OBJECT){
        result->append_meta(AccountMeta(token, false, true));
    }
    if(token_record.get_type() == Variant::OBJECT){
        result->append_meta(AccountMeta(token_record, false, true));
    }

    result->append_meta(AccountMeta(collection_delegate_record, false, false));

    result->append_meta(AccountMeta(collection_mint, false, false));
    result->append_meta(AccountMeta(collection_metadata, false, true));
    result->append_meta(AccountMeta(collection_master_edition, false, false));
    result->append_meta(AccountMeta(collection_update_authority, false, false));

    result->append_meta(AccountMeta(MplTokenMetadata::get_pid(), false, false));
    result->append_meta(AccountMeta(TokenProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(AssociatedTokenAccountProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(SystemProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111"), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("SysvarS1otHashes111111111111111111111111111"), false, false));

    if(authorization_rules.get_type() == Variant::OBJECT){
        result->append_meta(AccountMeta(Pubkey::new_from_string("auth9SigNpDKz4sJJ1DfCTuZrZNSAgh9sFD3rboVmgg"), false, false));
        result->append_meta(AccountMeta(authorization_rules, false, false));
    }

    return result;
}

Variant MplCandyGuard::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}

Variant MplCandyGuard::new_associated_candy_guard_key(const Variant &candy_machine_key){
    Array seeds;
    seeds.append(String("candy_guard").to_ascii_buffer());
    seeds.append(Pubkey(candy_machine_key).get_bytes());
    return Pubkey::new_pda_bytes(seeds, get_pid());
}

}