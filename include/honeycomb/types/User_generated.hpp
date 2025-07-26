#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type User.
 */
class User : public Resource {
	GDCLASS(User, Resource)

private:
	int32_t id;
	PackedByteArray address;
	PackedByteArray tree_id;
	int64_t leaf_idx;
	Variant proof;
	Variant info;
	Variant socialInfo;
	Variant wallets;
	Array profiles;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this User.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the id property.
	 * @param val New value for id.
	 */
	void set_id(const int32_t &val);
	/**
	 * @brief Get the id property.
	 * @return Current value of id.
	 */
	int32_t get_id() const;

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
	 * @brief Set the info property.
	 * @param val New value for info.
	 */
	void set_info(const Variant &val);
	/**
	 * @brief Get the info property.
	 * @return Current value of info.
	 */
	Variant get_info() const;

	/**
	 * @brief Set the socialInfo property.
	 * @param val New value for socialInfo.
	 */
	void set_socialInfo(const Variant &val);
	/**
	 * @brief Get the socialInfo property.
	 * @return Current value of socialInfo.
	 */
	Variant get_socialInfo() const;

	/**
	 * @brief Set the wallets property.
	 * @param val New value for wallets.
	 */
	void set_wallets(const Variant &val);
	/**
	 * @brief Get the wallets property.
	 * @return Current value of wallets.
	 */
	Variant get_wallets() const;

	/**
	 * @brief Set the profiles property.
	 * @param val New value for profiles.
	 */
	void set_profiles(const Array &val);
	/**
	 * @brief Get the profiles property.
	 * @return Current value of profiles.
	 */
	Array get_profiles() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USER
