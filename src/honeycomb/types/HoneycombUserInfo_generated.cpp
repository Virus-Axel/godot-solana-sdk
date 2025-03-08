#include "honeycomb/types/HoneycombUserInfo_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void HoneycombUserInfo::set_username(const String &val) {
	this->username = val;
}

String HoneycombUserInfo::get_username() const {
	return this->username;
}

void HoneycombUserInfo::set_name(const String &val) {
	this->name = val;
}

String HoneycombUserInfo::get_name() const {
	return this->name;
}

void HoneycombUserInfo::set_bio(const String &val) {
	this->bio = val;
}

String HoneycombUserInfo::get_bio() const {
	return this->bio;
}

void HoneycombUserInfo::set_pfp(const String &val) {
	this->pfp = val;
}

String HoneycombUserInfo::get_pfp() const {
	return this->pfp;
}

void HoneycombUserInfo::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_username"), &HoneycombUserInfo::get_username);
	ClassDB::bind_method(D_METHOD("set_username", "value"), &HoneycombUserInfo::set_username);
	ClassDB::add_property("HoneycombUserInfo", PropertyInfo(Variant::Type::STRING, "username"), "set_username", "get_username");
	ClassDB::bind_method(D_METHOD("get_name"), &HoneycombUserInfo::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "value"), &HoneycombUserInfo::set_name);
	ClassDB::add_property("HoneycombUserInfo", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
	ClassDB::bind_method(D_METHOD("get_bio"), &HoneycombUserInfo::get_bio);
	ClassDB::bind_method(D_METHOD("set_bio", "value"), &HoneycombUserInfo::set_bio);
	ClassDB::add_property("HoneycombUserInfo", PropertyInfo(Variant::Type::STRING, "bio"), "set_bio", "get_bio");
	ClassDB::bind_method(D_METHOD("get_pfp"), &HoneycombUserInfo::get_pfp);
	ClassDB::bind_method(D_METHOD("set_pfp", "value"), &HoneycombUserInfo::set_pfp);
	ClassDB::add_property("HoneycombUserInfo", PropertyInfo(Variant::Type::STRING, "pfp"), "set_pfp", "get_pfp");
	ClassDB::bind_method(D_METHOD("to_dict"), &HoneycombUserInfo::to_dict);
}
Dictionary HoneycombUserInfo::to_dict() {
	Dictionary res;
	if (!username.is_empty()) {
		res["username"] = username;
	}
	if (!name.is_empty()) {
		res["name"] = name;
	}
	if (!bio.is_empty()) {
		res["bio"] = bio;
	}
	if (!pfp.is_empty()) {
		res["pfp"] = pfp;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot