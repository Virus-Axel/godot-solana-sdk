#include "honeycomb/types/EarnedRewardsFilter_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void EarnedRewardsFilter::set_delta(const int32_t &val) {
	this->delta = val;
}

int32_t EarnedRewardsFilter::get_delta() const {
	return this->delta;
}

void EarnedRewardsFilter::set_rewardIdx(const int32_t &val) {
	this->rewardIdx = val;
}

int32_t EarnedRewardsFilter::get_rewardIdx() const {
	return this->rewardIdx;
}

void EarnedRewardsFilter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_delta"), &EarnedRewardsFilter::get_delta);
	ClassDB::bind_method(D_METHOD("set_delta", "value"), &EarnedRewardsFilter::set_delta);
	ClassDB::add_property("EarnedRewardsFilter", PropertyInfo(Variant::Type::INT, "delta"), "set_delta", "get_delta");
	ClassDB::bind_method(D_METHOD("get_rewardIdx"), &EarnedRewardsFilter::get_rewardIdx);
	ClassDB::bind_method(D_METHOD("set_rewardIdx", "value"), &EarnedRewardsFilter::set_rewardIdx);
	ClassDB::add_property("EarnedRewardsFilter", PropertyInfo(Variant::Type::INT, "rewardIdx"), "set_rewardIdx", "get_rewardIdx");
	ClassDB::bind_method(D_METHOD("to_dict"), &EarnedRewardsFilter::to_dict);
}
Dictionary EarnedRewardsFilter::to_dict() {
	Dictionary res;
	res["delta"] = delta;
	res["rewardIdx"] = rewardIdx;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot