#include "honeycomb/types/MealInput.hpp"

namespace godot{
namespace honeycomb_resource{

void MealInput::set_resourceAddress(const String& val){
this->resourceAddress = val;
}

String MealInput::get_resourceAddress(){
return this->resourceAddress;
}

void MealInput::set_amount(const int64_t& val){
this->amount = val;
}

int64_t MealInput::get_amount(){
return this->amount;
}

Dictionary MealInput::to_dict(){
Dictionary res;
res["resourceAddress"] = resourceAddress;
res["amount"] = amount;
return res;
}

void MealInput::from_dict(const Dictionary& dict){
resourceAddress = dict["resourceAddress"];
amount = dict["amount"];
}

void MealInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_resourceAddress"), &MealInput::get_resourceAddress);
ClassDB::bind_method(D_METHOD("set_resourceAddress", "value"), &MealInput::set_resourceAddress);
ClassDB::add_property("MealInput", PropertyInfo(Variant::Type::STRING, "resourceAddress"), "set_resourceAddress", "get_resourceAddress");
ClassDB::bind_method(D_METHOD("get_amount"), &MealInput::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &MealInput::set_amount);
ClassDB::add_property("MealInput", PropertyInfo(Variant::Type::INT, "amount"), "set_amount", "get_amount");
}
} // honeycomb_resource
} // godot