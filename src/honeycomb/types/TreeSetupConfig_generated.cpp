#include "honeycomb/types/TreeSetupConfig_generated.hpp"

#include "honeycomb/types/BasicTreeConfig_generated.hpp"
#include "honeycomb/types/AdvancedTreeConfig_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void TreeSetupConfig::set_basic(const Variant& val) {
this->basic = val;
}

Variant TreeSetupConfig::get_basic() const {
return this->basic;
}

void TreeSetupConfig::set_advanced(const Variant& val) {
this->advanced = val;
}

Variant TreeSetupConfig::get_advanced() const {
return this->advanced;
}

void TreeSetupConfig::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_basic"), &TreeSetupConfig::get_basic);
ClassDB::bind_method(D_METHOD("set_basic", "value"), &TreeSetupConfig::set_basic);
ClassDB::add_property("TreeSetupConfig", PropertyInfo(Variant::Type::OBJECT, "basic"), "set_basic", "get_basic");
ClassDB::bind_method(D_METHOD("get_advanced"), &TreeSetupConfig::get_advanced);
ClassDB::bind_method(D_METHOD("set_advanced", "value"), &TreeSetupConfig::set_advanced);
ClassDB::add_property("TreeSetupConfig", PropertyInfo(Variant::Type::OBJECT, "advanced"), "set_advanced", "get_advanced");
ClassDB::bind_method(D_METHOD("to_dict"), &TreeSetupConfig::to_dict);
}
Dictionary TreeSetupConfig::to_dict() {
Dictionary res;

          if (basic.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::BasicTreeConfig>(basic);
              if (ptr) {
                  res["basic"] = ptr->to_dict();
              }
          }

          if (advanced.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::AdvancedTreeConfig>(advanced);
              if (ptr) {
                  res["advanced"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot