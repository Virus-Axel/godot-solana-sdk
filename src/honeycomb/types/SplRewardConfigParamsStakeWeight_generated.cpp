#include "honeycomb/types/SplRewardConfigParamsStakeWeight_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SplRewardConfigParamsStakeWeight::set_pools(const Array &val) {
	this->pools = val;
}

Array SplRewardConfigParamsStakeWeight::get_pools() const {
	return this->pools;
}

void SplRewardConfigParamsStakeWeight::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_pools"), &SplRewardConfigParamsStakeWeight::get_pools);
	ClassDB::bind_method(D_METHOD("set_pools", "value"), &SplRewardConfigParamsStakeWeight::set_pools);
	ClassDB::add_property("SplRewardConfigParamsStakeWeight", PropertyInfo(Variant::Type::ARRAY, "pools"), "set_pools", "get_pools");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplRewardConfigParamsStakeWeight::to_dict);
}
Dictionary SplRewardConfigParamsStakeWeight::to_dict() {
	Dictionary res;
	res["pools"] = pools;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot