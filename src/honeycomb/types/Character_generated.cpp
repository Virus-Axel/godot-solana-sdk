#include "honeycomb/types/Character_generated.hpp"

#include "honeycomb/types/CharacterCooldown_generated.hpp"
#include "honeycomb/types/CharacterSource_generated.hpp"
#include "honeycomb/types/CharacterUsedBy_generated.hpp"
#include "honeycomb/types/Proof_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Character::set_address(const PackedByteArray &val) {
	this->address = val;
}

PackedByteArray Character::get_address() const {
	return this->address;
}

void Character::set_tree_id(const PackedByteArray &val) {
	this->tree_id = val;
}

PackedByteArray Character::get_tree_id() const {
	return this->tree_id;
}

void Character::set_leaf_idx(const int64_t &val) {
	this->leaf_idx = val;
}

int64_t Character::get_leaf_idx() const {
	return this->leaf_idx;
}

void Character::set_owner(const Variant &val) {
	this->owner = val;
}

Variant Character::get_owner() const {
	return this->owner;
}

void Character::set_source(const Variant &val) {
	this->source = val;
}

Variant Character::get_source() const {
	return this->source;
}

void Character::set_usedBy(const Variant &val) {
	this->usedBy = val;
}

Variant Character::get_usedBy() const {
	return this->usedBy;
}

void Character::set_equipments(const Array &val) {
	this->equipments = val;
}

Array Character::get_equipments() const {
	return this->equipments;
}

void Character::set_proof(const Variant &val) {
	this->proof = val;
}

Variant Character::get_proof() const {
	return this->proof;
}

void Character::set_asset(const Dictionary &val) {
	this->asset = val;
}

Dictionary Character::get_asset() const {
	return this->asset;
}

void Character::set_cooldown(const Variant &val) {
	this->cooldown = val;
}

Variant Character::get_cooldown() const {
	return this->cooldown;
}

void Character::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &Character::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &Character::set_address);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("get_tree_id"), &Character::get_tree_id);
	ClassDB::bind_method(D_METHOD("set_tree_id", "value"), &Character::set_tree_id);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree_id"), "set_tree_id", "get_tree_id");
	ClassDB::bind_method(D_METHOD("get_leaf_idx"), &Character::get_leaf_idx);
	ClassDB::bind_method(D_METHOD("set_leaf_idx", "value"), &Character::set_leaf_idx);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::INT, "leaf_idx"), "set_leaf_idx", "get_leaf_idx");
	ClassDB::bind_method(D_METHOD("get_owner"), &Character::get_owner);
	ClassDB::bind_method(D_METHOD("set_owner", "value"), &Character::set_owner);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::OBJECT, "owner"), "set_owner", "get_owner");
	ClassDB::bind_method(D_METHOD("get_source"), &Character::get_source);
	ClassDB::bind_method(D_METHOD("set_source", "value"), &Character::set_source);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::OBJECT, "source"), "set_source", "get_source");
	ClassDB::bind_method(D_METHOD("get_usedBy"), &Character::get_usedBy);
	ClassDB::bind_method(D_METHOD("set_usedBy", "value"), &Character::set_usedBy);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::OBJECT, "usedBy"), "set_usedBy", "get_usedBy");
	ClassDB::bind_method(D_METHOD("get_equipments"), &Character::get_equipments);
	ClassDB::bind_method(D_METHOD("set_equipments", "value"), &Character::set_equipments);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::ARRAY, "equipments"), "set_equipments", "get_equipments");
	ClassDB::bind_method(D_METHOD("get_proof"), &Character::get_proof);
	ClassDB::bind_method(D_METHOD("set_proof", "value"), &Character::set_proof);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::OBJECT, "proof"), "set_proof", "get_proof");
	ClassDB::bind_method(D_METHOD("get_asset"), &Character::get_asset);
	ClassDB::bind_method(D_METHOD("set_asset", "value"), &Character::set_asset);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::DICTIONARY, "asset"), "set_asset", "get_asset");
	ClassDB::bind_method(D_METHOD("get_cooldown"), &Character::get_cooldown);
	ClassDB::bind_method(D_METHOD("set_cooldown", "value"), &Character::set_cooldown);
	ClassDB::add_property("Character", PropertyInfo(Variant::Type::OBJECT, "cooldown"), "set_cooldown", "get_cooldown");
	ClassDB::bind_method(D_METHOD("to_dict"), &Character::to_dict);
}
Dictionary Character::to_dict() {
	Dictionary res;
	res["address"] = address;
	res["tree_id"] = tree_id;
	res["leaf_idx"] = leaf_idx;
	if (owner.has_method("to_string")) {
		res["owner"] = owner.call("to_string");
	} else {
		res["owner"] = owner;
	}

	if (source.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::CharacterSource>(source);
		if (ptr) {
			res["source"] = ptr->to_dict();
		}
	}

	if (usedBy.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::CharacterUsedBy>(usedBy);
		if (ptr) {
			res["usedBy"] = ptr->to_dict();
		}
	}
	res["equipments"] = equipments;

	if (proof.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::Proof>(proof);
		if (ptr) {
			res["proof"] = ptr->to_dict();
		}
	}
	res["asset"] = asset;

	if (cooldown.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::CharacterCooldown>(cooldown);
		if (ptr) {
			res["cooldown"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot