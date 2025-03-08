#include "honeycomb/types/CharacterModel_generated.hpp"

#include "honeycomb/types/CharacterCooldown_generated.hpp"
#include "honeycomb/types/ControlledMerkleTrees_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void CharacterModel::set_address(const PackedByteArray &val) {
	this->address = val;
}

PackedByteArray CharacterModel::get_address() const {
	return this->address;
}

void CharacterModel::set_program_id(const PackedByteArray &val) {
	this->program_id = val;
}

PackedByteArray CharacterModel::get_program_id() const {
	return this->program_id;
}

void CharacterModel::set_discriminator(const PackedByteArray &val) {
	this->discriminator = val;
}

PackedByteArray CharacterModel::get_discriminator() const {
	return this->discriminator;
}

void CharacterModel::set_bump(const int32_t &val) {
	this->bump = val;
}

int32_t CharacterModel::get_bump() const {
	return this->bump;
}

void CharacterModel::set_key(const Variant &val) {
	this->key = val;
}

Variant CharacterModel::get_key() const {
	return this->key;
}

void CharacterModel::set_project(const Variant &val) {
	this->project = val;
}

Variant CharacterModel::get_project() const {
	return this->project;
}

void CharacterModel::set_config(const Dictionary &val) {
	this->config = val;
}

Dictionary CharacterModel::get_config() const {
	return this->config;
}

void CharacterModel::set_attributes(const Array &val) {
	this->attributes = val;
}

Array CharacterModel::get_attributes() const {
	return this->attributes;
}

void CharacterModel::set_merkle_trees(const Variant &val) {
	this->merkle_trees = val;
}

Variant CharacterModel::get_merkle_trees() const {
	return this->merkle_trees;
}

void CharacterModel::set_cooldown(const Variant &val) {
	this->cooldown = val;
}

Variant CharacterModel::get_cooldown() const {
	return this->cooldown;
}

void CharacterModel::set_equipableCriteria(const PackedStringArray &val) {
	this->equipableCriteria = val;
}

PackedStringArray CharacterModel::get_equipableCriteria() const {
	return this->equipableCriteria;
}

void CharacterModel::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &CharacterModel::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &CharacterModel::set_address);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("get_program_id"), &CharacterModel::get_program_id);
	ClassDB::bind_method(D_METHOD("set_program_id", "value"), &CharacterModel::set_program_id);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "program_id"), "set_program_id", "get_program_id");
	ClassDB::bind_method(D_METHOD("get_discriminator"), &CharacterModel::get_discriminator);
	ClassDB::bind_method(D_METHOD("set_discriminator", "value"), &CharacterModel::set_discriminator);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "discriminator"), "set_discriminator", "get_discriminator");
	ClassDB::bind_method(D_METHOD("get_bump"), &CharacterModel::get_bump);
	ClassDB::bind_method(D_METHOD("set_bump", "value"), &CharacterModel::set_bump);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
	ClassDB::bind_method(D_METHOD("get_key"), &CharacterModel::get_key);
	ClassDB::bind_method(D_METHOD("set_key", "value"), &CharacterModel::set_key);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::OBJECT, "key"), "set_key", "get_key");
	ClassDB::bind_method(D_METHOD("get_project"), &CharacterModel::get_project);
	ClassDB::bind_method(D_METHOD("set_project", "value"), &CharacterModel::set_project);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
	ClassDB::bind_method(D_METHOD("get_config"), &CharacterModel::get_config);
	ClassDB::bind_method(D_METHOD("set_config", "value"), &CharacterModel::set_config);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::DICTIONARY, "config"), "set_config", "get_config");
	ClassDB::bind_method(D_METHOD("get_attributes"), &CharacterModel::get_attributes);
	ClassDB::bind_method(D_METHOD("set_attributes", "value"), &CharacterModel::set_attributes);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::ARRAY, "attributes"), "set_attributes", "get_attributes");
	ClassDB::bind_method(D_METHOD("get_merkle_trees"), &CharacterModel::get_merkle_trees);
	ClassDB::bind_method(D_METHOD("set_merkle_trees", "value"), &CharacterModel::set_merkle_trees);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::OBJECT, "merkle_trees"), "set_merkle_trees", "get_merkle_trees");
	ClassDB::bind_method(D_METHOD("get_cooldown"), &CharacterModel::get_cooldown);
	ClassDB::bind_method(D_METHOD("set_cooldown", "value"), &CharacterModel::set_cooldown);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::OBJECT, "cooldown"), "set_cooldown", "get_cooldown");
	ClassDB::bind_method(D_METHOD("get_equipableCriteria"), &CharacterModel::get_equipableCriteria);
	ClassDB::bind_method(D_METHOD("set_equipableCriteria", "value"), &CharacterModel::set_equipableCriteria);
	ClassDB::add_property("CharacterModel", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "equipableCriteria"), "set_equipableCriteria", "get_equipableCriteria");
	ClassDB::bind_method(D_METHOD("to_dict"), &CharacterModel::to_dict);
}
Dictionary CharacterModel::to_dict() {
	Dictionary res;
	res["address"] = address;
	res["program_id"] = program_id;
	res["discriminator"] = discriminator;
	res["bump"] = bump;
	if (key.has_method("to_string")) {
		res["key"] = key.call("to_string");
	} else {
		res["key"] = key;
	}
	if (project.has_method("to_string")) {
		res["project"] = project.call("to_string");
	} else {
		res["project"] = project;
	}
	res["config"] = config;
	res["attributes"] = attributes;

	if (merkle_trees.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::ControlledMerkleTrees>(merkle_trees);
		if (ptr) {
			res["merkle_trees"] = ptr->to_dict();
		}
	}

	if (cooldown.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::CharacterCooldown>(cooldown);
		if (ptr) {
			res["cooldown"] = ptr->to_dict();
		}
	}
	res["equipableCriteria"] = equipableCriteria;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot