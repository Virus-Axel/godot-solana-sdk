#include "honeycomb/types/Wrapped_generated.hpp"

#include "honeycomb/enums_generated.hpp"
#include "pubkey.hpp"
#include "honeycomb/types/AssetCriteria_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void Wrapped::set_mint(const Variant& val) {
this->mint = val;
}

Variant Wrapped::get_mint() const {
return this->mint;
}

void Wrapped::set_criteria(const Variant& val) {
this->criteria = val;
}

Variant Wrapped::get_criteria() const {
return this->criteria;
}

void Wrapped::set_kind(const String& val) {
    this->kind = val;
}

String Wrapped::get_kind() const {
    return this->kind;
}

void Wrapped::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_mint"), &Wrapped::get_mint);
ClassDB::bind_method(D_METHOD("set_mint", "value"), &Wrapped::set_mint);
ClassDB::add_property("Wrapped", PropertyInfo(Variant::Type::OBJECT, "mint"), "set_mint", "get_mint");
ClassDB::bind_method(D_METHOD("get_criteria"), &Wrapped::get_criteria);
ClassDB::bind_method(D_METHOD("set_criteria", "value"), &Wrapped::set_criteria);
ClassDB::add_property("Wrapped", PropertyInfo(Variant::Type::OBJECT, "criteria"), "set_criteria", "get_criteria");
ClassDB::bind_method(D_METHOD("get_kind"), &Wrapped::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &Wrapped::set_kind);
ClassDB::add_property("Wrapped", PropertyInfo(Variant::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("to_dict"), &Wrapped::to_dict);
}
Dictionary Wrapped::to_dict() {
Dictionary res;
if (mint.has_method("to_string")) {
res["mint"] = mint.call("to_string");
} else {
res["mint"] = mint;
}

          if (criteria.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::AssetCriteria>(criteria);
              if (ptr) {
                  res["criteria"] = ptr->to_dict();
              }
          }
res["kind"] = this->kind;
return res;
}
} // namespace honeycomb_resource
} // namespace godot