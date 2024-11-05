#include "honeycomb/types/AddMultiplierMetadataInput.hpp"

namespace godot{
namespace honeycomb_resource{

void AddMultiplierMetadataInput::set_value(const int64_t& val){
this->value = val;
}

int64_t& AddMultiplierMetadataInput::get_value(){
return this->value;
}

void AddMultiplierMetadataInput::set_type(const Variant& val){
this->type = val;
}

Variant& AddMultiplierMetadataInput::get_type(){
return this->type;
}

Dictionary AddMultiplierMetadataInput::to_dict(){
Dictionary res;
res["value"] = value;
res["type"] = Object::cast_to<godot::honeycomb_resource::MultiplierTypeInput>(type)->to_dict();
return res;
}

void AddMultiplierMetadataInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_value"), &AddMultiplierMetadataInput::get_value);
ClassDB::bind_method(D_METHOD("set_value", "value"), &AddMultiplierMetadataInput::set_value);
ClassDB::add_property("AddMultiplierMetadataInput", PropertyInfo(Variant::Type::INT, "value"), "set_value", "get_value");
ClassDB::bind_method(D_METHOD("get_type"), &AddMultiplierMetadataInput::get_type);
ClassDB::bind_method(D_METHOD("set_type", "value"), &AddMultiplierMetadataInput::set_type);
ClassDB::add_property("AddMultiplierMetadataInput", PropertyInfo(Variant::Type::VARIANT, "type"), "set_type", "get_type");
}
} // honeycomb_resource
} // godot