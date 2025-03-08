#include "honeycomb/types/CharactersFilter_generated.hpp"

#include "honeycomb/types/CharacterSourceFilter_generated.hpp"
#include "honeycomb/types/CharacterUsedByFilter_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void CharactersFilter::set_owner(const Variant &val) {
	this->owner = val;
}

Variant CharactersFilter::get_owner() const {
	return this->owner;
}

void CharactersFilter::set_source(const Variant &val) {
	this->source = val;
}

Variant CharactersFilter::get_source() const {
	return this->source;
}

void CharactersFilter::set_usedBy(const Variant &val) {
	this->usedBy = val;
}

Variant CharactersFilter::get_usedBy() const {
	return this->usedBy;
}

void CharactersFilter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_owner"), &CharactersFilter::get_owner);
	ClassDB::bind_method(D_METHOD("set_owner", "value"), &CharactersFilter::set_owner);
	ClassDB::add_property("CharactersFilter", PropertyInfo(Variant::Type::OBJECT, "owner"), "set_owner", "get_owner");
	ClassDB::bind_method(D_METHOD("get_source"), &CharactersFilter::get_source);
	ClassDB::bind_method(D_METHOD("set_source", "value"), &CharactersFilter::set_source);
	ClassDB::add_property("CharactersFilter", PropertyInfo(Variant::Type::OBJECT, "source"), "set_source", "get_source");
	ClassDB::bind_method(D_METHOD("get_usedBy"), &CharactersFilter::get_usedBy);
	ClassDB::bind_method(D_METHOD("set_usedBy", "value"), &CharactersFilter::set_usedBy);
	ClassDB::add_property("CharactersFilter", PropertyInfo(Variant::Type::OBJECT, "usedBy"), "set_usedBy", "get_usedBy");
	ClassDB::bind_method(D_METHOD("to_dict"), &CharactersFilter::to_dict);
}
Dictionary CharactersFilter::to_dict() {
	Dictionary res;
	if (owner.has_method("to_string")) {
		res["owner"] = owner.call("to_string");
	} else {
		res["owner"] = owner;
	}

	if (source.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::CharacterSourceFilter>(source);
		if (ptr) {
			res["source"] = ptr->to_dict();
		}
	}

	if (usedBy.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::CharacterUsedByFilter>(usedBy);
		if (ptr) {
			res["usedBy"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot