#include "honeycomb/types/Proof_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void Proof::set_root(const PackedByteArray& val) {
this->root = val;
}

PackedByteArray Proof::get_root() const {
return this->root;
}

void Proof::set_proof(const Array& val) {
this->proof = val;
}

Array Proof::get_proof() const {
return this->proof;
}

void Proof::set_node_index(const int64_t& val) {
this->node_index = val;
}

int64_t Proof::get_node_index() const {
return this->node_index;
}

void Proof::set_leaf(const PackedByteArray& val) {
this->leaf = val;
}

PackedByteArray Proof::get_leaf() const {
return this->leaf;
}

void Proof::set_maxDepth(const int32_t& val) {
this->maxDepth = val;
}

int32_t Proof::get_maxDepth() const {
return this->maxDepth;
}

void Proof::set_leaf_index(const int64_t& val) {
this->leaf_index = val;
}

int64_t Proof::get_leaf_index() const {
return this->leaf_index;
}

void Proof::set_tree_id(const PackedByteArray& val) {
this->tree_id = val;
}

PackedByteArray Proof::get_tree_id() const {
return this->tree_id;
}

void Proof::set_canopy_depth(const int32_t& val) {
this->canopy_depth = val;
}

int32_t Proof::get_canopy_depth() const {
return this->canopy_depth;
}

void Proof::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_root"), &Proof::get_root);
ClassDB::bind_method(D_METHOD("set_root", "value"), &Proof::set_root);
ClassDB::add_property("Proof", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "root"), "set_root", "get_root");
ClassDB::bind_method(D_METHOD("get_proof"), &Proof::get_proof);
ClassDB::bind_method(D_METHOD("set_proof", "value"), &Proof::set_proof);
ClassDB::add_property("Proof", PropertyInfo(Variant::Type::ARRAY, "proof"), "set_proof", "get_proof");
ClassDB::bind_method(D_METHOD("get_node_index"), &Proof::get_node_index);
ClassDB::bind_method(D_METHOD("set_node_index", "value"), &Proof::set_node_index);
ClassDB::add_property("Proof", PropertyInfo(Variant::Type::INT, "node_index"), "set_node_index", "get_node_index");
ClassDB::bind_method(D_METHOD("get_leaf"), &Proof::get_leaf);
ClassDB::bind_method(D_METHOD("set_leaf", "value"), &Proof::set_leaf);
ClassDB::add_property("Proof", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "leaf"), "set_leaf", "get_leaf");
ClassDB::bind_method(D_METHOD("get_maxDepth"), &Proof::get_maxDepth);
ClassDB::bind_method(D_METHOD("set_maxDepth", "value"), &Proof::set_maxDepth);
ClassDB::add_property("Proof", PropertyInfo(Variant::Type::INT, "maxDepth"), "set_maxDepth", "get_maxDepth");
ClassDB::bind_method(D_METHOD("get_leaf_index"), &Proof::get_leaf_index);
ClassDB::bind_method(D_METHOD("set_leaf_index", "value"), &Proof::set_leaf_index);
ClassDB::add_property("Proof", PropertyInfo(Variant::Type::INT, "leaf_index"), "set_leaf_index", "get_leaf_index");
ClassDB::bind_method(D_METHOD("get_tree_id"), &Proof::get_tree_id);
ClassDB::bind_method(D_METHOD("set_tree_id", "value"), &Proof::set_tree_id);
ClassDB::add_property("Proof", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree_id"), "set_tree_id", "get_tree_id");
ClassDB::bind_method(D_METHOD("get_canopy_depth"), &Proof::get_canopy_depth);
ClassDB::bind_method(D_METHOD("set_canopy_depth", "value"), &Proof::set_canopy_depth);
ClassDB::add_property("Proof", PropertyInfo(Variant::Type::INT, "canopy_depth"), "set_canopy_depth", "get_canopy_depth");
ClassDB::bind_method(D_METHOD("to_dict"), &Proof::to_dict);
}
Dictionary Proof::to_dict() {
Dictionary res;
res["root"] = root;
res["proof"] = proof;
res["node_index"] = node_index;
res["leaf"] = leaf;
res["maxDepth"] = maxDepth;
res["leaf_index"] = leaf_index;
res["tree_id"] = tree_id;
res["canopy_depth"] = canopy_depth;
return res;
}
} // namespace honeycomb_resource
} // namespace godot