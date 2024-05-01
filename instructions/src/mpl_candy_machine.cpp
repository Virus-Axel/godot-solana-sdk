#include <mpl_candy_machine.hpp>

#include <pubkey.hpp>
#include <instruction.hpp>
#include "mpl_token_metadata.hpp"
#include "system_program.hpp"
#include "spl_token.hpp"
#include "associated_token_account.hpp"
#include "mpl_token_metadata.hpp"
#include "solana_utils.hpp"
#include "solana_client.hpp"
#include "meta_data.hpp"
#include "utils.hpp"

namespace godot{

void CandyGuardAccessList::_bind_methods(){
}

PackedByteArray CandyGuardAccessList::serialize() const{
    PackedByteArray result;
    result.append_array(serialize_guard_settings());
    result.append_array(serialize_groups());

    return result;
}

PackedByteArray CandyGuardAccessList::serialize_groups() const{
    PackedByteArray result;

    result.resize(4);
    result.encode_u32(0, groups.size());

    for(unsigned int i = 0; i < groups.size(); i++){
        CandyGuardAccessList *group = Object::cast_to<CandyGuardAccessList>(groups[i]);
        PackedByteArray fix_size_label = label.to_ascii_buffer();

        const int MAX_LABEL_SIZE = 6;
        fix_size_label.resize(MAX_LABEL_SIZE);

        result.append_array(fix_size_label);
        result.append_array(group->serialize_guard_settings());
    }

    return result;
}

PackedByteArray CandyGuardAccessList::serialize_guard_settings() const{
    PackedByteArray result;
    result.resize(8);
    uint64_t enabled_guards = 0;

    if(enable_bot_tax){
        enabled_guards += 1;
        PackedByteArray bot_tax_bytes;
        bot_tax_bytes.resize(9);
        bot_tax_bytes.encode_u64(0, bot_tax_lamports);
        bot_tax_bytes[8] = (int)validate_last_instruction;
        result.append_array(bot_tax_bytes);
    }
    if(enable_sol_payment){
        enabled_guards += 2;
        PackedByteArray sol_payment_bytes;
        sol_payment_bytes.resize(8);
        sol_payment_bytes.encode_u64(0, sol_payment_lamports);
        result.append_array(sol_payment_bytes);
        result.append_array(Pubkey(sol_payment_destination).to_bytes());
    }
    if(enable_token_payment){
        enabled_guards += 4;
        PackedByteArray token_payment_bytes;
        token_payment_bytes.resize(8);
        token_payment_bytes.encode_u64(0, token_payment_amount);
        result.append_array(token_payment_bytes);
        result.append_array(Pubkey(token_payment_mint).to_bytes());
        result.append_array(Pubkey(token_payment_destination).to_bytes());
    }
    if(enable_start_date){
        enabled_guards += 8;
        PackedByteArray start_date_bytes;
        start_date_bytes.resize(8);
        start_date_bytes.encode_u64(0, start_date);
        result.append_array(start_date_bytes);
    }
    if(enable_third_party_signer){
        enabled_guards += 16;
        result.append_array(Pubkey(signer_key).to_bytes());
    }
    if(enable_token_gate){
        enabled_guards += 32;
        PackedByteArray token_gate_bytes;
        token_gate_bytes.resize(8);
        token_gate_bytes.encode_u64(0, token_gate_amount);
        result.append_array(token_gate_bytes);
        result.append_array(Pubkey(token_gate_mint).to_bytes());
    }
    if(enable_gatekeeper){
        enabled_guards += 64;
        result.append_array(Pubkey(gatekeeper_network).to_bytes());
        result.append((int) expire_on_use);
    }
    if(enable_end_date){
        enabled_guards += 128;
        PackedByteArray end_date_bytes;
        end_date_bytes.resize(8);
        end_date_bytes.encode_s64(0, token_gate_amount);
        result.append_array(end_date_bytes);
    }
    if(enable_allow_list){
        enabled_guards += 256;
        result.append_array(allowed_merkle_root);
    }
    if(enable_mint_limit){
        enabled_guards += 512;
        PackedByteArray mint_limit_bytes;
        mint_limit_bytes.resize(3);
        mint_limit_bytes[0] = limit_id;
        mint_limit_bytes.encode_u16(1, mint_limit);
        result.append_array(mint_limit_bytes);
    }
    if(enable_nft_payment){
        enabled_guards += 1024;
        result.append_array(Pubkey(required_nft_payment_collection).to_bytes());
        result.append_array(Pubkey(nft_payment_destination).to_bytes());
    }
    if(enable_redeem_account){
        enabled_guards += 2048;
        PackedByteArray redeem_bytes;
        redeem_bytes.resize(8);
        redeem_bytes.encode_u64(0, max_redeem_amount);
        result.append_array(redeem_bytes);
    }
    if(enable_address_gate){
        enabled_guards += 4096;
        result.append_array(Pubkey(address_gate_reciever).to_bytes());
    }
    if(enable_nft_gate){
        enabled_guards += 8192;
        result.append_array(Pubkey(required_nft_gate_collection).to_bytes());
    }
    if(enable_nft_burn){
        enabled_guards += 16384;
        result.append_array(Pubkey(required_nft_burn_collection).to_bytes());
    }
    if(enable_token_burn){
        enabled_guards += 32768;
        PackedByteArray token_burn_bytes;
        token_burn_bytes.resize(8);
        token_burn_bytes.encode_u64(0, max_redeem_amount);
        result.append_array(token_burn_bytes);
        result.append_array(Pubkey(token_burn_mint).to_bytes());
    }
    if(enable_freeze_sol_payment){
        enabled_guards += 65536;
        PackedByteArray freeze_sol_bytes;
        freeze_sol_bytes.resize(8);
        freeze_sol_bytes.encode_u64(0, freeze_amount_lamports);
        result.append_array(freeze_sol_bytes);
        result.append_array(Pubkey(freeze_sol_destination).to_bytes());
    }
    if(enable_freeze_token_payment){
        enabled_guards += 65536;
        PackedByteArray freeze_token_bytes;
        freeze_token_bytes.resize(8);
        freeze_token_bytes.encode_u64(0, amount);
        result.append_array(freeze_token_bytes);
        result.append_array(Pubkey(mint).to_bytes());
        result.append_array(Pubkey(freeze_token_destination_ata).to_bytes());
    }
    if(enable_program_gate){
        enabled_guards += 131072;
        PackedByteArray array_length_bytes;
        array_length_bytes.resize(8);
        array_length_bytes.encode_u64(0, amount);
        result.append_array(array_length_bytes);
        for(unsigned int i = 0; i < program_gate_addresses.size(); i++){
            result.append_array(Pubkey(program_gate_addresses[i]).to_bytes());
        }
    }
    if(enable_allocation){
        enabled_guards += 262144;
        PackedByteArray allocation_bytes;
        allocation_bytes.resize(5);
        allocation_bytes.encode_u8(0, allocation_id);
        allocation_bytes.encode_u32(1, allocation_size);
        result.append_array(allocation_bytes);
    }
    
    result.encode_u64(0, enabled_guards);

    return result;
}

CandyGuardAccessList& CandyGuardAccessList::get_group(const String &label){
    if(label == "default")
        return *this;

    for(unsigned int i = 0; i < groups.size(); i++){
        if(groups[i].get_type() != Variant::OBJECT){
            continue;
        }
        else if(Object::cast_to<CandyGuardAccessList>(groups[i])->label == label){
            return *Object::cast_to<CandyGuardAccessList>(groups[i]);
        }
    }
    internal::gdextension_interface_print_warning("Nonexisting group", "get_group", __FILE__, __LINE__, true);

    return *this;
}

TypedArray<AccountMeta> CandyGuardAccessList::get_mint_arg_accounts(const Variant& payer){
    TypedArray<AccountMeta> result;

    if(enable_third_party_signer){
        result.append(AccountMeta::new_account_meta(signer_key, true, true));
    }
    if(enable_gatekeeper){
        result.append(AccountMeta::new_account_meta(gatekeeper_network, false, false));
    }
    if(enable_nft_payment){
        result.append(AccountMeta::new_account_meta(nft_payment_destination, false, true));
    }
    if(enable_nft_gate){
        result.append(AccountMeta::new_account_meta(required_nft_gate_collection, false, false));
    }
    if(enable_nft_burn){
        result.append(AccountMeta::new_account_meta(required_nft_burn_collection, false, true));
    }
    if(enable_sol_payment){
        result.append(AccountMeta::new_account_meta(sol_payment_destination, false, true));
    }
    if(enable_token_payment){
        const Variant token_payment_ata = Pubkey::new_associated_token_address(payer, token_payment_mint);
        result.append(AccountMeta::new_account_meta(token_payment_ata, false, true));
        result.append(AccountMeta::new_account_meta(token_payment_destination, false, true));
    }
    if(enable_token_gate){
        const Variant token_gate_ata = Pubkey::new_associated_token_address(payer, token_gate_mint);
        result.append(AccountMeta::new_account_meta(token_gate_ata, false, true));
    }
    if(enable_token_burn){
        const Variant token_burn_ata = Pubkey::new_associated_token_address(payer, token_burn_mint);
        result.append(AccountMeta::new_account_meta(token_burn_ata, false, true));
        result.append(AccountMeta::new_account_meta(token_burn_mint, false, true));
    }
    if(enable_freeze_sol_payment){
        result.append(AccountMeta::new_account_meta(freeze_sol_destination, false, true));
    }
    if(enable_freeze_token_payment){
        result.append(AccountMeta::new_account_meta(mint, false, false));
        result.append(AccountMeta::new_account_meta(freeze_token_destination_ata, false, true));
    }
    if(enable_program_gate){
        for(unsigned int i = 0; i < program_gate_addresses.size(); i++){
            result.append(AccountMeta::new_account_meta(program_gate_addresses, false, false));
        }
    }

    return result;
}

PackedByteArray CandyGuardAccessList::serialize_mint_settings() const{
    PackedByteArray result;
    result.resize(4);

    if(enable_gatekeeper){
        result.append((int) expire_on_use);
    }
    if(enable_allow_list){
        result.append_array(allowed_merkle_root);
    }
    if(enable_mint_limit){
        result.append(limit_id);
    }

    // insert mint setting size
    result.encode_u32(0, result.size() - 4);

    return result;
}

bool CandyGuardAccessList::_set(const StringName &p_name, const Variant &p_value){
    String name = p_name;

    if(name == "enable_address_gate"){
        notify_property_list_changed();
        enable_address_gate = p_value;
        return true;
    }
    else if(name == "address_gate_reciever"){
        address_gate_reciever = p_value;
        return true;
    }
    else if(name == "enable_allocation"){
        notify_property_list_changed();
        enable_allocation = p_value;
        return true;
    }
    else if(name == "allocation_id"){
        allocation_id = p_value;
        return true;
    }
    else if(name == "allocation_size"){
        allocation_size = p_value;
        return true;
    }
    else if(name == "enable_allow_list"){
        notify_property_list_changed();
        enable_allow_list = p_value;
        return true;
    }
    else if(name == "allowed_merkle_root"){
        allowed_merkle_root = p_value;
        return true;
    }
    else if(name == "enable_bot_tax"){
        notify_property_list_changed();
        enable_bot_tax = p_value;
        return true;
    }
    else if(name == "bot_tax_lamports"){
        bot_tax_lamports = p_value;
        return true;
    }
    else if(name == "validate_last_instruction"){
        validate_last_instruction = p_value;
        return true;
    }
    else if(name == "enable_end_date"){
        notify_property_list_changed();
        enable_end_date = p_value;
        return true;
    }
    else if(name == "end_date"){
        end_date = p_value;
        return true;
    }
    else if(name == "enable_freeze_sol_payment"){
        notify_property_list_changed();
        enable_freeze_sol_payment = p_value;
        return true;
    }
    else if(name == "freeze_amount_lamports"){
        freeze_amount_lamports = p_value;
        return true;
    }
    else if(name == "freeze_sol_destination"){
        freeze_sol_destination = p_value;
        return true;
    }
    else if(name == "enable_freeze_token_payment"){
        notify_property_list_changed();
        enable_freeze_token_payment = p_value;
        return true;
    }
    else if(name == "amount"){
        amount = p_value;
        return true;
    }
    else if(name == "mint"){
        mint = p_value;
        return true;
    }
    else if(name == "freeze_token_destination_ata"){
        freeze_token_destination_ata = p_value;
        return true;
    }
    else if(name == "enable_gatekeeper"){
        notify_property_list_changed();
        enable_gatekeeper = p_value;
        return true;
    }
    else if(name == "gatekeeper_network"){
        gatekeeper_network = p_value;
        return true;
    }
    else if(name == "expire_on_use"){
        expire_on_use = p_value;
        return true;
    }
    else if(name == "enable_mint_limit"){
        notify_property_list_changed();
        enable_mint_limit = p_value;
        return true;
    }
    else if(name == "limit_id"){
        limit_id = p_value;
        return true;
    }
    else if(name == "mint_limit"){
        mint_limit = p_value;
        return true;
    }
    else if(name == "enable_nft_burn"){
        notify_property_list_changed();
        enable_nft_burn = p_value;
        return true;
    }
    else if(name == "required_nft_burn_collection"){
        required_nft_burn_collection = p_value;
        return true;
    }
    else if(name == "enable_nft_gate"){
        notify_property_list_changed();
        enable_nft_gate = p_value;
        return true;
    }
    else if(name == "required_nft_gate_collection"){
        required_nft_gate_collection = p_value;
        return true;
    }
    else if(name == "enable_nft_payment"){
        notify_property_list_changed();
        enable_nft_payment = p_value;
        return true;
    }
    else if(name == "required_nft_payment_collection"){
        required_nft_payment_collection = p_value;
        return true;
    }
    else if(name == "nft_payment_destination"){
        nft_payment_destination = p_value;
        return true;
    }
    else if(name == "enable_program_gate"){
        notify_property_list_changed();
        enable_program_gate = p_value;
        return true;
    }
    else if(name == "program_gate_addresses"){
        program_gate_addresses = p_value;
        return true;
    }
    else if(name == "enable_redeem_account"){
        notify_property_list_changed();
        enable_redeem_account = p_value;
        return true;
    }
    else if(name == "max_redeem_amount"){
        max_redeem_amount = p_value;
        return true;
    }
    else if(name == "enable_sol_payment"){
        notify_property_list_changed();
        enable_sol_payment = p_value;
        return true;
    }
    else if(name == "sol_payment_lamports"){
        sol_payment_lamports = p_value;
        return true;
    }
    else if(name == "sol_payment_destination"){
        sol_payment_destination = p_value;
        return true;
    }
    else if(name == "enable_start_date"){
        notify_property_list_changed();
        enable_start_date = p_value;
        return true;
    }
    else if(name == "start_date"){
        start_date = p_value;
        return true;
    }
    else if(name == "enable_third_party_signer"){
        notify_property_list_changed();
        enable_third_party_signer = p_value;
        return true;
    }
    else if(name == "signer_key"){
        signer_key = p_value;
        return true;
    }
    else if(name == "enable_token_burn"){
        notify_property_list_changed();
        enable_token_burn = p_value;
        return true;
    }
    else if(name == "token_burn_amount"){
        token_burn_amount = p_value;
        return true;
    }
    else if(name == "token_burn_mint"){
        token_burn_mint = p_value;
        return true;
    }
    else if(name == "enable_token_gate"){
        notify_property_list_changed();
        enable_token_gate = p_value;
        return true;
    }
    else if(name == "token_gate_amount"){
        token_gate_amount = p_value;
        return true;
    }
    else if(name == "token_gate_mint"){
        token_gate_mint = p_value;
        return true;
    }
    else if(name == "enable_token_payment"){
        notify_property_list_changed();
        enable_token_payment = p_value;
        return true;
    }
    else if(name == "token_payment_amount"){
        token_payment_amount = p_value;
        return true;
    }
    else if(name == "token_payment_mint"){
        token_payment_mint = p_value;
        return true;
    }
    else if(name == "token_payment_destination"){
        token_payment_destination = p_value;
        return true;
    }
    else if(name == "groups"){
        groups = (TypedArray<CandyGuardAccessList>)p_value;
        for(unsigned int i = 0; i < groups.size(); i++){
            if(groups[i].get_type() == Variant::OBJECT){
                if(Object::cast_to<CandyGuardAccessList>(groups[i])->top_level){
                    Object::cast_to<CandyGuardAccessList>(groups[i])->top_level = false;
                    Object::cast_to<CandyGuardAccessList>(groups[i])->label = "";
                }
            }
        }
        return true;
    }
    else if(name == "label"){
        label = p_value;
        return true;
    }
    else if(name == "top_level"){
        top_level = p_value;
        return true;
    }
	return false;
}

bool CandyGuardAccessList::_get(const StringName &p_name, Variant &r_ret) const{
    String name = p_name;
	if(name == "enable_address_gate"){
        r_ret = enable_address_gate;
        return true;
    }
    else if(name == "address_gate_reciever"){
        r_ret = address_gate_reciever;
        return true;
    }
    else if(name == "enable_allocation"){
        r_ret = enable_allocation;
        return true;
    }
    else if(name == "allocation_id"){
        r_ret = allocation_id;
        return true;
    }
    else if(name == "allocation_size"){
        r_ret = allocation_size;
        return true;
    }
    else if(name == "enable_allow_list"){
        r_ret = enable_allow_list;
        return true;
    }
    else if(name == "allowed_merkle_root"){
        r_ret = allowed_merkle_root;
        return true;
    }
    else if(name == "enable_bot_tax"){
        r_ret = enable_bot_tax;
        return true;
    }
    else if(name == "bot_tax_lamports"){
        r_ret = bot_tax_lamports;
        return true;
    }
    else if(name == "validate_last_instruction"){
        r_ret = validate_last_instruction;
        return true;
    }
    else if(name == "enable_end_date"){
        r_ret = enable_end_date;
        return true;
    }
    else if(name == "end_date"){
        r_ret = end_date;
        return true;
    }
    else if(name == "enable_freeze_sol_payment"){
        r_ret = enable_freeze_sol_payment;
        return true;
    }
    else if(name == "freeze_amount_lamports"){
        r_ret = freeze_amount_lamports;
        return true;
    }
    else if(name == "freeze_sol_destination"){
        r_ret = freeze_sol_destination;
        return true;
    }
    else if(name == "enable_freeze_token_payment"){
        r_ret = enable_freeze_token_payment;
        return true;
    }
    else if(name == "amount"){
        r_ret = amount;
        return true;
    }
    else if(name == "mint"){
        r_ret = mint;
        return true;
    }
    else if(name == "freeze_token_destination_ata"){
        r_ret = freeze_token_destination_ata;
        return true;
    }
    else if(name == "enable_gatekeeper"){
        r_ret = enable_gatekeeper;
        return true;
    }
    else if(name == "gatekeeper_network"){
        r_ret = gatekeeper_network;
        return true;
    }
    else if(name == "expire_on_use"){
        r_ret = expire_on_use;
        return true;
    }
    else if(name == "enable_mint_limit"){
        r_ret = enable_mint_limit;
        return true;
    }
    else if(name == "limit_id"){
        r_ret = limit_id;
        return true;
    }
    else if(name == "mint_limit"){
        r_ret = mint_limit;
        return true;
    }
    else if(name == "enable_nft_burn"){
        r_ret = enable_nft_burn;
        return true;
    }
    else if(name == "required_nft_burn_collection"){
        r_ret = required_nft_burn_collection;
        return true;
    }
    else if(name == "enable_nft_gate"){
        r_ret = enable_nft_gate;
        return true;
    }
    else if(name == "required_nft_gate_collection"){
        r_ret = required_nft_gate_collection;
        return true;
    }
    else if(name == "enable_nft_payment"){
        r_ret = enable_nft_payment;
        return true;
    }
    else if(name == "required_nft_payment_collection"){
        r_ret = required_nft_payment_collection;
        return true;
    }
    else if(name == "nft_payment_destination"){
        r_ret = nft_payment_destination;
        return true;
    }
    else if(name == "enable_program_gate"){
        r_ret = enable_program_gate;
        return true;
    }
    else if(name == "program_gate_addresses"){
        r_ret = program_gate_addresses;
        return true;
    }
    else if(name == "enable_redeem_account"){
        r_ret = enable_redeem_account;
        return true;
    }
    else if(name == "max_redeem_amount"){
        r_ret = max_redeem_amount;
        return true;
    }
    else if(name == "enable_sol_payment"){
        r_ret = enable_sol_payment;
        return true;
    }
    else if(name == "sol_payment_lamports"){
        r_ret = sol_payment_lamports;
        return true;
    }
    else if(name == "sol_payment_destination"){
        r_ret = sol_payment_destination;
        return true;
    }
    else if(name == "enable_start_date"){
        r_ret = enable_start_date;
        return true;
    }
    else if(name == "start_date"){
        r_ret = start_date;
        return true;
    }
    else if(name == "enable_third_party_signer"){
        r_ret = enable_third_party_signer;
        return true;
    }
    else if(name == "signer_key"){
        r_ret = signer_key;
        return true;
    }
    else if(name == "enable_token_burn"){
        r_ret = enable_token_burn;
        return true;
    }
    else if(name == "token_burn_amount"){
        r_ret = token_burn_amount;
        return true;
    }
    else if(name == "token_burn_mint"){
        r_ret = token_burn_mint;
        return true;
    }
    else if(name == "enable_token_gate"){
        r_ret = enable_token_gate;
        return true;
    }
    else if(name == "token_gate_amount"){
        r_ret = token_gate_amount;
        return true;
    }
    else if(name == "token_gate_mint"){
        r_ret = token_gate_mint;
        return true;
    }
    else if(name == "enable_token_payment"){
        r_ret = enable_token_payment;
        return true;
    }
    else if(name == "token_payment_amount"){
        r_ret = token_payment_amount;
        return true;
    }
    else if(name == "token_payment_mint"){
        r_ret = token_payment_mint;
        return true;
    }
    else if(name == "token_payment_destination"){
        r_ret = token_payment_destination;
        return true;
    }
    else if(name == "groups"){
        r_ret = groups;
        return true;
    }
    else if(name == "label"){
        r_ret = label;
        return true;
    }
    else if(name == "top_level"){
        r_ret = top_level;
        return true;
    }
	return false;
}

void CandyGuardAccessList::_get_property_list(List<PropertyInfo> *p_list) const {
    p_list->push_back(PropertyInfo(Variant::BOOL, "top_level", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
    if(top_level){
        p_list->push_back(PropertyInfo(Variant::STRING, "label", PROPERTY_HINT_TYPE_STRING, "default", PROPERTY_USAGE_READ_ONLY + PROPERTY_USAGE_DEFAULT));
    }
    else{
        p_list->push_back(PropertyInfo(Variant::STRING, "label"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_address_gate"));
    if(enable_address_gate){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "address_gate_reciever", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_allocation"));
    if(enable_allocation){
        p_list->push_back(PropertyInfo(Variant::INT, "allocation_id", PROPERTY_HINT_RANGE, "0,255"));
        p_list->push_back(PropertyInfo(Variant::INT, "allocation_size", PROPERTY_HINT_RANGE, "0,65535"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_allow_list"));
    if(enable_allow_list){
        p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "allowed_merkle_root"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_bot_tax"));
    if(enable_bot_tax){
        p_list->push_back(PropertyInfo(Variant::INT, "bot_tax_lamports"));
        p_list->push_back(PropertyInfo(Variant::BOOL, "validate_last_instruction"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_end_date"));
    if(enable_end_date){
        p_list->push_back(PropertyInfo(Variant::INT, "end_date"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_freeze_sol_payment"));
    if(enable_freeze_sol_payment){
        p_list->push_back(PropertyInfo(Variant::INT, "freeze_amount_lamports"));
        p_list->push_back(PropertyInfo(Variant::OBJECT, "freeze_sol_destination", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_freeze_token_payment"));
    if(enable_freeze_token_payment){
        p_list->push_back(PropertyInfo(Variant::INT, "amount"));
        p_list->push_back(PropertyInfo(Variant::OBJECT, "mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
        p_list->push_back(PropertyInfo(Variant::OBJECT, "freeze_token_destination_ata", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_gatekeeper"));
    if(enable_gatekeeper){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "gatekeeper_network", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
        p_list->push_back(PropertyInfo(Variant::BOOL, "expire_on_use"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_mint_limit"));
    if(enable_mint_limit){
        p_list->push_back(PropertyInfo(Variant::INT, "limit_id", PROPERTY_HINT_RANGE, "0,255"));
        p_list->push_back(PropertyInfo(Variant::INT, "mint_limit", PROPERTY_HINT_RANGE, "0,65535"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_nft_burn"));
    if(enable_nft_burn){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "required_nft_burn_collection", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_nft_gate"));
    if(enable_nft_gate){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "required_nft_gate_collection", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_nft_payment"));
    if(enable_nft_payment){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "required_nft_payment_collection", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
        p_list->push_back(PropertyInfo(Variant::OBJECT, "nft_payment_destination", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_program_gate"));
    if(enable_program_gate){
        p_list->push_back(PropertyInfo(Variant::ARRAY, "program_gate_addresses"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_redeem_account"));
    if(enable_redeem_account){
        p_list->push_back(PropertyInfo(Variant::INT, "max_redeem_amount"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_sol_payment"));
    if(enable_sol_payment){
        p_list->push_back(PropertyInfo(Variant::INT, "sol_payment_lamports"));
        p_list->push_back(PropertyInfo(Variant::OBJECT, "sol_payment_destination", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_start_date"));
    if(enable_start_date){
        p_list->push_back(PropertyInfo(Variant::INT, "start_date"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_third_party_signer"));
    if(enable_third_party_signer){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "signer_key", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_token_burn"));
    if(enable_token_burn){
        p_list->push_back(PropertyInfo(Variant::INT, "token_burn_amount"));
        p_list->push_back(PropertyInfo(Variant::OBJECT, "token_burn_mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_token_gate"));
    if(enable_token_gate){
        p_list->push_back(PropertyInfo(Variant::INT, "token_gate_amount"));
        p_list->push_back(PropertyInfo(Variant::OBJECT, "token_gate_mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_token_payment"));
    if(enable_token_payment){
        p_list->push_back(PropertyInfo(Variant::INT, "token_payment_amount"));
        p_list->push_back(PropertyInfo(Variant::OBJECT, "token_payment_mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
        p_list->push_back(PropertyInfo(Variant::OBJECT, "token_payment_destination", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }
    if(top_level){
        p_list->push_back(PropertyInfo(Variant::ARRAY, "groups", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("CandyGuardAccessList")));
    }
    else{
        p_list->push_back(PropertyInfo(Variant::ARRAY, "groups", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("CandyGuardAccessList"), PROPERTY_USAGE_STORAGE));
    }
}

void CandyMachineData::_bind_methods(){
    ClassDB::bind_method(D_METHOD("set_token_standard", "value"), &CandyMachineData::set_token_standard);
    ClassDB::bind_method(D_METHOD("get_token_standard"), &CandyMachineData::get_token_standard);

    ClassDB::bind_method(D_METHOD("set_features", "value"), &CandyMachineData::set_features);
    ClassDB::bind_method(D_METHOD("get_features"), &CandyMachineData::get_features);

    ClassDB::bind_method(D_METHOD("set_authority", "value"), &CandyMachineData::set_authority);
    ClassDB::bind_method(D_METHOD("get_authority"), &CandyMachineData::get_authority);

    ClassDB::bind_method(D_METHOD("set_mint_authority", "value"), &CandyMachineData::set_mint_authority);
    ClassDB::bind_method(D_METHOD("get_mint_authority"), &CandyMachineData::get_mint_authority);

    ClassDB::bind_method(D_METHOD("set_collection_mint", "value"), &CandyMachineData::set_collection_mint);
    ClassDB::bind_method(D_METHOD("get_collection_mint"), &CandyMachineData::get_collection_mint);

    ClassDB::bind_method(D_METHOD("set_items_redeemed", "value"), &CandyMachineData::set_items_redeemed);
    ClassDB::bind_method(D_METHOD("get_items_redeemed"), &CandyMachineData::get_items_redeemed);

    ClassDB::bind_method(D_METHOD("set_items_available", "value"), &CandyMachineData::set_items_available);
    ClassDB::bind_method(D_METHOD("get_items_available"), &CandyMachineData::get_items_available);

    ClassDB::bind_method(D_METHOD("set_symbol", "value"), &CandyMachineData::set_symbol);
    ClassDB::bind_method(D_METHOD("get_symbol"), &CandyMachineData::get_symbol);

    ClassDB::bind_method(D_METHOD("set_seller_fee_basis_points", "value"), &CandyMachineData::set_seller_fee_basis_points);
    ClassDB::bind_method(D_METHOD("get_seller_fee_basis_points"), &CandyMachineData::get_seller_fee_basis_points);

    ClassDB::bind_method(D_METHOD("set_max_supply", "value"), &CandyMachineData::set_max_supply);
    ClassDB::bind_method(D_METHOD("get_max_supply"), &CandyMachineData::get_max_supply);

    ClassDB::bind_method(D_METHOD("set_is_mutable", "value"), &CandyMachineData::set_is_mutable);
    ClassDB::bind_method(D_METHOD("get_is_mutable"), &CandyMachineData::get_is_mutable);

    ClassDB::bind_method(D_METHOD("set_creators", "value"), &CandyMachineData::set_creators);
    ClassDB::bind_method(D_METHOD("get_creators"), &CandyMachineData::get_creators);

    ClassDB::bind_method(D_METHOD("set_prefix_name", "value"), &CandyMachineData::set_prefix_name);
    ClassDB::bind_method(D_METHOD("get_prefix_name"), &CandyMachineData::get_prefix_name);

    ClassDB::bind_method(D_METHOD("set_name_length", "value"), &CandyMachineData::set_name_length);
    ClassDB::bind_method(D_METHOD("get_name_length"), &CandyMachineData::get_name_length);

    ClassDB::bind_method(D_METHOD("set_prefix_uri", "value"), &CandyMachineData::set_prefix_uri);
    ClassDB::bind_method(D_METHOD("get_prefix_uri"), &CandyMachineData::get_prefix_uri);

    ClassDB::bind_method(D_METHOD("set_uri_length", "value"), &CandyMachineData::set_uri_length);
    ClassDB::bind_method(D_METHOD("get_uri_length"), &CandyMachineData::get_uri_length);

    ClassDB::bind_method(D_METHOD("set_is_sequential", "value"), &CandyMachineData::set_is_sequential);
    ClassDB::bind_method(D_METHOD("get_is_sequential"), &CandyMachineData::get_is_sequential);

    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "token_standard"), "set_token_standard", "get_token_standard");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "features"), "set_features", "get_features");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "authority"), "set_authority", "get_authority");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "mint_authority"), "set_mint_authority", "get_mint_authority");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "collection_mint"), "set_collection_mint", "get_collection_mint");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "items_redeemed"), "set_items_redeemed", "get_items_redeemed");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "items_available"), "set_items_available", "get_items_available");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::STRING, "symbol"), "set_symbol", "get_symbol");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "seller_fee_basis_points"), "set_seller_fee_basis_points", "get_seller_fee_basis_points");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "max_supply"), "set_max_supply", "get_max_supply");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::BOOL, "is_mutable"), "set_is_mutable", "get_is_mutable");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::ARRAY, "creators"), "set_creators", "get_creators");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::STRING, "prefix_name"), "set_prefix_name", "get_prefix_name");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "name_length"), "set_name_length", "get_name_length");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::STRING, "prefix_uri"), "set_prefix_uri", "get_prefix_uri");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "uri_length"), "set_uri_length", "get_uri_length");
    ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::BOOL, "is_sequential"), "set_is_sequential", "get_is_sequential");
}

void CandyMachineData::set_token_standard(const uint32_t value){
    token_standard = value;
}
uint32_t CandyMachineData::get_token_standard(){
    return token_standard;
}

void CandyMachineData::set_features(const PackedByteArray& value){
    features = value;
}
PackedByteArray CandyMachineData::get_features(){
    return features;
}

void CandyMachineData::set_authority(const Variant& value){
    authority = value;
}
Variant CandyMachineData::get_authority(){
    return authority;
}

void CandyMachineData::set_mint_authority(const Variant& value){
    mint_authority = value;
}
Variant CandyMachineData::get_mint_authority(){
    return mint_authority;
}

void CandyMachineData::set_collection_mint(const Variant& value){
    collection_mint = value;
}
Variant CandyMachineData::get_collection_mint(){
    return collection_mint;
}

void CandyMachineData::set_items_redeemed(const uint64_t value){
    items_redeemed = value;
}
uint64_t CandyMachineData::get_items_redeemed(){
    return items_redeemed;
}

void CandyMachineData::set_items_available(const uint64_t value){
    items_available = value;
}
uint64_t CandyMachineData::get_items_available(){
    return items_available;
}

void CandyMachineData::set_symbol(const String& value){
    symbol = value;
}
String CandyMachineData::get_symbol(){
    return symbol;
}

void CandyMachineData::set_seller_fee_basis_points(const uint32_t value){
    seller_fee_basis_points = value;
}
uint32_t CandyMachineData::get_seller_fee_basis_points(){
    return seller_fee_basis_points;
}

void CandyMachineData::set_max_supply(const uint64_t value){
    max_supply = value;
}
uint64_t CandyMachineData::get_max_supply(){
    return max_supply;
}

void CandyMachineData::set_is_mutable(const bool value){
    is_mutable = value;
}
bool CandyMachineData::get_is_mutable(){
    return is_mutable;
}

void CandyMachineData::set_creators(const Array &value){
    creators = value;
}
Array CandyMachineData::get_creators(){
    return creators;
}

void CandyMachineData::set_prefix_name(const String &value){
    prefix_name = value;
}
String CandyMachineData::get_prefix_name(){
    return prefix_name;
}

void CandyMachineData::set_name_length(const uint32_t value){
    name_length = value;
}
uint32_t CandyMachineData::get_name_length(){
    return name_length;
}

void CandyMachineData::set_prefix_uri(const String &value){
    prefix_uri = value;
}
String CandyMachineData::get_prefix_uri(){
    return prefix_uri;
}

void CandyMachineData::set_uri_length(const uint32_t value){
    uri_length = value;
}
uint32_t CandyMachineData::get_uri_length(){
    return uri_length;
}

void CandyMachineData::set_is_sequential(const bool value){
    is_sequential = value;
}
bool CandyMachineData::get_is_sequential(){
    return is_sequential;
}

PackedByteArray CandyMachineData::serialize(){
    const int CANDY_DATA_LENGTH = 8 + 4 + symbol.length() + 2 + 8 + 1 + 4 + 34 * creators.size() + 1 + (4 + prefix_name.length() + 4 + 4 + prefix_uri.length() + 4 + 1) * (int)(prefix_name.length() > 0) + 1;
    PackedByteArray result;
    result.resize(CANDY_DATA_LENGTH);

    result.encode_u64(0, items_available);
    result.encode_u32(8, symbol.length());
    for(unsigned int i = 0; i < symbol.length(); i++){
        result[12 + i] = symbol[i];
    }
    int cursor = 12 + symbol.length();
    result.encode_u16(cursor, seller_fee_basis_points);
    cursor += 2;
    result.encode_u64(cursor, max_supply);
    cursor += 8;
    result[cursor] = (int)is_mutable;
    cursor ++;
    result.encode_u32(cursor, creators.size());
    cursor += 4;
    for(unsigned int i = 0; i < creators.size(); i++){
        PackedByteArray creator_bytes = Object::cast_to<MetaDataCreator>(creators[i])->serialize();
        for(unsigned int j = 0; j < creator_bytes.size(); j++){
            result[cursor + j] = creator_bytes[j];
        }
        cursor += creator_bytes.size();
    }
    if(prefix_name.is_empty()){
        result[cursor] = 0;
        return result;
    }
    result[cursor] = 1;
    cursor++;

    result.encode_u32(cursor, prefix_name.length());
    cursor += 4;
    for(unsigned int i = 0; i < prefix_name.length(); i++){
        result[cursor + i] = prefix_name[i];
    }
    cursor += prefix_name.length();

    result.encode_u32(cursor, name_length);
    cursor += 4;
    result.encode_u32(cursor, prefix_uri.length());
    cursor += 4;
    for(unsigned int i = 0; i < prefix_uri.length(); i++){
        result[cursor + i] = prefix_uri[i];
    }
    cursor += prefix_uri.length();
    result.encode_u32(cursor, uri_length);
    cursor += 4;
    result[cursor] = (int)is_sequential;
    cursor ++;

    // No hidden settings.
    result[cursor] = 0;

    return result;
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

PackedByteArray MplCandyMachine::mint2_discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {120, 121, 23, 146, 173, 110, 199, 205};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

PackedByteArray MplCandyMachine::initialize_discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {175, 175, 109, 31, 13, 152, 155, 237};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

PackedByteArray MplCandyMachine::initialize2_discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {67, 153, 175, 39, 218, 16, 38, 32};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

void MplCandyMachine::_bind_methods(){
    ClassDB::bind_static_method("MplCandyMachine", D_METHOD("initialize", "authority", "candy_machine_account", "collection_mint", "candy_machine_data", "pnft=false"), &MplCandyMachine::initialize);
    ClassDB::bind_static_method("MplCandyMachine", D_METHOD("mint", "payer", "receiver", "mint", "collection_mint", "collection_update_authority", "candy_machine_key"), &MplCandyMachine::mint);

    ClassDB::bind_static_method("MplCandyMachine", D_METHOD("get_pid"), &MplCandyMachine::get_pid);
    ClassDB::bind_static_method("MplCandyMachine", D_METHOD("get_candy_machine_info", "candy_machine_key"), &MplCandyMachine::get_candy_machine_info);
}

Variant MplCandyMachine::initialize(const Variant &authority, const Variant &candy_machine_account, const Variant &collection_mint, const Variant &candy_machine_data, bool pnft){
    Instruction *result = memnew(Instruction);

    PackedByteArray data = initialize2_discriminator();
    data.append_array(Object::cast_to<CandyMachineData>(candy_machine_data)->serialize());


    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(candy_machine_account, false, true));

    Array seeds;
    seeds.append(String("candy_machine").to_ascii_buffer());
    seeds.append(Pubkey(candy_machine_account).to_bytes());

    Variant candy_machine_creator = Pubkey::new_pda_bytes(seeds, get_pid());

    result->append_meta(AccountMeta(candy_machine_creator, false, true));
    result->append_meta(AccountMeta(authority, false, false));
    result->append_meta(AccountMeta(authority, true, true));

    result->append_meta(AccountMeta(Pubkey::new_from_string("CndyV3LdqHUfDLmE5naZjVN8rBZz4tqhdefbAnjHG3JR"), false, false));

    result->append_meta(AccountMeta(MplTokenMetadata::new_associated_metadata_pubkey(collection_mint), false, false));
    result->append_meta(AccountMeta(collection_mint, false, false));
    result->append_meta(AccountMeta(MplTokenMetadata::new_associated_metadata_pubkey_master_edition(collection_mint), false, false));
    
    result->append_meta(AccountMeta(authority, true, true));
    result->append_meta(AccountMeta(TokenProgram::new_delegate_record_address(authority, collection_mint, candy_machine_creator, godot::TokenProgram::MetaDataDelegateRole::COLLECTION), false, true));

    result->append_meta(AccountMeta(MplTokenMetadata::get_pid(), false, false));
    result->append_meta(AccountMeta(SystemProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111"), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("CndyV3LdqHUfDLmE5naZjVN8rBZz4tqhdefbAnjHG3JR"), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("CndyV3LdqHUfDLmE5naZjVN8rBZz4tqhdefbAnjHG3JR"), false, false));

    return result;
}

Variant MplCandyMachine::mint(
    const Variant &payer,
    const Variant &receiver,
    const Variant &mint,
    const Variant &collection_mint,
    const Variant &collection_update_authority,
    const Variant &candy_machine_key
){
    Instruction *result = memnew(Instruction);

    PackedByteArray data = mint2_discriminator();

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    Variant candy_machine_creator = new_candy_machine_authority_pda(candy_machine_key);

    result->append_meta(AccountMeta(candy_machine_key, false, true));

    Array seeds;
    seeds.append(String("candy_machine").to_ascii_buffer());
    seeds.append(Pubkey(candy_machine_key).to_bytes());

    result->append_meta(AccountMeta(Pubkey::new_pda_bytes(seeds, get_pid()), false, true));
    result->append_meta(AccountMeta(payer, true, false));
    result->append_meta(AccountMeta(payer, true, true));
    result->append_meta(AccountMeta(receiver, false, false));
    result->append_meta(AccountMeta(mint, false, true));
    result->append_meta(AccountMeta(payer, true, false));
    result->append_meta(AccountMeta(Pubkey::new_associated_token_address(receiver, mint), false, true));
    result->append_meta(AccountMeta(TokenProgram::new_token_record_address(payer, mint), false, true));

    result->append_meta(AccountMeta(TokenProgram::new_delegate_record_address(collection_update_authority, mint, candy_machine_creator, TokenProgram::MetaDataDelegateRole::COLLECTION), false, false));


    result->append_meta(AccountMeta(collection_mint, false, false));
    result->append_meta(AccountMeta(MplTokenMetadata::new_associated_metadata_pubkey(collection_mint), false, true));
    result->append_meta(AccountMeta(MplTokenMetadata::new_associated_metadata_pubkey_master_edition(collection_mint), false, false));
    result->append_meta(AccountMeta(collection_update_authority, true, true));

    result->append_meta(AccountMeta(MplTokenMetadata::get_pid(), false, false));
    result->append_meta(AccountMeta(TokenProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(AssociatedTokenAccountProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(SystemProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111"), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("SysvarS1otHashes111111111111111111111111111"), false, false));

    return result;
}

Variant MplCandyMachine::new_candy_machine_authority_pda(const Variant& candy_machine_key){
    Array seeds;
    seeds.append(String("candy_machine").to_ascii_buffer());
    seeds.append(Pubkey(candy_machine_key).to_bytes());

    return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant MplCandyMachine::get_candy_machine_info(const Variant& candy_machine_key){
    int cursor = 9;

    SolanaClient temp_client;
    temp_client.set_async(false);
    Dictionary rpc_result = temp_client.get_account_info(Pubkey(candy_machine_key).to_string());

    if(!rpc_result.has("result")){
        return nullptr;
    }

    Dictionary result_dict = rpc_result["result"];
    Variant value;
    if(!result_dict.has("value")){
        return nullptr;
    }
    value = result_dict["value"];
    if(value.get_type() != Variant::DICTIONARY){
        return nullptr;
    }

    Dictionary account = value;

    Array account_data_tuple = account["data"];
    String encoded_data = account_data_tuple[0];

    PackedByteArray account_data = SolanaUtils::bs64_decode(encoded_data);
    CandyMachineData *res = memnew(CandyMachineData);
    res->set_token_standard(account_data[cursor]);
    cursor++;
    PackedByteArray features;
    for(int i = 0; i < 6; i++){
        features.append(account_data[cursor]);
        cursor++;
    }
    res->set_features(features);

    res->set_authority(Pubkey::new_from_bytes(account_data.slice(cursor, cursor + 32)));
    cursor += 32;
    res->set_mint_authority(Pubkey::new_from_bytes(account_data.slice(cursor, cursor + 32)));
    cursor += 32;
    res->set_collection_mint(Pubkey::new_from_bytes(account_data.slice(cursor, cursor + 32)));
    cursor += 32;

    res->set_items_redeemed(account_data.decode_u64(cursor));
    cursor += 8;
    res->set_items_available(account_data.decode_u64(cursor));
    cursor += 8;

    int name_length = account_data.decode_u32(cursor);
    cursor += 4;
    res->set_symbol(account_data.slice(cursor, cursor + name_length).get_string_from_ascii());
    cursor += name_length;

    res->set_seller_fee_basis_points(account_data.decode_u16(cursor));
    cursor += 2;
    res->set_max_supply(account_data.decode_u64(cursor));
    cursor += 8;
    res->set_is_mutable(account_data[cursor] == 1);
    cursor ++;
    uint32_t creators_length = account_data.decode_u32(cursor);
    cursor += 4;
    Array read_creators;
    for(int i = 0; i < creators_length; i++){
        MetaDataCreator* creator = memnew(MetaDataCreator);
        creator->set_address(Pubkey::new_from_bytes(account_data.slice(cursor, cursor + 32)));
        cursor += 32;
        creator->set_verified(account_data[cursor] == 1);
        cursor++;
        creator->set_share(account_data[cursor]);
        cursor += 1;
        read_creators.append(creator);
    }
    
    res->set_creators(read_creators);

    if(account_data[cursor] != 1){
        return res;
    }
    cursor++;

    int prefix_length = account_data.decode_u32(cursor);
    
    cursor += 4;
    res->set_prefix_name(account_data.slice(cursor, cursor + prefix_length).get_string_from_ascii());

    cursor += prefix_length;

    res->set_name_length(account_data.decode_u32(cursor));
    cursor += 4;

    int prefix_uri_length = account_data.decode_u32(cursor);
    cursor += 4;
    res->set_prefix_uri(account_data.slice(cursor, cursor + prefix_uri_length).get_string_from_ascii());
    cursor += prefix_uri_length;

    res->set_uri_length(account_data.decode_u32(cursor));
    cursor += 4;

    res->set_is_sequential(account_data[cursor] == 1);

    return res;
}

Variant MplCandyMachine::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}

PackedByteArray MplCandyGuard::serialize_label(const String& label){
    const int MAX_LABEL_SIZE = 6;

    PackedByteArray result;
    
    if(label == "default"){
        result.append(0);
        return result;
    }
    result.resize(5);
    result[0] = 1;
    result.encode_u32(1, MAX_LABEL_SIZE);
    
    PackedByteArray fixed_size_data = label.to_ascii_buffer();
    
    fixed_size_data.resize(MAX_LABEL_SIZE);
    result.append_array(fixed_size_data);

    return result;
}

void MplCandyGuard::_bind_methods(){
    ClassDB::bind_static_method("MplCandyGuard", D_METHOD("initialize", "owner", "authority", "payer", "candy_guard_acl"), &MplCandyGuard::initialize);
    ClassDB::bind_static_method("MplCandyGuard", D_METHOD("mint", "candy_machine_id", "candy_guard_id", "payer", "receiver", "mint", "nft_mint_authority", "collection_mint", "collection_update_authority", "candy_guard_acl", "label"), &MplCandyGuard::mint);

    ClassDB::bind_static_method("MplCandyGuard", D_METHOD("get_pid"), &MplCandyGuard::get_pid);
}

Variant MplCandyGuard::initialize(const Variant& owner, const Variant& candy_guard_authority, const Variant& payer, const Variant &candy_guard_acl){
    Instruction *result = memnew(Instruction);

    PackedByteArray data = MplCandyMachine::initialize_discriminator();
    data.append_array(Object::cast_to<CandyGuardAccessList>(candy_guard_acl)->serialize());

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);
    result->append_meta(AccountMeta(MplCandyMachine::new_candy_machine_authority_pda(owner), false, true));
    result->append_meta(AccountMeta(owner, true, false));

    result->append_meta(AccountMeta(candy_guard_authority, false, false));
    result->append_meta(AccountMeta(payer, true, false));
    result->append_meta(AccountMeta(SystemProgram::get_pid(), false, false));

    return result;
}

Variant MplCandyGuard::mint(
    const Variant &candy_machine_id,
    const Variant &guard_account_id,
    const Variant &payer,
    const Variant &receiver,
    const Variant &mint,
    const Variant &nft_mint_authority,
    const Variant &collection_mint,
    const Variant &collection_update_authority,
    const Variant &candy_guard_acl,
    const String &label
){
    Instruction *result = memnew(Instruction);

    PackedByteArray data = MplCandyMachine::mint2_discriminator();
    data.append_array(Object::cast_to<CandyGuardAccessList>(candy_guard_acl)->get_group(label).serialize_mint_settings());
    data.append_array(serialize_label(label));
    
    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    const Variant candy_machine_creator = MplCandyMachine::new_candy_machine_authority_pda(candy_machine_id);

    const Variant associated_token_account = Pubkey::new_associated_token_address(payer, mint);

    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(guard_account_id, false, false));
    result->append_meta(AccountMeta(MplCandyMachine::get_pid(), false, false));
    result->append_meta(AccountMeta(candy_machine_id, false, true));
    result->append_meta(AccountMeta(MplCandyMachine::new_candy_machine_authority_pda(candy_machine_id), false, true));

    result->append_meta(AccountMeta(payer, true, true));
    result->append_meta(AccountMeta(receiver, true, true));
    result->append_meta(AccountMeta(mint, true, true));
    result->append_meta(AccountMeta(nft_mint_authority, true, false));
    result->append_meta(AccountMeta(MplTokenMetadata::new_associated_metadata_pubkey(mint), false, true));
    result->append_meta(AccountMeta(MplTokenMetadata::new_associated_metadata_pubkey_master_edition(mint), false, true));

    result->append_meta(AccountMeta(Pubkey::new_associated_token_address(receiver, mint), false, true));
    result->append_meta(AccountMeta(TokenProgram::new_token_record_address(associated_token_account, mint), false, true));

    result->append_meta(AccountMeta(TokenProgram::new_delegate_record_address(collection_update_authority, collection_mint, candy_machine_creator, TokenProgram::MetaDataDelegateRole::COLLECTION), false, false));

    result->append_meta(AccountMeta(collection_mint, false, false));
    result->append_meta(AccountMeta(MplTokenMetadata::new_associated_metadata_pubkey(collection_mint), false, true));
    result->append_meta(AccountMeta(MplTokenMetadata::new_associated_metadata_pubkey_master_edition(collection_mint), false, false));
    result->append_meta(AccountMeta(collection_update_authority, false, false));

    result->append_meta(AccountMeta(MplTokenMetadata::get_pid(), false, false));
    result->append_meta(AccountMeta(TokenProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(AssociatedTokenAccountProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(SystemProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111"), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("SysvarS1otHashes111111111111111111111111111"), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("Guard1JwRhJkVH6XZhzoYxeBVQe872VH6QggF4BWmS9g"), false, false));
    result->append_meta(AccountMeta(Pubkey::new_from_string("Guard1JwRhJkVH6XZhzoYxeBVQe872VH6QggF4BWmS9g"), false, false));
    
    TypedArray<AccountMeta> mint_arg_accounts = Object::cast_to<CandyGuardAccessList>(candy_guard_acl)->get_group(label).get_mint_arg_accounts(receiver);
    
    for(unsigned int i = 0; i < mint_arg_accounts.size(); i++){
        result->append_meta(mint_arg_accounts[i]);
    }

    return result;
}

Variant MplCandyGuard::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}

Variant MplCandyGuard::new_associated_candy_guard_key(const Variant &candy_machine_key){
    Array seeds;
    seeds.append(String("candy_guard").to_ascii_buffer());
    seeds.append(Pubkey(candy_machine_key).to_bytes());
    return Pubkey::new_pda_bytes(seeds, get_pid());
}

}