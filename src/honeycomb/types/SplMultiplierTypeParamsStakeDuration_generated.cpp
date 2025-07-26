#include "honeycomb/types/SplMultiplierTypeParamsStakeDuration_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SplMultiplierTypeParamsStakeDuration::set_minDuration(const int64_t &val) {
	this->minDuration = val;
}

int64_t SplMultiplierTypeParamsStakeDuration::get_minDuration() const {
	return this->minDuration;
}

void SplMultiplierTypeParamsStakeDuration::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_minDuration"), &SplMultiplierTypeParamsStakeDuration::get_minDuration);
	ClassDB::bind_method(D_METHOD("set_minDuration", "value"), &SplMultiplierTypeParamsStakeDuration::set_minDuration);
	ClassDB::add_property("SplMultiplierTypeParamsStakeDuration", PropertyInfo(Variant::Type::INT, "minDuration"), "set_minDuration", "get_minDuration");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplMultiplierTypeParamsStakeDuration::to_dict);
}
Dictionary SplMultiplierTypeParamsStakeDuration::to_dict() {
	Dictionary res;
	res["minDuration"] = minDuration;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot