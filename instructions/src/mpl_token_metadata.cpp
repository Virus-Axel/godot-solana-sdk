#include "mpl_token_metadata.hpp"

#include "system_program.hpp"
#include "spl_token.hpp"
#include "pubkey.hpp"
#include "solana_client.hpp"
#include "solana_utils.hpp"

namespace godot{

const std::string MplTokenMetadata::ID = "metaqbxxUerdq28cj1RbAWkYQm3ybzjb6a8bt518x1s";

MplTokenMetadata::MplTokenMetadata(){
    metadata_client = memnew(SolanaClient);
    metadata_client->set_async_override(true);
}

void MplTokenMetadata::_process(double delta){
    if(pending_fetch){
        metadata_client->_process(delta);
    }
}

void MplTokenMetadata::_bind_methods(){
    ClassDB::add_signal("MplTokenMetadata", MethodInfo("metadata_fetched", PropertyInfo(Variant::OBJECT, "metadata")));

    ClassDB::bind_method(D_METHOD("set_url_override", "url_override"), &MplTokenMetadata::set_url_override);
    ClassDB::bind_method(D_METHOD("get_url_override"), &MplTokenMetadata::get_url_override);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("new_associated_metadata_pubkey", "mint"), &MplTokenMetadata::new_associated_metadata_pubkey);
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("new_associated_metadata_pubkey_master_edition", "mint"), &MplTokenMetadata::new_associated_metadata_pubkey_master_edition);

    ClassDB::bind_method(D_METHOD("get_mint_metadata", "mint"), &MplTokenMetadata::get_mint_metadata);
    ClassDB::bind_method(D_METHOD("metadata_callback", "rpc_response"), &MplTokenMetadata::metadata_callback);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("create_metadata_account", "account_pubkey", "mint", "mint_authority", "payer", "update_authority", "meta_data", "is_mutable", "collection_size"), &MplTokenMetadata::create_metadata_account);
    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("create_master_edition", "edition", "mint", "update_authority", "mint_authority", "metadata_account", "payer", "max_supply"), &MplTokenMetadata::create_master_edition);

    ClassDB::bind_static_method("MplTokenMetadata", D_METHOD("get_pid"), &MplTokenMetadata::get_pid);

    ClassDB::add_property("MplTokenMetadata", PropertyInfo(Variant::STRING, "url_override", PROPERTY_HINT_NONE), "set_url_override", "get_url_override");
}

Variant MplTokenMetadata::new_associated_metadata_pubkey(const Variant& mint){
    TypedArray<PackedByteArray> arr;

    arr.append(String("metadata").to_ascii_buffer());
    arr.append(Pubkey(get_pid()).to_bytes());
    arr.append(Pubkey(mint).to_bytes());

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
    arr.append(Pubkey(get_pid()).to_bytes());
    arr.append(Pubkey(mint).to_bytes());
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
    metadata_client->connect("http_response_received", callback, ConnectFlags::CONNECT_ONE_SHOT);
    Dictionary rpc_result = metadata_client->get_account_info(Pubkey(metadata_account).to_string());

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

    const uint32_t name_length = account_data.decode_u32(NAME_LOCATION);
    const PackedByteArray parsed_name =  account_data.slice(NAME_LOCATION + 4, NAME_LOCATION + 4 + name_length);
    const uint32_t real_name_length = parsed_name.find(0);

    const int SYMBOL_LOCATION = NAME_LOCATION + name_length + 4;

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
    
    const int URI_LOCATION = SYMBOL_LOCATION + symbol_length + 4;

    const uint32_t uri_length = account_data.decode_u32(URI_LOCATION);
    const PackedByteArray parsed_uri =  account_data.slice(URI_LOCATION + 4, URI_LOCATION + 4 + uri_length);
    const uint32_t real_uri_length = parsed_uri.find(0);
    if(real_uri_length > 0){
        result->set_uri(parsed_uri.slice(0, real_uri_length).get_string_from_ascii());
    }
    else{
        result->set_uri(parsed_uri.get_string_from_ascii());
    }

    const int SELLER_FEE_BASIS_POINT_LOCATION = URI_LOCATION + uri_length + 4;

    result->set_seller_fee_basis_points(account_data.decode_u16(SELLER_FEE_BASIS_POINT_LOCATION));

    const uint32_t CREATOR_LOCATION = SELLER_FEE_BASIS_POINT_LOCATION + 2;

    uint32_t collection_location = CREATOR_LOCATION + 1;

    // Check for creators.
    if(account_data[CREATOR_LOCATION] == 1){
        uint32_t creator_amount = account_data.decode_u32(CREATOR_LOCATION + 1);
        for(int i = 0; i < creator_amount; i++){
            MetaDataCreator * creator = memnew(MetaDataCreator);

            Variant creator_address = Pubkey::new_from_bytes(account_data.slice(CREATOR_LOCATION + 5 + 34 * i, CREATOR_LOCATION + 37 + 34 * i));
            creator->set_address(creator_address);
            creator->set_verified(account_data[CREATOR_LOCATION + 37 + 34 * i] == 1);
            creator->set_share(account_data[CREATOR_LOCATION + 38 + 34 * i]);

            result->add_creator(creator);
        }
        collection_location += creator_amount * 34 + 4;
    }

    collection_location += 6;
    // Unknown 0
    // primary sale happened
    // is mutable
    // edition nonce
    // token standard two bytes?

    // Check collection data.
    if(account_data[collection_location] == 1){
        MetaDataCollection * collection = memnew(MetaDataCollection);

        Variant collection_address = Pubkey::new_from_bytes(account_data.slice(collection_location + 2, collection_location + 34));
        collection->set_key(collection_address);
        collection->set_verified(account_data[collection_location + 1] == 1);

        result->set_collection(collection);
    }

    // TODO(Virax): Parse uses data.

    emit_signal("metadata_fetched", result);
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
    rent->from_string("SysvarRent111111111111111111111111111111111");
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
    rent->from_string("SysvarRent111111111111111111111111111111111");
    result->append_meta(AccountMeta(rent, false, false));

    return result;
}

Variant MplTokenMetadata::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}

void MplTokenMetadata::set_url_override(const String& url_override){
    metadata_client->set_url_override(url_override);
}

String MplTokenMetadata::get_url_override(){
    return metadata_client->get_url_override();
}

}