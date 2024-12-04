#include "honeycomb/types/ServiceDelegationAssetManager.hpp"

namespace godot{
namespace honeycomb_resource{

void ServiceDelegationAssetManager::set_index(const int32_t& val){
this->index = val;
}

int32_t ServiceDelegationAssetManager::get_index(){
return this->index;
}

void ServiceDelegationAssetManager::set_permission(const int32_t& val){
this->permission = val;
}

int32_t ServiceDelegationAssetManager::get_permission(){
return this->permission;
}

Dictionary ServiceDelegationAssetManager::to_dict(){
Dictionary res;
res["index"] = index;
res["permission"] = permission;
return res;
}

void ServiceDelegationAssetManager::from_dict(const Dictionary& dict){
index = dict["index"];
permission = dict["permission"];
}

void ServiceDelegationAssetManager::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationAssetManager::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationAssetManager::set_index);
ClassDB::add_property("ServiceDelegationAssetManager", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationAssetManager::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationAssetManager::set_permission);
ClassDB::add_property("ServiceDelegationAssetManager", PropertyInfo(Variant::Type::INT, "permission"), "set_permission", "get_permission");
}
} // honeycomb_resource
} // godot