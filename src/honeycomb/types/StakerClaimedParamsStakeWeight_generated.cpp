#include "honeycomb/types/StakerClaimedParamsStakeWeight_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void StakerClaimedParamsStakeWeight::set_amounts(const PackedInt32Array &val) {
	this->amounts = val;
}

PackedInt32Array StakerClaimedParamsStakeWeight::get_amounts() const {
	return this->amounts;
}

void StakerClaimedParamsStakeWeight::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_amounts"), &StakerClaimedParamsStakeWeight::get_amounts);
	ClassDB::bind_method(D_METHOD("set_amounts", "value"), &StakerClaimedParamsStakeWeight::set_amounts);
	ClassDB::add_property("StakerClaimedParamsStakeWeight", PropertyInfo(Variant::Type::PACKED_INT32_ARRAY, "amounts"), "set_amounts", "get_amounts");
	ClassDB::bind_method(D_METHOD("to_dict"), &StakerClaimedParamsStakeWeight::to_dict);
}
Dictionary StakerClaimedParamsStakeWeight::to_dict() {
	Dictionary res;
	res["amounts"] = amounts;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot