#include "honeycomb/types/CharacterCooldownInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void CharacterCooldownInput::set_ejection(const int32_t& val) {
this->ejection = val;
}

int32_t CharacterCooldownInput::get_ejection() const {
return this->ejection;
}

void CharacterCooldownInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_ejection"), &CharacterCooldownInput::get_ejection);
ClassDB::bind_method(D_METHOD("set_ejection", "value"), &CharacterCooldownInput::set_ejection);
ClassDB::add_property("CharacterCooldownInput", PropertyInfo(Variant::Type::INT, "ejection"), "set_ejection", "get_ejection");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterCooldownInput::to_dict);
}
Dictionary CharacterCooldownInput::to_dict() {
Dictionary res;
res["ejection"] = ejection;
return res;
}
} // namespace honeycomb_resource
} // namespace godot