#include "pubkey.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Pubkey::_bind_methods() {
}

Pubkey::Pubkey() {
    data_pointer = create_pubkey();
}

const void* Pubkey::to_ptr() const{
    return data_pointer;
}

Pubkey::~Pubkey() {
    free_pubkey(data_pointer);
}
