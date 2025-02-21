#include "honeycomb/types/ServiceDelegationAssetAssembler.hpp"

#include "honeycomb/enums.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceDelegationAssetAssembler::set_index(const int32_t& val) {
this->index = val;
}

int32_t ServiceDelegationAssetAssembler::get_index() const {
return this->index;
}

void ServiceDelegationAssetAssembler::set_permission(const String& val) {
    this->permission = val;
}

String ServiceDelegationAssetAssembler::get_permission() const {
    return this->permission;
}

void ServiceDelegationAssetAssembler::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_index"), &ServiceDelegationAssetAssembler::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ServiceDelegationAssetAssembler::set_index);
ClassDB::add_property("ServiceDelegationAssetAssembler", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_permission"), &ServiceDelegationAssetAssembler::get_permission);
ClassDB::bind_method(D_METHOD("set_permission", "value"), &ServiceDelegationAssetAssembler::set_permission);
ClassDB::add_property("ServiceDelegationAssetAssembler", PropertyInfo(Variant::STRING, "permission"), "set_permission", "get_permission");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationAssetAssembler::to_dict);
}
Dictionary ServiceDelegationAssetAssembler::to_dict() {
Dictionary res;
res["index"] = index;
res["permission"] = this->permission;
return res;
}
} // namespace honeycomb_resource
} // namespace godot