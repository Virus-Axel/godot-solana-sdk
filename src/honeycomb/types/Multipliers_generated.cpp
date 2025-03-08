#include "honeycomb/types/Multipliers_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Multipliers::set_address(const PackedByteArray &val) {
	this->address = val;
}

PackedByteArray Multipliers::get_address() const {
	return this->address;
}

void Multipliers::set_bump(const int32_t &val) {
	this->bump = val;
}

int32_t Multipliers::get_bump() const {
	return this->bump;
}

void Multipliers::set_stakingPool(const Variant &val) {
	this->stakingPool = val;
}

Variant Multipliers::get_stakingPool() const {
	return this->stakingPool;
}

void Multipliers::set_decimals(const int32_t &val) {
	this->decimals = val;
}

int32_t Multipliers::get_decimals() const {
	return this->decimals;
}

void Multipliers::set_durationMultipliers(const Array &val) {
	this->durationMultipliers = val;
}

Array Multipliers::get_durationMultipliers() const {
	return this->durationMultipliers;
}

void Multipliers::set_countMultipliers(const Array &val) {
	this->countMultipliers = val;
}

Array Multipliers::get_countMultipliers() const {
	return this->countMultipliers;
}

void Multipliers::set_creatorMultipliers(const Array &val) {
	this->creatorMultipliers = val;
}

Array Multipliers::get_creatorMultipliers() const {
	return this->creatorMultipliers;
}

void Multipliers::set_collectionMultipliers(const Array &val) {
	this->collectionMultipliers = val;
}

Array Multipliers::get_collectionMultipliers() const {
	return this->collectionMultipliers;
}

void Multipliers::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &Multipliers::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &Multipliers::set_address);
	ClassDB::add_property("Multipliers", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("get_bump"), &Multipliers::get_bump);
	ClassDB::bind_method(D_METHOD("set_bump", "value"), &Multipliers::set_bump);
	ClassDB::add_property("Multipliers", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
	ClassDB::bind_method(D_METHOD("get_stakingPool"), &Multipliers::get_stakingPool);
	ClassDB::bind_method(D_METHOD("set_stakingPool", "value"), &Multipliers::set_stakingPool);
	ClassDB::add_property("Multipliers", PropertyInfo(Variant::Type::OBJECT, "stakingPool"), "set_stakingPool", "get_stakingPool");
	ClassDB::bind_method(D_METHOD("get_decimals"), &Multipliers::get_decimals);
	ClassDB::bind_method(D_METHOD("set_decimals", "value"), &Multipliers::set_decimals);
	ClassDB::add_property("Multipliers", PropertyInfo(Variant::Type::INT, "decimals"), "set_decimals", "get_decimals");
	ClassDB::bind_method(D_METHOD("get_durationMultipliers"), &Multipliers::get_durationMultipliers);
	ClassDB::bind_method(D_METHOD("set_durationMultipliers", "value"), &Multipliers::set_durationMultipliers);
	ClassDB::add_property("Multipliers", PropertyInfo(Variant::Type::ARRAY, "durationMultipliers"), "set_durationMultipliers", "get_durationMultipliers");
	ClassDB::bind_method(D_METHOD("get_countMultipliers"), &Multipliers::get_countMultipliers);
	ClassDB::bind_method(D_METHOD("set_countMultipliers", "value"), &Multipliers::set_countMultipliers);
	ClassDB::add_property("Multipliers", PropertyInfo(Variant::Type::ARRAY, "countMultipliers"), "set_countMultipliers", "get_countMultipliers");
	ClassDB::bind_method(D_METHOD("get_creatorMultipliers"), &Multipliers::get_creatorMultipliers);
	ClassDB::bind_method(D_METHOD("set_creatorMultipliers", "value"), &Multipliers::set_creatorMultipliers);
	ClassDB::add_property("Multipliers", PropertyInfo(Variant::Type::ARRAY, "creatorMultipliers"), "set_creatorMultipliers", "get_creatorMultipliers");
	ClassDB::bind_method(D_METHOD("get_collectionMultipliers"), &Multipliers::get_collectionMultipliers);
	ClassDB::bind_method(D_METHOD("set_collectionMultipliers", "value"), &Multipliers::set_collectionMultipliers);
	ClassDB::add_property("Multipliers", PropertyInfo(Variant::Type::ARRAY, "collectionMultipliers"), "set_collectionMultipliers", "get_collectionMultipliers");
	ClassDB::bind_method(D_METHOD("to_dict"), &Multipliers::to_dict);
}
Dictionary Multipliers::to_dict() {
	Dictionary res;
	res["address"] = address;
	res["bump"] = bump;
	if (stakingPool.has_method("to_string")) {
		res["stakingPool"] = stakingPool.call("to_string");
	} else {
		res["stakingPool"] = stakingPool;
	}
	res["decimals"] = decimals;
	res["durationMultipliers"] = durationMultipliers;
	res["countMultipliers"] = countMultipliers;
	res["creatorMultipliers"] = creatorMultipliers;
	res["collectionMultipliers"] = collectionMultipliers;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot