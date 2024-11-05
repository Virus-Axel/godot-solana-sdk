#include "honeycomb/types/PartialUserInfoInput.hpp"

namespace godot{
namespace honeycomb_resource{

void PartialUserInfoInput::set_username(const String& val){
this->username = val;
}

String PartialUserInfoInput::get_username(){
return this->username;
}

void PartialUserInfoInput::set_name(const String& val){
this->name = val;
}

String PartialUserInfoInput::get_name(){
return this->name;
}

void PartialUserInfoInput::set_bio(const String& val){
this->bio = val;
}

String PartialUserInfoInput::get_bio(){
return this->bio;
}

void PartialUserInfoInput::set_pfp(const String& val){
this->pfp = val;
}

String PartialUserInfoInput::get_pfp(){
return this->pfp;
}

Dictionary PartialUserInfoInput::to_dict(){
Dictionary res;
res["username"] = username;
res["name"] = name;
res["bio"] = bio;
res["pfp"] = pfp;
return res;
}

void PartialUserInfoInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_username"), &PartialUserInfoInput::get_username);
ClassDB::bind_method(D_METHOD("set_username", "value"), &PartialUserInfoInput::set_username);
ClassDB::add_property("PartialUserInfoInput", PropertyInfo(Variant::Type::STRING, "username"), "set_username", "get_username");
ClassDB::bind_method(D_METHOD("get_name"), &PartialUserInfoInput::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &PartialUserInfoInput::set_name);
ClassDB::add_property("PartialUserInfoInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_bio"), &PartialUserInfoInput::get_bio);
ClassDB::bind_method(D_METHOD("set_bio", "value"), &PartialUserInfoInput::set_bio);
ClassDB::add_property("PartialUserInfoInput", PropertyInfo(Variant::Type::STRING, "bio"), "set_bio", "get_bio");
ClassDB::bind_method(D_METHOD("get_pfp"), &PartialUserInfoInput::get_pfp);
ClassDB::bind_method(D_METHOD("set_pfp", "value"), &PartialUserInfoInput::set_pfp);
ClassDB::add_property("PartialUserInfoInput", PropertyInfo(Variant::Type::STRING, "pfp"), "set_pfp", "get_pfp");
}
} // honeycomb_resource
} // godot