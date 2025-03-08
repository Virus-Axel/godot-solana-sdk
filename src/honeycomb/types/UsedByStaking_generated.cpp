#include "honeycomb/types/UsedByStaking_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void UsedByStaking::set_pool(const Variant& val) {
this->pool = val;
}

Variant UsedByStaking::get_pool() const {
return this->pool;
}

void UsedByStaking::set_staker(const Variant& val) {
this->staker = val;
}

Variant UsedByStaking::get_staker() const {
return this->staker;
}

void UsedByStaking::set_stakedAt(const int32_t& val) {
this->stakedAt = val;
}

int32_t UsedByStaking::get_stakedAt() const {
return this->stakedAt;
}

void UsedByStaking::set_claimedAt(const int32_t& val) {
this->claimedAt = val;
}

int32_t UsedByStaking::get_claimedAt() const {
return this->claimedAt;
}

void UsedByStaking::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_pool"), &UsedByStaking::get_pool);
ClassDB::bind_method(D_METHOD("set_pool", "value"), &UsedByStaking::set_pool);
ClassDB::add_property("UsedByStaking", PropertyInfo(Variant::Type::OBJECT, "pool"), "set_pool", "get_pool");
ClassDB::bind_method(D_METHOD("get_staker"), &UsedByStaking::get_staker);
ClassDB::bind_method(D_METHOD("set_staker", "value"), &UsedByStaking::set_staker);
ClassDB::add_property("UsedByStaking", PropertyInfo(Variant::Type::OBJECT, "staker"), "set_staker", "get_staker");
ClassDB::bind_method(D_METHOD("get_stakedAt"), &UsedByStaking::get_stakedAt);
ClassDB::bind_method(D_METHOD("set_stakedAt", "value"), &UsedByStaking::set_stakedAt);
ClassDB::add_property("UsedByStaking", PropertyInfo(Variant::Type::INT, "stakedAt"), "set_stakedAt", "get_stakedAt");
ClassDB::bind_method(D_METHOD("get_claimedAt"), &UsedByStaking::get_claimedAt);
ClassDB::bind_method(D_METHOD("set_claimedAt", "value"), &UsedByStaking::set_claimedAt);
ClassDB::add_property("UsedByStaking", PropertyInfo(Variant::Type::INT, "claimedAt"), "set_claimedAt", "get_claimedAt");
ClassDB::bind_method(D_METHOD("to_dict"), &UsedByStaking::to_dict);
}
Dictionary UsedByStaking::to_dict() {
Dictionary res;
if (pool.has_method("to_string")) {
res["pool"] = pool.call("to_string");
} else {
res["pool"] = pool;
}
if (staker.has_method("to_string")) {
res["staker"] = staker.call("to_string");
} else {
res["staker"] = staker;
}
res["stakedAt"] = stakedAt;
res["claimedAt"] = claimedAt;
return res;
}
} // namespace honeycomb_resource
} // namespace godot