#include "mpl_token_metadata.hpp"

#include "system_program.hpp"
#include "spl_token.hpp"
#include "pubkey.hpp"
#include "solana_client.hpp"
#include "solana_utils.hpp"
#include "godot_cpp/classes/os.hpp"
#include "meta_data/collection.hpp"
#include "meta_data/create_metadata_args.hpp"

namespace godot{

const std::string MplTokenMetadata::ID = "metaqbxxUerdq28cj1RbAWkYQm3ybzjb6a8bt518x1s";

MplTokenMetadata::MplTokenMetadata(){
    set_async_override(true);
    Array params;
    params.push_back("");
    Callable(this, "emit_signal").bindv(params);
}

void MplTokenMetadata::_bind_methods(){
    ClassDB::add_signal("MplTokenMetadata", MethodInfo("metadata_fetched", PropertyInfo(Variant::OBJECT, "metadata")));
    
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("new_associated_metadata_pubkey", "mint"), &MplTokenMetadata::new_associated_metadata_pubkey);
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("new_associated_metadata_pubkey_master_edition", "mint"), &MplTokenMetadata::new_associated_metadata_pubkey_master_edition);

    ClassDB::bind_method(D_METHOD("get_mint_metadata", "mint"), &MplTokenMetadata::get_mint_metadata);
    ClassDB::bind_method(D_METHOD("metadata_callback", "rpc_response"), &MplTokenMetadata::metadata_callback);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("create_metadata_account", "mint", "mint_authority", "update_authority", "meta_data", "is_mutable"), &MplTokenMetadata::create_metadata_account);
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("update_metadata_account", "metadata_account", "update_authority"), &MplTokenMetadata::update_metadata_account);
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("create_master_edition", "mint", "update_authority", "mint_authority", "payer", "max_supply"), &MplTokenMetadata::create_master_edition);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("get_pid"), &MplTokenMetadata::get_pid);
}

Variant MplTokenMetadata::new_associated_metadata_pubkey(const Variant& mint){
    TypedArray<PackedByteArray> arr;

    arr.append(String("metadata").to_ascii_buffer());
    arr.append(Pubkey::bytes_from_variant(get_pid()));
    arr.append(Pubkey::bytes_from_variant(mint));

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

    memfree(res);
    internal::gdextension_interface_print_warning("y points were not valid", "new_associated_token_address", __FILE__, __LINE__, false);
    return nullptr;
}

Variant MplTokenMetadata::new_associated_metadata_pubkey_master_edition(const Variant& mint){
    TypedArray<PackedByteArray> arr;

    arr.append(String("metadata").to_ascii_buffer());
    arr.append(Pubkey::bytes_from_variant(get_pid()));
    arr.append(Pubkey::bytes_from_variant(mint));
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
    if(pending_fetch){
        return ERR_UNAVAILABLE;
    }
    pending_fetch = true;
    Variant metadata_account = new_associated_metadata_pubkey(mint);

    Callable callback(this, "metadata_callback");
    connect("http_response_received", callback, ConnectFlags::CONNECT_ONE_SHOT);
    get_account_info(Pubkey::string_from_variant(metadata_account));

    return OK;
}

void MplTokenMetadata::metadata_callback(const Dictionary& rpc_result){
    pending_fetch = false;
    if(!rpc_result.has("result")){
        emit_signal("metadata_fetched", nullptr);
        return;
    }

    Variant value = ((Dictionary)rpc_result["result"])["value"];
    if(value.get_type() != Variant::DICTIONARY){
        emit_signal("metadata_fetched", nullptr);
        return;
    }

    Dictionary account = value;

    Array account_data_tuple = account["data"];
    String encoded_data = account_data_tuple[0];

    PackedByteArray account_data = SolanaUtils::bs64_decode(encoded_data);

    const int NAME_LOCATION = 65;

    MetaData *result = memnew(MetaData);

    int c = 1;
    result->set_update_authority(Pubkey::new_from_bytes(account_data.slice(c, c + 32)));
    c += 32;
    result->set_mint(Pubkey::new_from_bytes(account_data.slice(c, c + 32)));
    c += 32;

    const uint32_t name_length = account_data.decode_u32(c);
    c += 4;
    const PackedByteArray parsed_name =  account_data.slice(c, c + name_length);
    const uint32_t real_name_length = parsed_name.find(0);
    c += name_length;

    if(real_name_length > 0){
        result->set_token_name(parsed_name.slice(0, real_name_length).get_string_from_ascii());
    }
    else{
        result->set_token_name(parsed_name.get_string_from_ascii());
    }
    
    const uint32_t symbol_length = account_data.decode_u32(c);
    c += 4;
    const PackedByteArray parsed_symbol =  account_data.slice(c, c + symbol_length);
    const uint32_t real_symbol_length = parsed_symbol.find(0);
    if(real_symbol_length > 0){
        result->set_symbol(parsed_symbol.slice(0, real_symbol_length).get_string_from_ascii());
    }
    else{
        result->set_symbol(parsed_symbol.get_string_from_ascii());
    }
    
    c += symbol_length;

    const uint32_t uri_length = account_data.decode_u32(c);
    c += 4;
    const PackedByteArray parsed_uri =  account_data.slice(c, c + uri_length);
    c += uri_length;
    const uint32_t real_uri_length = parsed_uri.find(0);
    if(real_uri_length > 0){
        result->set_uri(parsed_uri.slice(0, real_uri_length).get_string_from_ascii());
    }
    else{
        result->set_uri(parsed_uri.get_string_from_ascii());
    }

    result->set_seller_fee_basis_points(account_data.decode_u16(c));
    c += 2;

    // Check for creators.
    if(account_data[c++] == 1){
        uint32_t creator_amount = account_data.decode_u32(c);
        c += 4;
        for(int i = 0; i < creator_amount; i++){
            MetaDataCreator * creator = memnew(MetaDataCreator);

            Variant creator_address = Pubkey::new_from_bytes(account_data.slice(c, c + 32));
            creator->set_address(creator_address);
            c += 32;

            creator->set_verified(account_data[c] == 1);
            c++;
            creator->set_share(account_data[c]);
            c++;

            result->add_creator(creator);
        }
    }

    result->set_primary_sale_happened(account_data[c] == 1);
    c++;
    result->set_is_mutable(account_data[c] == 1);
    c++;

    if(account_data[c]){
        c++;
        result->set_edition_nonce(account_data[c]);
    }
    c++;
    if(account_data[c]){
        c++;
        result->set_token_standard(account_data[c]);
    }
    c++;

    // Check collection data.
    if(account_data[c++] == 1){
        MetaDataCollection * collection = memnew(MetaDataCollection);

        collection->set_verified(account_data[c] == 1);
        c++;
        Variant collection_address = Pubkey::new_from_bytes(account_data.slice(c, c + 32));
        collection->set_key(collection_address);

        result->set_collection(collection);
    }

    // TODO(Virax): Parse uses data.

    emit_signal("metadata_fetched", result);
}

