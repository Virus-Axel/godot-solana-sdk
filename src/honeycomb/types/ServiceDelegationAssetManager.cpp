#include "honeycomb/types/ServiceDelegationAssetManager.hpp"

#include "honeycomb/enums.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceDelegationAssetManager::set_index(const int32_t& val) {
this->index = val;
}

int32_t ServiceDelegationAssetManager::get_index() const {
return this->index;
}

void ServiceDelegationAssetManager::set_permission(const String& val) {
    this->permission = val;
}

String ServiceDelegationAssetManager::get_permission() const {
    return this->permission;
}

void ServiceDelegationAssetManager::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationAssetManager::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationAssetManager::set_index);
ClassDB::add_property("ServiceDelegationAssetManager", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationAssetManager::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationAssetManager::set_permission);
ClassDB::add_property("ServiceDelegationAssetManager", PropertyInfo(Variant::STRING, "permission"), "set_permission", "get_permission");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationAssetManager::to_dict);
}
Dictionary ServiceDelegationAssetManager::to_dict() {
Dictionary res;
res["index"] = index;
res["permission"] = this->permission;
return res;
}
} // namespace honeycomb_resource
} // namespace godot