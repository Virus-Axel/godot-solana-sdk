#include "honeycomb/types/BasicTreeConfig.hpp"

namespace godot{
namespace honeycomb_resource{

void BasicTreeConfig::set_numAssets(const int32_t& val){
this->numAssets = val;
}

int32_t BasicTreeConfig::get_numAssets(){
return this->numAssets;
}

Dictionary BasicTreeConfig::to_dict(){
Dictionary res;
res["numAssets"] = numAssets;
return res;
}

void BasicTreeConfig::from_dict(const Dictionary& dict){
numAssets = dict["numAssets"];
}

void BasicTreeConfig::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_numAssets"), &BasicTreeConfig::get_numAssets);
ClassDB::bind_method(D_METHOD("set_numAssets", "value"), &BasicTreeConfig::set_numAssets);
ClassDB::add_property("BasicTreeConfig", PropertyInfo(Variant::Type::INT, "numAssets"), "set_numAssets", "get_numAssets");
}
} // honeycomb_resource
} // godot