#include "honeycomb/types/ParticipateOnMissionData_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void ParticipateOnMissionData::set_mission(const String& val) {
this->mission = val;
}

String ParticipateOnMissionData::get_mission() const {
return this->mission;
}

void ParticipateOnMissionData::set_characterAddresses(const PackedStringArray& val) {
this->characterAddresses = val;
}

PackedStringArray ParticipateOnMissionData::get_characterAddresses() const {
return this->characterAddresses;
}

void ParticipateOnMissionData::set_authority(const String& val) {
this->authority = val;
}

String ParticipateOnMissionData::get_authority() const {
return this->authority;
}

void ParticipateOnMissionData::set_userId(const int32_t& val) {
this->userId = val;
}

int32_t ParticipateOnMissionData::get_userId() const {
return this->userId;
}

void ParticipateOnMissionData::set_payer(const String& val) {
this->payer = val;
}

String ParticipateOnMissionData::get_payer() const {
return this->payer;
}

void ParticipateOnMissionData::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_mission"), &ParticipateOnMissionData::get_mission);
ClassDB::bind_method(D_METHOD("set_mission", "value"), &ParticipateOnMissionData::set_mission);
ClassDB::add_property("ParticipateOnMissionData", PropertyInfo(Variant::Type::STRING, "mission"), "set_mission", "get_mission");
ClassDB::bind_method(D_METHOD("get_characterAddresses"), &ParticipateOnMissionData::get_characterAddresses);
ClassDB::bind_method(D_METHOD("set_characterAddresses", "value"), &ParticipateOnMissionData::set_characterAddresses);
ClassDB::add_property("ParticipateOnMissionData", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "characterAddresses"), "set_characterAddresses", "get_characterAddresses");
ClassDB::bind_method(D_METHOD("get_authority"), &ParticipateOnMissionData::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &ParticipateOnMissionData::set_authority);
ClassDB::add_property("ParticipateOnMissionData", PropertyInfo(Variant::Type::STRING, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_userId"), &ParticipateOnMissionData::get_userId);
ClassDB::bind_method(D_METHOD("set_userId", "value"), &ParticipateOnMissionData::set_userId);
ClassDB::add_property("ParticipateOnMissionData", PropertyInfo(Variant::Type::INT, "userId"), "set_userId", "get_userId");
ClassDB::bind_method(D_METHOD("get_payer"), &ParticipateOnMissionData::get_payer);
ClassDB::bind_method(D_METHOD("set_payer", "value"), &ParticipateOnMissionData::set_payer);
ClassDB::add_property("ParticipateOnMissionData", PropertyInfo(Variant::Type::STRING, "payer"), "set_payer", "get_payer");
ClassDB::bind_method(D_METHOD("to_dict"), &ParticipateOnMissionData::to_dict);
}
Dictionary ParticipateOnMissionData::to_dict() {
Dictionary res;
if (!mission.is_empty()) {
    res["mission"] = mission;
}
res["characterAddresses"] = characterAddresses;
if (!authority.is_empty()) {
    res["authority"] = authority;
}
res["userId"] = userId;
if (!payer.is_empty()) {
    res["payer"] = payer;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot