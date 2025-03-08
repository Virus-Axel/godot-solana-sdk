#include "honeycomb/types/ServiceDelegationInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void ServiceDelegationInput::set_HiveControl(const Array& val) {
this->HiveControl = val;
}

Array ServiceDelegationInput::get_HiveControl() const {
return this->HiveControl;
}

void ServiceDelegationInput::set_CharacterManager(const Array& val) {
this->CharacterManager = val;
}

Array ServiceDelegationInput::get_CharacterManager() const {
return this->CharacterManager;
}

void ServiceDelegationInput::set_ResourceManager(const Array& val) {
this->ResourceManager = val;
}

Array ServiceDelegationInput::get_ResourceManager() const {
return this->ResourceManager;
}

void ServiceDelegationInput::set_NectarStaking(const Array& val) {
this->NectarStaking = val;
}

Array ServiceDelegationInput::get_NectarStaking() const {
return this->NectarStaking;
}

void ServiceDelegationInput::set_NectarMissions(const Array& val) {
this->NectarMissions = val;
}

Array ServiceDelegationInput::get_NectarMissions() const {
return this->NectarMissions;
}

void ServiceDelegationInput::set_BuzzGuild(const Array& val) {
this->BuzzGuild = val;
}

Array ServiceDelegationInput::get_BuzzGuild() const {
return this->BuzzGuild;
}

void ServiceDelegationInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_HiveControl"), &ServiceDelegationInput::get_HiveControl);
ClassDB::bind_method(D_METHOD("set_HiveControl", "value"), &ServiceDelegationInput::set_HiveControl);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "HiveControl"), "set_HiveControl", "get_HiveControl");
ClassDB::bind_method(D_METHOD("get_CharacterManager"), &ServiceDelegationInput::get_CharacterManager);
ClassDB::bind_method(D_METHOD("set_CharacterManager", "value"), &ServiceDelegationInput::set_CharacterManager);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "CharacterManager"), "set_CharacterManager", "get_CharacterManager");
ClassDB::bind_method(D_METHOD("get_ResourceManager"), &ServiceDelegationInput::get_ResourceManager);
ClassDB::bind_method(D_METHOD("set_ResourceManager", "value"), &ServiceDelegationInput::set_ResourceManager);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "ResourceManager"), "set_ResourceManager", "get_ResourceManager");
ClassDB::bind_method(D_METHOD("get_NectarStaking"), &ServiceDelegationInput::get_NectarStaking);
ClassDB::bind_method(D_METHOD("set_NectarStaking", "value"), &ServiceDelegationInput::set_NectarStaking);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "NectarStaking"), "set_NectarStaking", "get_NectarStaking");
ClassDB::bind_method(D_METHOD("get_NectarMissions"), &ServiceDelegationInput::get_NectarMissions);
ClassDB::bind_method(D_METHOD("set_NectarMissions", "value"), &ServiceDelegationInput::set_NectarMissions);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "NectarMissions"), "set_NectarMissions", "get_NectarMissions");
ClassDB::bind_method(D_METHOD("get_BuzzGuild"), &ServiceDelegationInput::get_BuzzGuild);
ClassDB::bind_method(D_METHOD("set_BuzzGuild", "value"), &ServiceDelegationInput::set_BuzzGuild);
ClassDB::add_property("ServiceDelegationInput", PropertyInfo(Variant::Type::ARRAY, "BuzzGuild"), "set_BuzzGuild", "get_BuzzGuild");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceDelegationInput::to_dict);
}
Dictionary ServiceDelegationInput::to_dict() {
Dictionary res;
res["HiveControl"] = HiveControl;
res["CharacterManager"] = CharacterManager;
res["ResourceManager"] = ResourceManager;
res["NectarStaking"] = NectarStaking;
res["NectarMissions"] = NectarMissions;
res["BuzzGuild"] = BuzzGuild;
return res;
}
} // namespace honeycomb_resource
} // namespace godot