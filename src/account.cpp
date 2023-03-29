#include "account.hpp"
#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void Account::_bind_methods() {
}

Account::Account() {
    data_pointer = nullptr;
}

void Account::create(uint64_t lamports, uint64_t space, const Pubkey& owner){
    data_pointer = create_account(lamports, space, owner.to_ptr());
}

Account::~Account() {
    if(data_pointer != nullptr){
        free_account(data_pointer);
    }
}
}