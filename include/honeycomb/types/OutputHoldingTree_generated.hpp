#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_OUTPUTHOLDINGTREE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_OUTPUTHOLDINGTREE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type OutputHoldingTree.
 */
class OutputHoldingTree : public Resource {
	GDCLASS(OutputHoldingTree, Resource)

private:
	PackedByteArray tree;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this OutputHoldingTree.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the tree property.
	 * @param val New value for tree.
	 */
	void set_tree(const PackedByteArray &val);
	/**
	 * @brief Get the tree property.
	 * @return Current value of tree.
	 */
	PackedByteArray get_tree() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_OUTPUTHOLDINGTREE
