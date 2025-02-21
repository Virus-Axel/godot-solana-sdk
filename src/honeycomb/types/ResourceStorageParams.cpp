#include "honeycomb/types/ResourceStorageParams.hpp"

#include "honeycomb/types/ControlledMerkleTrees.hpp"

namespace godot {
namespace honeycomb_resource {

void ResourceStorageParams::set_merkle_trees(const Variant& val) {
this->merkle_trees = val;
}

Variant ResourceStorageParams::get_merkle_trees() const {
return this->merkle_trees;
}

void ResourceStorageParams::set_promiseSupply(const int64_t& val) {
this->promiseSupply = val;
}

int64_t ResourceStorageParams::get_promiseSupply() const {
return this->promiseSupply;
}

void ResourceStorageParams::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_merkle_trees"), &ResourceStorageParams::get_merkle_trees);
ClassDB::bind_method(D_METHOD("set_merkle_trees", "value"), &ResourceStorageParams::set_merkle_trees);
ClassDB::add_property("ResourceStorageParams", PropertyInfo(Variant::Type::OBJECT, "merkle_trees"), "set_merkle_trees", "get_merkle_trees");
ClassDB::bind_method(D_METHOD("get_promiseSupply"), &ResourceStorageParams::get_promiseSupply);
ClassDB::bind_method(D_METHOD("set_promiseSupply", "value"), &ResourceStorageParams::set_promiseSupply);
ClassDB::add_property("ResourceStorageParams", PropertyInfo(Variant::Type::INT, "promiseSupply"), "set_promiseSupply", "get_promiseSupply");
ClassDB::bind_method(D_METHOD("to_dict"), &ResourceStorageParams::to_dict);
}
Dictionary ResourceStorageParams::to_dict() {
Dictionary res;

          if (merkle_trees.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ControlledMerkleTrees>(merkle_trees);
              if (ptr) {
                  res["merkle_trees"] = ptr->to_dict();
              }
          }
res["promiseSupply"] = promiseSupply;
return res;
}
} // namespace honeycomb_resource
} // namespace godot