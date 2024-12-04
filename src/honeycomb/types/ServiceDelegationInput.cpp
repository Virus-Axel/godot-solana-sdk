#include "honeycomb/types/ServiceDelegationInput.hpp"

namespace godot{
namespace honeycomb_resource{

void ServiceDelegationInput::set_HiveControl(const Array& val){
this->HiveControl = val;
}

Array ServiceDelegationInput::get_HiveControl(){
return this->HiveControl;
}

void ServiceDelegationInput::set_AssetAssembler(const Array& val){
this->AssetAssembler = val;
}

Array ServiceDelegationInput::get_AssetAssembler(){
return this->AssetAssembler;
}

void ServiceDelegationInput::set_AssetManager(const Array& val){
this->AssetManager = val;
}

Array ServiceDelegationInput::get_AssetManager(){
return this->AssetManager;
}

void ServiceDelegationInput::set_CurrencyManager(const Array& val){
this->CurrencyManager = val;
}

Array ServiceDelegationInput::get_CurrencyManager(){
return this->CurrencyManager;
}

void ServiceDelegationInput::set_NectarStaking(const Array& val){
this->NectarStaking = val;
}

Array ServiceDelegationInput::get_NectarStaking(){
return this->NectarStaking;
}

void ServiceDelegationInput::set_NectarMissions(const Array& val){
this->NectarMissions = val;
}

Array ServiceDelegationInput::get_NectarMissions(){
return this->NectarMissions;
}

void ServiceDelegationInput::set_BuzzGuild(const Array& val){
this->BuzzGuild = val;
}

Array ServiceDelegationInput::get_BuzzGuild(){
return this->BuzzGuild;
}

Dictionary ServiceDelegationInput::to_dict(){
Dictionary res;
res["HiveControl"] = HiveControl;
res["AssetAssembler"] = AssetAssembler;
res["AssetManager"] = AssetManager;
res["CurrencyManager"] = CurrencyManager;
res["NectarStaking"] = NectarStaking;
res["NectarMissions"] = NectarMissions;
res["BuzzGuild"] = BuzzGuild;
return res;
}

void ServiceDelegationInput::from_dict(const Dictionary& dict){
HiveControl = dict["HiveControl"];
AssetAssembler = dict["AssetAssembler"];
AssetManager = dict["AssetManager"];
CurrencyManager = dict["CurrencyManager"];
NectarStaking = dict["NectarStaking"];
NectarMissions = dict["NectarMissions"];
BuzzGuild = dict["BuzzGuild"];
}

void ServiceDelegationInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_HiveControl"), &ServiceDelegationInput::get_HiveControl);
ClassDB::bind_method(D_METHOD("set_HiveControl", "value"), &ServiceDelegationInput::set_HiveControl);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "HiveControl"), "set_HiveControl", "get_HiveControl");
ClassDB::bind_method(D_METHOD("get_AssetAssembler"), &ServiceDelegationInput::get_AssetAssembler);
ClassDB::bind_method(D_METHOD("set_AssetAssembler", "value"), &ServiceDelegationInput::set_AssetAssembler);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "AssetAssembler"), "set_AssetAssembler", "get_AssetAssembler");
ClassDB::bind_method(D_METHOD("get_AssetManager"), &ServiceDelegationInput::get_AssetManager);
ClassDB::bind_method(D_METHOD("set_AssetManager", "value"), &ServiceDelegationInput::set_AssetManager);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "AssetManager"), "set_AssetManager", "get_AssetManager");
ClassDB::bind_method(D_METHOD("get_CurrencyManager"), &ServiceDelegationInput::get_CurrencyManager);
ClassDB::bind_method(D_METHOD("set_CurrencyManager", "value"), &ServiceDelegationInput::set_CurrencyManager);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "CurrencyManager"), "set_CurrencyManager", "get_CurrencyManager");
ClassDB::bind_method(D_METHOD("get_NectarStaking"), &ServiceDelegationInput::get_NectarStaking);
ClassDB::bind_method(D_METHOD("set_NectarStaking", "value"), &ServiceDelegationInput::set_NectarStaking);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "NectarStaking"), "set_NectarStaking", "get_NectarStaking");
ClassDB::bind_method(D_METHOD("get_NectarMissions"), &ServiceDelegationInput::get_NectarMissions);
ClassDB::bind_method(D_METHOD("set_NectarMissions", "value"), &ServiceDelegationInput::set_NectarMissions);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "NectarMissions"), "set_NectarMissions", "get_NectarMissions");
ClassDB::bind_method(D_METHOD("get_BuzzGuild"), &ServiceDelegationInput::get_BuzzGuild);
ClassDB::bind_method(D_METHOD("set_BuzzGuild", "value"), &ServiceDelegationInput::set_BuzzGuild);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "BuzzGuild"), "set_BuzzGuild", "get_BuzzGuild");
}
} // honeycomb_resource
} // godot