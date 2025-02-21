#include "honeycomb/types/UpdateBadgeCriteriaInput.hpp"

#include "honeycomb/enums.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void UpdateBadgeCriteriaInput::set_projectAddress(const Variant& val) {
this->projectAddress = val;
}

Variant UpdateBadgeCriteriaInput::get_projectAddress() const {
return this->projectAddress;
}

void UpdateBadgeCriteriaInput::set_authority(const Variant& val) {
this->authority = val;
}

Variant UpdateBadgeCriteriaInput::get_authority() const {
return this->authority;
}

void UpdateBadgeCriteriaInput::set_criteriaIndex(const int32_t& val) {
this->criteriaIndex = val;
}

int32_t UpdateBadgeCriteriaInput::get_criteriaIndex() const {
return this->criteriaIndex;
}

void UpdateBadgeCriteriaInput::set_payer(const Variant& val) {
this->payer = val;
}

Variant UpdateBadgeCriteriaInput::get_payer() const {
return this->payer;
}

void UpdateBadgeCriteriaInput::set_startTime(const int32_t& val) {
this->startTime = val;
}

int32_t UpdateBadgeCriteriaInput::get_startTime() const {
return this->startTime;
}

void UpdateBadgeCriteriaInput::set_endTime(const int32_t& val) {
this->endTime = val;
}

int32_t UpdateBadgeCriteriaInput::get_endTime() const {
return this->endTime;
}

void UpdateBadgeCriteriaInput::set_condition(const String& val) {
    this->condition = val;
}

String UpdateBadgeCriteriaInput::get_condition() const {
    return this->condition;
}

void UpdateBadgeCriteriaInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_projectAddress"), &UpdateBadgeCriteriaInput::get_projectAddress);
ClassDB::bind_method(D_METHOD("set_projectAddress", "value"), &UpdateBadgeCriteriaInput::set_projectAddress);
ClassDB::add_property("UpdateBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "projectAddress"), "set_projectAddress", "get_projectAddress");
ClassDB::bind_method(D_METHOD("get_authority"), &UpdateBadgeCriteriaInput::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &UpdateBadgeCriteriaInput::set_authority);
ClassDB::add_property("UpdateBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_criteriaIndex"), &UpdateBadgeCriteriaInput::get_criteriaIndex);
ClassDB::bind_method(D_METHOD("set_criteriaIndex", "value"), &UpdateBadgeCriteriaInput::set_criteriaIndex);
ClassDB::add_property("UpdateBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "criteriaIndex"), "set_criteriaIndex", "get_criteriaIndex");
ClassDB::bind_method(D_METHOD("get_payer"), &UpdateBadgeCriteriaInput::get_payer);
ClassDB::bind_method(D_METHOD("set_payer", "value"), &UpdateBadgeCriteriaInput::set_payer);
ClassDB::add_property("UpdateBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "payer"), "set_payer", "get_payer");
ClassDB::bind_method(D_METHOD("get_startTime"), &UpdateBadgeCriteriaInput::get_startTime);
ClassDB::bind_method(D_METHOD("set_startTime", "value"), &UpdateBadgeCriteriaInput::set_startTime);
ClassDB::add_property("UpdateBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "startTime"), "set_startTime", "get_startTime");
ClassDB::bind_method(D_METHOD("get_endTime"), &UpdateBadgeCriteriaInput::get_endTime);
ClassDB::bind_method(D_METHOD("set_endTime", "value"), &UpdateBadgeCriteriaInput::set_endTime);
ClassDB::add_property("UpdateBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
ClassDB::bind_method(D_METHOD("get_condition"), &UpdateBadgeCriteriaInput::get_condition);
ClassDB::bind_method(D_METHOD("set_condition", "value"), &UpdateBadgeCriteriaInput::set_condition);
ClassDB::add_property("UpdateBadgeCriteriaInput", PropertyInfo(Variant::STRING, "condition"), "set_condition", "get_condition");
ClassDB::bind_method(D_METHOD("to_dict"), &UpdateBadgeCriteriaInput::to_dict);
}
Dictionary UpdateBadgeCriteriaInput::to_dict() {
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
res["criteriaIndex"] = criteriaIndex;
if (payer.has_method("to_string")) {
res["payer"] = payer.call("to_string");
} else {
res["payer"] = payer;
}
res["startTime"] = startTime;
res["endTime"] = endTime;
res["condition"] = this->condition;
return res;
}
} // namespace honeycomb_resource
} // namespace godot