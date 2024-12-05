#include "honeycomb/types/InitResourceInput.hpp"

namespace godot{
namespace honeycomb_resource{

void InitResourceInput::set_name(const String& val){
this->name = val;
}

String InitResourceInput::get_name(){
return this->name;
}

void InitResourceInput::set_symbol(const String& val){
this->symbol = val;
}

String InitResourceInput::get_symbol(){
return this->symbol;
}

void InitResourceInput::set_uri(const String& val){
this->uri = val;
}

String InitResourceInput::get_uri(){
return this->uri;
}

void InitResourceInput::set_decimals(const int32_t& val){
this->decimals = val;
}

int32_t InitResourceInput::get_decimals(){
return this->decimals;
}

void InitResourceInput::set_storage(const int32_t& val){
this->storage = val;
}

int32_t InitResourceInput::get_storage(){
return this->storage;
}

Dictionary InitResourceInput::to_dict(){
Dictionary res;
res["name"] = name;
res["symbol"] = symbol;
res["uri"] = uri;
res["decimals"] = decimals;
res["storage"] = storage;
return res;
}

void InitResourceInput::from_dict(const Dictionary& dict){
name = dict["name"];
symbol = dict["symbol"];
uri = dict["uri"];
decimals = dict["decimals"];
storage = dict["storage"];
}

void InitResourceInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_name"), &InitResourceInput::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &InitResourceInput::set_name);
ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_symbol"), &InitResourceInput::get_symbol);
ClassDB::bind_method(D_METHOD("set_symbol", "value"), &InitResourceInput::set_symbol);
ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::STRING, "symbol"), "set_symbol", "get_symbol");
ClassDB::bind_method(D_METHOD("get_uri"), &InitResourceInput::get_uri);
ClassDB::bind_method(D_METHOD("set_uri", "value"), &InitResourceInput::set_uri);
ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::STRING, "uri"), "set_uri", "get_uri");
ClassDB::bind_method(D_METHOD("get_decimals"), &InitResourceInput::get_decimals);
ClassDB::bind_method(D_METHOD("set_decimals", "value"), &InitResourceInput::set_decimals);
ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::INT, "decimals"), "set_decimals", "get_decimals");
ClassDB::bind_method(D_METHOD("get_storage"), &InitResourceInput::get_storage);
ClassDB::bind_method(D_METHOD("set_storage", "value"), &InitResourceInput::set_storage);
ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::INT, "storage"), "set_storage", "get_storage");
ClassDB::bind_method(D_METHOD("to_dict"), &InitResourceInput::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &InitResourceInput::from_dict);
}
} // honeycomb_resource
} // godot