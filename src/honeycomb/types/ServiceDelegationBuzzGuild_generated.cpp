#include "honeycomb/types/ServiceDelegationBuzzGuild_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceDelegationBuzzGuild::set_index(const int32_t& val) {
this->index = val;
}

int32_t ServiceDelegationBuzzGuild::get_index() const {
return this->index;
}

void ServiceDelegationBuzzGuild::set_permission(const String& val) {
    this->permission = val;
}

String ServiceDelegationBuzzGuild::get_permission() const {
    return this->permission;
}

void ServiceDelegationBuzzGuild::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationBuzzGuild::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationBuzzGuild::set_index);
ClassDB::add_property("ServiceDelegationBuzzGuild", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationBuzzGuild::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationBuzzGuild::set_permission);
ClassDB::add_property("ServiceDelegationBuzzGuild", PropertyInfo(Variant::STRING, "permission"), "set_permission", "get_permission");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationBuzzGuild::to_dict);
}
Dictionary ServiceDelegationBuzzGuild::to_dict() {
Dictionary res;
res["index"] = index;
res["permission"] = this->permission;
return res;
}
} // namespace honeycomb_resource
} // namespace godot