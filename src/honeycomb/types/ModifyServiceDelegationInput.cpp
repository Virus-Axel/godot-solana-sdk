#include "honeycomb/types/ModifyServiceDelegationInput.hpp"

#include "honeycomb/types/ServiceDelegationHiveControl.hpp"
#include "honeycomb/types/ServiceDelegationAssetAssembler.hpp"
#include "honeycomb/types/ServiceDelegationAssetManager.hpp"
#include "honeycomb/types/ServiceDelegationResourceManager.hpp"
#include "honeycomb/types/ServiceDelegationNectarStaking.hpp"
#include "honeycomb/types/ServiceDelegationNectarMissions.hpp"
#include "honeycomb/types/ServiceDelegationBuzzGuild.hpp"

namespace godot {
namespace honeycomb_resource {

void ModifyServiceDelegationInput::set_HiveControl(const Variant& val) {
this->HiveControl = val;
}

Variant ModifyServiceDelegationInput::get_HiveControl() const {
return this->HiveControl;
}

void ModifyServiceDelegationInput::set_AssetAssembler(const Variant& val) {
this->AssetAssembler = val;
}

Variant ModifyServiceDelegationInput::get_AssetAssembler() const {
return this->AssetAssembler;
}

void ModifyServiceDelegationInput::set_AssetManager(const Variant& val) {
this->AssetManager = val;
}

Variant ModifyServiceDelegationInput::get_AssetManager() const {
return this->AssetManager;
}

void ModifyServiceDelegationInput::set_ResourceManager(const Variant& val) {
this->ResourceManager = val;
}

Variant ModifyServiceDelegationInput::get_ResourceManager() const {
return this->ResourceManager;
}

void ModifyServiceDelegationInput::set_NectarStaking(const Variant& val) {
this->NectarStaking = val;
}

Variant ModifyServiceDelegationInput::get_NectarStaking() const {
return this->NectarStaking;
}

void ModifyServiceDelegationInput::set_NectarMissions(const Variant& val) {
this->NectarMissions = val;
}

Variant ModifyServiceDelegationInput::get_NectarMissions() const {
return this->NectarMissions;
}

void ModifyServiceDelegationInput::set_BuzzGuild(const Variant& val) {
this->BuzzGuild = val;
}

Variant ModifyServiceDelegationInput::get_BuzzGuild() const {
return this->BuzzGuild;
}

void ModifyServiceDelegationInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_HiveControl"), &ModifyServiceDelegationInput::get_HiveControl);
ClassDB::bind_method(D_METHOD("set_HiveControl", "value"), &ModifyServiceDelegationInput::set_HiveControl);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "HiveControl"), "set_HiveControl", "get_HiveControl");
ClassDB::bind_method(D_METHOD("get_AssetAssembler"), &ModifyServiceDelegationInput::get_AssetAssembler);
ClassDB::bind_method(D_METHOD("set_AssetAssembler", "value"), &ModifyServiceDelegationInput::set_AssetAssembler);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "AssetAssembler"), "set_AssetAssembler", "get_AssetAssembler");
ClassDB::bind_method(D_METHOD("get_AssetManager"), &ModifyServiceDelegationInput::get_AssetManager);
ClassDB::bind_method(D_METHOD("set_AssetManager", "value"), &ModifyServiceDelegationInput::set_AssetManager);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "AssetManager"), "set_AssetManager", "get_AssetManager");
ClassDB::bind_method(D_METHOD("get_ResourceManager"), &ModifyServiceDelegationInput::get_ResourceManager);
ClassDB::bind_method(D_METHOD("set_ResourceManager", "value"), &ModifyServiceDelegationInput::set_ResourceManager);
ClassDB::add_property("ModifyServiceDelegationInput", PropertyInfo(Variant::Type::OBJECT, "ResourceManager"), "set_ResourceManager", "get_ResourceManager");
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
}
Dictionary ModifyServiceDelegationInput::to_dict() {
Dictionary res;

          if (HiveControl.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ServiceDelegationHiveControl>(HiveControl);
              if (ptr) {
                  res["HiveControl"] = ptr->to_dict();
              }
          }

          if (AssetAssembler.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ServiceDelegationAssetAssembler>(AssetAssembler);
              if (ptr) {
                  res["AssetAssembler"] = ptr->to_dict();
              }
          }

          if (AssetManager.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ServiceDelegationAssetManager>(AssetManager);
              if (ptr) {
                  res["AssetManager"] = ptr->to_dict();
              }
          }

          if (ResourceManager.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ServiceDelegationResourceManager>(ResourceManager);
              if (ptr) {
                  res["ResourceManager"] = ptr->to_dict();
              }
          }

          if (NectarStaking.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ServiceDelegationNectarStaking>(NectarStaking);
              if (ptr) {
                  res["NectarStaking"] = ptr->to_dict();
              }
          }

          if (NectarMissions.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ServiceDelegationNectarMissions>(NectarMissions);
              if (ptr) {
                  res["NectarMissions"] = ptr->to_dict();
              }
          }

          if (BuzzGuild.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ServiceDelegationBuzzGuild>(BuzzGuild);
              if (ptr) {
                  res["BuzzGuild"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot