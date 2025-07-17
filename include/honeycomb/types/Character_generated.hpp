#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Character.
 */
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
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Character.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const PackedByteArray &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	PackedByteArray get_address() const;

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
	 * @brief Set the owner property.
	 * @param val New value for owner.
	 */
	void set_owner(const Variant &val);
	/**
	 * @brief Get the owner property.
	 * @return Current value of owner.
	 */
	Variant get_owner() const;

	/**
	 * @brief Set the source property.
	 * @param val New value for source.
	 */
	void set_source(const Variant &val);
	/**
	 * @brief Get the source property.
	 * @return Current value of source.
	 */
	Variant get_source() const;

	/**
	 * @brief Set the usedBy property.
	 * @param val New value for usedBy.
	 */
	void set_usedBy(const Variant &val);
	/**
	 * @brief Get the usedBy property.
	 * @return Current value of usedBy.
	 */
	Variant get_usedBy() const;

	/**
	 * @brief Set the equipments property.
	 * @param val New value for equipments.
	 */
	void set_equipments(const Array &val);
	/**
	 * @brief Get the equipments property.
	 * @return Current value of equipments.
	 */
	Array get_equipments() const;

	/**
	 * @brief Set the proof property.
	 * @param val New value for proof.
	 */
	void set_proof(const Variant &val);
	/**
	 * @brief Get the proof property.
	 * @return Current value of proof.
	 */
	Variant get_proof() const;

	/**
	 * @brief Set the asset property.
	 * @param val New value for asset.
	 */
	void set_asset(const Dictionary &val);
	/**
	 * @brief Get the asset property.
	 * @return Current value of asset.
	 */
	Dictionary get_asset() const;

	/**
	 * @brief Set the cooldown property.
	 * @param val New value for cooldown.
	 */
	void set_cooldown(const Variant &val);
	/**
	 * @brief Get the cooldown property.
	 * @return Current value of cooldown.
	 */
	Variant get_cooldown() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTER
