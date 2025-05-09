#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_GLOBAL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_GLOBAL

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Global.
 */
class Global : public Resource {
	GDCLASS(Global, Resource)

private:
	PackedByteArray address;
	int32_t bump;
	Array config;
	Variant userTrees;
	int32_t totalUsers;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Global.
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
	 * @brief Set the bump property.
	 * @param val New value for bump.
	 */
	void set_bump(const int32_t &val);
	/**
	 * @brief Get the bump property.
	 * @return Current value of bump.
	 */
	int32_t get_bump() const;

	/**
	 * @brief Set the config property.
	 * @param val New value for config.
	 */
	void set_config(const Array &val);
	/**
	 * @brief Get the config property.
	 * @return Current value of config.
	 */
	Array get_config() const;

	/**
	 * @brief Set the userTrees property.
	 * @param val New value for userTrees.
	 */
	void set_userTrees(const Variant &val);
	/**
	 * @brief Get the userTrees property.
	 * @return Current value of userTrees.
	 */
	Variant get_userTrees() const;

	/**
	 * @brief Set the totalUsers property.
	 * @param val New value for totalUsers.
	 */
	void set_totalUsers(const int32_t &val);
	/**
	 * @brief Get the totalUsers property.
	 * @return Current value of totalUsers.
	 */
	int32_t get_totalUsers() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_GLOBAL
