#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBNODE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBNODE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class HoneycombNode : public Resource {
	GDCLASS(HoneycombNode, Resource)
private:
	String id;
	PackedByteArray tree;
	int64_t node_idx;
	int64_t leaf_idx;
	int64_t seq;
	int64_t level;
	PackedByteArray hash;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_id(const String &val);
	String get_id() const;
	void set_tree(const PackedByteArray &val);
	PackedByteArray get_tree() const;
	void set_node_idx(const int64_t &val);
	int64_t get_node_idx() const;
	void set_leaf_idx(const int64_t &val);
	int64_t get_leaf_idx() const;
	void set_seq(const int64_t &val);
	int64_t get_seq() const;
	void set_level(const int64_t &val);
	int64_t get_level() const;
	void set_hash(const PackedByteArray &val);
	PackedByteArray get_hash() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
