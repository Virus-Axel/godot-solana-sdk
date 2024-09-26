#include "shdwdrive.hpp"

#include "solana_client.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <pubkey.hpp>
#include <keypair.hpp>

namespace godot{

const std::string ShdwDrive::ID = "metaqbxxUerdq28cj1RbAWkYQm3ybzjb6a8bt518x1s";

PackedByteArray StorageAccountV2::discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {133, 53, 253, 82, 212, 5, 201, 218};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

void StorageAccountV2::from_bytes(const PackedByteArray& bytes){
    ERR_FAIL_COND_EDMSG(bytes.slice(0, 8) == discriminator(), "Account is not StorageAccountV2.");

    int c = 8;
    immutable = bytes[c++];
    to_be_deleted = bytes[c++];
    delete_request_epoch = bytes.decode_u32(c);
    c += 4;
    storage = bytes.decode_u64(c);
    c += 8;
    owner1 = Pubkey::new_from_bytes(bytes.slice(c, c + 32));
    c += 32;
    account_counter_seed = bytes.decode_u32(c);
    c += 4;
    creation_time = bytes.decode_u32(c);
    c += 4;
    creation_epoch = bytes.decode_u32(c);
    c += 4;
    last_fee_epoch = bytes.decode_u32(c);
    c += 4;
    uint32_t identifier_size = bytes.decode_u32(c);
    c += 4;
    identifier = bytes.slice(c, c + identifier_size).get_string_from_ascii();
}

PackedByteArray UserInfo::discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {83, 134, 200, 56, 144, 56, 10, 62};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

void UserInfo::from_bytes(const PackedByteArray& bytes){
    ERR_FAIL_COND_EDMSG(bytes.slice(0, 8) == discriminator(), "Account is not UserInfo.");

    account_counter = bytes.decode_u32(8);
    delete_counter = bytes.decode_u32(12);
    agreed_to_terms = bytes[16] == 1;
    had_bad_scam_scan = bytes[17] == 1;
}

Variant ShdwDrive::fetch_user_info(const Variant address){
    UserInfo *user_info = memnew(UserInfo);
    
    SolanaClient *client = memnew(SolanaClient);
    Callable callback = Callable(this, "fetch_userinfo_callback");

    client->set_callback(callback);
    client->get_account_info(address);
    return OK;
}

Variant ShdwDrive::fetch_storage_account(const Variant address){
    StorageAccountV2 *storage_account = memnew(StorageAccountV2);
    
    SolanaClient *client = memnew(SolanaClient);
    Callable callback = Callable(this, "fetch_storage_account_callback");

    client->set_callback(callback);
    client->get_account_info(address);
    return OK;
}

uint64_t ShdwDrive::human_size_to_bytes(const String& human_size){
    return 0;
}

void ShdwDrive::fetch_userinfo_callback(const Dictionary& params){
    // TODO(Virax): Add helper for this common pattern.
    if(!params.has("result")){
        return;
    }
    if(!((Dictionary)params["result"]).has("value")){
        return;
    }

    Dictionary account = ((Dictionary)params["result"])["value"];
    const PackedByteArray data_info = account["data"];

    UserInfo* user_info = memnew(UserInfo);
    user_info->from_bytes(data_info);

    emit_signal("userinfo_fetched", user_info);
}

void ShdwDrive::fetch_storage_account_callback(const Dictionary& params){
    // TODO(Virax): Add helper for this common pattern.
    if(!params.has("result")){
        return;
    }
    if(!((Dictionary)params["result"]).has("value")){
        return;
    }

    Dictionary account = ((Dictionary)params["result"])["value"];
    const PackedByteArray data_info = account["data"];

    StorageAccountV2* storage_account = memnew(StorageAccountV2);
    storage_account->from_bytes(data_info);

    emit_signal("storage_account_fetched", storage_account);
}

void ShdwDrive::_bind_methods(){
    ClassDB::add_signal("ShdwDrive", MethodInfo("user_info_fetched", PropertyInfo(Variant::OBJECT, "user_info")));
    ClassDB::add_signal("ShdwDrive", MethodInfo("storage_account_fetched", PropertyInfo(Variant::OBJECT, "storage_account")));

    ClassDB::bind_method(D_METHOD("fetch_userinfo_callback", "params"), &ShdwDrive::fetch_userinfo_callback);
    ClassDB::bind_method(D_METHOD("fetch_storage_account_callback", "params"), &ShdwDrive::fetch_storage_account_callback);
}

Variant ShdwDrive::create_storage_account(const Variant& owner_keypair, const String& name, const String& size){
    Callable callback = Callable(this, "send_create_storage_tx");

    connect("user_info_fetched", callback, CONNECT_ONE_SHOT);
    connect("storage_account_fetched", callback, CONNECT_ONE_SHOT);

    fetch_user_info(new_user_info_pubkey(owner_keypair));
    fetch_storage_account(owner_keypair);

    return nullptr;
}

void ShdwDrive::send_create_storage_tx(){
    ERR_FAIL_COND(user_info == nullptr);
    ERR_FAIL_COND(storage_account == nullptr);


}

Variant ShdwDrive::new_user_info_pubkey(const Variant& base_keypair){
    Array seeds;
    seeds.append(String("user-info").to_ascii_buffer());
    seeds.append(Object::cast_to<Keypair>(base_keypair));
    return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant ShdwDrive::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}

}