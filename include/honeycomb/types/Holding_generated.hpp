#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HOLDING
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HOLDING
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class Holding : public Resource {
	GDCLASS(Holding, Resource)
private:
	PackedByteArray address;
	PackedByteArray tree_id;
	int64_t leaf_idx;
	Variant holder;
	int64_t balance;
	Array characteristics;
	Variant proof;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const PackedByteArray &val);
	PackedByteArray get_address() const;
	void set_tree_id(const PackedByteArray &val);
	PackedByteArray get_tree_id() const;
	void set_leaf_idx(const int64_t &val);
	int64_t get_leaf_idx() const;
	void set_holder(const Variant &val);
	Variant get_holder() const;
	void set_balance(const int64_t &val);
	int64_t get_balance() const;
	void set_characteristics(const Array &val);
	Array get_characteristics() const;
	void set_proof(const Variant &val);
	Variant get_proof() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
