#include "honeycomb/types/HoneycombNode_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void HoneycombNode::set_id(const String& val) {
this->id = val;
}

String HoneycombNode::get_id() const {
return this->id;
}

void HoneycombNode::set_tree(const PackedByteArray& val) {
this->tree = val;
}

PackedByteArray HoneycombNode::get_tree() const {
return this->tree;
}

void HoneycombNode::set_node_idx(const int64_t& val) {
this->node_idx = val;
}

int64_t HoneycombNode::get_node_idx() const {
return this->node_idx;
}

void HoneycombNode::set_leaf_idx(const int64_t& val) {
this->leaf_idx = val;
}

int64_t HoneycombNode::get_leaf_idx() const {
return this->leaf_idx;
}

void HoneycombNode::set_seq(const int64_t& val) {
this->seq = val;
}

int64_t HoneycombNode::get_seq() const {
return this->seq;
}

void HoneycombNode::set_level(const int64_t& val) {
this->level = val;
}

int64_t HoneycombNode::get_level() const {
return this->level;
}

void HoneycombNode::set_hash(const PackedByteArray& val) {
this->hash = val;
}

PackedByteArray HoneycombNode::get_hash() const {
return this->hash;
}

void HoneycombNode::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_id"), &HoneycombNode::get_id);
ClassDB::bind_method(D_METHOD("set_id", "value"), &HoneycombNode::set_id);
ClassDB::add_property("HoneycombNode", PropertyInfo(Variant::Type::STRING, "id"), "set_id", "get_id");
ClassDB::bind_method(D_METHOD("get_tree"), &HoneycombNode::get_tree);
ClassDB::bind_method(D_METHOD("set_tree", "value"), &HoneycombNode::set_tree);
ClassDB::add_property("HoneycombNode", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree"), "set_tree", "get_tree");
ClassDB::bind_method(D_METHOD("get_node_idx"), &HoneycombNode::get_node_idx);
ClassDB::bind_method(D_METHOD("set_node_idx", "value"), &HoneycombNode::set_node_idx);
ClassDB::add_property("HoneycombNode", PropertyInfo(Variant::Type::INT, "node_idx"), "set_node_idx", "get_node_idx");
ClassDB::bind_method(D_METHOD("get_leaf_idx"), &HoneycombNode::get_leaf_idx);
ClassDB::bind_method(D_METHOD("set_leaf_idx", "value"), &HoneycombNode::set_leaf_idx);
ClassDB::add_property("HoneycombNode", PropertyInfo(Variant::Type::INT, "leaf_idx"), "set_leaf_idx", "get_leaf_idx");
ClassDB::bind_method(D_METHOD("get_seq"), &HoneycombNode::get_seq);
ClassDB::bind_method(D_METHOD("set_seq", "value"), &HoneycombNode::set_seq);
ClassDB::add_property("HoneycombNode", PropertyInfo(Variant::Type::INT, "seq"), "set_seq", "get_seq");
ClassDB::bind_method(D_METHOD("get_level"), &HoneycombNode::get_level);
ClassDB::bind_method(D_METHOD("set_level", "value"), &HoneycombNode::set_level);
ClassDB::add_property("HoneycombNode", PropertyInfo(Variant::Type::INT, "level"), "set_level", "get_level");
ClassDB::bind_method(D_METHOD("get_hash"), &HoneycombNode::get_hash);
ClassDB::bind_method(D_METHOD("set_hash", "value"), &HoneycombNode::set_hash);
ClassDB::add_property("HoneycombNode", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "hash"), "set_hash", "get_hash");
ClassDB::bind_method(D_METHOD("to_dict"), &HoneycombNode::to_dict);
}
Dictionary HoneycombNode::to_dict() {
Dictionary res;
if (!id.is_empty()) {
    res["id"] = id;
}
res["tree"] = tree;
res["node_idx"] = node_idx;
res["leaf_idx"] = leaf_idx;
res["seq"] = seq;
res["level"] = level;
res["hash"] = hash;
return res;
}
} // namespace honeycomb_resource
} // namespace godot