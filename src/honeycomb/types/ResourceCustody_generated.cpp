#include "honeycomb/types/ResourceCustody_generated.hpp"

#include "honeycomb/enums_generated.hpp"
#include "honeycomb/types/ResourceCustodyParams_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ResourceCustody::set_kind(const String& val) {
    this->kind = val;
}

String ResourceCustody::get_kind() const {
    return this->kind;
}

void ResourceCustody::set_params(const Variant& val) {
this->params = val;
}

Variant ResourceCustody::get_params() const {
return this->params;
}

void ResourceCustody::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &ResourceCustody::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &ResourceCustody::set_kind);
ClassDB::add_property("ResourceCustody", PropertyInfo(Variant::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &ResourceCustody::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &ResourceCustody::set_params);
ClassDB::add_property("ResourceCustody", PropertyInfo(Variant::Type::OBJECT, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &ResourceCustody::to_dict);
}
Dictionary ResourceCustody::to_dict() {
Dictionary res;
res["kind"] = this->kind;

          if (params.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ResourceCustodyParams>(params);
              if (ptr) {
                  res["params"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot