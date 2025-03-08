#include "honeycomb/types/DelegateAuthority_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void DelegateAuthority::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray DelegateAuthority::get_address() const {
return this->address;
}

void DelegateAuthority::set_bump(const int32_t& val) {
this->bump = val;
}

int32_t DelegateAuthority::get_bump() const {
return this->bump;
}

void DelegateAuthority::set_project(const Variant& val) {
this->project = val;
}

Variant DelegateAuthority::get_project() const {
return this->project;
}

void DelegateAuthority::set_authority(const Variant& val) {
this->authority = val;
}

Variant DelegateAuthority::get_authority() const {
return this->authority;
}

void DelegateAuthority::set_delegations(const Array& val) {
this->delegations = val;
}

Array DelegateAuthority::get_delegations() const {
return this->delegations;
}

void DelegateAuthority::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &DelegateAuthority::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &DelegateAuthority::set_address);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_bump"), &DelegateAuthority::get_bump);
ClassDB::bind_method(D_METHOD("set_bump", "value"), &DelegateAuthority::set_bump);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
ClassDB::bind_method(D_METHOD("get_project"), &DelegateAuthority::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &DelegateAuthority::set_project);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_authority"), &DelegateAuthority::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &DelegateAuthority::set_authority);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::OBJECT, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_delegations"), &DelegateAuthority::get_delegations);
ClassDB::bind_method(D_METHOD("set_delegations", "value"), &DelegateAuthority::set_delegations);
ClassDB::add_property("DelegateAuthority", PropertyInfo(Variant::Type::ARRAY, "delegations"), "set_delegations", "get_delegations");
ClassDB::bind_method(D_METHOD("to_dict"), &DelegateAuthority::to_dict);
}
Dictionary DelegateAuthority::to_dict() {
Dictionary res;
res["address"] = address;
res["bump"] = bump;
if (project.has_method("to_string")) {
res["project"] = project.call("to_string");
} else {
res["project"] = project;
}
if (authority.has_method("to_string")) {
res["authority"] = authority.call("to_string");
} else {
res["authority"] = authority;
}
res["delegations"] = delegations;
return res;
}
} // namespace honeycomb_resource
} // namespace godot