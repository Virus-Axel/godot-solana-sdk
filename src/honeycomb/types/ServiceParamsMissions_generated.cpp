#include "honeycomb/types/ServiceParamsMissions_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceParamsMissions::set_poolId(const Variant& val) {
this->poolId = val;
}

Variant ServiceParamsMissions::get_poolId() const {
return this->poolId;
}

void ServiceParamsMissions::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_poolId"), &ServiceParamsMissions::get_poolId);
ClassDB::bind_method(D_METHOD("set_poolId", "value"), &ServiceParamsMissions::set_poolId);
ClassDB::add_property("ServiceParamsMissions", PropertyInfo(Variant::Type::OBJECT, "poolId"), "set_poolId", "get_poolId");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceParamsMissions::to_dict);
}
Dictionary ServiceParamsMissions::to_dict() {
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