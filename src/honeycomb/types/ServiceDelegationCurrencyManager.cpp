#include "honeycomb/types/ServiceDelegationCurrencyManager.hpp"

namespace godot{
namespace honeycomb_resource{

void ServiceDelegationCurrencyManager::set_permission(const int32_t& val){
this->permission = val;
}

int32_t ServiceDelegationCurrencyManager::get_permission(){
return this->permission;
}

Dictionary ServiceDelegationCurrencyManager::to_dict(){
Dictionary res;
res["permission"] = permission;
return res;
}

void ServiceDelegationCurrencyManager::from_dict(const Dictionary& dict){
permission = dict["permission"];
}

void ServiceDelegationCurrencyManager::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationCurrencyManager::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationCurrencyManager::set_permission);
ClassDB::add_property("ServiceDelegationCurrencyManager", PropertyInfo(Variant::Type::INT, "permission"), "set_permission", "get_permission");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationCurrencyManager::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &ServiceDelegationCurrencyManager::from_dict);
}
} // honeycomb_resource
} // godot