#include "account.hpp"

#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Account::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_new", "lamports", "space", "owner"), &Account::create_new);
}

Account::Account() {
    data_pointer = nullptr;
}

void Account::create_new(uint64_t lamports, uint64_t space, const Variant& owner){
}

Account::~Account() {
    if(data_pointer != nullptr){
        ;//free_account(data_pointer);
    }
}