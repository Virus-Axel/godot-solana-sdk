#include "honeycomb/types/StakerClaimedParamsApy_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void StakerClaimedParamsApy::set_lastRewardTime(const int64_t &val) {
	this->lastRewardTime = val;
}

int64_t StakerClaimedParamsApy::get_lastRewardTime() const {
	return this->lastRewardTime;
}

void StakerClaimedParamsApy::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_lastRewardTime"), &StakerClaimedParamsApy::get_lastRewardTime);
	ClassDB::bind_method(D_METHOD("set_lastRewardTime", "value"), &StakerClaimedParamsApy::set_lastRewardTime);
	ClassDB::add_property("StakerClaimedParamsApy", PropertyInfo(Variant::Type::INT, "lastRewardTime"), "set_lastRewardTime", "get_lastRewardTime");
	ClassDB::bind_method(D_METHOD("to_dict"), &StakerClaimedParamsApy::to_dict);
}
Dictionary StakerClaimedParamsApy::to_dict() {
	Dictionary res;
	res["lastRewardTime"] = lastRewardTime;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot