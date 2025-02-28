#include "honeycomb/types/ResourceKindParamsHplFungible_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void ResourceKindParamsHplFungible::set_decimals(const int32_t& val) {
this->decimals = val;
}

int32_t ResourceKindParamsHplFungible::get_decimals() const {
return this->decimals;
}

void ResourceKindParamsHplFungible::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_decimals"), &ResourceKindParamsHplFungible::get_decimals);
ClassDB::bind_method(D_METHOD("set_decimals", "value"), &ResourceKindParamsHplFungible::set_decimals);
ClassDB::add_property("ResourceKindParamsHplFungible", PropertyInfo(Variant::Type::INT, "decimals"), "set_decimals", "get_decimals");
ClassDB::bind_method(D_METHOD("to_dict"), &ResourceKindParamsHplFungible::to_dict);
}
Dictionary ResourceKindParamsHplFungible::to_dict() {
Dictionary res;
res["decimals"] = decimals;
return res;
}
} // namespace honeycomb_resource
} // namespace godot