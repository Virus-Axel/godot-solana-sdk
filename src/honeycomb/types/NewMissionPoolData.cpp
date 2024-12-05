#include "honeycomb/types/NewMissionPoolData.hpp"

namespace godot{
namespace honeycomb_resource{

void NewMissionPoolData::set_project(const String& val){
this->project = val;
}

String NewMissionPoolData::get_project(){
return this->project;
}

void NewMissionPoolData::set_authority(const String& val){
this->authority = val;
}

String NewMissionPoolData::get_authority(){
return this->authority;
}

void NewMissionPoolData::set_delegateAuthority(const String& val){
this->delegateAuthority = val;
}

String NewMissionPoolData::get_delegateAuthority(){
return this->delegateAuthority;
}

void NewMissionPoolData::set_payer(const String& val){
this->payer = val;
}

String NewMissionPoolData::get_payer(){
return this->payer;
}

void NewMissionPoolData::set_characterModel(const String& val){
this->characterModel = val;
}

String NewMissionPoolData::get_characterModel(){
return this->characterModel;
}

void NewMissionPoolData::set_name(const String& val){
this->name = val;
}

String NewMissionPoolData::get_name(){
return this->name;
}

Dictionary NewMissionPoolData::to_dict(){
Dictionary res;
res["project"] = project;
res["authority"] = authority;
res["delegateAuthority"] = delegateAuthority;
res["payer"] = payer;
res["characterModel"] = characterModel;
res["name"] = name;
return res;
}

void NewMissionPoolData::from_dict(const Dictionary& dict){
project = dict["project"];
authority = dict["authority"];
delegateAuthority = dict["delegateAuthority"];
payer = dict["payer"];
characterModel = dict["characterModel"];
name = dict["name"];
}

void NewMissionPoolData::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_project"), &NewMissionPoolData::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &NewMissionPoolData::set_project);
ClassDB::add_property("NewMissionPoolData", PropertyInfo(Variant::Type::STRING, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_authority"), &NewMissionPoolData::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &NewMissionPoolData::set_authority);
ClassDB::add_property("NewMissionPoolData", PropertyInfo(Variant::Type::STRING, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_delegateAuthority"), &NewMissionPoolData::get_delegateAuthority);
ClassDB::bind_method(D_METHOD("set_delegateAuthority", "value"), &NewMissionPoolData::set_delegateAuthority);
ClassDB::add_property("NewMissionPoolData", PropertyInfo(Variant::Type::STRING, "delegateAuthority"), "set_delegateAuthority", "get_delegateAuthority");
ClassDB::bind_method(D_METHOD("get_payer"), &NewMissionPoolData::get_payer);
ClassDB::bind_method(D_METHOD("set_payer", "value"), &NewMissionPoolData::set_payer);
ClassDB::add_property("NewMissionPoolData", PropertyInfo(Variant::Type::STRING, "payer"), "set_payer", "get_payer");
ClassDB::bind_method(D_METHOD("get_characterModel"), &NewMissionPoolData::get_characterModel);
ClassDB::bind_method(D_METHOD("set_characterModel", "value"), &NewMissionPoolData::set_characterModel);
ClassDB::add_property("NewMissionPoolData", PropertyInfo(Variant::Type::STRING, "characterModel"), "set_characterModel", "get_characterModel");
ClassDB::bind_method(D_METHOD("get_name"), &NewMissionPoolData::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &NewMissionPoolData::set_name);
ClassDB::add_property("NewMissionPoolData", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("to_dict"), &NewMissionPoolData::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &NewMissionPoolData::from_dict);
}
} // honeycomb_resource
} // godot