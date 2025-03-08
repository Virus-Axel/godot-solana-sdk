#include "honeycomb/types/Reward_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void Reward::set_min(const int64_t &val) {
	this->min = val;
}

int64_t Reward::get_min() const {
	return this->min;
}

void Reward::set_max(const int64_t &val) {
	this->max = val;
}

int64_t Reward::get_max() const {
	return this->max;
}

void Reward::set_rewardType(const Dictionary &val) {
	this->rewardType = val;
}

Dictionary Reward::get_rewardType() const {
	return this->rewardType;
}

void Reward::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_min"), &Reward::get_min);
	ClassDB::bind_method(D_METHOD("set_min", "value"), &Reward::set_min);
	ClassDB::add_property("Reward", PropertyInfo(Variant::Type::INT, "min"), "set_min", "get_min");
	ClassDB::bind_method(D_METHOD("get_max"), &Reward::get_max);
	ClassDB::bind_method(D_METHOD("set_max", "value"), &Reward::set_max);
	ClassDB::add_property("Reward", PropertyInfo(Variant::Type::INT, "max"), "set_max", "get_max");
	ClassDB::bind_method(D_METHOD("get_rewardType"), &Reward::get_rewardType);
	ClassDB::bind_method(D_METHOD("set_rewardType", "value"), &Reward::set_rewardType);
	ClassDB::add_property("Reward", PropertyInfo(Variant::Type::DICTIONARY, "rewardType"), "set_rewardType", "get_rewardType");
	ClassDB::bind_method(D_METHOD("to_dict"), &Reward::to_dict);
}
Dictionary Reward::to_dict() {
	Dictionary res;
	res["min"] = min;
	res["max"] = max;
	res["rewardType"] = rewardType;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot