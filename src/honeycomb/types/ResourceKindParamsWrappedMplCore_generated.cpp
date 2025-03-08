#include "honeycomb/types/ResourceKindParamsWrappedMplCore_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ResourceKindParamsWrappedMplCore::set_characteristics(const PackedStringArray &val) {
	this->characteristics = val;
}

PackedStringArray ResourceKindParamsWrappedMplCore::get_characteristics() const {
	return this->characteristics;
}

void ResourceKindParamsWrappedMplCore::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_characteristics"), &ResourceKindParamsWrappedMplCore::get_characteristics);
	ClassDB::bind_method(D_METHOD("set_characteristics", "value"), &ResourceKindParamsWrappedMplCore::set_characteristics);
	ClassDB::add_property("ResourceKindParamsWrappedMplCore", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "characteristics"), "set_characteristics", "get_characteristics");
	ClassDB::bind_method(D_METHOD("to_dict"), &ResourceKindParamsWrappedMplCore::to_dict);
}
Dictionary ResourceKindParamsWrappedMplCore::to_dict() {
	Dictionary res;
	res["characteristics"] = characteristics;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot