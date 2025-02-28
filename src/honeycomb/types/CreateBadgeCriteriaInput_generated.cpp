#include "honeycomb/types/CreateBadgeCriteriaInput_generated.hpp"

#include "honeycomb/enums_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateBadgeCriteriaInput::set_projectAddress(const Variant& val) {
this->projectAddress = val;
}

Variant CreateBadgeCriteriaInput::get_projectAddress() const {
return this->projectAddress;
}

void CreateBadgeCriteriaInput::set_authority(const Variant& val) {
this->authority = val;
}

Variant CreateBadgeCriteriaInput::get_authority() const {
return this->authority;
}

void CreateBadgeCriteriaInput::set_payer(const Variant& val) {
this->payer = val;
}

Variant CreateBadgeCriteriaInput::get_payer() const {
return this->payer;
}

void CreateBadgeCriteriaInput::set_startTime(const int32_t& val) {
this->startTime = val;
}

int32_t CreateBadgeCriteriaInput::get_startTime() const {
return this->startTime;
}

void CreateBadgeCriteriaInput::set_endTime(const int32_t& val) {
this->endTime = val;
}

int32_t CreateBadgeCriteriaInput::get_endTime() const {
return this->endTime;
}

void CreateBadgeCriteriaInput::set_badgeIndex(const int32_t& val) {
this->badgeIndex = val;
}

int32_t CreateBadgeCriteriaInput::get_badgeIndex() const {
return this->badgeIndex;
}

void CreateBadgeCriteriaInput::set_condition(const String& val) {
    this->condition = val;
}

String CreateBadgeCriteriaInput::get_condition() const {
    return this->condition;
}

void CreateBadgeCriteriaInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_projectAddress"), &CreateBadgeCriteriaInput::get_projectAddress);
ClassDB::bind_method(D_METHOD("set_projectAddress", "value"), &CreateBadgeCriteriaInput::set_projectAddress);
ClassDB::add_property("CreateBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "projectAddress"), "set_projectAddress", "get_projectAddress");
ClassDB::bind_method(D_METHOD("get_authority"), &CreateBadgeCriteriaInput::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &CreateBadgeCriteriaInput::set_authority);
ClassDB::add_property("CreateBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_payer"), &CreateBadgeCriteriaInput::get_payer);
ClassDB::bind_method(D_METHOD("set_payer", "value"), &CreateBadgeCriteriaInput::set_payer);
ClassDB::add_property("CreateBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "payer"), "set_payer", "get_payer");
ClassDB::bind_method(D_METHOD("get_startTime"), &CreateBadgeCriteriaInput::get_startTime);
ClassDB::bind_method(D_METHOD("set_startTime", "value"), &CreateBadgeCriteriaInput::set_startTime);
ClassDB::add_property("CreateBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "startTime"), "set_startTime", "get_startTime");
ClassDB::bind_method(D_METHOD("get_endTime"), &CreateBadgeCriteriaInput::get_endTime);
ClassDB::bind_method(D_METHOD("set_endTime", "value"), &CreateBadgeCriteriaInput::set_endTime);
ClassDB::add_property("CreateBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
ClassDB::bind_method(D_METHOD("get_badgeIndex"), &CreateBadgeCriteriaInput::get_badgeIndex);
ClassDB::bind_method(D_METHOD("set_badgeIndex", "value"), &CreateBadgeCriteriaInput::set_badgeIndex);
ClassDB::add_property("CreateBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "badgeIndex"), "set_badgeIndex", "get_badgeIndex");
ClassDB::bind_method(D_METHOD("get_condition"), &CreateBadgeCriteriaInput::get_condition);
ClassDB::bind_method(D_METHOD("set_condition", "value"), &CreateBadgeCriteriaInput::set_condition);
ClassDB::add_property("CreateBadgeCriteriaInput", PropertyInfo(Variant::STRING, "condition"), "set_condition", "get_condition");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateBadgeCriteriaInput::to_dict);
}
Dictionary CreateBadgeCriteriaInput::to_dict() {
Dictionary res;
if (projectAddress.has_method("to_string")) {
res["projectAddress"] = projectAddress.call("to_string");
} else {
res["projectAddress"] = projectAddress;
}
if (authority.has_method("to_string")) {
res["authority"] = authority.call("to_string");
} else {
res["authority"] = authority;
}
if (payer.has_method("to_string")) {
res["payer"] = payer.call("to_string");
} else {
res["payer"] = payer;
}
res["startTime"] = startTime;
res["endTime"] = endTime;
res["badgeIndex"] = badgeIndex;
res["condition"] = this->condition;
return res;
}
} // namespace honeycomb_resource
} // namespace godot