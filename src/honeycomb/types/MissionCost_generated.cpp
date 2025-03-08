#include "honeycomb/types/MissionCost_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void MissionCost::set_amount(const int64_t& val) {
this->amount = val;
}

int64_t MissionCost::get_amount() const {
return this->amount;
}

void MissionCost::set_resource_address(const Variant& val) {
this->resource_address = val;
}

Variant MissionCost::get_resource_address() const {
return this->resource_address;
}

void MissionCost::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_amount"), &MissionCost::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &MissionCost::set_amount);
ClassDB::add_property("MissionCost", PropertyInfo(Variant::Type::INT, "amount"), "set_amount", "get_amount");
ClassDB::bind_method(D_METHOD("get_resource_address"), &MissionCost::get_resource_address);
ClassDB::bind_method(D_METHOD("set_resource_address", "value"), &MissionCost::set_resource_address);
ClassDB::add_property("MissionCost", PropertyInfo(Variant::Type::OBJECT, "resource_address"), "set_resource_address", "get_resource_address");
ClassDB::bind_method(D_METHOD("to_dict"), &MissionCost::to_dict);
}
Dictionary MissionCost::to_dict() {
Dictionary res;
res["amount"] = amount;
if (resource_address.has_method("to_string")) {
res["resource_address"] = resource_address.call("to_string");
} else {
res["resource_address"] = resource_address;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot