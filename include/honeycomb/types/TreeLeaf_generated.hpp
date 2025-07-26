#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREELEAF
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREELEAF

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type TreeLeaf.
 */
class TreeLeaf : public Resource {
	GDCLASS(TreeLeaf, Resource)

private:
	PackedByteArray tree;
	int64_t index;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this TreeLeaf.
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

	/**
	 * @brief Set the index property.
	 * @param val New value for index.
	 */
	void set_index(const int64_t &val);
	/**
	 * @brief Get the index property.
	 * @return Current value of index.
	 */
	int64_t get_index() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREELEAF
