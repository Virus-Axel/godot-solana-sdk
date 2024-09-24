#include "shdwdrive.hpp"


#include <godot_cpp/classes/engine.hpp>

namespace godot{

Variant ShdwDrive::fetch(const Variant address){
    UserInfo *user_info = memnew(UserInfo);
    
    SolanaClient *client = memnew(SolanaClient);
    Callable callback = Callable(this, "");

    client->set_callback(callback);
    client->get_account_info(address);
    return OK;
}

uint64_t ShdwDrive::human_size_to_bytes(const String& human_size){
    return 0;
}

Variant ShdwDrive::create_storage_account(const String& name, const String& size){
 return nullptr;
}

}