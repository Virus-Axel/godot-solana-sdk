#include "honeycomb/types/AssociatedProgramInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void AssociatedProgramInput::set_address(const String& val) {
this->address = val;
}

String AssociatedProgramInput::get_address() const {
return this->address;
}

void AssociatedProgramInput::set_trustedActions(const Array& val) {
this->trustedActions = val;
}

Array AssociatedProgramInput::get_trustedActions() const {
return this->trustedActions;
}

void AssociatedProgramInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &AssociatedProgramInput::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &AssociatedProgramInput::set_address);
ClassDB::add_property("AssociatedProgramInput", PropertyInfo(Variant::Type::STRING, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_trustedActions"), &AssociatedProgramInput::get_trustedActions);
ClassDB::bind_method(D_METHOD("set_trustedActions", "value"), &AssociatedProgramInput::set_trustedActions);
ClassDB::add_property("AssociatedProgramInput", PropertyInfo(Variant::Type::ARRAY, "trustedActions"), "set_trustedActions", "get_trustedActions");
ClassDB::bind_method(D_METHOD("to_dict"), &AssociatedProgramInput::to_dict);
}
Dictionary AssociatedProgramInput::to_dict() {
Dictionary res;
if (!address.is_empty()) {
    res["address"] = address;
}
res["trustedActions"] = trustedActions;
return res;
}
} // namespace honeycomb_resource
} // namespace godot