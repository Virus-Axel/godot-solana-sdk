#include "honeycomb/types/TimeRequirementParams_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void TimeRequirementParams::set_duration(const int64_t &val) {
	this->duration = val;
}

int64_t TimeRequirementParams::get_duration() const {
	return this->duration;
}

void TimeRequirementParams::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_duration"), &TimeRequirementParams::get_duration);
	ClassDB::bind_method(D_METHOD("set_duration", "value"), &TimeRequirementParams::set_duration);
	ClassDB::add_property("TimeRequirementParams", PropertyInfo(Variant::Type::INT, "duration"), "set_duration", "get_duration");
	ClassDB::bind_method(D_METHOD("to_dict"), &TimeRequirementParams::to_dict);
}
Dictionary TimeRequirementParams::to_dict() {
	Dictionary res;
	res["duration"] = duration;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot