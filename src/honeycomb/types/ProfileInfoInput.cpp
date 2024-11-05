#include "honeycomb/types/ProfileInfoInput.hpp"

namespace godot{
namespace honeycomb_resource{

void ProfileInfoInput::set_name(const String& val){
this->name = val;
}

String& ProfileInfoInput::get_name(){
return this->name;
}

void ProfileInfoInput::set_bio(const String& val){
this->bio = val;
}

String& ProfileInfoInput::get_bio(){
return this->bio;
}

void ProfileInfoInput::set_pfp(const String& val){
this->pfp = val;
}

String& ProfileInfoInput::get_pfp(){
return this->pfp;
}

Dictionary ProfileInfoInput::to_dict(){
Dictionary res;
res["name"] = name;
res["bio"] = bio;
res["pfp"] = pfp;
return res;
}

void ProfileInfoInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_name"), &ProfileInfoInput::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &ProfileInfoInput::set_name);
ClassDB::add_property("ProfileInfoInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_bio"), &ProfileInfoInput::get_bio);
ClassDB::bind_method(D_METHOD("set_bio", "value"), &ProfileInfoInput::set_bio);
ClassDB::add_property("ProfileInfoInput", PropertyInfo(Variant::Type::STRING, "bio"), "set_bio", "get_bio");
ClassDB::bind_method(D_METHOD("get_pfp"), &ProfileInfoInput::get_pfp);
ClassDB::bind_method(D_METHOD("set_pfp", "value"), &ProfileInfoInput::set_pfp);
ClassDB::add_property("ProfileInfoInput", PropertyInfo(Variant::Type::STRING, "pfp"), "set_pfp", "get_pfp");
}
} // honeycomb_resource
} // godot