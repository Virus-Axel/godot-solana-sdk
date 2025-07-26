#include "honeycomb/types/CreateSplRewardPoolArgsInput_generated.hpp"

#include "honeycomb/types/RewardPoolConfigApyInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateSplRewardPoolArgsInput::set_ApyConfig(const Variant &val) {
	this->ApyConfig = val;
}

Variant CreateSplRewardPoolArgsInput::get_ApyConfig() const {
	return this->ApyConfig;
}

void CreateSplRewardPoolArgsInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_ApyConfig"), &CreateSplRewardPoolArgsInput::get_ApyConfig);
	ClassDB::bind_method(D_METHOD("set_ApyConfig", "value"), &CreateSplRewardPoolArgsInput::set_ApyConfig);
	ClassDB::add_property("CreateSplRewardPoolArgsInput", PropertyInfo(Variant::Type::OBJECT, "ApyConfig"), "set_ApyConfig", "get_ApyConfig");
	ClassDB::bind_method(D_METHOD("to_dict"), &CreateSplRewardPoolArgsInput::to_dict);
}
Dictionary CreateSplRewardPoolArgsInput::to_dict() {
	Dictionary res;

	if (ApyConfig.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::RewardPoolConfigApyInput>(ApyConfig);
		if (ptr) {
			res["ApyConfig"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot