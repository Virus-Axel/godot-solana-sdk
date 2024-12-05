#include "honeycomb/types/ParticipateOnMissionData.hpp"

namespace godot{
namespace honeycomb_resource{

void ParticipateOnMissionData::set_mission(const String& val){
this->mission = val;
}

String ParticipateOnMissionData::get_mission(){
return this->mission;
}

void ParticipateOnMissionData::set_characterAddresses(const PackedStringArray& val){
this->characterAddresses = val;
}

PackedStringArray ParticipateOnMissionData::get_characterAddresses(){
return this->characterAddresses;
}

void ParticipateOnMissionData::set_authority(const String& val){
this->authority = val;
}

String ParticipateOnMissionData::get_authority(){
return this->authority;
}

void ParticipateOnMissionData::set_payer(const String& val){
this->payer = val;
}

String ParticipateOnMissionData::get_payer(){
return this->payer;
}

Dictionary ParticipateOnMissionData::to_dict(){
Dictionary res;
res["mission"] = mission;
res["characterAddresses"] = characterAddresses;
res["authority"] = authority;
res["payer"] = payer;
return res;
}

void ParticipateOnMissionData::from_dict(const Dictionary& dict){
mission = dict["mission"];
characterAddresses = dict["characterAddresses"];
authority = dict["authority"];
payer = dict["payer"];
}

void ParticipateOnMissionData::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_mission"), &ParticipateOnMissionData::get_mission);
ClassDB::bind_method(D_METHOD("set_mission", "value"), &ParticipateOnMissionData::set_mission);
ClassDB::add_property("ParticipateOnMissionData", PropertyInfo(Variant::Type::STRING, "mission"), "set_mission", "get_mission");
ClassDB::bind_method(D_METHOD("get_characterAddresses"), &ParticipateOnMissionData::get_characterAddresses);
ClassDB::bind_method(D_METHOD("set_characterAddresses", "value"), &ParticipateOnMissionData::set_characterAddresses);
ClassDB::add_property("ParticipateOnMissionData", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "characterAddresses"), "set_characterAddresses", "get_characterAddresses");
ClassDB::bind_method(D_METHOD("get_authority"), &ParticipateOnMissionData::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &ParticipateOnMissionData::set_authority);
ClassDB::add_property("ParticipateOnMissionData", PropertyInfo(Variant::Type::STRING, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_payer"), &ParticipateOnMissionData::get_payer);
ClassDB::bind_method(D_METHOD("set_payer", "value"), &ParticipateOnMissionData::set_payer);
ClassDB::add_property("ParticipateOnMissionData", PropertyInfo(Variant::Type::STRING, "payer"), "set_payer", "get_payer");
ClassDB::bind_method(D_METHOD("to_dict"), &ParticipateOnMissionData::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &ParticipateOnMissionData::from_dict);
}
} // honeycomb_resource
} // godot