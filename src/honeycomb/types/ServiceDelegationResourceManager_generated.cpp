#include "honeycomb/types/ServiceDelegationResourceManager_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceDelegationResourceManager::set_permission(const String& val) {
    this->permission = val;
}

String ServiceDelegationResourceManager::get_permission() const {
    return this->permission;
}

void ServiceDelegationResourceManager::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationResourceManager::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationResourceManager::set_permission);
ClassDB::add_property("ServiceDelegationResourceManager", PropertyInfo(Variant::STRING, "permission"), "set_permission", "get_permission");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationResourceManager::to_dict);
}
Dictionary ServiceDelegationResourceManager::to_dict() {
Dictionary res;
res["permission"] = this->permission;
return res;
}
} // namespace honeycomb_resource
} // namespace godot