#include "honeycomb/types/UpdateStakingPoolMetadataInput.hpp"


namespace godot {
namespace honeycomb_resource {

void UpdateStakingPoolMetadataInput::set_name(const String& val) {
this->name = val;
}

String UpdateStakingPoolMetadataInput::get_name() const {
return this->name;
}

void UpdateStakingPoolMetadataInput::set_rewardsPerDuration(const int64_t& val) {
this->rewardsPerDuration = val;
}

int64_t UpdateStakingPoolMetadataInput::get_rewardsPerDuration() const {
return this->rewardsPerDuration;
}

void UpdateStakingPoolMetadataInput::set_rewardsDuration(const int64_t& val) {
this->rewardsDuration = val;
}

int64_t UpdateStakingPoolMetadataInput::get_rewardsDuration() const {
return this->rewardsDuration;
}

void UpdateStakingPoolMetadataInput::set_maxRewardsDuration(const int64_t& val) {
this->maxRewardsDuration = val;
}

int64_t UpdateStakingPoolMetadataInput::get_maxRewardsDuration() const {
return this->maxRewardsDuration;
}

void UpdateStakingPoolMetadataInput::set_minStakeDuration(const int64_t& val) {
this->minStakeDuration = val;
}

int64_t UpdateStakingPoolMetadataInput::get_minStakeDuration() const {
return this->minStakeDuration;
}

void UpdateStakingPoolMetadataInput::set_cooldownDuration(const int64_t& val) {
this->cooldownDuration = val;
}

int64_t UpdateStakingPoolMetadataInput::get_cooldownDuration() const {
return this->cooldownDuration;
}

void UpdateStakingPoolMetadataInput::set_resetStakeDuration(const bool& val) {
this->resetStakeDuration = val;
}

bool UpdateStakingPoolMetadataInput::get_resetStakeDuration() const {
return this->resetStakeDuration;
}

void UpdateStakingPoolMetadataInput::set_startTime(const int64_t& val) {
this->startTime = val;
}

int64_t UpdateStakingPoolMetadataInput::get_startTime() const {
return this->startTime;
}

void UpdateStakingPoolMetadataInput::set_endTime(const int64_t& val) {
this->endTime = val;
}

int64_t UpdateStakingPoolMetadataInput::get_endTime() const {
return this->endTime;
}

void UpdateStakingPoolMetadataInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_name"), &UpdateStakingPoolMetadataInput::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &UpdateStakingPoolMetadataInput::set_name);
ClassDB::add_property("UpdateStakingPoolMetadataInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_rewardsPerDuration"), &UpdateStakingPoolMetadataInput::get_rewardsPerDuration);
ClassDB::bind_method(D_METHOD("set_rewardsPerDuration", "value"), &UpdateStakingPoolMetadataInput::set_rewardsPerDuration);
ClassDB::add_property("UpdateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "rewardsPerDuration"), "set_rewardsPerDuration", "get_rewardsPerDuration");
ClassDB::bind_method(D_METHOD("get_rewardsDuration"), &UpdateStakingPoolMetadataInput::get_rewardsDuration);
ClassDB::bind_method(D_METHOD("set_rewardsDuration", "value"), &UpdateStakingPoolMetadataInput::set_rewardsDuration);
ClassDB::add_property("UpdateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "rewardsDuration"), "set_rewardsDuration", "get_rewardsDuration");
ClassDB::bind_method(D_METHOD("get_maxRewardsDuration"), &UpdateStakingPoolMetadataInput::get_maxRewardsDuration);
ClassDB::bind_method(D_METHOD("set_maxRewardsDuration", "value"), &UpdateStakingPoolMetadataInput::set_maxRewardsDuration);
ClassDB::add_property("UpdateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "maxRewardsDuration"), "set_maxRewardsDuration", "get_maxRewardsDuration");
ClassDB::bind_method(D_METHOD("get_minStakeDuration"), &UpdateStakingPoolMetadataInput::get_minStakeDuration);
ClassDB::bind_method(D_METHOD("set_minStakeDuration", "value"), &UpdateStakingPoolMetadataInput::set_minStakeDuration);
ClassDB::add_property("UpdateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "minStakeDuration"), "set_minStakeDuration", "get_minStakeDuration");
ClassDB::bind_method(D_METHOD("get_cooldownDuration"), &UpdateStakingPoolMetadataInput::get_cooldownDuration);
ClassDB::bind_method(D_METHOD("set_cooldownDuration", "value"), &UpdateStakingPoolMetadataInput::set_cooldownDuration);
ClassDB::add_property("UpdateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "cooldownDuration"), "set_cooldownDuration", "get_cooldownDuration");
ClassDB::bind_method(D_METHOD("get_resetStakeDuration"), &UpdateStakingPoolMetadataInput::get_resetStakeDuration);
ClassDB::bind_method(D_METHOD("set_resetStakeDuration", "value"), &UpdateStakingPoolMetadataInput::set_resetStakeDuration);
ClassDB::add_property("UpdateStakingPoolMetadataInput", PropertyInfo(Variant::Type::BOOL, "resetStakeDuration"), "set_resetStakeDuration", "get_resetStakeDuration");
ClassDB::bind_method(D_METHOD("get_startTime"), &UpdateStakingPoolMetadataInput::get_startTime);
ClassDB::bind_method(D_METHOD("set_startTime", "value"), &UpdateStakingPoolMetadataInput::set_startTime);
ClassDB::add_property("UpdateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "startTime"), "set_startTime", "get_startTime");
ClassDB::bind_method(D_METHOD("get_endTime"), &UpdateStakingPoolMetadataInput::get_endTime);
ClassDB::bind_method(D_METHOD("set_endTime", "value"), &UpdateStakingPoolMetadataInput::set_endTime);
ClassDB::add_property("UpdateStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
ClassDB::bind_method(D_METHOD("to_dict"), &UpdateStakingPoolMetadataInput::to_dict);
}
Dictionary UpdateStakingPoolMetadataInput::to_dict() {
Dictionary res;
if (!name.is_empty()) {
    res["name"] = name;
}
res["rewardsPerDuration"] = rewardsPerDuration;
res["rewardsDuration"] = rewardsDuration;
res["maxRewardsDuration"] = maxRewardsDuration;
res["minStakeDuration"] = minStakeDuration;
res["cooldownDuration"] = cooldownDuration;
res["resetStakeDuration"] = resetStakeDuration;
res["startTime"] = startTime;
res["endTime"] = endTime;
return res;
}
} // namespace honeycomb_resource
} // namespace godot