#include "honeycomb/types/ResourceKindParamsWrappedFungible_generated.hpp"

#include "honeycomb/types/ResourceCustody_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ResourceKindParamsWrappedFungible::set_decimals(const int32_t &val) {
	this->decimals = val;
}

int32_t ResourceKindParamsWrappedFungible::get_decimals() const {
	return this->decimals;
}

void ResourceKindParamsWrappedFungible::set_custody(const Variant &val) {
	this->custody = val;
}

Variant ResourceKindParamsWrappedFungible::get_custody() const {
	return this->custody;
}

void ResourceKindParamsWrappedFungible::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_decimals"), &ResourceKindParamsWrappedFungible::get_decimals);
	ClassDB::bind_method(D_METHOD("set_decimals", "value"), &ResourceKindParamsWrappedFungible::set_decimals);
	ClassDB::add_property("ResourceKindParamsWrappedFungible", PropertyInfo(Variant::Type::INT, "decimals"), "set_decimals", "get_decimals");
	ClassDB::bind_method(D_METHOD("get_custody"), &ResourceKindParamsWrappedFungible::get_custody);
	ClassDB::bind_method(D_METHOD("set_custody", "value"), &ResourceKindParamsWrappedFungible::set_custody);
	ClassDB::add_property("ResourceKindParamsWrappedFungible", PropertyInfo(Variant::Type::OBJECT, "custody"), "set_custody", "get_custody");
	ClassDB::bind_method(D_METHOD("to_dict"), &ResourceKindParamsWrappedFungible::to_dict);
}
Dictionary ResourceKindParamsWrappedFungible::to_dict() {
	Dictionary res;
	res["decimals"] = decimals;

	if (custody.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::ResourceCustody>(custody);
		if (ptr) {
			res["custody"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot