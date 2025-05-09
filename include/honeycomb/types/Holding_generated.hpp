#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HOLDING
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HOLDING

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Holding.
 */
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
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Holding.
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
	 * @brief Set the holder property.
	 * @param val New value for holder.
	 */
	void set_holder(const Variant &val);
	/**
	 * @brief Get the holder property.
	 * @return Current value of holder.
	 */
	Variant get_holder() const;

	/**
	 * @brief Set the balance property.
	 * @param val New value for balance.
	 */
	void set_balance(const int64_t &val);
	/**
	 * @brief Get the balance property.
	 * @return Current value of balance.
	 */
	int64_t get_balance() const;

	/**
	 * @brief Set the characteristics property.
	 * @param val New value for characteristics.
	 */
	void set_characteristics(const Array &val);
	/**
	 * @brief Get the characteristics property.
	 * @return Current value of characteristics.
	 */
	Array get_characteristics() const;

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
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HOLDING
