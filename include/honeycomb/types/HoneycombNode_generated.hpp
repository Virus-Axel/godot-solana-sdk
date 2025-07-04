#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBNODE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBNODE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type HoneycombNode.
 */
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
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this HoneycombNode.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the id property.
	 * @param val New value for id.
	 */
	void set_id(const String &val);
	/**
	 * @brief Get the id property.
	 * @return Current value of id.
	 */
	String get_id() const;

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
	 * @brief Set the node_idx property.
	 * @param val New value for node_idx.
	 */
	void set_node_idx(const int64_t &val);
	/**
	 * @brief Get the node_idx property.
	 * @return Current value of node_idx.
	 */
	int64_t get_node_idx() const;

	/**
	 * @brief Set the leaf_idx property.
	 * @param val New value for leaf_idx.
	 */
	void set_leaf_idx(const int64_t &val);
	/**
	 * @brief Get the leaf_idx property.
	 * @return Current value of leaf_idx.
	 */
	int64_t get_leaf_idx() const;

	/**
	 * @brief Set the seq property.
	 * @param val New value for seq.
	 */
	void set_seq(const int64_t &val);
	/**
	 * @brief Get the seq property.
	 * @return Current value of seq.
	 */
	int64_t get_seq() const;

	/**
	 * @brief Set the level property.
	 * @param val New value for level.
	 */
	void set_level(const int64_t &val);
	/**
	 * @brief Get the level property.
	 * @return Current value of level.
	 */
	int64_t get_level() const;

	/**
	 * @brief Set the hash property.
	 * @param val New value for hash.
	 */
	void set_hash(const PackedByteArray &val);
	/**
	 * @brief Get the hash property.
	 * @return Current value of hash.
	 */
	PackedByteArray get_hash() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBNODE
