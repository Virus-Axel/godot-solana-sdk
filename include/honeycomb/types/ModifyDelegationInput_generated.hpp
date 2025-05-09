#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYDELEGATIONINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYDELEGATIONINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ModifyDelegationInput.
 */
class ModifyDelegationInput : public Resource {
	GDCLASS(ModifyDelegationInput, Resource)

private:
	Variant delegation;
	int32_t index;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ModifyDelegationInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the delegation property.
	 * @param val New value for delegation.
	 */
	void set_delegation(const Variant &val);
	/**
	 * @brief Get the delegation property.
	 * @return Current value of delegation.
	 */
	Variant get_delegation() const;

	/**
	 * @brief Set the index property.
	 * @param val New value for index.
	 */
	void set_index(const int32_t &val);
	/**
	 * @brief Get the index property.
	 * @return Current value of index.
	 */
	int32_t get_index() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYDELEGATIONINPUT
