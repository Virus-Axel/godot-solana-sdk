#include "honeycomb/types/ServiceDelegationCharacterManager_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceDelegationCharacterManager::set_index(const int32_t& val) {
this->index = val;
}

int32_t ServiceDelegationCharacterManager::get_index() const {
return this->index;
}

void ServiceDelegationCharacterManager::set_permission(const String& val) {
    this->permission = val;
}

String ServiceDelegationCharacterManager::get_permission() const {
    return this->permission;
}

void ServiceDelegationCharacterManager::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationCharacterManager::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationCharacterManager::set_index);
ClassDB::add_property("ServiceDelegationCharacterManager", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationCharacterManager::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationCharacterManager::set_permission);
ClassDB::add_property("ServiceDelegationCharacterManager", PropertyInfo(Variant::STRING, "permission"), "set_permission", "get_permission");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationCharacterManager::to_dict);
}
Dictionary ServiceDelegationCharacterManager::to_dict() {
Dictionary res;
res["index"] = index;
res["permission"] = this->permission;
return res;
}
} // namespace honeycomb_resource
} // namespace godot