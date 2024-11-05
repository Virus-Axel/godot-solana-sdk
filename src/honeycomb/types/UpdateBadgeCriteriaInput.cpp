#include "honeycomb/types/UpdateBadgeCriteriaInput.hpp"

#include "pubkey.hpp"
#include "pubkey.hpp"
#include "pubkey.hpp"
namespace godot{
namespace honeycomb_resource{

void UpdateBadgeCriteriaInput::set_projectAddress(const Variant& val){
this->projectAddress = val;
}

Variant UpdateBadgeCriteriaInput::get_projectAddress(){
return this->projectAddress;
}

void UpdateBadgeCriteriaInput::set_authority(const Variant& val){
this->authority = val;
}

Variant UpdateBadgeCriteriaInput::get_authority(){
return this->authority;
}

void UpdateBadgeCriteriaInput::set_criteriaIndex(const int32_t& val){
this->criteriaIndex = val;
}

int32_t UpdateBadgeCriteriaInput::get_criteriaIndex(){
return this->criteriaIndex;
}

void UpdateBadgeCriteriaInput::set_payer(const Variant& val){
this->payer = val;
}

Variant UpdateBadgeCriteriaInput::get_payer(){
return this->payer;
}

void UpdateBadgeCriteriaInput::set_startTime(const int32_t& val){
this->startTime = val;
}

int32_t UpdateBadgeCriteriaInput::get_startTime(){
return this->startTime;
}

void UpdateBadgeCriteriaInput::set_endTime(const int32_t& val){
this->endTime = val;
}

int32_t UpdateBadgeCriteriaInput::get_endTime(){
return this->endTime;
}

void UpdateBadgeCriteriaInput::set_condition(const int32_t& val){
this->condition = val;
}

int32_t UpdateBadgeCriteriaInput::get_condition(){
return this->condition;
}

Dictionary UpdateBadgeCriteriaInput::to_dict(){
Dictionary res;
res["projectAddress"] = Object::cast_to<Pubkey>(projectAddress)->to_string();
res["authority"] = Object::cast_to<Pubkey>(authority)->to_string();
res["criteriaIndex"] = criteriaIndex;
res["payer"] = Object::cast_to<Pubkey>(payer)->to_string();
res["startTime"] = startTime;
res["endTime"] = endTime;
res["condition"] = condition;
return res;
}

void UpdateBadgeCriteriaInput::_bind_methods(){
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
ClassDB::add_property("UpdateBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "condition"), "set_condition", "get_condition");
}
} // honeycomb_resource
} // godot