#include "honeycomb/types/StakerClaimedParamsNone_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void StakerClaimedParamsNone::set_none(const bool &val) {
	this->none = val;
}

bool StakerClaimedParamsNone::get_none() const {
	return this->none;
}

void StakerClaimedParamsNone::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_none"), &StakerClaimedParamsNone::get_none);
	ClassDB::bind_method(D_METHOD("set_none", "value"), &StakerClaimedParamsNone::set_none);
	ClassDB::add_property("StakerClaimedParamsNone", PropertyInfo(Variant::Type::BOOL, "none"), "set_none", "get_none");
	ClassDB::bind_method(D_METHOD("to_dict"), &StakerClaimedParamsNone::to_dict);
}
Dictionary StakerClaimedParamsNone::to_dict() {
	Dictionary res;
	res["none"] = none;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot