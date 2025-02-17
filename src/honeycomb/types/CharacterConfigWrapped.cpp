#include "honeycomb/types/CharacterConfigWrapped.hpp"


namespace godot {
namespace honeycomb_resource {

void CharacterConfigWrapped::set_kind(const String& val) {
this->kind = val;
}

String CharacterConfigWrapped::get_kind() const {
return this->kind;
}

void CharacterConfigWrapped::set_criterias(const Array& val) {
this->criterias = val;
}

Array CharacterConfigWrapped::get_criterias() const {
return this->criterias;
}

void CharacterConfigWrapped::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &CharacterConfigWrapped::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &CharacterConfigWrapped::set_kind);
ClassDB::add_property("CharacterConfigWrapped", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_criterias"), &CharacterConfigWrapped::get_criterias);
ClassDB::bind_method(D_METHOD("set_criterias", "value"), &CharacterConfigWrapped::set_criterias);
ClassDB::add_property("CharacterConfigWrapped", PropertyInfo(Variant::Type::ARRAY, "criterias"), "set_criterias", "get_criterias");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterConfigWrapped::to_dict);
}
Dictionary CharacterConfigWrapped::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
res["criterias"] = criterias;
return res;
}
} // namespace honeycomb_resource
} // namespace godot