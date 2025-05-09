#include "honeycomb/types/AddRemoveRewardsFromRewardPoolActionInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void AddRemoveRewardsFromRewardPoolActionInput::set_add(const int64_t &val) {
	this->add = val;
}

int64_t AddRemoveRewardsFromRewardPoolActionInput::get_add() const {
	return this->add;
}

void AddRemoveRewardsFromRewardPoolActionInput::set_remove(const int64_t &val) {
	this->remove = val;
}

int64_t AddRemoveRewardsFromRewardPoolActionInput::get_remove() const {
	return this->remove;
}

void AddRemoveRewardsFromRewardPoolActionInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_add"), &AddRemoveRewardsFromRewardPoolActionInput::get_add);
	ClassDB::bind_method(D_METHOD("set_add", "value"), &AddRemoveRewardsFromRewardPoolActionInput::set_add);
	ClassDB::add_property("AddRemoveRewardsFromRewardPoolActionInput", PropertyInfo(Variant::Type::INT, "add"), "set_add", "get_add");
	ClassDB::bind_method(D_METHOD("get_remove"), &AddRemoveRewardsFromRewardPoolActionInput::get_remove);
	ClassDB::bind_method(D_METHOD("set_remove", "value"), &AddRemoveRewardsFromRewardPoolActionInput::set_remove);
	ClassDB::add_property("AddRemoveRewardsFromRewardPoolActionInput", PropertyInfo(Variant::Type::INT, "remove"), "set_remove", "get_remove");
	ClassDB::bind_method(D_METHOD("to_dict"), &AddRemoveRewardsFromRewardPoolActionInput::to_dict);
}
Dictionary AddRemoveRewardsFromRewardPoolActionInput::to_dict() {
	Dictionary res;
	res["add"] = add;
	res["remove"] = remove;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot