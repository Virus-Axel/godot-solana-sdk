#include "honeycomb/types/ServiceDelegationNectarMissions_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceDelegationNectarMissions::set_index(const int32_t &val) {
	this->index = val;
}

int32_t ServiceDelegationNectarMissions::get_index() const {
	return this->index;
}

void ServiceDelegationNectarMissions::set_permission(const String &val) {
	this->permission = val;
}

String ServiceDelegationNectarMissions::get_permission() const {
	return this->permission;
}

void ServiceDelegationNectarMissions::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationNectarMissions::get_index);
	ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationNectarMissions::set_index);
	ClassDB::add_property("ServiceDelegationNectarMissions", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
	ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationNectarMissions::get_permission);
	ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationNectarMissions::set_permission);
	ClassDB::add_property("ServiceDelegationNectarMissions", PropertyInfo(Variant::STRING, "permission"), "set_permission", "get_permission");
	ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationNectarMissions::to_dict);
}
Dictionary ServiceDelegationNectarMissions::to_dict() {
	Dictionary res;
	res["index"] = index;
	res["permission"] = this->permission;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot