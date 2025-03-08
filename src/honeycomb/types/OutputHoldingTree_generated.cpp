#include "honeycomb/types/OutputHoldingTree_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void OutputHoldingTree::set_tree(const PackedByteArray &val) {
	this->tree = val;
}

PackedByteArray OutputHoldingTree::get_tree() const {
	return this->tree;
}

void OutputHoldingTree::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_tree"), &OutputHoldingTree::get_tree);
	ClassDB::bind_method(D_METHOD("set_tree", "value"), &OutputHoldingTree::set_tree);
	ClassDB::add_property("OutputHoldingTree", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree"), "set_tree", "get_tree");
	ClassDB::bind_method(D_METHOD("to_dict"), &OutputHoldingTree::to_dict);
}
Dictionary OutputHoldingTree::to_dict() {
	Dictionary res;
	res["tree"] = tree;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot