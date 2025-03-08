#include "honeycomb/types/PlatformData_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void PlatformData::set_custom(const Array &val) {
	this->custom = val;
}

Array PlatformData::get_custom() const {
	return this->custom;
}

void PlatformData::set_xp(const int64_t &val) {
	this->xp = val;
}

int64_t PlatformData::get_xp() const {
	return this->xp;
}

void PlatformData::set_achievements(const PackedInt32Array &val) {
	this->achievements = val;
}

PackedInt32Array PlatformData::get_achievements() const {
	return this->achievements;
}

void PlatformData::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_custom"), &PlatformData::get_custom);
	ClassDB::bind_method(D_METHOD("set_custom", "value"), &PlatformData::set_custom);
	ClassDB::add_property("PlatformData", PropertyInfo(Variant::Type::ARRAY, "custom"), "set_custom", "get_custom");
	ClassDB::bind_method(D_METHOD("get_xp"), &PlatformData::get_xp);
	ClassDB::bind_method(D_METHOD("set_xp", "value"), &PlatformData::set_xp);
	ClassDB::add_property("PlatformData", PropertyInfo(Variant::Type::INT, "xp"), "set_xp", "get_xp");
	ClassDB::bind_method(D_METHOD("get_achievements"), &PlatformData::get_achievements);
	ClassDB::bind_method(D_METHOD("set_achievements", "value"), &PlatformData::set_achievements);
	ClassDB::add_property("PlatformData", PropertyInfo(Variant::Type::PACKED_INT32_ARRAY, "achievements"), "set_achievements", "get_achievements");
	ClassDB::bind_method(D_METHOD("to_dict"), &PlatformData::to_dict);
}
Dictionary PlatformData::to_dict() {
	Dictionary res;
	res["custom"] = custom;
	res["xp"] = xp;
	res["achievements"] = achievements;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot