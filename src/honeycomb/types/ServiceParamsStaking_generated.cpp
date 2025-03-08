#include "honeycomb/types/ServiceParamsStaking_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceParamsStaking::set_poolId(const Variant &val) {
	this->poolId = val;
}

Variant ServiceParamsStaking::get_poolId() const {
	return this->poolId;
}

void ServiceParamsStaking::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_poolId"), &ServiceParamsStaking::get_poolId);
	ClassDB::bind_method(D_METHOD("set_poolId", "value"), &ServiceParamsStaking::set_poolId);
	ClassDB::add_property("ServiceParamsStaking", PropertyInfo(Variant::Type::OBJECT, "poolId"), "set_poolId", "get_poolId");
	ClassDB::bind_method(D_METHOD("to_dict"), &ServiceParamsStaking::to_dict);
}
Dictionary ServiceParamsStaking::to_dict() {
	Dictionary res;
	if (poolId.has_method("to_string")) {
		res["poolId"] = poolId.call("to_string");
	} else {
		res["poolId"] = poolId;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot