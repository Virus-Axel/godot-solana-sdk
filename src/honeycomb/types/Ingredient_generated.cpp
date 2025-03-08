#include "honeycomb/types/Ingredient_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Ingredient::set_resourceAddress(const Variant& val) {
this->resourceAddress = val;
}

Variant Ingredient::get_resourceAddress() const {
return this->resourceAddress;
}

void Ingredient::set_amount(const int64_t& val) {
this->amount = val;
}

int64_t Ingredient::get_amount() const {
return this->amount;
}

void Ingredient::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_resourceAddress"), &Ingredient::get_resourceAddress);
ClassDB::bind_method(D_METHOD("set_resourceAddress", "value"), &Ingredient::set_resourceAddress);
ClassDB::add_property("Ingredient", PropertyInfo(Variant::Type::OBJECT, "resourceAddress"), "set_resourceAddress", "get_resourceAddress");
ClassDB::bind_method(D_METHOD("get_amount"), &Ingredient::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &Ingredient::set_amount);
ClassDB::add_property("Ingredient", PropertyInfo(Variant::Type::INT, "amount"), "set_amount", "get_amount");
ClassDB::bind_method(D_METHOD("to_dict"), &Ingredient::to_dict);
}
Dictionary Ingredient::to_dict() {
Dictionary res;
if (resourceAddress.has_method("to_string")) {
res["resourceAddress"] = resourceAddress.call("to_string");
} else {
res["resourceAddress"] = resourceAddress;
}
res["amount"] = amount;
return res;
}
} // namespace honeycomb_resource
} // namespace godot