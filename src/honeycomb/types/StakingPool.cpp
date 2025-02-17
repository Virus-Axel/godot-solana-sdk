#include "honeycomb/types/StakingPool.hpp"

#include "pubkey.hpp"
#include "honeycomb/types/LockType.hpp"

namespace godot {
namespace honeycomb_resource {

void StakingPool::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray StakingPool::get_address() const {
return this->address;
}

void StakingPool::set_bump(const int32_t& val) {
this->bump = val;
}

int32_t StakingPool::get_bump() const {
return this->bump;
}

void StakingPool::set_project(const Variant& val) {
this->project = val;
}

Variant StakingPool::get_project() const {
return this->project;
}

void StakingPool::set_key(const Variant& val) {
this->key = val;
}

Variant StakingPool::get_key() const {
return this->key;
}

void StakingPool::set_resource(const Variant& val) {
this->resource = val;
}

Variant StakingPool::get_resource() const {
return this->resource;
}

void StakingPool::set_lockType(const Variant& val) {
this->lockType = val;
}

Variant StakingPool::get_lockType() const {
return this->lockType;
}

void StakingPool::set_name(const String& val) {
this->name = val;
}

String StakingPool::get_name() const {
return this->name;
}

void StakingPool::set_rewardsPerDuration(const int64_t& val) {
this->rewardsPerDuration = val;
}

int64_t StakingPool::get_rewardsPerDuration() const {
return this->rewardsPerDuration;
}

void StakingPool::set_rewardsDuration(const int64_t& val) {
this->rewardsDuration = val;
}

int64_t StakingPool::get_rewardsDuration() const {
return this->rewardsDuration;
}

void StakingPool::set_maxRewardsDuration(const int64_t& val) {
this->maxRewardsDuration = val;
}

int64_t StakingPool::get_maxRewardsDuration() const {
return this->maxRewardsDuration;
}

void StakingPool::set_minStakeDuration(const int64_t& val) {
this->minStakeDuration = val;
}

int64_t StakingPool::get_minStakeDuration() const {
return this->minStakeDuration;
}

void StakingPool::set_cooldownDuration(const int64_t& val) {
this->cooldownDuration = val;
}

int64_t StakingPool::get_cooldownDuration() const {
return this->cooldownDuration;
}

void StakingPool::set_resetStakeDuration(const bool& val) {
this->resetStakeDuration = val;
}

bool StakingPool::get_resetStakeDuration() const {
return this->resetStakeDuration;
}

void StakingPool::set_allowedMints(const bool& val) {
this->allowedMints = val;
}

bool StakingPool::get_allowedMints() const {
return this->allowedMints;
}

void StakingPool::set_totalStaked(const int64_t& val) {
this->totalStaked = val;
}

int64_t StakingPool::get_totalStaked() const {
return this->totalStaked;
}

void StakingPool::set_startTime(const int64_t& val) {
this->startTime = val;
}

int64_t StakingPool::get_startTime() const {
return this->startTime;
}

void StakingPool::set_endTime(const int64_t& val) {
this->endTime = val;
}

int64_t StakingPool::get_endTime() const {
return this->endTime;
}

void StakingPool::set_characterModels(const Array& val) {
this->characterModels = val;
}

Array StakingPool::get_characterModels() const {
return this->characterModels;
}

void StakingPool::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &StakingPool::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &StakingPool::set_address);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_bump"), &StakingPool::get_bump);
ClassDB::bind_method(D_METHOD("set_bump", "value"), &StakingPool::set_bump);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
ClassDB::bind_method(D_METHOD("get_project"), &StakingPool::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &StakingPool::set_project);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_key"), &StakingPool::get_key);
ClassDB::bind_method(D_METHOD("set_key", "value"), &StakingPool::set_key);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::OBJECT, "key"), "set_key", "get_key");
ClassDB::bind_method(D_METHOD("get_resource"), &StakingPool::get_resource);
ClassDB::bind_method(D_METHOD("set_resource", "value"), &StakingPool::set_resource);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::OBJECT, "resource"), "set_resource", "get_resource");
ClassDB::bind_method(D_METHOD("get_lockType"), &StakingPool::get_lockType);
ClassDB::bind_method(D_METHOD("set_lockType", "value"), &StakingPool::set_lockType);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::OBJECT, "lockType"), "set_lockType", "get_lockType");
ClassDB::bind_method(D_METHOD("get_name"), &StakingPool::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &StakingPool::set_name);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_rewardsPerDuration"), &StakingPool::get_rewardsPerDuration);
ClassDB::bind_method(D_METHOD("set_rewardsPerDuration", "value"), &StakingPool::set_rewardsPerDuration);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::INT, "rewardsPerDuration"), "set_rewardsPerDuration", "get_rewardsPerDuration");
ClassDB::bind_method(D_METHOD("get_rewardsDuration"), &StakingPool::get_rewardsDuration);
ClassDB::bind_method(D_METHOD("set_rewardsDuration", "value"), &StakingPool::set_rewardsDuration);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::INT, "rewardsDuration"), "set_rewardsDuration", "get_rewardsDuration");
ClassDB::bind_method(D_METHOD("get_maxRewardsDuration"), &StakingPool::get_maxRewardsDuration);
ClassDB::bind_method(D_METHOD("set_maxRewardsDuration", "value"), &StakingPool::set_maxRewardsDuration);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::INT, "maxRewardsDuration"), "set_maxRewardsDuration", "get_maxRewardsDuration");
ClassDB::bind_method(D_METHOD("get_minStakeDuration"), &StakingPool::get_minStakeDuration);
ClassDB::bind_method(D_METHOD("set_minStakeDuration", "value"), &StakingPool::set_minStakeDuration);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::INT, "minStakeDuration"), "set_minStakeDuration", "get_minStakeDuration");
ClassDB::bind_method(D_METHOD("get_cooldownDuration"), &StakingPool::get_cooldownDuration);
ClassDB::bind_method(D_METHOD("set_cooldownDuration", "value"), &StakingPool::set_cooldownDuration);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::INT, "cooldownDuration"), "set_cooldownDuration", "get_cooldownDuration");
ClassDB::bind_method(D_METHOD("get_resetStakeDuration"), &StakingPool::get_resetStakeDuration);
ClassDB::bind_method(D_METHOD("set_resetStakeDuration", "value"), &StakingPool::set_resetStakeDuration);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::BOOL, "resetStakeDuration"), "set_resetStakeDuration", "get_resetStakeDuration");
ClassDB::bind_method(D_METHOD("get_allowedMints"), &StakingPool::get_allowedMints);
ClassDB::bind_method(D_METHOD("set_allowedMints", "value"), &StakingPool::set_allowedMints);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::BOOL, "allowedMints"), "set_allowedMints", "get_allowedMints");
ClassDB::bind_method(D_METHOD("get_totalStaked"), &StakingPool::get_totalStaked);
ClassDB::bind_method(D_METHOD("set_totalStaked", "value"), &StakingPool::set_totalStaked);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::INT, "totalStaked"), "set_totalStaked", "get_totalStaked");
ClassDB::bind_method(D_METHOD("get_startTime"), &StakingPool::get_startTime);
ClassDB::bind_method(D_METHOD("set_startTime", "value"), &StakingPool::set_startTime);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::INT, "startTime"), "set_startTime", "get_startTime");
ClassDB::bind_method(D_METHOD("get_endTime"), &StakingPool::get_endTime);
ClassDB::bind_method(D_METHOD("set_endTime", "value"), &StakingPool::set_endTime);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
ClassDB::bind_method(D_METHOD("get_characterModels"), &StakingPool::get_characterModels);
ClassDB::bind_method(D_METHOD("set_characterModels", "value"), &StakingPool::set_characterModels);
ClassDB::add_property("StakingPool", PropertyInfo(Variant::Type::ARRAY, "characterModels"), "set_characterModels", "get_characterModels");
ClassDB::bind_method(D_METHOD("to_dict"), &StakingPool::to_dict);
}
Dictionary StakingPool::to_dict() {
Dictionary res;
res["address"] = address;
res["bump"] = bump;
if (project.has_method("to_string")) {
res["project"] = project.call("to_string");
} else {
res["project"] = project;
}
if (key.has_method("to_string")) {
res["key"] = key.call("to_string");
} else {
res["key"] = key;
}
if (resource.has_method("to_string")) {
res["resource"] = resource.call("to_string");
} else {
res["resource"] = resource;
}

          if (lockType.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::LockType>(lockType);
              if (ptr) {
                  res["lockType"] = ptr->to_dict();
              }
          }
if (!name.is_empty()) {
    res["name"] = name;
}
res["rewardsPerDuration"] = rewardsPerDuration;
res["rewardsDuration"] = rewardsDuration;
res["maxRewardsDuration"] = maxRewardsDuration;
res["minStakeDuration"] = minStakeDuration;
res["cooldownDuration"] = cooldownDuration;
res["resetStakeDuration"] = resetStakeDuration;
res["allowedMints"] = allowedMints;
res["totalStaked"] = totalStaked;
res["startTime"] = startTime;
res["endTime"] = endTime;
res["characterModels"] = characterModels;
return res;
}
} // namespace honeycomb_resource
} // namespace godot