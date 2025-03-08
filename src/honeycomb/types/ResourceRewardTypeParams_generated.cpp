#include "honeycomb/types/ResourceRewardTypeParams_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ResourceRewardTypeParams::set_address(const Variant &val) {
	this->address = val;
}

Variant ResourceRewardTypeParams::get_address() const {
	return this->address;
}

void ResourceRewardTypeParams::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &ResourceRewardTypeParams::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &ResourceRewardTypeParams::set_address);
	ClassDB::add_property("ResourceRewardTypeParams", PropertyInfo(Variant::Type::OBJECT, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("to_dict"), &ResourceRewardTypeParams::to_dict);
}
Dictionary ResourceRewardTypeParams::to_dict() {
	Dictionary res;
	if (address.has_method("to_string")) {
		res["address"] = address.call("to_string");
	} else {
		res["address"] = address;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot