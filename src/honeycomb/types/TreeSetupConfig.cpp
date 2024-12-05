#include "honeycomb/types/TreeSetupConfig.hpp"

#include "honeycomb/types/BasicTreeConfig.hpp"
#include "honeycomb/types/AdvancedTreeConfig.hpp"
namespace godot{
namespace honeycomb_resource{

void TreeSetupConfig::set_basic(const Variant& val){
this->basic = val;
}

Variant TreeSetupConfig::get_basic(){
return this->basic;
}

void TreeSetupConfig::set_advanced(const Variant& val){
this->advanced = val;
}

Variant TreeSetupConfig::get_advanced(){
return this->advanced;
}

Dictionary TreeSetupConfig::to_dict(){
Dictionary res;
res["basic"] = Object::cast_to<godot::honeycomb_resource::BasicTreeConfig>(basic)->to_dict();
res["advanced"] = Object::cast_to<godot::honeycomb_resource::AdvancedTreeConfig>(advanced)->to_dict();
return res;
}

void TreeSetupConfig::from_dict(const Dictionary& dict){
Object::cast_to<godot::honeycomb_resource::BasicTreeConfig>(basic)->from_dict(dict["basic"]);
Object::cast_to<godot::honeycomb_resource::AdvancedTreeConfig>(advanced)->from_dict(dict["advanced"]);
}

void TreeSetupConfig::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_basic"), &TreeSetupConfig::get_basic);
ClassDB::bind_method(D_METHOD("set_basic", "value"), &TreeSetupConfig::set_basic);
ClassDB::add_property("TreeSetupConfig", PropertyInfo(Variant::Type::OBJECT, "basic"), "set_basic", "get_basic");
ClassDB::bind_method(D_METHOD("get_advanced"), &TreeSetupConfig::get_advanced);
ClassDB::bind_method(D_METHOD("set_advanced", "value"), &TreeSetupConfig::set_advanced);
ClassDB::add_property("TreeSetupConfig", PropertyInfo(Variant::Type::OBJECT, "advanced"), "set_advanced", "get_advanced");
ClassDB::bind_method(D_METHOD("to_dict"), &TreeSetupConfig::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &TreeSetupConfig::from_dict);
}
} // honeycomb_resource
} // godot