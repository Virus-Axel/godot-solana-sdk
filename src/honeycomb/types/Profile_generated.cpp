#include "honeycomb/types/Profile_generated.hpp"

#include "honeycomb/types/PlatformData_generated.hpp"
#include "honeycomb/types/ProfileInfo_generated.hpp"
#include "honeycomb/types/Proof_generated.hpp"
#include "honeycomb/types/User_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Profile::set_address(const PackedByteArray &val) {
	this->address = val;
}

PackedByteArray Profile::get_address() const {
	return this->address;
}

void Profile::set_tree_id(const PackedByteArray &val) {
	this->tree_id = val;
}

PackedByteArray Profile::get_tree_id() const {
	return this->tree_id;
}

void Profile::set_leaf_idx(const int64_t &val) {
	this->leaf_idx = val;
}

int64_t Profile::get_leaf_idx() const {
	return this->leaf_idx;
}

void Profile::set_proof(const Variant &val) {
	this->proof = val;
}

Variant Profile::get_proof() const {
	return this->proof;
}

void Profile::set_project(const Variant &val) {
	this->project = val;
}

Variant Profile::get_project() const {
	return this->project;
}

void Profile::set_userId(const int32_t &val) {
	this->userId = val;
}

int32_t Profile::get_userId() const {
	return this->userId;
}

void Profile::set_identity(const String &val) {
	this->identity = val;
}

String Profile::get_identity() const {
	return this->identity;
}

void Profile::set_info(const Variant &val) {
	this->info = val;
}

Variant Profile::get_info() const {
	return this->info;
}

void Profile::set_platformData(const Variant &val) {
	this->platformData = val;
}

Variant Profile::get_platformData() const {
	return this->platformData;
}

void Profile::set_customData(const Array &val) {
	this->customData = val;
}

Array Profile::get_customData() const {
	return this->customData;
}

void Profile::set_user(const Variant &val) {
	this->user = val;
}

Variant Profile::get_user() const {
	return this->user;
}

void Profile::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &Profile::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &Profile::set_address);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("get_tree_id"), &Profile::get_tree_id);
	ClassDB::bind_method(D_METHOD("set_tree_id", "value"), &Profile::set_tree_id);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree_id"), "set_tree_id", "get_tree_id");
	ClassDB::bind_method(D_METHOD("get_leaf_idx"), &Profile::get_leaf_idx);
	ClassDB::bind_method(D_METHOD("set_leaf_idx", "value"), &Profile::set_leaf_idx);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::INT, "leaf_idx"), "set_leaf_idx", "get_leaf_idx");
	ClassDB::bind_method(D_METHOD("get_proof"), &Profile::get_proof);
	ClassDB::bind_method(D_METHOD("set_proof", "value"), &Profile::set_proof);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::OBJECT, "proof"), "set_proof", "get_proof");
	ClassDB::bind_method(D_METHOD("get_project"), &Profile::get_project);
	ClassDB::bind_method(D_METHOD("set_project", "value"), &Profile::set_project);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
	ClassDB::bind_method(D_METHOD("get_userId"), &Profile::get_userId);
	ClassDB::bind_method(D_METHOD("set_userId", "value"), &Profile::set_userId);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::INT, "userId"), "set_userId", "get_userId");
	ClassDB::bind_method(D_METHOD("get_identity"), &Profile::get_identity);
	ClassDB::bind_method(D_METHOD("set_identity", "value"), &Profile::set_identity);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::STRING, "identity"), "set_identity", "get_identity");
	ClassDB::bind_method(D_METHOD("get_info"), &Profile::get_info);
	ClassDB::bind_method(D_METHOD("set_info", "value"), &Profile::set_info);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::OBJECT, "info"), "set_info", "get_info");
	ClassDB::bind_method(D_METHOD("get_platformData"), &Profile::get_platformData);
	ClassDB::bind_method(D_METHOD("set_platformData", "value"), &Profile::set_platformData);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::OBJECT, "platformData"), "set_platformData", "get_platformData");
	ClassDB::bind_method(D_METHOD("get_customData"), &Profile::get_customData);
	ClassDB::bind_method(D_METHOD("set_customData", "value"), &Profile::set_customData);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::ARRAY, "customData"), "set_customData", "get_customData");
	ClassDB::bind_method(D_METHOD("get_user"), &Profile::get_user);
	ClassDB::bind_method(D_METHOD("set_user", "value"), &Profile::set_user);
	ClassDB::add_property("Profile", PropertyInfo(Variant::Type::OBJECT, "user"), "set_user", "get_user");
	ClassDB::bind_method(D_METHOD("to_dict"), &Profile::to_dict);
}
Dictionary Profile::to_dict() {
	Dictionary res;
	res["address"] = address;
	res["tree_id"] = tree_id;
	res["leaf_idx"] = leaf_idx;

	if (proof.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::Proof>(proof);
		if (ptr) {
			res["proof"] = ptr->to_dict();
		}
	}
	if (project.has_method("to_string")) {
		res["project"] = project.call("to_string");
	} else {
		res["project"] = project;
	}
	res["userId"] = userId;
	if (!identity.is_empty()) {
		res["identity"] = identity;
	}

	if (info.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::ProfileInfo>(info);
		if (ptr) {
			res["info"] = ptr->to_dict();
		}
	}

	if (platformData.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::PlatformData>(platformData);
		if (ptr) {
			res["platformData"] = ptr->to_dict();
		}
	}
	res["customData"] = customData;

	if (user.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::User>(user);
		if (ptr) {
			res["user"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot