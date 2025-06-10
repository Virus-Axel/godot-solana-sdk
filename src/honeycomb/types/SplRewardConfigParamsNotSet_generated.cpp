#include "honeycomb/types/SplRewardConfigParamsNotSet_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SplRewardConfigParamsNotSet::set_notSet(const bool &val) {
	this->notSet = val;
}

bool SplRewardConfigParamsNotSet::get_notSet() const {
	return this->notSet;
}

void SplRewardConfigParamsNotSet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_notSet"), &SplRewardConfigParamsNotSet::get_notSet);
	ClassDB::bind_method(D_METHOD("set_notSet", "value"), &SplRewardConfigParamsNotSet::set_notSet);
	ClassDB::add_property("SplRewardConfigParamsNotSet", PropertyInfo(Variant::Type::BOOL, "notSet"), "set_notSet", "get_notSet");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplRewardConfigParamsNotSet::to_dict);
}
Dictionary SplRewardConfigParamsNotSet::to_dict() {
	Dictionary res;
	res["notSet"] = notSet;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot