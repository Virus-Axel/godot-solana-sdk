#include "honeycomb/types/ResourceKindParamsHplNonFungible.hpp"


namespace godot {
namespace honeycomb_resource {

void ResourceKindParamsHplNonFungible::set_characteristics(const PackedStringArray& val) {
this->characteristics = val;
}

PackedStringArray ResourceKindParamsHplNonFungible::get_characteristics() const {
return this->characteristics;
}

void ResourceKindParamsHplNonFungible::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_characteristics"), &ResourceKindParamsHplNonFungible::get_characteristics);
ClassDB::bind_method(D_METHOD("set_characteristics", "value"), &ResourceKindParamsHplNonFungible::set_characteristics);
ClassDB::add_property("ResourceKindParamsHplNonFungible", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "characteristics"), "set_characteristics", "get_characteristics");
ClassDB::bind_method(D_METHOD("to_dict"), &ResourceKindParamsHplNonFungible::to_dict);
}
Dictionary ResourceKindParamsHplNonFungible::to_dict() {
Dictionary res;
res["characteristics"] = characteristics;
return res;
}
} // namespace honeycomb_resource
} // namespace godot