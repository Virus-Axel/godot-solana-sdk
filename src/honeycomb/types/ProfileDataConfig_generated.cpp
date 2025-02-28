#include "honeycomb/types/ProfileDataConfig_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void ProfileDataConfig::set_achievements(const PackedStringArray& val) {
this->achievements = val;
}

PackedStringArray ProfileDataConfig::get_achievements() const {
return this->achievements;
}

void ProfileDataConfig::set_customDataFields(const PackedStringArray& val) {
this->customDataFields = val;
}

PackedStringArray ProfileDataConfig::get_customDataFields() const {
return this->customDataFields;
}

void ProfileDataConfig::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_achievements"), &ProfileDataConfig::get_achievements);
ClassDB::bind_method(D_METHOD("set_achievements", "value"), &ProfileDataConfig::set_achievements);
ClassDB::add_property("ProfileDataConfig", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "achievements"), "set_achievements", "get_achievements");
ClassDB::bind_method(D_METHOD("get_customDataFields"), &ProfileDataConfig::get_customDataFields);
ClassDB::bind_method(D_METHOD("set_customDataFields", "value"), &ProfileDataConfig::set_customDataFields);
ClassDB::add_property("ProfileDataConfig", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "customDataFields"), "set_customDataFields", "get_customDataFields");
ClassDB::bind_method(D_METHOD("to_dict"), &ProfileDataConfig::to_dict);
}
Dictionary ProfileDataConfig::to_dict() {
Dictionary res;
res["achievements"] = achievements;
res["customDataFields"] = customDataFields;
return res;
}
} // namespace honeycomb_resource
} // namespace godot