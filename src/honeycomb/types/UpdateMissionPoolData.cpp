#include "honeycomb/types/UpdateMissionPoolData.hpp"

namespace godot{
namespace honeycomb_resource{

void UpdateMissionPoolData::set_project(const String& val){
this->project = val;
}

String UpdateMissionPoolData::get_project(){
return this->project;
}

void UpdateMissionPoolData::set_missionPool(const String& val){
this->missionPool = val;
}

String UpdateMissionPoolData::get_missionPool(){
return this->missionPool;
}

void UpdateMissionPoolData::set_characterModel(const String& val){
this->characterModel = val;
}

String UpdateMissionPoolData::get_characterModel(){
return this->characterModel;
}

void UpdateMissionPoolData::set_authority(const String& val){
this->authority = val;
}

String UpdateMissionPoolData::get_authority(){
return this->authority;
}

void UpdateMissionPoolData::set_delegateAuthority(const String& val){
this->delegateAuthority = val;
}

String UpdateMissionPoolData::get_delegateAuthority(){
return this->delegateAuthority;
}

void UpdateMissionPoolData::set_payer(const String& val){
this->payer = val;
}

String UpdateMissionPoolData::get_payer(){
return this->payer;
}

Dictionary UpdateMissionPoolData::to_dict(){
Dictionary res;
res["project"] = project;
res["missionPool"] = missionPool;
res["characterModel"] = characterModel;
res["authority"] = authority;
res["delegateAuthority"] = delegateAuthority;
res["payer"] = payer;
return res;
}

void UpdateMissionPoolData::from_dict(const Dictionary& dict){
project = dict["project"];
missionPool = dict["missionPool"];
characterModel = dict["characterModel"];
authority = dict["authority"];
delegateAuthority = dict["delegateAuthority"];
payer = dict["payer"];
}

void UpdateMissionPoolData::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_project"), &UpdateMissionPoolData::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &UpdateMissionPoolData::set_project);
ClassDB::add_property("UpdateMissionPoolData", PropertyInfo(Variant::Type::STRING, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_missionPool"), &UpdateMissionPoolData::get_missionPool);
ClassDB::bind_method(D_METHOD("set_missionPool", "value"), &UpdateMissionPoolData::set_missionPool);
ClassDB::add_property("UpdateMissionPoolData", PropertyInfo(Variant::Type::STRING, "missionPool"), "set_missionPool", "get_missionPool");
ClassDB::bind_method(D_METHOD("get_characterModel"), &UpdateMissionPoolData::get_characterModel);
ClassDB::bind_method(D_METHOD("set_characterModel", "value"), &UpdateMissionPoolData::set_characterModel);
ClassDB::add_property("UpdateMissionPoolData", PropertyInfo(Variant::Type::STRING, "characterModel"), "set_characterModel", "get_characterModel");
ClassDB::bind_method(D_METHOD("get_authority"), &UpdateMissionPoolData::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &UpdateMissionPoolData::set_authority);
ClassDB::add_property("UpdateMissionPoolData", PropertyInfo(Variant::Type::STRING, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_delegateAuthority"), &UpdateMissionPoolData::get_delegateAuthority);
ClassDB::bind_method(D_METHOD("set_delegateAuthority", "value"), &UpdateMissionPoolData::set_delegateAuthority);
ClassDB::add_property("UpdateMissionPoolData", PropertyInfo(Variant::Type::STRING, "delegateAuthority"), "set_delegateAuthority", "get_delegateAuthority");
ClassDB::bind_method(D_METHOD("get_payer"), &UpdateMissionPoolData::get_payer);
ClassDB::bind_method(D_METHOD("set_payer", "value"), &UpdateMissionPoolData::set_payer);
ClassDB::add_property("UpdateMissionPoolData", PropertyInfo(Variant::Type::STRING, "payer"), "set_payer", "get_payer");
ClassDB::bind_method(D_METHOD("to_dict"), &UpdateMissionPoolData::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &UpdateMissionPoolData::from_dict);
}
} // honeycomb_resource
} // godot