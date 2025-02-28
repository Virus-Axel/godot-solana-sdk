#include "honeycomb/types/UserInfoInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void UserInfoInput::set_name(const String& val) {
this->name = val;
}

String UserInfoInput::get_name() const {
return this->name;
}

void UserInfoInput::set_bio(const String& val) {
this->bio = val;
}

String UserInfoInput::get_bio() const {
return this->bio;
}

void UserInfoInput::set_pfp(const String& val) {
this->pfp = val;
}

String UserInfoInput::get_pfp() const {
return this->pfp;
}

void UserInfoInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_name"), &UserInfoInput::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &UserInfoInput::set_name);
ClassDB::add_property("UserInfoInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_bio"), &UserInfoInput::get_bio);
ClassDB::bind_method(D_METHOD("set_bio", "value"), &UserInfoInput::set_bio);
ClassDB::add_property("UserInfoInput", PropertyInfo(Variant::Type::STRING, "bio"), "set_bio", "get_bio");
ClassDB::bind_method(D_METHOD("get_pfp"), &UserInfoInput::get_pfp);
ClassDB::bind_method(D_METHOD("set_pfp", "value"), &UserInfoInput::set_pfp);
ClassDB::add_property("UserInfoInput", PropertyInfo(Variant::Type::STRING, "pfp"), "set_pfp", "get_pfp");
ClassDB::bind_method(D_METHOD("to_dict"), &UserInfoInput::to_dict);
}
Dictionary UserInfoInput::to_dict() {
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