Variant MplTokenMetadata::create_metadata_account(const Variant& mint, const Variant& mint_authority, const Variant& update_authority, const Variant &meta_data, bool is_mutable){
    ERR_FAIL_COND_V_EDMSG(meta_data.get_type() != Variant::OBJECT, nullptr, "metadata must be a CreateMetadataArgs type.");
    ERR_FAIL_COND_V_EDMSG(((Object*)meta_data)->get_class() != "CreateMetaDataArgs", nullptr, "metadata must be a CreateMetadataArgs type.");

    Instruction *result = memnew(Instruction);
    PackedByteArray data;

    const CreateMetaDataArgs *data_ptr = Object::cast_to<CreateMetaDataArgs>(meta_data);
    PackedByteArray serialized_meta_data = data_ptr->serialize(is_mutable);

    data.append(33);
    data.append_array(serialized_meta_data);

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    const Variant metadata_ata = MplTokenMetadata::new_associated_metadata_pubkey(mint);
    const Variant pid_key = SystemProgram::get_pid();
    result->append_meta(*memnew(AccountMeta(metadata_ata, false, true)));
    result->append_meta(*memnew(AccountMeta(mint, false, false)));
    result->append_meta(*memnew(AccountMeta(mint_authority, true, false)));
    result->append_meta(*memnew(AccountMeta(update_authority, true, true)));
    result->append_meta(*memnew(AccountMeta(update_authority, false, false)));
    result->append_meta(*memnew(AccountMeta(pid_key, false, false)));

    Pubkey *rent = memnew(Pubkey);

    // TODO(Virax): Create an easier way to get the sysvarRent.
    rent->from_string("SysvarRent111111111111111111111111111111111");
    result->append_meta(*memnew(AccountMeta(rent, false, false)));

    return result;
}

Variant MplTokenMetadata::update_metadata_account(const Variant& metadata_account, const Variant& update_authority){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;

    data.append(15);
    data.append(0);
    data.append(1);
    data.append_array(Pubkey::bytes_from_variant(update_authority));
    data.append(0);
    data.append(0);

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(*memnew(AccountMeta(metadata_account, false, true)));
    result->append_meta(*memnew(AccountMeta(update_authority, true, false)));

    return result;
}

Variant MplTokenMetadata::create_master_edition(const Variant& mint, const Variant& update_authority, const Variant& mint_authority, const Variant &payer, const Variant &max_supply){
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

    const Variant mint_master_edition = MplTokenMetadata::new_associated_metadata_pubkey_master_edition(mint);
    const Variant mint_metadata = MplTokenMetadata::new_associated_metadata_pubkey(mint);
    result->append_meta(*memnew(AccountMeta(mint_master_edition, false, true)));
    result->append_meta(*memnew(AccountMeta(mint, false, true)));

    result->append_meta(*memnew(AccountMeta(update_authority, false, false)));
    result->append_meta(*memnew(AccountMeta(mint_authority, true, false)));
    result->append_meta(*memnew(AccountMeta(payer, true, true)));
    
    result->append_meta(*memnew(AccountMeta(mint_metadata, false, true)));

    const Variant token_pid = TokenProgram::get_pid();
    const Variant system_pid = SystemProgram::get_pid();
    const Variant sysvar_rent = Pubkey::new_from_string("SysvarRent111111111111111111111111111111111");
    result->append_meta(*memnew(AccountMeta(token_pid, false, false)));
    result->append_meta(*memnew(AccountMeta(system_pid, false, false)));
    result->append_meta(*memnew(AccountMeta(sysvar_rent, false, false)));

    return result;
}

Variant MplTokenMetadata::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}

}