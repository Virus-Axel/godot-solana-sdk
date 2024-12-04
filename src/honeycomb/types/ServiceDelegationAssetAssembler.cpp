#include "honeycomb/types/ServiceDelegationAssetAssembler.hpp"

namespace godot{
namespace honeycomb_resource{

void ServiceDelegationAssetAssembler::set_index(const int32_t& val){
this->index = val;
}

int32_t ServiceDelegationAssetAssembler::get_index(){
return this->index;
}

void ServiceDelegationAssetAssembler::set_permission(const int32_t& val){
this->permission = val;
}

int32_t ServiceDelegationAssetAssembler::get_permission(){
return this->permission;
}

Dictionary ServiceDelegationAssetAssembler::to_dict(){
Dictionary res;
res["index"] = index;
res["permission"] = permission;
return res;
}

void ServiceDelegationAssetAssembler::from_dict(const Dictionary& dict){
index = dict["index"];
permission = dict["permission"];
}

void ServiceDelegationAssetAssembler::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationAssetAssembler::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationAssetAssembler::set_index);
ClassDB::add_property("ServiceDelegationAssetAssembler", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationAssetAssembler::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationAssetAssembler::set_permission);
ClassDB::add_property("ServiceDelegationAssetAssembler", PropertyInfo(Variant::Type::INT, "permission"), "set_permission", "get_permission");
}
} // honeycomb_resource
} // godot