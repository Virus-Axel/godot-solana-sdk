#include "honeycomb/types/Meal_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Meal::set_resourceAddress(const Variant& val) {
this->resourceAddress = val;
}

Variant Meal::get_resourceAddress() const {
return this->resourceAddress;
}

void Meal::set_amount(const int64_t& val) {
this->amount = val;
}

int64_t Meal::get_amount() const {
return this->amount;
}

void Meal::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_resourceAddress"), &Meal::get_resourceAddress);
ClassDB::bind_method(D_METHOD("set_resourceAddress", "value"), &Meal::set_resourceAddress);
ClassDB::add_property("Meal", PropertyInfo(Variant::Type::OBJECT, "resourceAddress"), "set_resourceAddress", "get_resourceAddress");
ClassDB::bind_method(D_METHOD("get_amount"), &Meal::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &Meal::set_amount);
ClassDB::add_property("Meal", PropertyInfo(Variant::Type::INT, "amount"), "set_amount", "get_amount");
ClassDB::bind_method(D_METHOD("to_dict"), &Meal::to_dict);
}
Dictionary Meal::to_dict() {
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