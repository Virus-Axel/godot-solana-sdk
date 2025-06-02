#include "honeycomb/types/CreateSplStakingPoolMetadataInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateSplStakingPoolMetadataInput::set_name(const String &val) {
	this->name = val;
}

String CreateSplStakingPoolMetadataInput::get_name() const {
	return this->name;
}

void CreateSplStakingPoolMetadataInput::set_minStakeDurationSecs(const int64_t &val) {
	this->minStakeDurationSecs = val;
}

int64_t CreateSplStakingPoolMetadataInput::get_minStakeDurationSecs() const {
	return this->minStakeDurationSecs;
}

void CreateSplStakingPoolMetadataInput::set_maxStakeDurationSecs(const int64_t &val) {
	this->maxStakeDurationSecs = val;
}

int64_t CreateSplStakingPoolMetadataInput::get_maxStakeDurationSecs() const {
	return this->maxStakeDurationSecs;
}

void CreateSplStakingPoolMetadataInput::set_startTime(const int64_t &val) {
	this->startTime = val;
}

int64_t CreateSplStakingPoolMetadataInput::get_startTime() const {
	return this->startTime;
}

void CreateSplStakingPoolMetadataInput::set_endTime(const int64_t &val) {
	this->endTime = val;
}

int64_t CreateSplStakingPoolMetadataInput::get_endTime() const {
	return this->endTime;
}

void CreateSplStakingPoolMetadataInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_name"), &CreateSplStakingPoolMetadataInput::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "value"), &CreateSplStakingPoolMetadataInput::set_name);
	ClassDB::add_property("CreateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
	ClassDB::bind_method(D_METHOD("get_minStakeDurationSecs"), &CreateSplStakingPoolMetadataInput::get_minStakeDurationSecs);
	ClassDB::bind_method(D_METHOD("set_minStakeDurationSecs", "value"), &CreateSplStakingPoolMetadataInput::set_minStakeDurationSecs);
	ClassDB::add_property("CreateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "minStakeDurationSecs"), "set_minStakeDurationSecs", "get_minStakeDurationSecs");
	ClassDB::bind_method(D_METHOD("get_maxStakeDurationSecs"), &CreateSplStakingPoolMetadataInput::get_maxStakeDurationSecs);
	ClassDB::bind_method(D_METHOD("set_maxStakeDurationSecs", "value"), &CreateSplStakingPoolMetadataInput::set_maxStakeDurationSecs);
	ClassDB::add_property("CreateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "maxStakeDurationSecs"), "set_maxStakeDurationSecs", "get_maxStakeDurationSecs");
	ClassDB::bind_method(D_METHOD("get_startTime"), &CreateSplStakingPoolMetadataInput::get_startTime);
	ClassDB::bind_method(D_METHOD("set_startTime", "value"), &CreateSplStakingPoolMetadataInput::set_startTime);
	ClassDB::add_property("CreateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "startTime"), "set_startTime", "get_startTime");
	ClassDB::bind_method(D_METHOD("get_endTime"), &CreateSplStakingPoolMetadataInput::get_endTime);
	ClassDB::bind_method(D_METHOD("set_endTime", "value"), &CreateSplStakingPoolMetadataInput::set_endTime);
	ClassDB::add_property("CreateSplStakingPoolMetadataInput", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
	ClassDB::bind_method(D_METHOD("to_dict"), &CreateSplStakingPoolMetadataInput::to_dict);
}
Dictionary CreateSplStakingPoolMetadataInput::to_dict() {
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