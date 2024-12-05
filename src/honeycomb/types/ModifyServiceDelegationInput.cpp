#include "honeycomb/types/ModifyServiceDelegationInput.hpp"

#include "honeycomb/types/ServiceDelegationHiveControl.hpp"
#include "honeycomb/types/ServiceDelegationAssetAssembler.hpp"
#include "honeycomb/types/ServiceDelegationAssetManager.hpp"
#include "honeycomb/types/ServiceDelegationCurrencyManager.hpp"
#include "honeycomb/types/ServiceDelegationNectarStaking.hpp"
#include "honeycomb/types/ServiceDelegationNectarMissions.hpp"
#include "honeycomb/types/ServiceDelegationBuzzGuild.hpp"
namespace godot{
namespace honeycomb_resource{

void ModifyServiceDelegationInput::set_HiveControl(const Variant& val){
this->HiveControl = val;
}

Variant ModifyServiceDelegationInput::get_HiveControl(){
return this->HiveControl;
}

void ModifyServiceDelegationInput::set_AssetAssembler(const Variant& val){
this->AssetAssembler = val;
}

Variant ModifyServiceDelegationInput::get_AssetAssembler(){
return this->AssetAssembler;
}

void ModifyServiceDelegationInput::set_AssetManager(const Variant& val){
this->AssetManager = val;
}

Variant ModifyServiceDelegationInput::get_AssetManager(){
return this->AssetManager;
}

void ModifyServiceDelegationInput::set_CurrencyManager(const Variant& val){
this->CurrencyManager = val;
}

Variant ModifyServiceDelegationInput::get_CurrencyManager(){
return this->CurrencyManager;
}

void ModifyServiceDelegationInput::set_NectarStaking(const Variant& val){
this->NectarStaking = val;
}

Variant ModifyServiceDelegationInput::get_NectarStaking(){
return this->NectarStaking;
}

void ModifyServiceDelegationInput::set_NectarMissions(const Variant& val){
this->NectarMissions = val;
}

Variant ModifyServiceDelegationInput::get_NectarMissions(){
return this->NectarMissions;
}

void ModifyServiceDelegationInput::set_BuzzGuild(const Variant& val){
this->BuzzGuild = val;
}

Variant ModifyServiceDelegationInput::get_BuzzGuild(){
return this->BuzzGuild;
}

Dictionary ModifyServiceDelegationInput::to_dict(){
Dictionary res;
res["HiveControl"] = Object::cast_to<godot::honeycomb_resource::ServiceDelegationHiveControl>(HiveControl)->to_dict();
res["AssetAssembler"] = Object::cast_to<godot::honeycomb_resource::ServiceDelegationAssetAssembler>(AssetAssembler)->to_dict();
res["AssetManager"] = Object::cast_to<godot::honeycomb_resource::ServiceDelegationAssetManager>(AssetManager)->to_dict();
res["CurrencyManager"] = Object::cast_to<godot::honeycomb_resource::ServiceDelegationCurrencyManager>(CurrencyManager)->to_dict();
res["NectarStaking"] = Object::cast_to<godot::honeycomb_resource::ServiceDelegationNectarStaking>(NectarStaking)->to_dict();
res["NectarMissions"] = Object::cast_to<godot::honeycomb_resource::ServiceDelegationNectarMissions>(NectarMissions)->to_dict();
res["BuzzGuild"] = Object::cast_to<godot::honeycomb_resource::ServiceDelegationBuzzGuild>(BuzzGuild)->to_dict();
return res;
}

void ModifyServiceDelegationInput::from_dict(const Dictionary& dict){
Object::cast_to<godot::honeycomb_resource::ServiceDelegationHiveControl>(HiveControl)->from_dict(dict["HiveControl"]);
Object::cast_to<godot::honeycomb_resource::ServiceDelegationAssetAssembler>(AssetAssembler)->from_dict(dict["AssetAssembler"]);
Object::cast_to<godot::honeycomb_resource::ServiceDelegationAssetManager>(AssetManager)->from_dict(dict["AssetManager"]);
Object::cast_to<godot::honeycomb_resource::ServiceDelegationCurrencyManager>(CurrencyManager)->from_dict(dict["CurrencyManager"]);
Object::cast_to<godot::honeycomb_resource::ServiceDelegationNectarStaking>(NectarStaking)->from_dict(dict["NectarStaking"]);
Object::cast_to<godot::honeycomb_resource::ServiceDelegationNectarMissions>(NectarMissions)->from_dict(dict["NectarMissions"]);
Object::cast_to<godot::honeycomb_resource::ServiceDelegationBuzzGuild>(BuzzGuild)->from_dict(dict["BuzzGuild"]);
}

void ModifyServiceDelegationInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_HiveControl"), &ModifyServiceDelegationInput::get_HiveControl);
ClassDB::bind_method(D_METHOD("set_HiveControl", "value"), &ModifyServiceDelegationInput::set_HiveControl);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "HiveControl"), "set_HiveControl", "get_HiveControl");
ClassDB::bind_method(D_METHOD("get_AssetAssembler"), &ModifyServiceDelegationInput::get_AssetAssembler);
ClassDB::bind_method(D_METHOD("set_AssetAssembler", "value"), &ModifyServiceDelegationInput::set_AssetAssembler);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "AssetAssembler"), "set_AssetAssembler", "get_AssetAssembler");
ClassDB::bind_method(D_METHOD("get_AssetManager"), &ModifyServiceDelegationInput::get_AssetManager);
ClassDB::bind_method(D_METHOD("set_AssetManager", "value"), &ModifyServiceDelegationInput::set_AssetManager);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "AssetManager"), "set_AssetManager", "get_AssetManager");
ClassDB::bind_method(D_METHOD("get_CurrencyManager"), &ModifyServiceDelegationInput::get_CurrencyManager);
ClassDB::bind_method(D_METHOD("set_CurrencyManager", "value"), &ModifyServiceDelegationInput::set_CurrencyManager);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "CurrencyManager"), "set_CurrencyManager", "get_CurrencyManager");
ClassDB::bind_method(D_METHOD("get_NectarStaking"), &ModifyServiceDelegationInput::get_NectarStaking);
ClassDB::bind_method(D_METHOD("set_NectarStaking", "value"), &ModifyServiceDelegationInput::set_NectarStaking);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "NectarStaking"), "set_NectarStaking", "get_NectarStaking");
ClassDB::bind_method(D_METHOD("get_NectarMissions"), &ModifyServiceDelegationInput::get_NectarMissions);
ClassDB::bind_method(D_METHOD("set_NectarMissions", "value"), &ModifyServiceDelegationInput::set_NectarMissions);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "NectarMissions"), "set_NectarMissions", "get_NectarMissions");
ClassDB::bind_method(D_METHOD("get_BuzzGuild"), &ModifyServiceDelegationInput::get_BuzzGuild);
ClassDB::bind_method(D_METHOD("set_BuzzGuild", "value"), &ModifyServiceDelegationInput::set_BuzzGuild);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "BuzzGuild"), "set_BuzzGuild", "get_BuzzGuild");
ClassDB::bind_method(D_METHOD("to_dict"), &ModifyServiceDelegationInput::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &ModifyServiceDelegationInput::from_dict);
}
} // honeycomb_resource
} // godot