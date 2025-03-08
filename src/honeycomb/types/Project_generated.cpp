#include "honeycomb/types/Project_generated.hpp"

#include "honeycomb/types/ControlledMerkleTrees_generated.hpp"
#include "honeycomb/types/ProfileDataConfig_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Project::set_address(const PackedByteArray &val) {
	this->address = val;
}

PackedByteArray Project::get_address() const {
	return this->address;
}

void Project::set_bump(const int32_t &val) {
	this->bump = val;
}

int32_t Project::get_bump() const {
	return this->bump;
}

void Project::set_authority(const Variant &val) {
	this->authority = val;
}

Variant Project::get_authority() const {
	return this->authority;
}

void Project::set_key(const Variant &val) {
	this->key = val;
}

Variant Project::get_key() const {
	return this->key;
}

void Project::set_driver(const Variant &val) {
	this->driver = val;
}

Variant Project::get_driver() const {
	return this->driver;
}

void Project::set_name(const String &val) {
	this->name = val;
}

String Project::get_name() const {
	return this->name;
}

void Project::set_services(const Array &val) {
	this->services = val;
}

Array Project::get_services() const {
	return this->services;
}

void Project::set_associatedPrograms(const Array &val) {
	this->associatedPrograms = val;
}

Array Project::get_associatedPrograms() const {
	return this->associatedPrograms;
}

void Project::set_profileDataConfig(const Variant &val) {
	this->profileDataConfig = val;
}

Variant Project::get_profileDataConfig() const {
	return this->profileDataConfig;
}

void Project::set_profileTrees(const Variant &val) {
	this->profileTrees = val;
}

Variant Project::get_profileTrees() const {
	return this->profileTrees;
}

void Project::set_badgeCriteria(const Array &val) {
	this->badgeCriteria = val;
}

Array Project::get_badgeCriteria() const {
	return this->badgeCriteria;
}

void Project::set_subsidyFees(const bool &val) {
	this->subsidyFees = val;
}

bool Project::get_subsidyFees() const {
	return this->subsidyFees;
}

void Project::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &Project::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &Project::set_address);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("get_bump"), &Project::get_bump);
	ClassDB::bind_method(D_METHOD("set_bump", "value"), &Project::set_bump);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
	ClassDB::bind_method(D_METHOD("get_authority"), &Project::get_authority);
	ClassDB::bind_method(D_METHOD("set_authority", "value"), &Project::set_authority);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::OBJECT, "authority"), "set_authority", "get_authority");
	ClassDB::bind_method(D_METHOD("get_key"), &Project::get_key);
	ClassDB::bind_method(D_METHOD("set_key", "value"), &Project::set_key);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::OBJECT, "key"), "set_key", "get_key");
	ClassDB::bind_method(D_METHOD("get_driver"), &Project::get_driver);
	ClassDB::bind_method(D_METHOD("set_driver", "value"), &Project::set_driver);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::OBJECT, "driver"), "set_driver", "get_driver");
	ClassDB::bind_method(D_METHOD("get_name"), &Project::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "value"), &Project::set_name);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
	ClassDB::bind_method(D_METHOD("get_services"), &Project::get_services);
	ClassDB::bind_method(D_METHOD("set_services", "value"), &Project::set_services);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::ARRAY, "services"), "set_services", "get_services");
	ClassDB::bind_method(D_METHOD("get_associatedPrograms"), &Project::get_associatedPrograms);
	ClassDB::bind_method(D_METHOD("set_associatedPrograms", "value"), &Project::set_associatedPrograms);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::ARRAY, "associatedPrograms"), "set_associatedPrograms", "get_associatedPrograms");
	ClassDB::bind_method(D_METHOD("get_profileDataConfig"), &Project::get_profileDataConfig);
	ClassDB::bind_method(D_METHOD("set_profileDataConfig", "value"), &Project::set_profileDataConfig);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::OBJECT, "profileDataConfig"), "set_profileDataConfig", "get_profileDataConfig");
	ClassDB::bind_method(D_METHOD("get_profileTrees"), &Project::get_profileTrees);
	ClassDB::bind_method(D_METHOD("set_profileTrees", "value"), &Project::set_profileTrees);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::OBJECT, "profileTrees"), "set_profileTrees", "get_profileTrees");
	ClassDB::bind_method(D_METHOD("get_badgeCriteria"), &Project::get_badgeCriteria);
	ClassDB::bind_method(D_METHOD("set_badgeCriteria", "value"), &Project::set_badgeCriteria);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::ARRAY, "badgeCriteria"), "set_badgeCriteria", "get_badgeCriteria");
	ClassDB::bind_method(D_METHOD("get_subsidyFees"), &Project::get_subsidyFees);
	ClassDB::bind_method(D_METHOD("set_subsidyFees", "value"), &Project::set_subsidyFees);
	ClassDB::add_property("Project", PropertyInfo(Variant::Type::BOOL, "subsidyFees"), "set_subsidyFees", "get_subsidyFees");
	ClassDB::bind_method(D_METHOD("to_dict"), &Project::to_dict);
}
Dictionary Project::to_dict() {
	Dictionary res;
	res["address"] = address;
	res["bump"] = bump;
	if (authority.has_method("to_string")) {
		res["authority"] = authority.call("to_string");
	} else {
		res["authority"] = authority;
	}
	if (key.has_method("to_string")) {
		res["key"] = key.call("to_string");
	} else {
		res["key"] = key;
	}
	if (driver.has_method("to_string")) {
		res["driver"] = driver.call("to_string");
	} else {
		res["driver"] = driver;
	}
	if (!name.is_empty()) {
		res["name"] = name;
	}
	res["services"] = services;
	res["associatedPrograms"] = associatedPrograms;

	if (profileDataConfig.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::ProfileDataConfig>(profileDataConfig);
		if (ptr) {
			res["profileDataConfig"] = ptr->to_dict();
		}
	}

	if (profileTrees.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::ControlledMerkleTrees>(profileTrees);
		if (ptr) {
			res["profileTrees"] = ptr->to_dict();
		}
	}
	res["badgeCriteria"] = badgeCriteria;
	res["subsidyFees"] = subsidyFees;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot