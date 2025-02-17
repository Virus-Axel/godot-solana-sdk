#include "honeycomb/types/TreeLeaf.hpp"


namespace godot {
namespace honeycomb_resource {

void TreeLeaf::set_tree(const PackedByteArray& val) {
this->tree = val;
}

PackedByteArray TreeLeaf::get_tree() const {
return this->tree;
}

void TreeLeaf::set_index(const int64_t& val) {
this->index = val;
}

int64_t TreeLeaf::get_index() const {
return this->index;
}

void TreeLeaf::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tree"), &TreeLeaf::get_tree);
ClassDB::bind_method(D_METHOD("set_tree", "value"), &TreeLeaf::set_tree);
ClassDB::add_property("TreeLeaf", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree"), "set_tree", "get_tree");
ClassDB::bind_method(D_METHOD("get_index"), &TreeLeaf::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &TreeLeaf::set_index);
ClassDB::add_property("TreeLeaf", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("to_dict"), &TreeLeaf::to_dict);
}
Dictionary TreeLeaf::to_dict() {
Dictionary res;
res["tree"] = tree;
res["index"] = index;
return res;
}
} // namespace honeycomb_resource
} // namespace godot