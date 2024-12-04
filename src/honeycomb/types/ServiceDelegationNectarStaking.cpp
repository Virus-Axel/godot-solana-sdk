#include "honeycomb/types/ServiceDelegationNectarStaking.hpp"

namespace godot{
namespace honeycomb_resource{

void ServiceDelegationNectarStaking::set_index(const int32_t& val){
this->index = val;
}

int32_t ServiceDelegationNectarStaking::get_index(){
return this->index;
}

void ServiceDelegationNectarStaking::set_permission(const int32_t& val){
this->permission = val;
}

int32_t ServiceDelegationNectarStaking::get_permission(){
return this->permission;
}

Dictionary ServiceDelegationNectarStaking::to_dict(){
Dictionary res;
res["index"] = index;
res["permission"] = permission;
return res;
}

void ServiceDelegationNectarStaking::from_dict(const Dictionary& dict){
index = dict["index"];
permission = dict["permission"];
}

void ServiceDelegationNectarStaking::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationNectarStaking::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationNectarStaking::set_index);
ClassDB::add_property("ServiceDelegationNectarStaking", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationNectarStaking::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationNectarStaking::set_permission);
ClassDB::add_property("ServiceDelegationNectarStaking", PropertyInfo(Variant::Type::INT, "permission"), "set_permission", "get_permission");
}
} // honeycomb_resource
} // godot