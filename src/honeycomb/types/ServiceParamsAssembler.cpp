#include "honeycomb/types/ServiceParamsAssembler.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceParamsAssembler::set_assemblerId(const Variant& val) {
this->assemblerId = val;
}

Variant ServiceParamsAssembler::get_assemblerId() const {
return this->assemblerId;
}

void ServiceParamsAssembler::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_assemblerId"), &ServiceParamsAssembler::get_assemblerId);
ClassDB::bind_method(D_METHOD("set_assemblerId", "value"), &ServiceParamsAssembler::set_assemblerId);
ClassDB::add_property("ServiceParamsAssembler", PropertyInfo(Variant::Type::OBJECT, "assemblerId"), "set_assemblerId", "get_assemblerId");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceParamsAssembler::to_dict);
}
Dictionary ServiceParamsAssembler::to_dict() {
Dictionary res;
if (assemblerId.has_method("to_string")) {
res["assemblerId"] = assemblerId.call("to_string");
} else {
res["assemblerId"] = assemblerId;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot