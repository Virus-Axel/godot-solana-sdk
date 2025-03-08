#include "honeycomb/types/PlatformDataInput_generated.hpp"

#include "honeycomb/types/CustomDataInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void PlatformDataInput::set_custom(const Variant& val) {
this->custom = val;
}

Variant PlatformDataInput::get_custom() const {
return this->custom;
}

void PlatformDataInput::set_addXp(const int64_t& val) {
this->addXp = val;
}

int64_t PlatformDataInput::get_addXp() const {
return this->addXp;
}

void PlatformDataInput::set_addAchievements(const PackedInt32Array& val) {
this->addAchievements = val;
}

PackedInt32Array PlatformDataInput::get_addAchievements() const {
return this->addAchievements;
}

void PlatformDataInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_custom"), &PlatformDataInput::get_custom);
ClassDB::bind_method(D_METHOD("set_custom", "value"), &PlatformDataInput::set_custom);
ClassDB::add_property("PlatformDataInput", PropertyInfo(Variant::Type::OBJECT, "custom"), "set_custom", "get_custom");
ClassDB::bind_method(D_METHOD("get_addXp"), &PlatformDataInput::get_addXp);
ClassDB::bind_method(D_METHOD("set_addXp", "value"), &PlatformDataInput::set_addXp);
ClassDB::add_property("PlatformDataInput", PropertyInfo(Variant::Type::INT, "addXp"), "set_addXp", "get_addXp");
ClassDB::bind_method(D_METHOD("get_addAchievements"), &PlatformDataInput::get_addAchievements);
ClassDB::bind_method(D_METHOD("set_addAchievements", "value"), &PlatformDataInput::set_addAchievements);
ClassDB::add_property("PlatformDataInput", PropertyInfo(Variant::Type::PACKED_INT32_ARRAY, "addAchievements"), "set_addAchievements", "get_addAchievements");
ClassDB::bind_method(D_METHOD("to_dict"), &PlatformDataInput::to_dict);
}
Dictionary PlatformDataInput::to_dict() {
Dictionary res;

          if (custom.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::CustomDataInput>(custom);
              if (ptr) {
                  res["custom"] = ptr->to_dict();
              }
          }
res["addXp"] = addXp;
res["addAchievements"] = addAchievements;
return res;
}
} // namespace honeycomb_resource
} // namespace godot