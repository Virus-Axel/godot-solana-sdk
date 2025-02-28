#include "honeycomb/types/UpdateMissionPoolData_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void UpdateMissionPoolData::set_project(const String& val) {
this->project = val;
}

String UpdateMissionPoolData::get_project() const {
return this->project;
}

void UpdateMissionPoolData::set_missionPool(const String& val) {
this->missionPool = val;
}

String UpdateMissionPoolData::get_missionPool() const {
return this->missionPool;
}

void UpdateMissionPoolData::set_characterModel(const String& val) {
this->characterModel = val;
}

String UpdateMissionPoolData::get_characterModel() const {
return this->characterModel;
}

void UpdateMissionPoolData::set_authority(const String& val) {
this->authority = val;
}

String UpdateMissionPoolData::get_authority() const {
return this->authority;
}

void UpdateMissionPoolData::set_delegateAuthority(const String& val) {
this->delegateAuthority = val;
}

String UpdateMissionPoolData::get_delegateAuthority() const {
return this->delegateAuthority;
}

void UpdateMissionPoolData::set_payer(const String& val) {
this->payer = val;
}

String UpdateMissionPoolData::get_payer() const {
return this->payer;
}

void UpdateMissionPoolData::_bind_methods() {
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
}
Dictionary UpdateMissionPoolData::to_dict() {
Dictionary res;
if (!project.is_empty()) {
    res["project"] = project;
}
if (!missionPool.is_empty()) {
    res["missionPool"] = missionPool;
}
if (!characterModel.is_empty()) {
    res["characterModel"] = characterModel;
}
if (!authority.is_empty()) {
    res["authority"] = authority;
}
if (!delegateAuthority.is_empty()) {
    res["delegateAuthority"] = delegateAuthority;
}
if (!payer.is_empty()) {
    res["payer"] = payer;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot