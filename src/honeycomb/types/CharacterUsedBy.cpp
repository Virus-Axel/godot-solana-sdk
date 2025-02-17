#include "honeycomb/types/CharacterUsedBy.hpp"


namespace godot {
namespace honeycomb_resource {

void CharacterUsedBy::set_kind(const String& val) {
this->kind = val;
}

String CharacterUsedBy::get_kind() const {
return this->kind;
}

void CharacterUsedBy::set_params(const Dictionary& val) {
this->params = val;
}

Dictionary CharacterUsedBy::get_params() const {
return this->params;
}

void CharacterUsedBy::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &CharacterUsedBy::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &CharacterUsedBy::set_kind);
ClassDB::add_property("CharacterUsedBy", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &CharacterUsedBy::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &CharacterUsedBy::set_params);
ClassDB::add_property("CharacterUsedBy", PropertyInfo(Variant::Type::DICTIONARY, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterUsedBy::to_dict);
}
Dictionary CharacterUsedBy::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
res["params"] = params;
return res;
}
} // namespace honeycomb_resource
} // namespace godot