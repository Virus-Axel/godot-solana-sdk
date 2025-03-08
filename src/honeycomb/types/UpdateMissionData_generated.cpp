#include "honeycomb/types/UpdateMissionData_generated.hpp"

#include "honeycomb/types/NewMissionCost_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void UpdateMissionData::set_mission(const String &val) {
	this->mission = val;
}

String UpdateMissionData::get_mission() const {
	return this->mission;
}

void UpdateMissionData::set_authority(const String &val) {
	this->authority = val;
}

String UpdateMissionData::get_authority() const {
	return this->authority;
}

void UpdateMissionData::set_delegateAuthority(const String &val) {
	this->delegateAuthority = val;
}

String UpdateMissionData::get_delegateAuthority() const {
	return this->delegateAuthority;
}

void UpdateMissionData::set_payer(const String &val) {
	this->payer = val;
}

String UpdateMissionData::get_payer() const {
	return this->payer;
}

void UpdateMissionData::set_name(const String &val) {
	this->name = val;
}

String UpdateMissionData::get_name() const {
	return this->name;
}

void UpdateMissionData::set_minXp(const int64_t &val) {
	this->minXp = val;
}

int64_t UpdateMissionData::get_minXp() const {
	return this->minXp;
}

void UpdateMissionData::set_cost(const Variant &val) {
	this->cost = val;
}

Variant UpdateMissionData::get_cost() const {
	return this->cost;
}

void UpdateMissionData::set_duration(const int64_t &val) {
	this->duration = val;
}

int64_t UpdateMissionData::get_duration() const {
	return this->duration;
}

void UpdateMissionData::set_removeAllRewards(const bool &val) {
	this->removeAllRewards = val;
}

bool UpdateMissionData::get_removeAllRewards() const {
	return this->removeAllRewards;
}

void UpdateMissionData::set_addRewards(const Array &val) {
	this->addRewards = val;
}

Array UpdateMissionData::get_addRewards() const {
	return this->addRewards;
}

void UpdateMissionData::set_removeRewardIndices(const PackedInt32Array &val) {
	this->removeRewardIndices = val;
}

PackedInt32Array UpdateMissionData::get_removeRewardIndices() const {
	return this->removeRewardIndices;
}

void UpdateMissionData::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_mission"), &UpdateMissionData::get_mission);
	ClassDB::bind_method(D_METHOD("set_mission", "value"), &UpdateMissionData::set_mission);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::STRING, "mission"), "set_mission", "get_mission");
	ClassDB::bind_method(D_METHOD("get_authority"), &UpdateMissionData::get_authority);
	ClassDB::bind_method(D_METHOD("set_authority", "value"), &UpdateMissionData::set_authority);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::STRING, "authority"), "set_authority", "get_authority");
	ClassDB::bind_method(D_METHOD("get_delegateAuthority"), &UpdateMissionData::get_delegateAuthority);
	ClassDB::bind_method(D_METHOD("set_delegateAuthority", "value"), &UpdateMissionData::set_delegateAuthority);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::STRING, "delegateAuthority"), "set_delegateAuthority", "get_delegateAuthority");
	ClassDB::bind_method(D_METHOD("get_payer"), &UpdateMissionData::get_payer);
	ClassDB::bind_method(D_METHOD("set_payer", "value"), &UpdateMissionData::set_payer);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::STRING, "payer"), "set_payer", "get_payer");
	ClassDB::bind_method(D_METHOD("get_name"), &UpdateMissionData::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "value"), &UpdateMissionData::set_name);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
	ClassDB::bind_method(D_METHOD("get_minXp"), &UpdateMissionData::get_minXp);
	ClassDB::bind_method(D_METHOD("set_minXp", "value"), &UpdateMissionData::set_minXp);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::INT, "minXp"), "set_minXp", "get_minXp");
	ClassDB::bind_method(D_METHOD("get_cost"), &UpdateMissionData::get_cost);
	ClassDB::bind_method(D_METHOD("set_cost", "value"), &UpdateMissionData::set_cost);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::OBJECT, "cost"), "set_cost", "get_cost");
	ClassDB::bind_method(D_METHOD("get_duration"), &UpdateMissionData::get_duration);
	ClassDB::bind_method(D_METHOD("set_duration", "value"), &UpdateMissionData::set_duration);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::INT, "duration"), "set_duration", "get_duration");
	ClassDB::bind_method(D_METHOD("get_removeAllRewards"), &UpdateMissionData::get_removeAllRewards);
	ClassDB::bind_method(D_METHOD("set_removeAllRewards", "value"), &UpdateMissionData::set_removeAllRewards);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::BOOL, "removeAllRewards"), "set_removeAllRewards", "get_removeAllRewards");
	ClassDB::bind_method(D_METHOD("get_addRewards"), &UpdateMissionData::get_addRewards);
	ClassDB::bind_method(D_METHOD("set_addRewards", "value"), &UpdateMissionData::set_addRewards);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::ARRAY, "addRewards"), "set_addRewards", "get_addRewards");
	ClassDB::bind_method(D_METHOD("get_removeRewardIndices"), &UpdateMissionData::get_removeRewardIndices);
	ClassDB::bind_method(D_METHOD("set_removeRewardIndices", "value"), &UpdateMissionData::set_removeRewardIndices);
	ClassDB::add_property("UpdateMissionData", PropertyInfo(Variant::Type::PACKED_INT32_ARRAY, "removeRewardIndices"), "set_removeRewardIndices", "get_removeRewardIndices");
	ClassDB::bind_method(D_METHOD("to_dict"), &UpdateMissionData::to_dict);
}
Dictionary UpdateMissionData::to_dict() {
	Dictionary res;
	if (!mission.is_empty()) {
		res["mission"] = mission;
	}
	if (!authority.is_empty()) {
		res["authority"] = authority;
	}
	if (!delegateAuthority.is_empty()) {
		res["delegateAuthority"] = delegateAuthority;
	}
	if (!payer.is_empty()) {
		res["payer"] = payer;
	}
	if (!name.is_empty()) {
		res["name"] = name;
	}
	res["minXp"] = minXp;

	if (cost.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::NewMissionCost>(cost);
		if (ptr) {
			res["cost"] = ptr->to_dict();
		}
	}
	res["duration"] = duration;
	res["removeAllRewards"] = removeAllRewards;
	res["addRewards"] = addRewards;
	res["removeRewardIndices"] = removeRewardIndices;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot