#include "honeycomb/types/CharacterSourceParamsFilter_generated.hpp"

#include "pubkey.hpp"
#include "honeycomb/types/AssetCriteriaFilter_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CharacterSourceParamsFilter::set_mint(const Variant& val) {
this->mint = val;
}

Variant CharacterSourceParamsFilter::get_mint() const {
return this->mint;
}

void CharacterSourceParamsFilter::set_criteria(const Variant& val) {
this->criteria = val;
}

Variant CharacterSourceParamsFilter::get_criteria() const {
return this->criteria;
}

void CharacterSourceParamsFilter::set_is_compressed(const bool& val) {
this->is_compressed = val;
}

bool CharacterSourceParamsFilter::get_is_compressed() const {
return this->is_compressed;
}

void CharacterSourceParamsFilter::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_mint"), &CharacterSourceParamsFilter::get_mint);
ClassDB::bind_method(D_METHOD("set_mint", "value"), &CharacterSourceParamsFilter::set_mint);
ClassDB::add_property("CharacterSourceParamsFilter", PropertyInfo(Variant::Type::OBJECT, "mint"), "set_mint", "get_mint");
ClassDB::bind_method(D_METHOD("get_criteria"), &CharacterSourceParamsFilter::get_criteria);
ClassDB::bind_method(D_METHOD("set_criteria", "value"), &CharacterSourceParamsFilter::set_criteria);
ClassDB::add_property("CharacterSourceParamsFilter", PropertyInfo(Variant::Type::OBJECT, "criteria"), "set_criteria", "get_criteria");
ClassDB::bind_method(D_METHOD("get_is_compressed"), &CharacterSourceParamsFilter::get_is_compressed);
ClassDB::bind_method(D_METHOD("set_is_compressed", "value"), &CharacterSourceParamsFilter::set_is_compressed);
ClassDB::add_property("CharacterSourceParamsFilter", PropertyInfo(Variant::Type::BOOL, "is_compressed"), "set_is_compressed", "get_is_compressed");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterSourceParamsFilter::to_dict);
}
Dictionary CharacterSourceParamsFilter::to_dict() {
Dictionary res;
if (mint.has_method("to_string")) {
res["mint"] = mint.call("to_string");
} else {
res["mint"] = mint;
}

          if (criteria.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::AssetCriteriaFilter>(criteria);
              if (ptr) {
                  res["criteria"] = ptr->to_dict();
              }
          }
res["is_compressed"] = is_compressed;
return res;
}
} // namespace honeycomb_resource
} // namespace godot