#include "honeycomb/types/MissionReward_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void MissionReward::set_kind(const String &val) {
	this->kind = val;
}

String MissionReward::get_kind() const {
	return this->kind;
}

void MissionReward::set_min(const int64_t &val) {
	this->min = val;
}

int64_t MissionReward::get_min() const {
	return this->min;
}

void MissionReward::set_max(const int64_t &val) {
	this->max = val;
}

int64_t MissionReward::get_max() const {
	return this->max;
}

void MissionReward::set_resource(const String &val) {
	this->resource = val;
}

String MissionReward::get_resource() const {
	return this->resource;
}

void MissionReward::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kind"), &MissionReward::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &MissionReward::set_kind);
	ClassDB::add_property("MissionReward", PropertyInfo(Variant::STRING, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("get_min"), &MissionReward::get_min);
	ClassDB::bind_method(D_METHOD("set_min", "value"), &MissionReward::set_min);
	ClassDB::add_property("MissionReward", PropertyInfo(Variant::Type::INT, "min"), "set_min", "get_min");
	ClassDB::bind_method(D_METHOD("get_max"), &MissionReward::get_max);
	ClassDB::bind_method(D_METHOD("set_max", "value"), &MissionReward::set_max);
	ClassDB::add_property("MissionReward", PropertyInfo(Variant::Type::INT, "max"), "set_max", "get_max");
	ClassDB::bind_method(D_METHOD("get_resource"), &MissionReward::get_resource);
	ClassDB::bind_method(D_METHOD("set_resource", "value"), &MissionReward::set_resource);
	ClassDB::add_property("MissionReward", PropertyInfo(Variant::Type::STRING, "resource"), "set_resource", "get_resource");
	ClassDB::bind_method(D_METHOD("to_dict"), &MissionReward::to_dict);
}
Dictionary MissionReward::to_dict() {
	Dictionary res;
	res["kind"] = this->kind;
	res["min"] = min;
	res["max"] = max;
	if (!resource.is_empty()) {
		res["resource"] = resource;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot