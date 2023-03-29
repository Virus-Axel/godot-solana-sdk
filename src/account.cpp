#include "account.hpp"
#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Account::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create", "lamports", "data_size", "owner"), &Pubkey::create);
}

Account::Account() {
    // initialize any variables here
}

Error Account::create(uint64_t lamports, uint64_t size, const Pubkey& owner){

}

Account::~Account() {
    // add your cleanup here
}
