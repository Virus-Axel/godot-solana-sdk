#include "honeycomb/types/AssociatedProgram.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void AssociatedProgram::set_address(const Variant& val) {
this->address = val;
}

Variant AssociatedProgram::get_address() const {
return this->address;
}

void AssociatedProgram::set_trustedActions(const Array& val) {
this->trustedActions = val;
}

Array AssociatedProgram::get_trustedActions() const {
return this->trustedActions;
}

void AssociatedProgram::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &AssociatedProgram::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &AssociatedProgram::set_address);
ClassDB::add_property("AssociatedProgram", PropertyInfo(Variant::Type::OBJECT, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_trustedActions"), &AssociatedProgram::get_trustedActions);
ClassDB::bind_method(D_METHOD("set_trustedActions", "value"), &AssociatedProgram::set_trustedActions);
ClassDB::add_property("AssociatedProgram", PropertyInfo(Variant::Type::ARRAY, "trustedActions"), "set_trustedActions", "get_trustedActions");
ClassDB::bind_method(D_METHOD("to_dict"), &AssociatedProgram::to_dict);
}
Dictionary AssociatedProgram::to_dict() {
Dictionary res;
if (address.has_method("to_string")) {
res["address"] = address.call("to_string");
} else {
res["address"] = address;
}
res["trustedActions"] = trustedActions;
return res;
}
} // namespace honeycomb_resource
} // namespace godot