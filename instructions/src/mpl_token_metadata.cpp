#include "mpl_token_metadata.hpp"

#include "system_program.hpp"
#include "spl_token.hpp"
#include "pubkey.hpp"
#include "solana_client.hpp"
#include "solana_sdk.hpp"

namespace godot{

const std::string MplTokenMetadata::ID = "metaqbxxUerdq28cj1RbAWkYQm3ybzjb6a8bt518x1s";

void MplTokenMetadata::_bind_methods(){
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("new_associated_metadata_pubkey", "mint"), &MplTokenMetadata::new_associated_metadata_pubkey);
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("new_associated_metadata_pubkey_master_edition", "mint"), &MplTokenMetadata::new_associated_metadata_pubkey_master_edition);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("get_mint_metadata", "mint"), &MplTokenMetadata::get_mint_metadata);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("create_metadata_account", "account_pubkey", "mint", "mint_authority", "payer", "update_authority", "meta_data", "is_mutable", "collection_size"), &MplTokenMetadata::create_metadata_account);
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("create_master_edition", "edition", "mint", "update_authority", "mint_authority", "metadata_account", "payer", "max_supply"), &MplTokenMetadata::create_master_edition);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("get_pid"), &MplTokenMetadata::get_pid);
}

Variant MplTokenMetadata::new_associated_metadata_pubkey(const Variant& mint){
    TypedArray<PackedByteArray> arr;

    arr.append(String("metadata").to_ascii_buffer());
    arr.append(Pubkey(get_pid()).get_bytes());
    arr.append(Pubkey(mint).get_bytes());

    arr.append(PackedByteArray());
    
    Pubkey *res = memnew(Pubkey);
    for(uint8_t i = 255; i > 0; i--){
        PackedByteArray bump_seed;
        bump_seed.push_back(i);
        arr[arr.size() - 1] = bump_seed;
        if(res->create_program_address_bytes(arr, get_pid())){
            return res;
        }
    }
    
    internal::gdextension_interface_print_warning("y points were not valid", "new_associated_token_address", __FILE__, __LINE__, false);
    return nullptr;
}

Variant MplTokenMetadata::new_associated_metadata_pubkey_master_edition(const Variant& mint){
    TypedArray<PackedByteArray> arr;

    arr.append(String("metadata").to_ascii_buffer());
    arr.append(Pubkey(get_pid()).get_bytes());
    arr.append(Pubkey(mint).get_bytes());
    arr.append(String("edition").to_ascii_buffer());

    arr.append(PackedByteArray());
    
    Pubkey *res = memnew(Pubkey);
    for(uint8_t i = 255; i > 0; i--){
        PackedByteArray bump_seed;
        bump_seed.push_back(i);
        arr[arr.size() - 1] = bump_seed;
        if(res->create_program_address_bytes(arr, get_pid())){
            return res;
        }
    }
    
    internal::gdextension_interface_print_warning("y points were not valid", "new_associated_token_address", __FILE__, __LINE__, false);
    return nullptr;
}

