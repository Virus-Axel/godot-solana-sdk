#include "account.hpp"

#include <cstdint>

#include "godot_cpp/variant/variant.hpp"
#include <godot_cpp/core/class_db.hpp>

namespace godot {

void Account::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_new", "lamports", "space", "owner"), &Account::create_new);
}

void Account::create_new(uint64_t lamports, uint64_t space, const Variant &owner) {
}

} //namespace godot