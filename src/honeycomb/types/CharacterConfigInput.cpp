#include "honeycomb/types/CharacterConfigInput.hpp"

#include "honeycomb/types/AssemblerConfigInput.hpp"
namespace godot{
namespace honeycomb_resource{

void CharacterConfigInput::set_kind(const String& val){
this->kind = val;
}

String CharacterConfigInput::get_kind(){
return this->kind;
}

void CharacterConfigInput::set_criterias(const Array& val){
this->criterias = val;
}

Array CharacterConfigInput::get_criterias(){
return this->criterias;
}

void CharacterConfigInput::set_assemblerConfigInput(const Variant& val){
this->assemblerConfigInput = val;
}

Variant CharacterConfigInput::get_assemblerConfigInput(){
return this->assemblerConfigInput;
}

Dictionary CharacterConfigInput::to_dict(){
Dictionary res;
res["kind"] = kind;
res["criterias"] = criterias;
res["assemblerConfigInput"] = Object::cast_to<godot::honeycomb_resource::AssemblerConfigInput>(assemblerConfigInput)->to_dict();
return res;
}

void CharacterConfigInput::from_dict(const Dictionary& dict){
kind = dict["kind"];
criterias = dict["criterias"];
Object::cast_to<godot::honeycomb_resource::AssemblerConfigInput>(assemblerConfigInput)->from_dict(dict["assemblerConfigInput"]);
}

void CharacterConfigInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_kind"), &CharacterConfigInput::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &CharacterConfigInput::set_kind);
ClassDB::add_property("CharacterConfigInput", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_criterias"), &CharacterConfigInput::get_criterias);
ClassDB::bind_method(D_METHOD("set_criterias", "value"), &CharacterConfigInput::set_criterias);
ClassDB::add_property("CharacterConfigInput", PropertyInfo(Variant::Type::ARRAY, "criterias"), "set_criterias", "get_criterias");
ClassDB::bind_method(D_METHOD("get_assemblerConfigInput"), &CharacterConfigInput::get_assemblerConfigInput);
ClassDB::bind_method(D_METHOD("set_assemblerConfigInput", "value"), &CharacterConfigInput::set_assemblerConfigInput);
ClassDB::add_property("CharacterConfigInput", PropertyInfo(Variant::Type::OBJECT, "assemblerConfigInput"), "set_assemblerConfigInput", "get_assemblerConfigInput");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterConfigInput::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &CharacterConfigInput::from_dict);
}
} // honeycomb_resource
} // godot