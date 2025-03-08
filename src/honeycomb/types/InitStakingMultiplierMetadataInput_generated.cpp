#include "honeycomb/types/InitStakingMultiplierMetadataInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void InitStakingMultiplierMetadataInput::set_decimals(const int32_t& val) {
this->decimals = val;
}

int32_t InitStakingMultiplierMetadataInput::get_decimals() const {
return this->decimals;
}

void InitStakingMultiplierMetadataInput::set_multipliers(const Array& val) {
this->multipliers = val;
}

Array InitStakingMultiplierMetadataInput::get_multipliers() const {
return this->multipliers;
}

void InitStakingMultiplierMetadataInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_decimals"), &InitStakingMultiplierMetadataInput::get_decimals);
ClassDB::bind_method(D_METHOD("set_decimals", "value"), &InitStakingMultiplierMetadataInput::set_decimals);
ClassDB::add_property("InitStakingMultiplierMetadataInput", PropertyInfo(Variant::Type::INT, "decimals"), "set_decimals", "get_decimals");
ClassDB::bind_method(D_METHOD("get_multipliers"), &InitStakingMultiplierMetadataInput::get_multipliers);
ClassDB::bind_method(D_METHOD("set_multipliers", "value"), &InitStakingMultiplierMetadataInput::set_multipliers);
ClassDB::add_property("InitStakingMultiplierMetadataInput", PropertyInfo(Variant::Type::ARRAY, "multipliers"), "set_multipliers", "get_multipliers");
ClassDB::bind_method(D_METHOD("to_dict"), &InitStakingMultiplierMetadataInput::to_dict);
}
Dictionary InitStakingMultiplierMetadataInput::to_dict() {
Dictionary res;
res["decimals"] = decimals;
res["multipliers"] = multipliers;
return res;
}
} // namespace honeycomb_resource
} // namespace godot