Variant MplTokenMetadata::get_mint_metadata(const Variant& mint){
    Variant metadata_account = new_associated_metadata_pubkey(mint);

    Dictionary rpc_result = SolanaClient::get_account_info(Pubkey(metadata_account).get_value());
    if(!rpc_result.has("result")){
        return nullptr;
    }

    Variant value = ((Dictionary)rpc_result["result"])["value"];
    if(value.get_type() != Variant::DICTIONARY){
        return nullptr;
    }

    Dictionary account = value;

    Array account_data_tuple = account["data"];
    String encoded_data = account_data_tuple[0];

    PackedByteArray account_data = SolanaSDK::bs64_decode(encoded_data);

    const int NAME_LOCATION = 65;
    const int SYMBOL_LOCATION = 101;
    const int URI_LOCATION = 115;
    const int SELLER_FEE_BASIS_POINT_LOCATION = 319;

    MetaData *result = memnew(MetaData);

    const uint32_t name_length = account_data.decode_u32(NAME_LOCATION);
    const PackedByteArray parsed_name =  account_data.slice(NAME_LOCATION + 4, NAME_LOCATION + 4 + name_length);
    const uint32_t real_name_length = parsed_name.find(0);
    if(real_name_length > 0){
        result->set_token_name(parsed_name.slice(0, real_name_length).get_string_from_ascii());
    }
    else{
        result->set_token_name(parsed_name.get_string_from_ascii());
    }
    
    const uint32_t symbol_length = account_data.decode_u32(SYMBOL_LOCATION);
    const PackedByteArray parsed_symbol =  account_data.slice(SYMBOL_LOCATION + 4, SYMBOL_LOCATION + 4 + symbol_length);
    const uint32_t real_symbol_length = parsed_symbol.find(0);
    if(real_symbol_length > 0){
        result->set_symbol(parsed_symbol.slice(0, real_symbol_length).get_string_from_ascii());
    }
    else{
        result->set_symbol(parsed_symbol.get_string_from_ascii());
    }
    
    const uint32_t uri_length = account_data.decode_u32(URI_LOCATION);
    const PackedByteArray parsed_uri =  account_data.slice(URI_LOCATION + 4, URI_LOCATION + 4 + uri_length);
    const uint32_t real_uri_length = parsed_uri.find(0);
    if(real_uri_length > 0){
        result->set_uri(parsed_uri.slice(0, real_uri_length).get_string_from_ascii());
    }
    else{
        result->set_uri(parsed_uri.get_string_from_ascii());
    }

    result->set_seller_fee_basis_points(account_data.decode_u16(SELLER_FEE_BASIS_POINT_LOCATION));

    return result;
}

Variant MplTokenMetadata::create_metadata_account(const Variant& account_pubkey, const Variant& mint, const Variant& mint_authority, const Variant& payer, const Variant& update_authority, const Variant &meta_data, bool is_mutable, uint64_t collection_size){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;

    const MetaData *data_ptr = Object::cast_to<MetaData>(meta_data);
    PackedByteArray serialized_meta_data = data_ptr->serialize(is_mutable);

    data.append(33);
    data.append_array(serialized_meta_data);

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(account_pubkey, false, true));
    result->append_meta(AccountMeta(mint, false, false));
    result->append_meta(AccountMeta(mint_authority, true, false));
    result->append_meta(AccountMeta(update_authority, true, false));
    result->append_meta(AccountMeta(SystemProgram::get_pid(), false, false));

    Pubkey *rent = memnew(Pubkey);

    // TODO(Virax): Create an easier way to get the sysvarRent.
    rent->set_value("SysvarRent111111111111111111111111111111111");
    result->append_meta(AccountMeta(rent, false, false));

    return result;
}

Variant MplTokenMetadata::create_master_edition(const Variant& edition, const Variant& mint, const Variant& update_authority, const Variant& mint_authority, const Variant& metadata_account, const Variant &payer, const Variant &max_supply){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;

    if(max_supply.get_type() != Variant::INT){
        data.resize(2);
        data[1] = 0;
    }
    else{
        data.resize(10);
        data[1] = 1;
        data.encode_u64(2, max_supply);
    }

    data[0] = 17;

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(edition, false, true));
    result->append_meta(AccountMeta(mint, false, true));
    result->append_meta(AccountMeta(update_authority, true, false));
    result->append_meta(AccountMeta(mint_authority, true, false));
    result->append_meta(AccountMeta(payer, true, true));
    result->append_meta(AccountMeta(TokenProgram::get_pid(), false, false));
    result->append_meta(AccountMeta(SystemProgram::get_pid(), false, false));

    Pubkey *rent = memnew(Pubkey);

    // TODO(Virax): Create an easier way to get the sysvarRent.
    rent->set_value("SysvarRent111111111111111111111111111111111");
    result->append_meta(AccountMeta(rent, false, false));

    return result;
}

Variant MplTokenMetadata::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}

}