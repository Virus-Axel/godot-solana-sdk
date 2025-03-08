#include "honeycomb/types/CharacterCooldown_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void CharacterCooldown::set_ejection(const int32_t& val) {
this->ejection = val;
}

int32_t CharacterCooldown::get_ejection() const {
return this->ejection;
}

void CharacterCooldown::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_ejection"), &CharacterCooldown::get_ejection);
ClassDB::bind_method(D_METHOD("set_ejection", "value"), &CharacterCooldown::set_ejection);
ClassDB::add_property("CharacterCooldown", PropertyInfo(Variant::Type::INT, "ejection"), "set_ejection", "get_ejection");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterCooldown::to_dict);
}
Dictionary CharacterCooldown::to_dict() {
Dictionary res;
res["ejection"] = ejection;
return res;
}
} // namespace honeycomb_resource
} // namespace godot