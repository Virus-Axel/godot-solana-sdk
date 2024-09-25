#include "shdwdrive.hpp"

#include "solana_client.hpp"
#include <godot_cpp/classes/engine.hpp>

namespace godot{

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

Variant ShdwDrive::fetch(const Variant address){
    UserInfo *user_info = memnew(UserInfo);
    
    SolanaClient *client = memnew(SolanaClient);
    Callable callback = Callable(this, "fetch_userinfo_callback");

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


}

void ShdwDrive::_bind_methods(){
    ClassDB::add_signal("ShdwDrive", MethodInfo("userinfo_fetched"));

    ClassDB::bind_method(D_METHOD("fetch_userinfo_callback"), &ShdwDrive::fetch_userinfo_callback);
}

Variant ShdwDrive::create_storage_account(const String& name, const String& size){
 return nullptr;
}

}