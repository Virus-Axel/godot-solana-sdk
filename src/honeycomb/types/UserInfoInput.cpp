#include "honeycomb/types/UserInfoInput.hpp"

namespace godot{
namespace honeycomb_resource{

void UserInfoInput::set_name(const String& val){
this->name = val;
}

String UserInfoInput::get_name(){
return this->name;
}

void UserInfoInput::set_bio(const String& val){
this->bio = val;
}

String UserInfoInput::get_bio(){
return this->bio;
}

void UserInfoInput::set_pfp(const String& val){
this->pfp = val;
}

String UserInfoInput::get_pfp(){
return this->pfp;
}

Dictionary UserInfoInput::to_dict(){
Dictionary res;
res["name"] = name;
res["bio"] = bio;
res["pfp"] = pfp;
return res;
}

void UserInfoInput::from_dict(const Dictionary& dict){
name = dict["name"];
bio = dict["bio"];
pfp = dict["pfp"];
}

void UserInfoInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_name"), &UserInfoInput::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &UserInfoInput::set_name);
ClassDB::add_property("UserInfoInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_bio"), &UserInfoInput::get_bio);
ClassDB::bind_method(D_METHOD("set_bio", "value"), &UserInfoInput::set_bio);
ClassDB::add_property("UserInfoInput", PropertyInfo(Variant::Type::STRING, "bio"), "set_bio", "get_bio");
ClassDB::bind_method(D_METHOD("get_pfp"), &UserInfoInput::get_pfp);
ClassDB::bind_method(D_METHOD("set_pfp", "value"), &UserInfoInput::set_pfp);
ClassDB::add_property("UserInfoInput", PropertyInfo(Variant::Type::STRING, "pfp"), "set_pfp", "get_pfp");
}
} // honeycomb_resource
} // godot