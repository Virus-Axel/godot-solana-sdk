#include "honeycomb/types/BasicTreeConfig_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void BasicTreeConfig::set_numAssets(const int32_t &val) {
	this->numAssets = val;
}

int32_t BasicTreeConfig::get_numAssets() const {
	return this->numAssets;
}

void BasicTreeConfig::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_numAssets"), &BasicTreeConfig::get_numAssets);
	ClassDB::bind_method(D_METHOD("set_numAssets", "value"), &BasicTreeConfig::set_numAssets);
	ClassDB::add_property("BasicTreeConfig", PropertyInfo(Variant::Type::INT, "numAssets"), "set_numAssets", "get_numAssets");
	ClassDB::bind_method(D_METHOD("to_dict"), &BasicTreeConfig::to_dict);
}
Dictionary BasicTreeConfig::to_dict() {
	Dictionary res;
	res["numAssets"] = numAssets;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot