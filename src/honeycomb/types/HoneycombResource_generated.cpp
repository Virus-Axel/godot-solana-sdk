#include "honeycomb/types/HoneycombResource_generated.hpp"

#include "honeycomb/types/ResourceKind_generated.hpp"
#include "honeycomb/types/ResourceStorage_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void HoneycombResource::set_address(const PackedByteArray &val) {
	this->address = val;
}

PackedByteArray HoneycombResource::get_address() const {
	return this->address;
}

void HoneycombResource::set_bump(const int32_t &val) {
	this->bump = val;
}

int32_t HoneycombResource::get_bump() const {
	return this->bump;
}

void HoneycombResource::set_project(const Variant &val) {
	this->project = val;
}

Variant HoneycombResource::get_project() const {
	return this->project;
}

void HoneycombResource::set_mint(const Variant &val) {
	this->mint = val;
}

Variant HoneycombResource::get_mint() const {
	return this->mint;
}

void HoneycombResource::set_storage(const Variant &val) {
	this->storage = val;
}

Variant HoneycombResource::get_storage() const {
	return this->storage;
}

void HoneycombResource::set_kind(const Variant &val) {
	this->kind = val;
}

Variant HoneycombResource::get_kind() const {
	return this->kind;
}

void HoneycombResource::set_tags(const PackedStringArray &val) {
	this->tags = val;
}

PackedStringArray HoneycombResource::get_tags() const {
	return this->tags;
}

void HoneycombResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &HoneycombResource::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &HoneycombResource::set_address);
	ClassDB::add_property("HoneycombResource", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("get_bump"), &HoneycombResource::get_bump);
	ClassDB::bind_method(D_METHOD("set_bump", "value"), &HoneycombResource::set_bump);
	ClassDB::add_property("HoneycombResource", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
	ClassDB::bind_method(D_METHOD("get_project"), &HoneycombResource::get_project);
	ClassDB::bind_method(D_METHOD("set_project", "value"), &HoneycombResource::set_project);
	ClassDB::add_property("HoneycombResource", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
	ClassDB::bind_method(D_METHOD("get_mint"), &HoneycombResource::get_mint);
	ClassDB::bind_method(D_METHOD("set_mint", "value"), &HoneycombResource::set_mint);
	ClassDB::add_property("HoneycombResource", PropertyInfo(Variant::Type::OBJECT, "mint"), "set_mint", "get_mint");
	ClassDB::bind_method(D_METHOD("get_storage"), &HoneycombResource::get_storage);
	ClassDB::bind_method(D_METHOD("set_storage", "value"), &HoneycombResource::set_storage);
	ClassDB::add_property("HoneycombResource", PropertyInfo(Variant::Type::OBJECT, "storage"), "set_storage", "get_storage");
	ClassDB::bind_method(D_METHOD("get_kind"), &HoneycombResource::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &HoneycombResource::set_kind);
	ClassDB::add_property("HoneycombResource", PropertyInfo(Variant::Type::OBJECT, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("get_tags"), &HoneycombResource::get_tags);
	ClassDB::bind_method(D_METHOD("set_tags", "value"), &HoneycombResource::set_tags);
	ClassDB::add_property("HoneycombResource", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "tags"), "set_tags", "get_tags");
	ClassDB::bind_method(D_METHOD("to_dict"), &HoneycombResource::to_dict);
}
Dictionary HoneycombResource::to_dict() {
	Dictionary res;
	res["address"] = address;
	res["bump"] = bump;
	if (project.has_method("to_string")) {
		res["project"] = project.call("to_string");
	} else {
		res["project"] = project;
	}
	if (mint.has_method("to_string")) {
		res["mint"] = mint.call("to_string");
	} else {
		res["mint"] = mint;
	}

	if (storage.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::ResourceStorage>(storage);
		if (ptr) {
			res["storage"] = ptr->to_dict();
		}
	}

	if (kind.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::ResourceKind>(kind);
		if (ptr) {
			res["kind"] = ptr->to_dict();
		}
	}
	res["tags"] = tags;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot