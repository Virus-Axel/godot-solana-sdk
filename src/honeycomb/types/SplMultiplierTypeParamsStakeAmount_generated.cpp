#include "honeycomb/types/SplMultiplierTypeParamsStakeAmount_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SplMultiplierTypeParamsStakeAmount::set_minAmount(const int64_t &val) {
	this->minAmount = val;
}

int64_t SplMultiplierTypeParamsStakeAmount::get_minAmount() const {
	return this->minAmount;
}

void SplMultiplierTypeParamsStakeAmount::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_minAmount"), &SplMultiplierTypeParamsStakeAmount::get_minAmount);
	ClassDB::bind_method(D_METHOD("set_minAmount", "value"), &SplMultiplierTypeParamsStakeAmount::set_minAmount);
	ClassDB::add_property("SplMultiplierTypeParamsStakeAmount", PropertyInfo(Variant::Type::INT, "minAmount"), "set_minAmount", "get_minAmount");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplMultiplierTypeParamsStakeAmount::to_dict);
}
Dictionary SplMultiplierTypeParamsStakeAmount::to_dict() {
	Dictionary res;
	res["minAmount"] = minAmount;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot