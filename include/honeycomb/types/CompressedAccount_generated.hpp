#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_COMPRESSEDACCOUNT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_COMPRESSEDACCOUNT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CompressedAccount : public Resource {
	GDCLASS(CompressedAccount, Resource)
private:
	PackedByteArray tree_id;
	int64_t leaf_idx;
	bool schema_validated;
	PackedByteArray raw_data;
	Dictionary parsed_data;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_tree_id(const PackedByteArray &val);
	PackedByteArray get_tree_id() const;
	void set_leaf_idx(const int64_t &val);
	int64_t get_leaf_idx() const;
	void set_schema_validated(const bool &val);
	bool get_schema_validated() const;
	void set_raw_data(const PackedByteArray &val);
	PackedByteArray get_raw_data() const;
	void set_parsed_data(const Dictionary &val);
	Dictionary get_parsed_data() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
