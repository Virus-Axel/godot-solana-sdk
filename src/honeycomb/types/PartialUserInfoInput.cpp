#include "honeycomb/types/PartialUserInfoInput.hpp"


namespace godot {
namespace honeycomb_resource {

void PartialUserInfoInput::set_name(const String& val) {
this->name = val;
}

String PartialUserInfoInput::get_name() const {
return this->name;
}

void PartialUserInfoInput::set_bio(const String& val) {
this->bio = val;
}

String PartialUserInfoInput::get_bio() const {
return this->bio;
}

void PartialUserInfoInput::set_pfp(const String& val) {
this->pfp = val;
}

String PartialUserInfoInput::get_pfp() const {
return this->pfp;
}

void PartialUserInfoInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_name"), &PartialUserInfoInput::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &PartialUserInfoInput::set_name);
ClassDB::add_property("PartialUserInfoInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_bio"), &PartialUserInfoInput::get_bio);
ClassDB::bind_method(D_METHOD("set_bio", "value"), &PartialUserInfoInput::set_bio);
ClassDB::add_property("PartialUserInfoInput", PropertyInfo(Variant::Type::STRING, "bio"), "set_bio", "get_bio");
ClassDB::bind_method(D_METHOD("get_pfp"), &PartialUserInfoInput::get_pfp);
ClassDB::bind_method(D_METHOD("set_pfp", "value"), &PartialUserInfoInput::set_pfp);
ClassDB::add_property("PartialUserInfoInput", PropertyInfo(Variant::Type::STRING, "pfp"), "set_pfp", "get_pfp");
ClassDB::bind_method(D_METHOD("to_dict"), &PartialUserInfoInput::to_dict);
}
Dictionary PartialUserInfoInput::to_dict() {
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