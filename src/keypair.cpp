#include "keypair.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void Keypair::_bind_methods() {
}

Keypair::Keypair() {
    data_pointer = create_keypair();
}

void* Keypair::to_ptr() const{
    return data_pointer;
}

Keypair::~Keypair() {
    free_keypair(data_pointer);
}

}