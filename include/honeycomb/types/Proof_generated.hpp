#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROOF
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROOF

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Proof.
 */
class Proof : public Resource {
	GDCLASS(Proof, Resource)

private:
	PackedByteArray root;
	Array proof;
	int64_t node_index;
	PackedByteArray leaf;
	int32_t maxDepth;
	int64_t leaf_index;
	PackedByteArray tree_id;
	int32_t canopy_depth;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Proof.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the root property.
	 * @param val New value for root.
	 */
	void set_root(const PackedByteArray &val);
	/**
	 * @brief Get the root property.
	 * @return Current value of root.
	 */
	PackedByteArray get_root() const;

	/**
	 * @brief Set the proof property.
	 * @param val New value for proof.
	 */
	void set_proof(const Array &val);
	/**
	 * @brief Get the proof property.
	 * @return Current value of proof.
	 */
	Array get_proof() const;

	/**
	 * @brief Set the node_index property.
	 * @param val New value for node_index.
	 */
	void set_node_index(const int64_t &val);
	/**
	 * @brief Get the node_index property.
	 * @return Current value of node_index.
	 */
	int64_t get_node_index() const;

	/**
	 * @brief Set the leaf property.
	 * @param val New value for leaf.
	 */
	void set_leaf(const PackedByteArray &val);
	/**
	 * @brief Get the leaf property.
	 * @return Current value of leaf.
	 */
	PackedByteArray get_leaf() const;

	/**
	 * @brief Set the maxDepth property.
	 * @param val New value for maxDepth.
	 */
	void set_maxDepth(const int32_t &val);
	/**
	 * @brief Get the maxDepth property.
	 * @return Current value of maxDepth.
	 */
	int32_t get_maxDepth() const;

	/**
	 * @brief Set the leaf_index property.
	 * @param val New value for leaf_index.
	 */
	void set_leaf_index(const int64_t &val);
	/**
	 * @brief Get the leaf_index property.
	 * @return Current value of leaf_index.
	 */
	int64_t get_leaf_index() const;

	/**
	 * @brief Set the tree_id property.
	 * @param val New value for tree_id.
	 */
	void set_tree_id(const PackedByteArray &val);
	/**
	 * @brief Get the tree_id property.
	 * @return Current value of tree_id.
	 */
	PackedByteArray get_tree_id() const;

	/**
	 * @brief Set the canopy_depth property.
	 * @param val New value for canopy_depth.
	 */
	void set_canopy_depth(const int32_t &val);
	/**
	 * @brief Get the canopy_depth property.
	 * @return Current value of canopy_depth.
	 */
	int32_t get_canopy_depth() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROOF
