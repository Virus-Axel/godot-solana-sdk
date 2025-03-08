#include "honeycomb/types/ServiceParamsRaffles_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceParamsRaffles::set_poolId(const Variant& val) {
this->poolId = val;
}

Variant ServiceParamsRaffles::get_poolId() const {
return this->poolId;
}

void ServiceParamsRaffles::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_poolId"), &ServiceParamsRaffles::get_poolId);
ClassDB::bind_method(D_METHOD("set_poolId", "value"), &ServiceParamsRaffles::set_poolId);
ClassDB::add_property("ServiceParamsRaffles", PropertyInfo(Variant::Type::OBJECT, "poolId"), "set_poolId", "get_poolId");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceParamsRaffles::to_dict);
}
Dictionary ServiceParamsRaffles::to_dict() {
Dictionary res;
if (poolId.has_method("to_string")) {
res["poolId"] = poolId.call("to_string");
} else {
res["poolId"] = poolId;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot