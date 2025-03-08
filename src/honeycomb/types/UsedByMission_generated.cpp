#include "honeycomb/types/UsedByMission_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void UsedByMission::set_missionId(const Variant &val) {
	this->missionId = val;
}

Variant UsedByMission::get_missionId() const {
	return this->missionId;
}

void UsedByMission::set_participationId(const String &val) {
	this->participationId = val;
}

String UsedByMission::get_participationId() const {
	return this->participationId;
}

void UsedByMission::set_rewards(const Array &val) {
	this->rewards = val;
}

Array UsedByMission::get_rewards() const {
	return this->rewards;
}

void UsedByMission::set_endTime(const int32_t &val) {
	this->endTime = val;
}

int32_t UsedByMission::get_endTime() const {
	return this->endTime;
}

void UsedByMission::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_missionId"), &UsedByMission::get_missionId);
	ClassDB::bind_method(D_METHOD("set_missionId", "value"), &UsedByMission::set_missionId);
	ClassDB::add_property("UsedByMission", PropertyInfo(Variant::Type::OBJECT, "missionId"), "set_missionId", "get_missionId");
	ClassDB::bind_method(D_METHOD("get_participationId"), &UsedByMission::get_participationId);
	ClassDB::bind_method(D_METHOD("set_participationId", "value"), &UsedByMission::set_participationId);
	ClassDB::add_property("UsedByMission", PropertyInfo(Variant::Type::STRING, "participationId"), "set_participationId", "get_participationId");
	ClassDB::bind_method(D_METHOD("get_rewards"), &UsedByMission::get_rewards);
	ClassDB::bind_method(D_METHOD("set_rewards", "value"), &UsedByMission::set_rewards);
	ClassDB::add_property("UsedByMission", PropertyInfo(Variant::Type::ARRAY, "rewards"), "set_rewards", "get_rewards");
	ClassDB::bind_method(D_METHOD("get_endTime"), &UsedByMission::get_endTime);
	ClassDB::bind_method(D_METHOD("set_endTime", "value"), &UsedByMission::set_endTime);
	ClassDB::add_property("UsedByMission", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
	ClassDB::bind_method(D_METHOD("to_dict"), &UsedByMission::to_dict);
}
Dictionary UsedByMission::to_dict() {
	Dictionary res;
	if (missionId.has_method("to_string")) {
		res["missionId"] = missionId.call("to_string");
	} else {
		res["missionId"] = missionId;
	}
	if (!participationId.is_empty()) {
		res["participationId"] = participationId;
	}
	res["rewards"] = rewards;
	res["endTime"] = endTime;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot