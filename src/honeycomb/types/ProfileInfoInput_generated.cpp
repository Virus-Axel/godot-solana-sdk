#include "honeycomb/types/ProfileInfoInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void ProfileInfoInput::set_name(const String& val) {
this->name = val;
}

String ProfileInfoInput::get_name() const {
return this->name;
}

void ProfileInfoInput::set_bio(const String& val) {
this->bio = val;
}

String ProfileInfoInput::get_bio() const {
return this->bio;
}

void ProfileInfoInput::set_pfp(const String& val) {
this->pfp = val;
}

String ProfileInfoInput::get_pfp() const {
return this->pfp;
}

void ProfileInfoInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_name"), &ProfileInfoInput::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &ProfileInfoInput::set_name);
ClassDB::add_property("ProfileInfoInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_bio"), &ProfileInfoInput::get_bio);
ClassDB::bind_method(D_METHOD("set_bio", "value"), &ProfileInfoInput::set_bio);
ClassDB::add_property("ProfileInfoInput", PropertyInfo(Variant::Type::STRING, "bio"), "set_bio", "get_bio");
ClassDB::bind_method(D_METHOD("get_pfp"), &ProfileInfoInput::get_pfp);
ClassDB::bind_method(D_METHOD("set_pfp", "value"), &ProfileInfoInput::set_pfp);
ClassDB::add_property("ProfileInfoInput", PropertyInfo(Variant::Type::STRING, "pfp"), "set_pfp", "get_pfp");
ClassDB::bind_method(D_METHOD("to_dict"), &ProfileInfoInput::to_dict);
}
Dictionary ProfileInfoInput::to_dict() {
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