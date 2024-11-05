#include "honeycomb/types/ClaimBadgeCriteriaInput.hpp"

#include "pubkey.hpp"
#include "pubkey.hpp"
#include "pubkey.hpp"
namespace godot{
namespace honeycomb_resource{

void ClaimBadgeCriteriaInput::set_projectAddress(const Variant& val){
this->projectAddress = val;
}

Variant ClaimBadgeCriteriaInput::get_projectAddress(){
return this->projectAddress;
}

void ClaimBadgeCriteriaInput::set_profileAddress(const Variant& val){
this->profileAddress = val;
}

Variant ClaimBadgeCriteriaInput::get_profileAddress(){
return this->profileAddress;
}

void ClaimBadgeCriteriaInput::set_payer(const Variant& val){
this->payer = val;
}

Variant ClaimBadgeCriteriaInput::get_payer(){
return this->payer;
}

void ClaimBadgeCriteriaInput::set_criteriaIndex(const int32_t& val){
this->criteriaIndex = val;
}

int32_t ClaimBadgeCriteriaInput::get_criteriaIndex(){
return this->criteriaIndex;
}

void ClaimBadgeCriteriaInput::set_proof(const int32_t& val){
this->proof = val;
}

int32_t ClaimBadgeCriteriaInput::get_proof(){
return this->proof;
}

Dictionary ClaimBadgeCriteriaInput::to_dict(){
Dictionary res;
res["projectAddress"] = Object::cast_to<Pubkey>(projectAddress)->to_string();
res["profileAddress"] = Object::cast_to<Pubkey>(profileAddress)->to_string();
res["payer"] = Object::cast_to<Pubkey>(payer)->to_string();
res["criteriaIndex"] = criteriaIndex;
res["proof"] = proof;
return res;
}

void ClaimBadgeCriteriaInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_projectAddress"), &ClaimBadgeCriteriaInput::get_projectAddress);
ClassDB::bind_method(D_METHOD("set_projectAddress", "value"), &ClaimBadgeCriteriaInput::set_projectAddress);
ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "projectAddress"), "set_projectAddress", "get_projectAddress");
ClassDB::bind_method(D_METHOD("get_profileAddress"), &ClaimBadgeCriteriaInput::get_profileAddress);
ClassDB::bind_method(D_METHOD("set_profileAddress", "value"), &ClaimBadgeCriteriaInput::set_profileAddress);
ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "profileAddress"), "set_profileAddress", "get_profileAddress");
ClassDB::bind_method(D_METHOD("get_payer"), &ClaimBadgeCriteriaInput::get_payer);
ClassDB::bind_method(D_METHOD("set_payer", "value"), &ClaimBadgeCriteriaInput::set_payer);
ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "payer"), "set_payer", "get_payer");
ClassDB::bind_method(D_METHOD("get_criteriaIndex"), &ClaimBadgeCriteriaInput::get_criteriaIndex);
ClassDB::bind_method(D_METHOD("set_criteriaIndex", "value"), &ClaimBadgeCriteriaInput::set_criteriaIndex);
ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "criteriaIndex"), "set_criteriaIndex", "get_criteriaIndex");
ClassDB::bind_method(D_METHOD("get_proof"), &ClaimBadgeCriteriaInput::get_proof);
ClassDB::bind_method(D_METHOD("set_proof", "value"), &ClaimBadgeCriteriaInput::set_proof);
ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "proof"), "set_proof", "get_proof");
}
} // honeycomb_resource
} // godot