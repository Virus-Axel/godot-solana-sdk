#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_OUTPUTHOLDINGTREE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_OUTPUTHOLDINGTREE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class OutputHoldingTree : public Resource {
	GDCLASS(OutputHoldingTree, Resource)
private:
	PackedByteArray tree;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_tree(const PackedByteArray &val);
	PackedByteArray get_tree() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
