#include "honeycomb/types/CreateStakingPoolMetadataInput.hpp"

namespace godot{
namespace honeycomb_resource{

void CreateStakingPoolMetadataInput::set_name(const String& val){
this->name = val;
}

String CreateStakingPoolMetadataInput::get_name(){
return this->name;
}

void CreateStakingPoolMetadataInput::set_rewardsPerDuration(const int64_t& val){
this->rewardsPerDuration = val;
}

int64_t CreateStakingPoolMetadataInput::get_rewardsPerDuration(){
return this->rewardsPerDuration;
}

void CreateStakingPoolMetadataInput::set_rewardsDuration(const int64_t& val){
this->rewardsDuration = val;
}

int64_t CreateStakingPoolMetadataInput::get_rewardsDuration(){
return this->rewardsDuration;
}

void CreateStakingPoolMetadataInput::set_maxRewardsDuration(const int64_t& val){
this->maxRewardsDuration = val;
}

int64_t CreateStakingPoolMetadataInput::get_maxRewardsDuration(){
return this->maxRewardsDuration;
}

void CreateStakingPoolMetadataInput::set_minStakeDuration(const int64_t& val){
this->minStakeDuration = val;
}

int64_t CreateStakingPoolMetadataInput::get_minStakeDuration(){
return this->minStakeDuration;
}

void CreateStakingPoolMetadataInput::set_cooldownDuration(const int64_t& val){
this->cooldownDuration = val;
}

int64_t CreateStakingPoolMetadataInput::get_cooldownDuration(){
return this->cooldownDuration;
}

void CreateStakingPoolMetadataInput::set_resetStakeDuration(const bool& val){
this->resetStakeDuration = val;
}

bool CreateStakingPoolMetadataInput::get_resetStakeDuration(){
return this->resetStakeDuration;
}

void CreateStakingPoolMetadataInput::set_startTime(const int64_t& val){
this->startTime = val;
}

int64_t CreateStakingPoolMetadataInput::get_startTime(){
return this->startTime;
}

void CreateStakingPoolMetadataInput::set_lockType(const int32_t& val){
this->lockType = val;
}

int32_t CreateStakingPoolMetadataInput::get_lockType(){
return this->lockType;
}

void CreateStakingPoolMetadataInput::set_endTime(const int64_t& val){
this->endTime = val;
}

int64_t CreateStakingPoolMetadataInput::get_endTime(){
return this->endTime;
}

Dictionary CreateStakingPoolMetadataInput::to_dict(){
Dictionary res;
res["name"] = name;
res["rewardsPerDuration"] = rewardsPerDuration;
res["rewardsDuration"] = rewardsDuration;
res["maxRewardsDuration"] = maxRewardsDuration;
res["minStakeDuration"] = minStakeDuration;
res["cooldownDuration"] = cooldownDuration;
res["resetStakeDuration"] = resetStakeDuration;
res["startTime"] = startTime;
res["lockType"] = lockType;
res["endTime"] = endTime;
return res;
}

void CreateStakingPoolMetadataInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_name"), &CreateStakingPoolMetadataInput::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &CreateStakingPoolMetadataInput::set_name);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_rewardsPerDuration"), &CreateStakingPoolMetadataInput::get_rewardsPerDuration);
ClassDB::bind_method(D_METHOD("set_rewardsPerDuration", "value"), &CreateStakingPoolMetadataInput::set_rewardsPerDuration);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "rewardsPerDuration"), "set_rewardsPerDuration", "get_rewardsPerDuration");
ClassDB::bind_method(D_METHOD("get_rewardsDuration"), &CreateStakingPoolMetadataInput::get_rewardsDuration);
ClassDB::bind_method(D_METHOD("set_rewardsDuration", "value"), &CreateStakingPoolMetadataInput::set_rewardsDuration);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "rewardsDuration"), "set_rewardsDuration", "get_rewardsDuration");
ClassDB::bind_method(D_METHOD("get_maxRewardsDuration"), &CreateStakingPoolMetadataInput::get_maxRewardsDuration);
ClassDB::bind_method(D_METHOD("set_maxRewardsDuration", "value"), &CreateStakingPoolMetadataInput::set_maxRewardsDuration);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "maxRewardsDuration"), "set_maxRewardsDuration", "get_maxRewardsDuration");
ClassDB::bind_method(D_METHOD("get_minStakeDuration"), &CreateStakingPoolMetadataInput::get_minStakeDuration);
ClassDB::bind_method(D_METHOD("set_minStakeDuration", "value"), &CreateStakingPoolMetadataInput::set_minStakeDuration);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "minStakeDuration"), "set_minStakeDuration", "get_minStakeDuration");
ClassDB::bind_method(D_METHOD("get_cooldownDuration"), &CreateStakingPoolMetadataInput::get_cooldownDuration);
ClassDB::bind_method(D_METHOD("set_cooldownDuration", "value"), &CreateStakingPoolMetadataInput::set_cooldownDuration);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "cooldownDuration"), "set_cooldownDuration", "get_cooldownDuration");
ClassDB::bind_method(D_METHOD("get_resetStakeDuration"), &CreateStakingPoolMetadataInput::get_resetStakeDuration);
ClassDB::bind_method(D_METHOD("set_resetStakeDuration", "value"), &CreateStakingPoolMetadataInput::set_resetStakeDuration);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::BOOL, "resetStakeDuration"), "set_resetStakeDuration", "get_resetStakeDuration");
ClassDB::bind_method(D_METHOD("get_startTime"), &CreateStakingPoolMetadataInput::get_startTime);
ClassDB::bind_method(D_METHOD("set_startTime", "value"), &CreateStakingPoolMetadataInput::set_startTime);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "startTime"), "set_startTime", "get_startTime");
ClassDB::bind_method(D_METHOD("get_lockType"), &CreateStakingPoolMetadataInput::get_lockType);
ClassDB::bind_method(D_METHOD("set_lockType", "value"), &CreateStakingPoolMetadataInput::set_lockType);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "lockType"), "set_lockType", "get_lockType");
ClassDB::bind_method(D_METHOD("get_endTime"), &CreateStakingPoolMetadataInput::get_endTime);
ClassDB::bind_method(D_METHOD("set_endTime", "value"), &CreateStakingPoolMetadataInput::set_endTime);
ClassDB::add_property("CreateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
}
} // honeycomb_resource
} // godot