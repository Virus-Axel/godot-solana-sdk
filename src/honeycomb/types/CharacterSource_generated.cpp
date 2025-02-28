#include "honeycomb/types/CharacterSource_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CharacterSource::set_kind(const String& val) {
    this->kind = val;
}

String CharacterSource::get_kind() const {
    return this->kind;
}

void CharacterSource::set_params(const Dictionary& val) {
this->params = val;
}

Dictionary CharacterSource::get_params() const {
return this->params;
}

void CharacterSource::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &CharacterSource::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &CharacterSource::set_kind);
ClassDB::add_property("CharacterSource", PropertyInfo(Variant::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &CharacterSource::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &CharacterSource::set_params);
ClassDB::add_property("CharacterSource", PropertyInfo(Variant::Type::DICTIONARY, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterSource::to_dict);
}
Dictionary CharacterSource::to_dict() {
Dictionary res;
res["kind"] = this->kind;
res["params"] = params;
return res;
}
} // namespace honeycomb_resource
} // namespace godot