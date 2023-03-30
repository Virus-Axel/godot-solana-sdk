#include "account_meta.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void AccountMeta::_bind_methods() {
}

AccountMeta::AccountMeta() {
    data_pointer = nullptr;
}

void AccountMeta::create(const Pubkey& account_key, bool is_signer, bool writeable){
    data_pointer = create_account_meta(account_key.to_ptr(), is_signer, writeable);
}

AccountMeta::~AccountMeta() {
    if(data_pointer != nullptr){
        free_account(data_pointer);
    }
}
}