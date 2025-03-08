#include "honeycomb/types/ServiceDelegation_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceDelegation::set_kind(const String &val) {
	this->kind = val;
}

String ServiceDelegation::get_kind() const {
	return this->kind;
}

void ServiceDelegation::set_index(const int32_t &val) {
	this->index = val;
}

int32_t ServiceDelegation::get_index() const {
	return this->index;
}

void ServiceDelegation::set_permission(const String &val) {
	this->permission = val;
}

String ServiceDelegation::get_permission() const {
	return this->permission;
}

void ServiceDelegation::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kind"), &ServiceDelegation::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &ServiceDelegation::set_kind);
	ClassDB::add_property("ServiceDelegation", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegation::get_index);
	ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegation::set_index);
	ClassDB::add_property("ServiceDelegation", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
	ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegation::get_permission);
	ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegation::set_permission);
	ClassDB::add_property("ServiceDelegation", PropertyInfo(Variant::Type::STRING, "permission"), "set_permission", "get_permission");
	ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegation::to_dict);
}
Dictionary ServiceDelegation::to_dict() {
	Dictionary res;
	if (!kind.is_empty()) {
		res["kind"] = kind;
	}
	res["index"] = index;
	if (!permission.is_empty()) {
		res["permission"] = permission;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot