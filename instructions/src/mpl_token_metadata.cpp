#include "mpl_token_metadata.hpp"

#include "system_program.hpp"
#include "spl_token.hpp"
#include "pubkey.hpp"

namespace godot{

const std::string MplTokenMetadata::ID = "metaqbxxUerdq28cj1RbAWkYQm3ybzjb6a8bt518x1s";

void MplTokenMetadata::_bind_methods(){
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("new_associated_metadata_pubkey", "mint"), &MplTokenMetadata::new_associated_metadata_pubkey);
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("new_associated_metadata_pubkey_master_edition", "mint"), &MplTokenMetadata::new_associated_metadata_pubkey_master_edition);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("create_metadata_account", "account_pubkey", "mint", "mint_authority", "payer", "update_authority", "meta_data", "is_mutable", "collection_size"), &MplTokenMetadata::create_metadata_account);
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("create_master_edition", "edition", "mint", "update_authority", "mint_authority", "metadata_account", "payer", "max_supply"), &MplTokenMetadata::create_master_edition);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("get_pid", "mint"), &MplTokenMetadata::get_pid);
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