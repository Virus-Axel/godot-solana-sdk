#include "honeycomb/types/NewMissionPoolData_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void NewMissionPoolData::set_project(const String& val) {
this->project = val;
}

String NewMissionPoolData::get_project() const {
return this->project;
}

void NewMissionPoolData::set_authority(const String& val) {
this->authority = val;
}

String NewMissionPoolData::get_authority() const {
return this->authority;
}

void NewMissionPoolData::set_delegateAuthority(const String& val) {
this->delegateAuthority = val;
}

String NewMissionPoolData::get_delegateAuthority() const {
return this->delegateAuthority;
}

void NewMissionPoolData::set_payer(const String& val) {
this->payer = val;
}

String NewMissionPoolData::get_payer() const {
return this->payer;
}

void NewMissionPoolData::set_characterModel(const String& val) {
this->characterModel = val;
}

String NewMissionPoolData::get_characterModel() const {
return this->characterModel;
}

void NewMissionPoolData::set_name(const String& val) {
this->name = val;
}

String NewMissionPoolData::get_name() const {
return this->name;
}

void NewMissionPoolData::_bind_methods() {
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
}
Dictionary NewMissionPoolData::to_dict() {
Dictionary res;
if (!project.is_empty()) {
    res["project"] = project;
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
if (!characterModel.is_empty()) {
    res["characterModel"] = characterModel;
}
if (!name.is_empty()) {
    res["name"] = name;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot