#include "honeycomb/types/ServiceDelegationHiveControl.hpp"

#include "honeycomb/enums.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceDelegationHiveControl::set_permission(const String& val) {
    this->permission = val;
}

String ServiceDelegationHiveControl::get_permission() const {
    return this->permission;
}

void ServiceDelegationHiveControl::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationHiveControl::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationHiveControl::set_permission);
ClassDB::add_property("ServiceDelegationHiveControl", PropertyInfo(Variant::STRING, "permission"), "set_permission", "get_permission");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationHiveControl::to_dict);
}
Dictionary ServiceDelegationHiveControl::to_dict() {
Dictionary res;
res["permission"] = this->permission;
return res;
}
} // namespace honeycomb_resource
} // namespace godot