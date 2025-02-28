#include "honeycomb/types/Mission_generated.hpp"

#include "pubkey.hpp"
#include "honeycomb/types/MissionCost_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void Mission::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray Mission::get_address() const {
return this->address;
}

void Mission::set_program_id(const PackedByteArray& val) {
this->program_id = val;
}

PackedByteArray Mission::get_program_id() const {
return this->program_id;
}

void Mission::set_discriminator(const PackedByteArray& val) {
this->discriminator = val;
}

PackedByteArray Mission::get_discriminator() const {
return this->discriminator;
}

void Mission::set_bump(const int32_t& val) {
this->bump = val;
}

int32_t Mission::get_bump() const {
return this->bump;
}

void Mission::set_project(const Variant& val) {
this->project = val;
}

Variant Mission::get_project() const {
return this->project;
}

void Mission::set_missionPool(const Variant& val) {
this->missionPool = val;
}

Variant Mission::get_missionPool() const {
return this->missionPool;
}

void Mission::set_name(const String& val) {
this->name = val;
}

String Mission::get_name() const {
return this->name;
}

void Mission::set_minXp(const int64_t& val) {
this->minXp = val;
}

int64_t Mission::get_minXp() const {
return this->minXp;
}

void Mission::set_cost(const Variant& val) {
this->cost = val;
}

Variant Mission::get_cost() const {
return this->cost;
}

void Mission::set_requirement(const Dictionary& val) {
this->requirement = val;
}

Dictionary Mission::get_requirement() const {
return this->requirement;
}

void Mission::set_rewards(const Array& val) {
this->rewards = val;
}

Array Mission::get_rewards() const {
return this->rewards;
}

void Mission::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &Mission::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &Mission::set_address);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_program_id"), &Mission::get_program_id);
ClassDB::bind_method(D_METHOD("set_program_id", "value"), &Mission::set_program_id);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "program_id"), "set_program_id", "get_program_id");
ClassDB::bind_method(D_METHOD("get_discriminator"), &Mission::get_discriminator);
ClassDB::bind_method(D_METHOD("set_discriminator", "value"), &Mission::set_discriminator);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "discriminator"), "set_discriminator", "get_discriminator");
ClassDB::bind_method(D_METHOD("get_bump"), &Mission::get_bump);
ClassDB::bind_method(D_METHOD("set_bump", "value"), &Mission::set_bump);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
ClassDB::bind_method(D_METHOD("get_project"), &Mission::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &Mission::set_project);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_missionPool"), &Mission::get_missionPool);
ClassDB::bind_method(D_METHOD("set_missionPool", "value"), &Mission::set_missionPool);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::OBJECT, "missionPool"), "set_missionPool", "get_missionPool");
ClassDB::bind_method(D_METHOD("get_name"), &Mission::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &Mission::set_name);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_minXp"), &Mission::get_minXp);
ClassDB::bind_method(D_METHOD("set_minXp", "value"), &Mission::set_minXp);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::INT, "minXp"), "set_minXp", "get_minXp");
ClassDB::bind_method(D_METHOD("get_cost"), &Mission::get_cost);
ClassDB::bind_method(D_METHOD("set_cost", "value"), &Mission::set_cost);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::OBJECT, "cost"), "set_cost", "get_cost");
ClassDB::bind_method(D_METHOD("get_requirement"), &Mission::get_requirement);
ClassDB::bind_method(D_METHOD("set_requirement", "value"), &Mission::set_requirement);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::DICTIONARY, "requirement"), "set_requirement", "get_requirement");
ClassDB::bind_method(D_METHOD("get_rewards"), &Mission::get_rewards);
ClassDB::bind_method(D_METHOD("set_rewards", "value"), &Mission::set_rewards);
ClassDB::add_property("Mission", PropertyInfo(Variant::Type::ARRAY, "rewards"), "set_rewards", "get_rewards");
ClassDB::bind_method(D_METHOD("to_dict"), &Mission::to_dict);
}
Dictionary Mission::to_dict() {
Dictionary res;
res["address"] = address;
res["program_id"] = program_id;
res["discriminator"] = discriminator;
res["bump"] = bump;
if (project.has_method("to_string")) {
res["project"] = project.call("to_string");
} else {
res["project"] = project;
}
if (missionPool.has_method("to_string")) {
res["missionPool"] = missionPool.call("to_string");
} else {
res["missionPool"] = missionPool;
}
if (!name.is_empty()) {
    res["name"] = name;
}
res["minXp"] = minXp;

          if (cost.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::MissionCost>(cost);
              if (ptr) {
                  res["cost"] = ptr->to_dict();
              }
          }
res["requirement"] = requirement;
res["rewards"] = rewards;
return res;
}
} // namespace honeycomb_resource
} // namespace godot