#include "honeycomb/types/Faucet.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Faucet::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray Faucet::get_address() const {
return this->address;
}

void Faucet::set_project(const Variant& val) {
this->project = val;
}

Variant Faucet::get_project() const {
return this->project;
}

void Faucet::set_resource(const Variant& val) {
this->resource = val;
}

Variant Faucet::get_resource() const {
return this->resource;
}

void Faucet::set_amount(const int64_t& val) {
this->amount = val;
}

int64_t Faucet::get_amount() const {
return this->amount;
}

void Faucet::set_repeatInterval(const int64_t& val) {
this->repeatInterval = val;
}

int64_t Faucet::get_repeatInterval() const {
return this->repeatInterval;
}

void Faucet::set_lastClaimed(const int64_t& val) {
this->lastClaimed = val;
}

int64_t Faucet::get_lastClaimed() const {
return this->lastClaimed;
}

void Faucet::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &Faucet::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &Faucet::set_address);
ClassDB::add_property("Faucet", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_project"), &Faucet::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &Faucet::set_project);
ClassDB::add_property("Faucet", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_resource"), &Faucet::get_resource);
ClassDB::bind_method(D_METHOD("set_resource", "value"), &Faucet::set_resource);
ClassDB::add_property("Faucet", PropertyInfo(Variant::Type::OBJECT, "resource"), "set_resource", "get_resource");
ClassDB::bind_method(D_METHOD("get_amount"), &Faucet::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &Faucet::set_amount);
ClassDB::add_property("Faucet", PropertyInfo(Variant::Type::INT, "amount"), "set_amount", "get_amount");
ClassDB::bind_method(D_METHOD("get_repeatInterval"), &Faucet::get_repeatInterval);
ClassDB::bind_method(D_METHOD("set_repeatInterval", "value"), &Faucet::set_repeatInterval);
ClassDB::add_property("Faucet", PropertyInfo(Variant::Type::INT, "repeatInterval"), "set_repeatInterval", "get_repeatInterval");
ClassDB::bind_method(D_METHOD("get_lastClaimed"), &Faucet::get_lastClaimed);
ClassDB::bind_method(D_METHOD("set_lastClaimed", "value"), &Faucet::set_lastClaimed);
ClassDB::add_property("Faucet", PropertyInfo(Variant::Type::INT, "lastClaimed"), "set_lastClaimed", "get_lastClaimed");
ClassDB::bind_method(D_METHOD("to_dict"), &Faucet::to_dict);
}
Dictionary Faucet::to_dict() {
Dictionary res;
res["address"] = address;
if (project.has_method("to_string")) {
res["project"] = project.call("to_string");
} else {
res["project"] = project;
}
if (resource.has_method("to_string")) {
res["resource"] = resource.call("to_string");
} else {
res["resource"] = resource;
}
res["amount"] = amount;
res["repeatInterval"] = repeatInterval;
res["lastClaimed"] = lastClaimed;
return res;
}
} // namespace honeycomb_resource
} // namespace godot