#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAIT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAIT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CharacterTrait : public Resource {
	GDCLASS(CharacterTrait, Resource)
private:
	PackedByteArray address;
	PackedByteArray tree_id;
	int64_t leaf_idx;
	String layer;
	String name;
	String uri;
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
	void set_layer(const String &val);
	String get_layer() const;
	void set_name(const String &val);
	String get_name() const;
	void set_uri(const String &val);
	String get_uri() const;
	void set_proof(const Variant &val);
	Variant get_proof() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
