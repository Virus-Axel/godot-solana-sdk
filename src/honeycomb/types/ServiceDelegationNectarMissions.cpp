#include "honeycomb/types/ServiceDelegationNectarMissions.hpp"

namespace godot{
namespace honeycomb_resource{

void ServiceDelegationNectarMissions::set_index(const int32_t& val){
this->index = val;
}

int32_t ServiceDelegationNectarMissions::get_index(){
return this->index;
}

void ServiceDelegationNectarMissions::set_permission(const int32_t& val){
this->permission = val;
}

int32_t ServiceDelegationNectarMissions::get_permission(){
return this->permission;
}

Dictionary ServiceDelegationNectarMissions::to_dict(){
Dictionary res;
res["index"] = index;
res["permission"] = permission;
return res;
}

void ServiceDelegationNectarMissions::from_dict(const Dictionary& dict){
index = dict["index"];
permission = dict["permission"];
}

void ServiceDelegationNectarMissions::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationNectarMissions::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationNectarMissions::set_index);
ClassDB::add_property("ServiceDelegationNectarMissions", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationNectarMissions::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationNectarMissions::set_permission);
ClassDB::add_property("ServiceDelegationNectarMissions", PropertyInfo(Variant::Type::INT, "permission"), "set_permission", "get_permission");
}
} // honeycomb_resource
} // godot