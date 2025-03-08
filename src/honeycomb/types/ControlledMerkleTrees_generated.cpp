#include "honeycomb/types/ControlledMerkleTrees_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ControlledMerkleTrees::set_active(const int32_t &val) {
	this->active = val;
}

int32_t ControlledMerkleTrees::get_active() const {
	return this->active;
}

void ControlledMerkleTrees::set_schema(const Dictionary &val) {
	this->schema = val;
}

Dictionary ControlledMerkleTrees::get_schema() const {
	return this->schema;
}

void ControlledMerkleTrees::set_merkle_trees(const Array &val) {
	this->merkle_trees = val;
}

Array ControlledMerkleTrees::get_merkle_trees() const {
	return this->merkle_trees;
}

void ControlledMerkleTrees::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_active"), &ControlledMerkleTrees::get_active);
	ClassDB::bind_method(D_METHOD("set_active", "value"), &ControlledMerkleTrees::set_active);
	ClassDB::add_property("ControlledMerkleTrees", PropertyInfo(Variant::Type::INT, "active"), "set_active", "get_active");
	ClassDB::bind_method(D_METHOD("get_schema"), &ControlledMerkleTrees::get_schema);
	ClassDB::bind_method(D_METHOD("set_schema", "value"), &ControlledMerkleTrees::set_schema);
	ClassDB::add_property("ControlledMerkleTrees", PropertyInfo(Variant::Type::DICTIONARY, "schema"), "set_schema", "get_schema");
	ClassDB::bind_method(D_METHOD("get_merkle_trees"), &ControlledMerkleTrees::get_merkle_trees);
	ClassDB::bind_method(D_METHOD("set_merkle_trees", "value"), &ControlledMerkleTrees::set_merkle_trees);
	ClassDB::add_property("ControlledMerkleTrees", PropertyInfo(Variant::Type::ARRAY, "merkle_trees"), "set_merkle_trees", "get_merkle_trees");
	ClassDB::bind_method(D_METHOD("to_dict"), &ControlledMerkleTrees::to_dict);
}
Dictionary ControlledMerkleTrees::to_dict() {
	Dictionary res;
	res["active"] = active;
	res["schema"] = schema;
	res["merkle_trees"] = merkle_trees;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot