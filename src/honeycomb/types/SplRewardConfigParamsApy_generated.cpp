#include "honeycomb/types/SplRewardConfigParamsApy_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void SplRewardConfigParamsApy::set_rewardTokenMint(const Variant &val) {
	this->rewardTokenMint = val;
}

Variant SplRewardConfigParamsApy::get_rewardTokenMint() const {
	return this->rewardTokenMint;
}

void SplRewardConfigParamsApy::set_rewardVault(const Variant &val) {
	this->rewardVault = val;
}

Variant SplRewardConfigParamsApy::get_rewardVault() const {
	return this->rewardVault;
}

void SplRewardConfigParamsApy::set_rewardsPerDuration(const int64_t &val) {
	this->rewardsPerDuration = val;
}

int64_t SplRewardConfigParamsApy::get_rewardsPerDuration() const {
	return this->rewardsPerDuration;
}

void SplRewardConfigParamsApy::set_rewardsDuration(const int64_t &val) {
	this->rewardsDuration = val;
}

int64_t SplRewardConfigParamsApy::get_rewardsDuration() const {
	return this->rewardsDuration;
}

void SplRewardConfigParamsApy::set_totalRewardAmount(const int64_t &val) {
	this->totalRewardAmount = val;
}

int64_t SplRewardConfigParamsApy::get_totalRewardAmount() const {
	return this->totalRewardAmount;
}

void SplRewardConfigParamsApy::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_rewardTokenMint"), &SplRewardConfigParamsApy::get_rewardTokenMint);
	ClassDB::bind_method(D_METHOD("set_rewardTokenMint", "value"), &SplRewardConfigParamsApy::set_rewardTokenMint);
	ClassDB::add_property("SplRewardConfigParamsApy", PropertyInfo(Variant::Type::OBJECT, "rewardTokenMint"), "set_rewardTokenMint", "get_rewardTokenMint");
	ClassDB::bind_method(D_METHOD("get_rewardVault"), &SplRewardConfigParamsApy::get_rewardVault);
	ClassDB::bind_method(D_METHOD("set_rewardVault", "value"), &SplRewardConfigParamsApy::set_rewardVault);
	ClassDB::add_property("SplRewardConfigParamsApy", PropertyInfo(Variant::Type::OBJECT, "rewardVault"), "set_rewardVault", "get_rewardVault");
	ClassDB::bind_method(D_METHOD("get_rewardsPerDuration"), &SplRewardConfigParamsApy::get_rewardsPerDuration);
	ClassDB::bind_method(D_METHOD("set_rewardsPerDuration", "value"), &SplRewardConfigParamsApy::set_rewardsPerDuration);
	ClassDB::add_property("SplRewardConfigParamsApy", PropertyInfo(Variant::Type::INT, "rewardsPerDuration"), "set_rewardsPerDuration", "get_rewardsPerDuration");
	ClassDB::bind_method(D_METHOD("get_rewardsDuration"), &SplRewardConfigParamsApy::get_rewardsDuration);
	ClassDB::bind_method(D_METHOD("set_rewardsDuration", "value"), &SplRewardConfigParamsApy::set_rewardsDuration);
	ClassDB::add_property("SplRewardConfigParamsApy", PropertyInfo(Variant::Type::INT, "rewardsDuration"), "set_rewardsDuration", "get_rewardsDuration");
	ClassDB::bind_method(D_METHOD("get_totalRewardAmount"), &SplRewardConfigParamsApy::get_totalRewardAmount);
	ClassDB::bind_method(D_METHOD("set_totalRewardAmount", "value"), &SplRewardConfigParamsApy::set_totalRewardAmount);
	ClassDB::add_property("SplRewardConfigParamsApy", PropertyInfo(Variant::Type::INT, "totalRewardAmount"), "set_totalRewardAmount", "get_totalRewardAmount");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplRewardConfigParamsApy::to_dict);
}
Dictionary SplRewardConfigParamsApy::to_dict() {
	Dictionary res;
	if (rewardTokenMint.has_method("to_string")) {
		res["rewardTokenMint"] = rewardTokenMint.call("to_string");
	} else {
		res["rewardTokenMint"] = rewardTokenMint;
	}
	if (rewardVault.has_method("to_string")) {
		res["rewardVault"] = rewardVault.call("to_string");
	} else {
		res["rewardVault"] = rewardVault;
	}
	res["rewardsPerDuration"] = rewardsPerDuration;
	res["rewardsDuration"] = rewardsDuration;
	res["totalRewardAmount"] = totalRewardAmount;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot