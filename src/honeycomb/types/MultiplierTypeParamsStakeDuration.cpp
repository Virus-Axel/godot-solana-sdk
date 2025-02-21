#include "honeycomb/types/MultiplierTypeParamsStakeDuration.hpp"


namespace godot {
namespace honeycomb_resource {

void MultiplierTypeParamsStakeDuration::set_minDuration(const int64_t& val) {
this->minDuration = val;
}

int64_t MultiplierTypeParamsStakeDuration::get_minDuration() const {
return this->minDuration;
}

void MultiplierTypeParamsStakeDuration::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_minDuration"), &MultiplierTypeParamsStakeDuration::get_minDuration);
ClassDB::bind_method(D_METHOD("set_minDuration", "value"), &MultiplierTypeParamsStakeDuration::set_minDuration);
ClassDB::add_property("MultiplierTypeParamsStakeDuration", PropertyInfo(Variant::Type::INT, "minDuration"), "set_minDuration", "get_minDuration");
ClassDB::bind_method(D_METHOD("to_dict"), &MultiplierTypeParamsStakeDuration::to_dict);
}
Dictionary MultiplierTypeParamsStakeDuration::to_dict() {
Dictionary res;
res["minDuration"] = minDuration;
return res;
}
} // namespace honeycomb_resource
} // namespace godot