#include "honeycomb/types/ProfileInfo_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void ProfileInfo::set_name(const String& val) {
this->name = val;
}

String ProfileInfo::get_name() const {
return this->name;
}

void ProfileInfo::set_bio(const String& val) {
this->bio = val;
}

String ProfileInfo::get_bio() const {
return this->bio;
}

void ProfileInfo::set_pfp(const String& val) {
this->pfp = val;
}

String ProfileInfo::get_pfp() const {
return this->pfp;
}

void ProfileInfo::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_name"), &ProfileInfo::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &ProfileInfo::set_name);
ClassDB::add_property("ProfileInfo", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_bio"), &ProfileInfo::get_bio);
ClassDB::bind_method(D_METHOD("set_bio", "value"), &ProfileInfo::set_bio);
ClassDB::add_property("ProfileInfo", PropertyInfo(Variant::Type::STRING, "bio"), "set_bio", "get_bio");
ClassDB::bind_method(D_METHOD("get_pfp"), &ProfileInfo::get_pfp);
ClassDB::bind_method(D_METHOD("set_pfp", "value"), &ProfileInfo::set_pfp);
ClassDB::add_property("ProfileInfo", PropertyInfo(Variant::Type::STRING, "pfp"), "set_pfp", "get_pfp");
ClassDB::bind_method(D_METHOD("to_dict"), &ProfileInfo::to_dict);
}
Dictionary ProfileInfo::to_dict() {
Dictionary res;
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