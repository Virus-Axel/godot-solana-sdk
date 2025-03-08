#include "honeycomb/types/ServiceParamsGuildKit_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceParamsGuildKit::set_kitId(const Variant &val) {
	this->kitId = val;
}

Variant ServiceParamsGuildKit::get_kitId() const {
	return this->kitId;
}

void ServiceParamsGuildKit::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kitId"), &ServiceParamsGuildKit::get_kitId);
	ClassDB::bind_method(D_METHOD("set_kitId", "value"), &ServiceParamsGuildKit::set_kitId);
	ClassDB::add_property("ServiceParamsGuildKit", PropertyInfo(Variant::Type::OBJECT, "kitId"), "set_kitId", "get_kitId");
	ClassDB::bind_method(D_METHOD("to_dict"), &ServiceParamsGuildKit::to_dict);
}
Dictionary ServiceParamsGuildKit::to_dict() {
	Dictionary res;
	if (kitId.has_method("to_string")) {
		res["kitId"] = kitId.call("to_string");
	} else {
		res["kitId"] = kitId;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot