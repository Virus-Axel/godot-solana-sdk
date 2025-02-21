#include "honeycomb/types/CharacterUsedByFilter.hpp"

#include "honeycomb/types/CharacterUsedByParamsFilter.hpp"

namespace godot {
namespace honeycomb_resource {

void CharacterUsedByFilter::set_kind(const String& val) {
this->kind = val;
}

String CharacterUsedByFilter::get_kind() const {
return this->kind;
}

void CharacterUsedByFilter::set_params(const Variant& val) {
this->params = val;
}

Variant CharacterUsedByFilter::get_params() const {
return this->params;
}

void CharacterUsedByFilter::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &CharacterUsedByFilter::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &CharacterUsedByFilter::set_kind);
ClassDB::add_property("CharacterUsedByFilter", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &CharacterUsedByFilter::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &CharacterUsedByFilter::set_params);
ClassDB::add_property("CharacterUsedByFilter", PropertyInfo(Variant::Type::OBJECT, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterUsedByFilter::to_dict);
}
Dictionary CharacterUsedByFilter::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}

          if (params.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::CharacterUsedByParamsFilter>(params);
              if (ptr) {
                  res["params"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot