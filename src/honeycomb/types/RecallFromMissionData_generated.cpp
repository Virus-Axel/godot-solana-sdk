#include "honeycomb/types/RecallFromMissionData_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void RecallFromMissionData::set_mission(const String &val) {
	this->mission = val;
}

String RecallFromMissionData::get_mission() const {
	return this->mission;
}

void RecallFromMissionData::set_characterAddresses(const PackedStringArray &val) {
	this->characterAddresses = val;
}

PackedStringArray RecallFromMissionData::get_characterAddresses() const {
	return this->characterAddresses;
}

void RecallFromMissionData::set_authority(const String &val) {
	this->authority = val;
}

String RecallFromMissionData::get_authority() const {
	return this->authority;
}

void RecallFromMissionData::set_userId(const int32_t &val) {
	this->userId = val;
}

int32_t RecallFromMissionData::get_userId() const {
	return this->userId;
}

void RecallFromMissionData::set_payer(const String &val) {
	this->payer = val;
}

String RecallFromMissionData::get_payer() const {
	return this->payer;
}

void RecallFromMissionData::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_mission"), &RecallFromMissionData::get_mission);
	ClassDB::bind_method(D_METHOD("set_mission", "value"), &RecallFromMissionData::set_mission);
	ClassDB::add_property("RecallFromMissionData", PropertyInfo(Variant::Type::STRING, "mission"), "set_mission", "get_mission");
	ClassDB::bind_method(D_METHOD("get_characterAddresses"), &RecallFromMissionData::get_characterAddresses);
	ClassDB::bind_method(D_METHOD("set_characterAddresses", "value"), &RecallFromMissionData::set_characterAddresses);
	ClassDB::add_property("RecallFromMissionData", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "characterAddresses"), "set_characterAddresses", "get_characterAddresses");
	ClassDB::bind_method(D_METHOD("get_authority"), &RecallFromMissionData::get_authority);
	ClassDB::bind_method(D_METHOD("set_authority", "value"), &RecallFromMissionData::set_authority);
	ClassDB::add_property("RecallFromMissionData", PropertyInfo(Variant::Type::STRING, "authority"), "set_authority", "get_authority");
	ClassDB::bind_method(D_METHOD("get_userId"), &RecallFromMissionData::get_userId);
	ClassDB::bind_method(D_METHOD("set_userId", "value"), &RecallFromMissionData::set_userId);
	ClassDB::add_property("RecallFromMissionData", PropertyInfo(Variant::Type::INT, "userId"), "set_userId", "get_userId");
	ClassDB::bind_method(D_METHOD("get_payer"), &RecallFromMissionData::get_payer);
	ClassDB::bind_method(D_METHOD("set_payer", "value"), &RecallFromMissionData::set_payer);
	ClassDB::add_property("RecallFromMissionData", PropertyInfo(Variant::Type::STRING, "payer"), "set_payer", "get_payer");
	ClassDB::bind_method(D_METHOD("to_dict"), &RecallFromMissionData::to_dict);
}
Dictionary RecallFromMissionData::to_dict() {
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