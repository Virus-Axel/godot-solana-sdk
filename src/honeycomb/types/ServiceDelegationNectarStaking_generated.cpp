#include "honeycomb/types/ServiceDelegationNectarStaking_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceDelegationNectarStaking::set_index(const int32_t &val) {
	this->index = val;
}

int32_t ServiceDelegationNectarStaking::get_index() const {
	return this->index;
}

void ServiceDelegationNectarStaking::set_permission(const String &val) {
	this->permission = val;
}

String ServiceDelegationNectarStaking::get_permission() const {
	return this->permission;
}

void ServiceDelegationNectarStaking::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationNectarStaking::get_index);
	ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationNectarStaking::set_index);
	ClassDB::add_property("ServiceDelegationNectarStaking", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
	ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationNectarStaking::get_permission);
	ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationNectarStaking::set_permission);
	ClassDB::add_property("ServiceDelegationNectarStaking", PropertyInfo(Variant::STRING, "permission"), "set_permission", "get_permission");
	ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationNectarStaking::to_dict);
}
Dictionary ServiceDelegationNectarStaking::to_dict() {
	Dictionary res;
	res["index"] = index;
	res["permission"] = this->permission;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot