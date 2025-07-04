#include "honeycomb/types/SplStakeWeightConfig_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SplStakeWeightConfig::set_weight(const int32_t &val) {
	this->weight = val;
}

int32_t SplStakeWeightConfig::get_weight() const {
	return this->weight;
}

void SplStakeWeightConfig::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_weight"), &SplStakeWeightConfig::get_weight);
	ClassDB::bind_method(D_METHOD("set_weight", "value"), &SplStakeWeightConfig::set_weight);
	ClassDB::add_property("SplStakeWeightConfig", PropertyInfo(Variant::Type::INT, "weight"), "set_weight", "get_weight");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplStakeWeightConfig::to_dict);
}
Dictionary SplStakeWeightConfig::to_dict() {
	Dictionary res;
	res["weight"] = weight;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot