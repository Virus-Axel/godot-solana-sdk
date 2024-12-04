#include "honeycomb/types/ServiceDelegationHiveControl.hpp"

namespace godot{
namespace honeycomb_resource{

void ServiceDelegationHiveControl::set_permission(const int32_t& val){
this->permission = val;
}

int32_t ServiceDelegationHiveControl::get_permission(){
return this->permission;
}

Dictionary ServiceDelegationHiveControl::to_dict(){
Dictionary res;
res["permission"] = permission;
return res;
}

void ServiceDelegationHiveControl::from_dict(const Dictionary& dict){
permission = dict["permission"];
}

void ServiceDelegationHiveControl::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationHiveControl::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationHiveControl::set_permission);
ClassDB::add_property("ServiceDelegationHiveControl", PropertyInfo(Variant::Type::INT, "permission"), "set_permission", "get_permission");
}
} // honeycomb_resource
} // godot