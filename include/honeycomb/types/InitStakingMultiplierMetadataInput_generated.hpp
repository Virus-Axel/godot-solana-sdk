#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITSTAKINGMULTIPLIERMETADATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITSTAKINGMULTIPLIERMETADATAINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type InitStakingMultiplierMetadataInput.
 */
class InitStakingMultiplierMetadataInput : public Resource {
	GDCLASS(InitStakingMultiplierMetadataInput, Resource)

private:
	int32_t decimals;
	Array multipliers;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this InitStakingMultiplierMetadataInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the decimals property.
	 * @param val New value for decimals.
	 */
	void set_decimals(const int32_t &val);
	/**
	 * @brief Get the decimals property.
	 * @return Current value of decimals.
	 */
	int32_t get_decimals() const;

	/**
	 * @brief Set the multipliers property.
	 * @param val New value for multipliers.
	 */
	void set_multipliers(const Array &val);
	/**
	 * @brief Get the multipliers property.
	 * @return Current value of multipliers.
	 */
	Array get_multipliers() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITSTAKINGMULTIPLIERMETADATAINPUT
