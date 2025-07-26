#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Profile.
 */
class Profile : public Resource {
	GDCLASS(Profile, Resource)

private:
	PackedByteArray address;
	PackedByteArray tree_id;
	int64_t leaf_idx;
	Variant proof;
	Variant project;
	int32_t userId;
	String identity;
	Variant info;
	Variant platformData;
	Array customData;
	Variant user;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Profile.
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
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const Variant &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	Variant get_project() const;

	/**
	 * @brief Set the userId property.
	 * @param val New value for userId.
	 */
	void set_userId(const int32_t &val);
	/**
	 * @brief Get the userId property.
	 * @return Current value of userId.
	 */
	int32_t get_userId() const;

	/**
	 * @brief Set the identity property.
	 * @param val New value for identity.
	 */
	void set_identity(const String &val);
	/**
	 * @brief Get the identity property.
	 * @return Current value of identity.
	 */
	String get_identity() const;

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
	 * @brief Set the platformData property.
	 * @param val New value for platformData.
	 */
	void set_platformData(const Variant &val);
	/**
	 * @brief Get the platformData property.
	 * @return Current value of platformData.
	 */
	Variant get_platformData() const;

	/**
	 * @brief Set the customData property.
	 * @param val New value for customData.
	 */
	void set_customData(const Array &val);
	/**
	 * @brief Get the customData property.
	 * @return Current value of customData.
	 */
	Array get_customData() const;

	/**
	 * @brief Set the user property.
	 * @param val New value for user.
	 */
	void set_user(const Variant &val);
	/**
	 * @brief Get the user property.
	 * @return Current value of user.
	 */
	Variant get_user() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILE
