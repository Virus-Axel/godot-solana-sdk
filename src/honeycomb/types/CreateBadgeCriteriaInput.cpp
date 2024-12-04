#include "honeycomb/types/CreateBadgeCriteriaInput.hpp"

#include "pubkey.hpp"
#include "pubkey.hpp"
#include "pubkey.hpp"
namespace godot{
namespace honeycomb_resource{

void CreateBadgeCriteriaInput::set_projectAddress(const Variant& val){
this->projectAddress = val;
}

Variant CreateBadgeCriteriaInput::get_projectAddress(){
return this->projectAddress;
}

void CreateBadgeCriteriaInput::set_authority(const Variant& val){
this->authority = val;
}

Variant CreateBadgeCriteriaInput::get_authority(){
return this->authority;
}

void CreateBadgeCriteriaInput::set_payer(const Variant& val){
this->payer = val;
}

Variant CreateBadgeCriteriaInput::get_payer(){
return this->payer;
}

void CreateBadgeCriteriaInput::set_startTime(const int32_t& val){
this->startTime = val;
}

int32_t CreateBadgeCriteriaInput::get_startTime(){
return this->startTime;
}

void CreateBadgeCriteriaInput::set_endTime(const int32_t& val){
this->endTime = val;
}

int32_t CreateBadgeCriteriaInput::get_endTime(){
return this->endTime;
}

void CreateBadgeCriteriaInput::set_badgeIndex(const int32_t& val){
this->badgeIndex = val;
}

int32_t CreateBadgeCriteriaInput::get_badgeIndex(){
return this->badgeIndex;
}

void CreateBadgeCriteriaInput::set_condition(const int32_t& val){
this->condition = val;
}

int32_t CreateBadgeCriteriaInput::get_condition(){
return this->condition;
}

Dictionary CreateBadgeCriteriaInput::to_dict(){
Dictionary res;
res["projectAddress"] = Object::cast_to<Pubkey>(projectAddress)->to_string();
res["authority"] = Object::cast_to<Pubkey>(authority)->to_string();
res["payer"] = Object::cast_to<Pubkey>(payer)->to_string();
res["startTime"] = startTime;
res["endTime"] = endTime;
res["badgeIndex"] = badgeIndex;
res["condition"] = condition;
return res;
}

void CreateBadgeCriteriaInput::from_dict(const Dictionary& dict){
Object::cast_to<Pubkey>(projectAddress)->from_string(dict["projectAddress"]);
Object::cast_to<Pubkey>(authority)->from_string(dict["authority"]);
Object::cast_to<Pubkey>(payer)->from_string(dict["payer"]);
startTime = dict["startTime"];
endTime = dict["endTime"];
badgeIndex = dict["badgeIndex"];
condition = dict["condition"];
}

void CreateBadgeCriteriaInput::_bind_methods(){
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
ClassDB::add_property("CreateBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "condition"), "set_condition", "get_condition");
}
} // honeycomb_resource
} // godot