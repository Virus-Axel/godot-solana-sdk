#include "honeycomb/types/ServiceDelegationBuzzGuild.hpp"

namespace godot{
namespace honeycomb_resource{

void ServiceDelegationBuzzGuild::set_index(const int32_t& val){
this->index = val;
}

int32_t ServiceDelegationBuzzGuild::get_index(){
return this->index;
}

void ServiceDelegationBuzzGuild::set_permission(const int32_t& val){
this->permission = val;
}

int32_t ServiceDelegationBuzzGuild::get_permission(){
return this->permission;
}

Dictionary ServiceDelegationBuzzGuild::to_dict(){
Dictionary res;
res["index"] = index;
res["permission"] = permission;
return res;
}

void ServiceDelegationBuzzGuild::from_dict(const Dictionary& dict){
index = dict["index"];
permission = dict["permission"];
}

void ServiceDelegationBuzzGuild::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationBuzzGuild::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationBuzzGuild::set_index);
ClassDB::add_property("ServiceDelegationBuzzGuild", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationBuzzGuild::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationBuzzGuild::set_permission);
ClassDB::add_property("ServiceDelegationBuzzGuild", PropertyInfo(Variant::Type::INT, "permission"), "set_permission", "get_permission");
}
} // honeycomb_resource
} // godot