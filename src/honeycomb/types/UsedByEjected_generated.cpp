#include "honeycomb/types/UsedByEjected_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void UsedByEjected::set_mint(const Variant& val) {
this->mint = val;
}

Variant UsedByEjected::get_mint() const {
return this->mint;
}

void UsedByEjected::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_mint"), &UsedByEjected::get_mint);
ClassDB::bind_method(D_METHOD("set_mint", "value"), &UsedByEjected::set_mint);
ClassDB::add_property("UsedByEjected", PropertyInfo(Variant::Type::OBJECT, "mint"), "set_mint", "get_mint");
ClassDB::bind_method(D_METHOD("to_dict"), &UsedByEjected::to_dict);
}
Dictionary UsedByEjected::to_dict() {
Dictionary res;
if (mint.has_method("to_string")) {
res["mint"] = mint.call("to_string");
} else {
res["mint"] = mint;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot