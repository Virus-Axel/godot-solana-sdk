#include "honeycomb/types/CompressedAccount_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void CompressedAccount::set_tree_id(const PackedByteArray& val) {
this->tree_id = val;
}

PackedByteArray CompressedAccount::get_tree_id() const {
return this->tree_id;
}

void CompressedAccount::set_leaf_idx(const int64_t& val) {
this->leaf_idx = val;
}

int64_t CompressedAccount::get_leaf_idx() const {
return this->leaf_idx;
}

void CompressedAccount::set_schema_validated(const bool& val) {
this->schema_validated = val;
}

bool CompressedAccount::get_schema_validated() const {
return this->schema_validated;
}

void CompressedAccount::set_raw_data(const PackedByteArray& val) {
this->raw_data = val;
}

PackedByteArray CompressedAccount::get_raw_data() const {
return this->raw_data;
}

void CompressedAccount::set_parsed_data(const Dictionary& val) {
this->parsed_data = val;
}

Dictionary CompressedAccount::get_parsed_data() const {
return this->parsed_data;
}

void CompressedAccount::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tree_id"), &CompressedAccount::get_tree_id);
ClassDB::bind_method(D_METHOD("set_tree_id", "value"), &CompressedAccount::set_tree_id);
ClassDB::add_property("CompressedAccount", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree_id"), "set_tree_id", "get_tree_id");
ClassDB::bind_method(D_METHOD("get_leaf_idx"), &CompressedAccount::get_leaf_idx);
ClassDB::bind_method(D_METHOD("set_leaf_idx", "value"), &CompressedAccount::set_leaf_idx);
ClassDB::add_property("CompressedAccount", PropertyInfo(Variant::Type::INT, "leaf_idx"), "set_leaf_idx", "get_leaf_idx");
ClassDB::bind_method(D_METHOD("get_schema_validated"), &CompressedAccount::get_schema_validated);
ClassDB::bind_method(D_METHOD("set_schema_validated", "value"), &CompressedAccount::set_schema_validated);
ClassDB::add_property("CompressedAccount", PropertyInfo(Variant::Type::BOOL, "schema_validated"), "set_schema_validated", "get_schema_validated");
ClassDB::bind_method(D_METHOD("get_raw_data"), &CompressedAccount::get_raw_data);
ClassDB::bind_method(D_METHOD("set_raw_data", "value"), &CompressedAccount::set_raw_data);
ClassDB::add_property("CompressedAccount", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "raw_data"), "set_raw_data", "get_raw_data");
ClassDB::bind_method(D_METHOD("get_parsed_data"), &CompressedAccount::get_parsed_data);
ClassDB::bind_method(D_METHOD("set_parsed_data", "value"), &CompressedAccount::set_parsed_data);
ClassDB::add_property("CompressedAccount", PropertyInfo(Variant::Type::DICTIONARY, "parsed_data"), "set_parsed_data", "get_parsed_data");
ClassDB::bind_method(D_METHOD("to_dict"), &CompressedAccount::to_dict);
}
Dictionary CompressedAccount::to_dict() {
Dictionary res;
res["tree_id"] = tree_id;
res["leaf_idx"] = leaf_idx;
res["schema_validated"] = schema_validated;
res["raw_data"] = raw_data;
res["parsed_data"] = parsed_data;
return res;
}
} // namespace honeycomb_resource
} // namespace godot