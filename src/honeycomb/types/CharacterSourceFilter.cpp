#include "honeycomb/types/CharacterSourceFilter.hpp"

#include "honeycomb/types/CharacterSourceParamsFilter.hpp"

namespace godot {
namespace honeycomb_resource {

void CharacterSourceFilter::set_kind(const String& val) {
this->kind = val;
}

String CharacterSourceFilter::get_kind() const {
return this->kind;
}

void CharacterSourceFilter::set_params(const Variant& val) {
this->params = val;
}

Variant CharacterSourceFilter::get_params() const {
return this->params;
}

void CharacterSourceFilter::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &CharacterSourceFilter::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &CharacterSourceFilter::set_kind);
ClassDB::add_property("CharacterSourceFilter", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &CharacterSourceFilter::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &CharacterSourceFilter::set_params);
ClassDB::add_property("CharacterSourceFilter", PropertyInfo(Variant::Type::OBJECT, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterSourceFilter::to_dict);
}
Dictionary CharacterSourceFilter::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}

          if (params.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::CharacterSourceParamsFilter>(params);
              if (ptr) {
                  res["params"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot