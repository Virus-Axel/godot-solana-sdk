#include "honeycomb/types/ResourceRewardType_generated.hpp"

#include "honeycomb/types/ResourceRewardTypeParams_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ResourceRewardType::set_kind(const String& val) {
this->kind = val;
}

String ResourceRewardType::get_kind() const {
return this->kind;
}

void ResourceRewardType::set_params(const Variant& val) {
this->params = val;
}

Variant ResourceRewardType::get_params() const {
return this->params;
}

void ResourceRewardType::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &ResourceRewardType::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &ResourceRewardType::set_kind);
ClassDB::add_property("ResourceRewardType", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &ResourceRewardType::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &ResourceRewardType::set_params);
ClassDB::add_property("ResourceRewardType", PropertyInfo(Variant::Type::OBJECT, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &ResourceRewardType::to_dict);
}
Dictionary ResourceRewardType::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}

          if (params.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ResourceRewardTypeParams>(params);
              if (ptr) {
                  res["params"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot