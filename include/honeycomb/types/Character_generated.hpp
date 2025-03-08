#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTER
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class Character : public Resource {
	GDCLASS(Character, Resource)
private:
	PackedByteArray address;
	PackedByteArray tree_id;
	int64_t leaf_idx;
	Variant owner;
	Variant source;
	Variant usedBy;
	Array equipments;
	Variant proof;
	Dictionary asset;
	Variant cooldown;

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
	void set_owner(const Variant &val);
	Variant get_owner() const;
	void set_source(const Variant &val);
	Variant get_source() const;
	void set_usedBy(const Variant &val);
	Variant get_usedBy() const;
	void set_equipments(const Array &val);
	Array get_equipments() const;
	void set_proof(const Variant &val);
	Variant get_proof() const;
	void set_asset(const Dictionary &val);
	Dictionary get_asset() const;
	void set_cooldown(const Variant &val);
	Variant get_cooldown() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
