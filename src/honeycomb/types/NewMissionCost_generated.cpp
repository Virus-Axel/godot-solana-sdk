#include "honeycomb/types/NewMissionCost_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void NewMissionCost::set_address(const String& val) {
this->address = val;
}

String NewMissionCost::get_address() const {
return this->address;
}

void NewMissionCost::set_amount(const int64_t& val) {
this->amount = val;
}

int64_t NewMissionCost::get_amount() const {
return this->amount;
}

void NewMissionCost::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &NewMissionCost::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &NewMissionCost::set_address);
ClassDB::add_property("NewMissionCost", PropertyInfo(Variant::Type::STRING, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_amount"), &NewMissionCost::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &NewMissionCost::set_amount);
ClassDB::add_property("NewMissionCost", PropertyInfo(Variant::Type::INT, "amount"), "set_amount", "get_amount");
ClassDB::bind_method(D_METHOD("to_dict"), &NewMissionCost::to_dict);
}
Dictionary NewMissionCost::to_dict() {
Dictionary res;
if (!address.is_empty()) {
    res["address"] = address;
}
res["amount"] = amount;
return res;
}
} // namespace honeycomb_resource
} // namespace godot