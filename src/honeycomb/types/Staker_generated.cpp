#include "honeycomb/types/Staker_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Staker::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray Staker::get_address() const {
return this->address;
}

void Staker::set_bump(const int32_t& val) {
this->bump = val;
}

int32_t Staker::get_bump() const {
return this->bump;
}

void Staker::set_stakingPool(const Variant& val) {
this->stakingPool = val;
}

Variant Staker::get_stakingPool() const {
return this->stakingPool;
}

void Staker::set_wallet(const Variant& val) {
this->wallet = val;
}

Variant Staker::get_wallet() const {
return this->wallet;
}

void Staker::set_totalStaked(const int64_t& val) {
this->totalStaked = val;
}

int64_t Staker::get_totalStaked() const {
return this->totalStaked;
}

void Staker::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &Staker::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &Staker::set_address);
ClassDB::add_property("Staker", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_bump"), &Staker::get_bump);
ClassDB::bind_method(D_METHOD("set_bump", "value"), &Staker::set_bump);
ClassDB::add_property("Staker", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
ClassDB::bind_method(D_METHOD("get_stakingPool"), &Staker::get_stakingPool);
ClassDB::bind_method(D_METHOD("set_stakingPool", "value"), &Staker::set_stakingPool);
ClassDB::add_property("Staker", PropertyInfo(Variant::Type::OBJECT, "stakingPool"), "set_stakingPool", "get_stakingPool");
ClassDB::bind_method(D_METHOD("get_wallet"), &Staker::get_wallet);
ClassDB::bind_method(D_METHOD("set_wallet", "value"), &Staker::set_wallet);
ClassDB::add_property("Staker", PropertyInfo(Variant::Type::OBJECT, "wallet"), "set_wallet", "get_wallet");
ClassDB::bind_method(D_METHOD("get_totalStaked"), &Staker::get_totalStaked);
ClassDB::bind_method(D_METHOD("set_totalStaked", "value"), &Staker::set_totalStaked);
ClassDB::add_property("Staker", PropertyInfo(Variant::Type::INT, "totalStaked"), "set_totalStaked", "get_totalStaked");
ClassDB::bind_method(D_METHOD("to_dict"), &Staker::to_dict);
}
Dictionary Staker::to_dict() {
Dictionary res;
res["address"] = address;
res["bump"] = bump;
if (stakingPool.has_method("to_string")) {
res["stakingPool"] = stakingPool.call("to_string");
} else {
res["stakingPool"] = stakingPool;
}
if (wallet.has_method("to_string")) {
res["wallet"] = wallet.call("to_string");
} else {
res["wallet"] = wallet;
}
res["totalStaked"] = totalStaked;
return res;
}
} // namespace honeycomb_resource
} // namespace godot