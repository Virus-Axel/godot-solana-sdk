#include "honeycomb/types/SplMultiplierInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SplMultiplierInput::set_minDuration(const int64_t &val) {
	this->minDuration = val;
}

int64_t SplMultiplierInput::get_minDuration() const {
	return this->minDuration;
}

void SplMultiplierInput::set_minAmount(const int64_t &val) {
	this->minAmount = val;
}

int64_t SplMultiplierInput::get_minAmount() const {
	return this->minAmount;
}

void SplMultiplierInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_minDuration"), &SplMultiplierInput::get_minDuration);
	ClassDB::bind_method(D_METHOD("set_minDuration", "value"), &SplMultiplierInput::set_minDuration);
	ClassDB::add_property("SplMultiplierInput", PropertyInfo(Variant::Type::INT, "minDuration"), "set_minDuration", "get_minDuration");
	ClassDB::bind_method(D_METHOD("get_minAmount"), &SplMultiplierInput::get_minAmount);
	ClassDB::bind_method(D_METHOD("set_minAmount", "value"), &SplMultiplierInput::set_minAmount);
	ClassDB::add_property("SplMultiplierInput", PropertyInfo(Variant::Type::INT, "minAmount"), "set_minAmount", "get_minAmount");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplMultiplierInput::to_dict);
}
Dictionary SplMultiplierInput::to_dict() {
	Dictionary res;
	res["minDuration"] = minDuration;
	res["minAmount"] = minAmount;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot