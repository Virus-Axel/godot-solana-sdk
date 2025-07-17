#include "honeycomb/types/UpdateSplStakingPoolMetadataInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void UpdateSplStakingPoolMetadataInput::set_name(const String &val) {
	this->name = val;
}

String UpdateSplStakingPoolMetadataInput::get_name() const {
	return this->name;
}

void UpdateSplStakingPoolMetadataInput::set_minStakeDurationSecs(const int64_t &val) {
	this->minStakeDurationSecs = val;
}

int64_t UpdateSplStakingPoolMetadataInput::get_minStakeDurationSecs() const {
	return this->minStakeDurationSecs;
}

void UpdateSplStakingPoolMetadataInput::set_maxStakeDurationSecs(const int64_t &val) {
	this->maxStakeDurationSecs = val;
}

int64_t UpdateSplStakingPoolMetadataInput::get_maxStakeDurationSecs() const {
	return this->maxStakeDurationSecs;
}

void UpdateSplStakingPoolMetadataInput::set_startTime(const int64_t &val) {
	this->startTime = val;
}

int64_t UpdateSplStakingPoolMetadataInput::get_startTime() const {
	return this->startTime;
}

void UpdateSplStakingPoolMetadataInput::set_endTime(const int64_t &val) {
	this->endTime = val;
}

int64_t UpdateSplStakingPoolMetadataInput::get_endTime() const {
	return this->endTime;
}

void UpdateSplStakingPoolMetadataInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_name"), &UpdateSplStakingPoolMetadataInput::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "value"), &UpdateSplStakingPoolMetadataInput::set_name);
	ClassDB::add_property("UpdateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
	ClassDB::bind_method(D_METHOD("get_minStakeDurationSecs"), &UpdateSplStakingPoolMetadataInput::get_minStakeDurationSecs);
	ClassDB::bind_method(D_METHOD("set_minStakeDurationSecs", "value"), &UpdateSplStakingPoolMetadataInput::set_minStakeDurationSecs);
	ClassDB::add_property("UpdateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "minStakeDurationSecs"), "set_minStakeDurationSecs", "get_minStakeDurationSecs");
	ClassDB::bind_method(D_METHOD("get_maxStakeDurationSecs"), &UpdateSplStakingPoolMetadataInput::get_maxStakeDurationSecs);
	ClassDB::bind_method(D_METHOD("set_maxStakeDurationSecs", "value"), &UpdateSplStakingPoolMetadataInput::set_maxStakeDurationSecs);
	ClassDB::add_property("UpdateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "maxStakeDurationSecs"), "set_maxStakeDurationSecs", "get_maxStakeDurationSecs");
	ClassDB::bind_method(D_METHOD("get_startTime"), &UpdateSplStakingPoolMetadataInput::get_startTime);
	ClassDB::bind_method(D_METHOD("set_startTime", "value"), &UpdateSplStakingPoolMetadataInput::set_startTime);
	ClassDB::add_property("UpdateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "startTime"), "set_startTime", "get_startTime");
	ClassDB::bind_method(D_METHOD("get_endTime"), &UpdateSplStakingPoolMetadataInput::get_endTime);
	ClassDB::bind_method(D_METHOD("set_endTime", "value"), &UpdateSplStakingPoolMetadataInput::set_endTime);
	ClassDB::add_property("UpdateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
	ClassDB::bind_method(D_METHOD("to_dict"), &UpdateSplStakingPoolMetadataInput::to_dict);
}
Dictionary UpdateSplStakingPoolMetadataInput::to_dict() {
	Dictionary res;
	if (!name.is_empty()) {
		res["name"] = name;
	}
	res["minStakeDurationSecs"] = minStakeDurationSecs;
	res["maxStakeDurationSecs"] = maxStakeDurationSecs;
	res["startTime"] = startTime;
	res["endTime"] = endTime;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot