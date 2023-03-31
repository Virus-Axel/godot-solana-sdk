#include "account_meta.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void AccountMeta::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_new", "account_key", "is_signer", "writeable"), &AccountMeta::create_new);
}

AccountMeta::AccountMeta() {
    data_pointer = nullptr;
}

void *AccountMeta::to_ptr(){
    return data_pointer;
}

void AccountMeta::create_new(const Variant& account_key, bool is_signer, bool writeable){
    Object *account_key_cast = account_key;
    Pubkey *account_key_ptr = Object::cast_to<Pubkey>(account_key_cast);
    data_pointer = create_account_meta(account_key_ptr->to_ptr(), is_signer, writeable);
}

AccountMeta::~AccountMeta() {
    if(data_pointer != nullptr){
        free_account(data_pointer);
    }
}
}