#include "honeycomb/types/IngredientsInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void IngredientsInput::set_resourceAddress(const String& val) {
this->resourceAddress = val;
}

String IngredientsInput::get_resourceAddress() const {
return this->resourceAddress;
}

void IngredientsInput::set_amount(const int64_t& val) {
this->amount = val;
}

int64_t IngredientsInput::get_amount() const {
return this->amount;
}

void IngredientsInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_resourceAddress"), &IngredientsInput::get_resourceAddress);
ClassDB::bind_method(D_METHOD("set_resourceAddress", "value"), &IngredientsInput::set_resourceAddress);
ClassDB::add_property("IngredientsInput", PropertyInfo(Variant::Type::STRING, "resourceAddress"), "set_resourceAddress", "get_resourceAddress");
ClassDB::bind_method(D_METHOD("get_amount"), &IngredientsInput::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &IngredientsInput::set_amount);
ClassDB::add_property("IngredientsInput", PropertyInfo(Variant::Type::INT, "amount"), "set_amount", "get_amount");
ClassDB::bind_method(D_METHOD("to_dict"), &IngredientsInput::to_dict);
}
Dictionary IngredientsInput::to_dict() {
Dictionary res;
if (!resourceAddress.is_empty()) {
    res["resourceAddress"] = resourceAddress;
}
res["amount"] = amount;
return res;
}
} // namespace honeycomb_resource
} // namespace godot