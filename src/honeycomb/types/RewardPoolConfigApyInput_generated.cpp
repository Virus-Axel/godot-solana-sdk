#include "honeycomb/types/RewardPoolConfigApyInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void RewardPoolConfigApyInput::set_rewardsPerDuration(const int64_t &val) {
	this->rewardsPerDuration = val;
}

int64_t RewardPoolConfigApyInput::get_rewardsPerDuration() const {
	return this->rewardsPerDuration;
}

void RewardPoolConfigApyInput::set_rewardsDuration(const int64_t &val) {
	this->rewardsDuration = val;
}

int64_t RewardPoolConfigApyInput::get_rewardsDuration() const {
	return this->rewardsDuration;
}

void RewardPoolConfigApyInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_rewardsPerDuration"), &RewardPoolConfigApyInput::get_rewardsPerDuration);
	ClassDB::bind_method(D_METHOD("set_rewardsPerDuration", "value"), &RewardPoolConfigApyInput::set_rewardsPerDuration);
	ClassDB::add_property("RewardPoolConfigApyInput", PropertyInfo(Variant::Type::INT, "rewardsPerDuration"), "set_rewardsPerDuration", "get_rewardsPerDuration");
	ClassDB::bind_method(D_METHOD("get_rewardsDuration"), &RewardPoolConfigApyInput::get_rewardsDuration);
	ClassDB::bind_method(D_METHOD("set_rewardsDuration", "value"), &RewardPoolConfigApyInput::set_rewardsDuration);
	ClassDB::add_property("RewardPoolConfigApyInput", PropertyInfo(Variant::Type::INT, "rewardsDuration"), "set_rewardsDuration", "get_rewardsDuration");
	ClassDB::bind_method(D_METHOD("to_dict"), &RewardPoolConfigApyInput::to_dict);
}
Dictionary RewardPoolConfigApyInput::to_dict() {
	Dictionary res;
	res["rewardsPerDuration"] = rewardsPerDuration;
	res["rewardsDuration"] = rewardsDuration;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot