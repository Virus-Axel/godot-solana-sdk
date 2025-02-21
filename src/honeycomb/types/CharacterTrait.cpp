#include "honeycomb/types/CharacterTrait.hpp"

#include "honeycomb/types/Proof.hpp"

namespace godot {
namespace honeycomb_resource {

void CharacterTrait::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray CharacterTrait::get_address() const {
return this->address;
}

void CharacterTrait::set_tree_id(const PackedByteArray& val) {
this->tree_id = val;
}

PackedByteArray CharacterTrait::get_tree_id() const {
return this->tree_id;
}

void CharacterTrait::set_leaf_idx(const int64_t& val) {
this->leaf_idx = val;
}

int64_t CharacterTrait::get_leaf_idx() const {
return this->leaf_idx;
}

void CharacterTrait::set_layer(const String& val) {
this->layer = val;
}

String CharacterTrait::get_layer() const {
return this->layer;
}

void CharacterTrait::set_name(const String& val) {
this->name = val;
}

String CharacterTrait::get_name() const {
return this->name;
}

void CharacterTrait::set_uri(const String& val) {
this->uri = val;
}

String CharacterTrait::get_uri() const {
return this->uri;
}

void CharacterTrait::set_proof(const Variant& val) {
this->proof = val;
}

Variant CharacterTrait::get_proof() const {
return this->proof;
}

void CharacterTrait::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &CharacterTrait::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &CharacterTrait::set_address);
ClassDB::add_property("CharacterTrait", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_tree_id"), &CharacterTrait::get_tree_id);
ClassDB::bind_method(D_METHOD("set_tree_id", "value"), &CharacterTrait::set_tree_id);
ClassDB::add_property("CharacterTrait", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree_id"), "set_tree_id", "get_tree_id");
ClassDB::bind_method(D_METHOD("get_leaf_idx"), &CharacterTrait::get_leaf_idx);
ClassDB::bind_method(D_METHOD("set_leaf_idx", "value"), &CharacterTrait::set_leaf_idx);
ClassDB::add_property("CharacterTrait", PropertyInfo(Variant::Type::INT, "leaf_idx"), "set_leaf_idx", "get_leaf_idx");
ClassDB::bind_method(D_METHOD("get_layer"), &CharacterTrait::get_layer);
ClassDB::bind_method(D_METHOD("set_layer", "value"), &CharacterTrait::set_layer);
ClassDB::add_property("CharacterTrait", PropertyInfo(Variant::Type::STRING, "layer"), "set_layer", "get_layer");
ClassDB::bind_method(D_METHOD("get_name"), &CharacterTrait::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &CharacterTrait::set_name);
ClassDB::add_property("CharacterTrait", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_uri"), &CharacterTrait::get_uri);
ClassDB::bind_method(D_METHOD("set_uri", "value"), &CharacterTrait::set_uri);
ClassDB::add_property("CharacterTrait", PropertyInfo(Variant::Type::STRING, "uri"), "set_uri", "get_uri");
ClassDB::bind_method(D_METHOD("get_proof"), &CharacterTrait::get_proof);
ClassDB::bind_method(D_METHOD("set_proof", "value"), &CharacterTrait::set_proof);
ClassDB::add_property("CharacterTrait", PropertyInfo(Variant::Type::OBJECT, "proof"), "set_proof", "get_proof");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterTrait::to_dict);
}
Dictionary CharacterTrait::to_dict() {
Dictionary res;
res["address"] = address;
res["tree_id"] = tree_id;
res["leaf_idx"] = leaf_idx;
if (!layer.is_empty()) {
    res["layer"] = layer;
}
if (!name.is_empty()) {
    res["name"] = name;
}
if (!uri.is_empty()) {
    res["uri"] = uri;
}

          if (proof.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::Proof>(proof);
              if (ptr) {
                  res["proof"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot