#include "honeycomb/types/ResourceCustodyParams_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ResourceCustodyParams::set_burnerDestination(const Variant& val) {
this->burnerDestination = val;
}

Variant ResourceCustodyParams::get_burnerDestination() const {
return this->burnerDestination;
}

void ResourceCustodyParams::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_burnerDestination"), &ResourceCustodyParams::get_burnerDestination);
ClassDB::bind_method(D_METHOD("set_burnerDestination", "value"), &ResourceCustodyParams::set_burnerDestination);
ClassDB::add_property("ResourceCustodyParams", PropertyInfo(Variant::Type::OBJECT, "burnerDestination"), "set_burnerDestination", "get_burnerDestination");
ClassDB::bind_method(D_METHOD("to_dict"), &ResourceCustodyParams::to_dict);
}
Dictionary ResourceCustodyParams::to_dict() {
Dictionary res;
if (burnerDestination.has_method("to_string")) {
res["burnerDestination"] = burnerDestination.call("to_string");
} else {
res["burnerDestination"] = burnerDestination;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot