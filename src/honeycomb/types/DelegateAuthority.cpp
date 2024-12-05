#include "honeycomb/types/DelegateAuthority.hpp"

#include "pubkey.hpp"
#include "pubkey.hpp"
namespace godot{
namespace honeycomb_resource{

void DelegateAuthority::set_address(const PackedByteArray& val){
this->address = val;
}

PackedByteArray DelegateAuthority::get_address(){
return this->address;
}

void DelegateAuthority::set_bump(const int32_t& val){
this->bump = val;
}

int32_t DelegateAuthority::get_bump(){
return this->bump;
}

void DelegateAuthority::set_project(const Variant& val){
this->project = val;
}

Variant DelegateAuthority::get_project(){
return this->project;
}

void DelegateAuthority::set_authority(const Variant& val){
this->authority = val;
}

Variant DelegateAuthority::get_authority(){
return this->authority;
}

void DelegateAuthority::set_delegations(const Array& val){
this->delegations = val;
}

Array DelegateAuthority::get_delegations(){
return this->delegations;
}

Dictionary DelegateAuthority::to_dict(){
Dictionary res;
res["address"] = address;
res["bump"] = bump;
res["project"] = Object::cast_to<Pubkey>(project)->to_string();
res["authority"] = Object::cast_to<Pubkey>(authority)->to_string();
res["delegations"] = delegations;
return res;
}

void DelegateAuthority::from_dict(const Dictionary& dict){
address = dict["address"];
bump = dict["bump"];
project = Pubkey::new_from_string(dict["project"]);
authority = Pubkey::new_from_string(dict["authority"]);
delegations = dict["delegations"];
}

void DelegateAuthority::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_address"), &DelegateAuthority::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &DelegateAuthority::set_address);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_bump"), &DelegateAuthority::get_bump);
ClassDB::bind_method(D_METHOD("set_bump", "value"), &DelegateAuthority::set_bump);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
ClassDB::bind_method(D_METHOD("get_project"), &DelegateAuthority::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &DelegateAuthority::set_project);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_authority"), &DelegateAuthority::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &DelegateAuthority::set_authority);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::OBJECT, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_delegations"), &DelegateAuthority::get_delegations);
ClassDB::bind_method(D_METHOD("set_delegations", "value"), &DelegateAuthority::set_delegations);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::ARRAY, "delegations"), "set_delegations", "get_delegations");
ClassDB::bind_method(D_METHOD("to_dict"), &DelegateAuthority::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &DelegateAuthority::from_dict);
}
} // honeycomb_resource
} // godot