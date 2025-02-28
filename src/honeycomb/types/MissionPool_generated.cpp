#include "honeycomb/types/MissionPool_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void MissionPool::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray MissionPool::get_address() const {
return this->address;
}

void MissionPool::set_program_id(const PackedByteArray& val) {
this->program_id = val;
}

PackedByteArray MissionPool::get_program_id() const {
return this->program_id;
}

void MissionPool::set_discriminator(const PackedByteArray& val) {
this->discriminator = val;
}

PackedByteArray MissionPool::get_discriminator() const {
return this->discriminator;
}

void MissionPool::set_bump(const int32_t& val) {
this->bump = val;
}

int32_t MissionPool::get_bump() const {
return this->bump;
}

void MissionPool::set_project(const Variant& val) {
this->project = val;
}

Variant MissionPool::get_project() const {
return this->project;
}

void MissionPool::set_name(const String& val) {
this->name = val;
}

String MissionPool::get_name() const {
return this->name;
}

void MissionPool::set_factionsMerkleRoot(const PackedByteArray& val) {
this->factionsMerkleRoot = val;
}

PackedByteArray MissionPool::get_factionsMerkleRoot() const {
return this->factionsMerkleRoot;
}

void MissionPool::set_randomizerRound(const int32_t& val) {
this->randomizerRound = val;
}

int32_t MissionPool::get_randomizerRound() const {
return this->randomizerRound;
}

void MissionPool::set_characterModels(const Array& val) {
this->characterModels = val;
}

Array MissionPool::get_characterModels() const {
return this->characterModels;
}

void MissionPool::set_guildKits(const PackedInt32Array& val) {
this->guildKits = val;
}

PackedInt32Array MissionPool::get_guildKits() const {
return this->guildKits;
}

void MissionPool::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &MissionPool::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &MissionPool::set_address);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_program_id"), &MissionPool::get_program_id);
ClassDB::bind_method(D_METHOD("set_program_id", "value"), &MissionPool::set_program_id);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "program_id"), "set_program_id", "get_program_id");
ClassDB::bind_method(D_METHOD("get_discriminator"), &MissionPool::get_discriminator);
ClassDB::bind_method(D_METHOD("set_discriminator", "value"), &MissionPool::set_discriminator);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "discriminator"), "set_discriminator", "get_discriminator");
ClassDB::bind_method(D_METHOD("get_bump"), &MissionPool::get_bump);
ClassDB::bind_method(D_METHOD("set_bump", "value"), &MissionPool::set_bump);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
ClassDB::bind_method(D_METHOD("get_project"), &MissionPool::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &MissionPool::set_project);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_name"), &MissionPool::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &MissionPool::set_name);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_factionsMerkleRoot"), &MissionPool::get_factionsMerkleRoot);
ClassDB::bind_method(D_METHOD("set_factionsMerkleRoot", "value"), &MissionPool::set_factionsMerkleRoot);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "factionsMerkleRoot"), "set_factionsMerkleRoot", "get_factionsMerkleRoot");
ClassDB::bind_method(D_METHOD("get_randomizerRound"), &MissionPool::get_randomizerRound);
ClassDB::bind_method(D_METHOD("set_randomizerRound", "value"), &MissionPool::set_randomizerRound);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::INT, "randomizerRound"), "set_randomizerRound", "get_randomizerRound");
ClassDB::bind_method(D_METHOD("get_characterModels"), &MissionPool::get_characterModels);
ClassDB::bind_method(D_METHOD("set_characterModels", "value"), &MissionPool::set_characterModels);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::ARRAY, "characterModels"), "set_characterModels", "get_characterModels");
ClassDB::bind_method(D_METHOD("get_guildKits"), &MissionPool::get_guildKits);
ClassDB::bind_method(D_METHOD("set_guildKits", "value"), &MissionPool::set_guildKits);
ClassDB::add_property("MissionPool", PropertyInfo(Variant::Type::PACKED_INT32_ARRAY, "guildKits"), "set_guildKits", "get_guildKits");
ClassDB::bind_method(D_METHOD("to_dict"), &MissionPool::to_dict);
}
Dictionary MissionPool::to_dict() {
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
if (!name.is_empty()) {
    res["name"] = name;
}
res["factionsMerkleRoot"] = factionsMerkleRoot;
res["randomizerRound"] = randomizerRound;
res["characterModels"] = characterModels;
res["guildKits"] = guildKits;
return res;
}
} // namespace honeycomb_resource
} // namespace godot