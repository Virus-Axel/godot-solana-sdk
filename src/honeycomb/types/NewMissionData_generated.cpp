#include "honeycomb/types/NewMissionData_generated.hpp"

#include "honeycomb/types/NewMissionCost_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void NewMissionData::set_project(const String& val) {
this->project = val;
}

String NewMissionData::get_project() const {
return this->project;
}

void NewMissionData::set_missionPool(const String& val) {
this->missionPool = val;
}

String NewMissionData::get_missionPool() const {
return this->missionPool;
}

void NewMissionData::set_authority(const String& val) {
this->authority = val;
}

String NewMissionData::get_authority() const {
return this->authority;
}

void NewMissionData::set_delegateAuthority(const String& val) {
this->delegateAuthority = val;
}

String NewMissionData::get_delegateAuthority() const {
return this->delegateAuthority;
}

void NewMissionData::set_payer(const String& val) {
this->payer = val;
}

String NewMissionData::get_payer() const {
return this->payer;
}

void NewMissionData::set_name(const String& val) {
this->name = val;
}

String NewMissionData::get_name() const {
return this->name;
}

void NewMissionData::set_minXp(const int64_t& val) {
this->minXp = val;
}

int64_t NewMissionData::get_minXp() const {
return this->minXp;
}

void NewMissionData::set_cost(const Variant& val) {
this->cost = val;
}

Variant NewMissionData::get_cost() const {
return this->cost;
}

void NewMissionData::set_duration(const int64_t& val) {
this->duration = val;
}

int64_t NewMissionData::get_duration() const {
return this->duration;
}

void NewMissionData::set_rewards(const Array& val) {
this->rewards = val;
}

Array NewMissionData::get_rewards() const {
return this->rewards;
}

void NewMissionData::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_project"), &NewMissionData::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &NewMissionData::set_project);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::STRING, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_missionPool"), &NewMissionData::get_missionPool);
ClassDB::bind_method(D_METHOD("set_missionPool", "value"), &NewMissionData::set_missionPool);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::STRING, "missionPool"), "set_missionPool", "get_missionPool");
ClassDB::bind_method(D_METHOD("get_authority"), &NewMissionData::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &NewMissionData::set_authority);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::STRING, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_delegateAuthority"), &NewMissionData::get_delegateAuthority);
ClassDB::bind_method(D_METHOD("set_delegateAuthority", "value"), &NewMissionData::set_delegateAuthority);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::STRING, "delegateAuthority"), "set_delegateAuthority", "get_delegateAuthority");
ClassDB::bind_method(D_METHOD("get_payer"), &NewMissionData::get_payer);
ClassDB::bind_method(D_METHOD("set_payer", "value"), &NewMissionData::set_payer);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::STRING, "payer"), "set_payer", "get_payer");
ClassDB::bind_method(D_METHOD("get_name"), &NewMissionData::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &NewMissionData::set_name);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_minXp"), &NewMissionData::get_minXp);
ClassDB::bind_method(D_METHOD("set_minXp", "value"), &NewMissionData::set_minXp);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::INT, "minXp"), "set_minXp", "get_minXp");
ClassDB::bind_method(D_METHOD("get_cost"), &NewMissionData::get_cost);
ClassDB::bind_method(D_METHOD("set_cost", "value"), &NewMissionData::set_cost);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::OBJECT, "cost"), "set_cost", "get_cost");
ClassDB::bind_method(D_METHOD("get_duration"), &NewMissionData::get_duration);
ClassDB::bind_method(D_METHOD("set_duration", "value"), &NewMissionData::set_duration);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::INT, "duration"), "set_duration", "get_duration");
ClassDB::bind_method(D_METHOD("get_rewards"), &NewMissionData::get_rewards);
ClassDB::bind_method(D_METHOD("set_rewards", "value"), &NewMissionData::set_rewards);
ClassDB::add_property("NewMissionData", PropertyInfo(Variant::Type::ARRAY, "rewards"), "set_rewards", "get_rewards");
ClassDB::bind_method(D_METHOD("to_dict"), &NewMissionData::to_dict);
}
Dictionary NewMissionData::to_dict() {
Dictionary res;
if (!project.is_empty()) {
    res["project"] = project;
}
if (!missionPool.is_empty()) {
    res["missionPool"] = missionPool;
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
              auto* ptr = Object::cast_to<godot::honeycomb_resource::NewMissionCost>(cost);
              if (ptr) {
                  res["cost"] = ptr->to_dict();
              }
          }
res["duration"] = duration;
res["rewards"] = rewards;
return res;
}
} // namespace honeycomb_resource
} // namespace godot