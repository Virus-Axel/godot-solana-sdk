#include "honeycomb/types/EarnedRewards_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void EarnedRewards::set_delta(const int32_t &val) {
	this->delta = val;
}

int32_t EarnedRewards::get_delta() const {
	return this->delta;
}

void EarnedRewards::set_rewardIdx(const int32_t &val) {
	this->rewardIdx = val;
}

int32_t EarnedRewards::get_rewardIdx() const {
	return this->rewardIdx;
}

void EarnedRewards::set_collected(const bool &val) {
	this->collected = val;
}

bool EarnedRewards::get_collected() const {
	return this->collected;
}

void EarnedRewards::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_delta"), &EarnedRewards::get_delta);
	ClassDB::bind_method(D_METHOD("set_delta", "value"), &EarnedRewards::set_delta);
	ClassDB::add_property("EarnedRewards", PropertyInfo(Variant::Type::INT, "delta"), "set_delta", "get_delta");
	ClassDB::bind_method(D_METHOD("get_rewardIdx"), &EarnedRewards::get_rewardIdx);
	ClassDB::bind_method(D_METHOD("set_rewardIdx", "value"), &EarnedRewards::set_rewardIdx);
	ClassDB::add_property("EarnedRewards", PropertyInfo(Variant::Type::INT, "rewardIdx"), "set_rewardIdx", "get_rewardIdx");
	ClassDB::bind_method(D_METHOD("get_collected"), &EarnedRewards::get_collected);
	ClassDB::bind_method(D_METHOD("set_collected", "value"), &EarnedRewards::set_collected);
	ClassDB::add_property("EarnedRewards", PropertyInfo(Variant::Type::BOOL, "collected"), "set_collected", "get_collected");
	ClassDB::bind_method(D_METHOD("to_dict"), &EarnedRewards::to_dict);
}
Dictionary EarnedRewards::to_dict() {
	Dictionary res;
	res["delta"] = delta;
	res["rewardIdx"] = rewardIdx;
	res["collected"] = collected;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot