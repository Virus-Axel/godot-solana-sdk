#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Multipliers.
 */
class Multipliers : public Resource {
	GDCLASS(Multipliers, Resource)

private:
	PackedByteArray address;
	int32_t bump;
	Variant stakingPool;
	int32_t decimals;
	Array durationMultipliers;
	Array countMultipliers;
	Array creatorMultipliers;
	Array collectionMultipliers;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Multipliers.
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
	 * @brief Set the stakingPool property.
	 * @param val New value for stakingPool.
	 */
	void set_stakingPool(const Variant &val);
	/**
	 * @brief Get the stakingPool property.
	 * @return Current value of stakingPool.
	 */
	Variant get_stakingPool() const;

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
	 * @brief Set the durationMultipliers property.
	 * @param val New value for durationMultipliers.
	 */
	void set_durationMultipliers(const Array &val);
	/**
	 * @brief Get the durationMultipliers property.
	 * @return Current value of durationMultipliers.
	 */
	Array get_durationMultipliers() const;

	/**
	 * @brief Set the countMultipliers property.
	 * @param val New value for countMultipliers.
	 */
	void set_countMultipliers(const Array &val);
	/**
	 * @brief Get the countMultipliers property.
	 * @return Current value of countMultipliers.
	 */
	Array get_countMultipliers() const;

	/**
	 * @brief Set the creatorMultipliers property.
	 * @param val New value for creatorMultipliers.
	 */
	void set_creatorMultipliers(const Array &val);
	/**
	 * @brief Get the creatorMultipliers property.
	 * @return Current value of creatorMultipliers.
	 */
	Array get_creatorMultipliers() const;

	/**
	 * @brief Set the collectionMultipliers property.
	 * @param val New value for collectionMultipliers.
	 */
	void set_collectionMultipliers(const Array &val);
	/**
	 * @brief Get the collectionMultipliers property.
	 * @return Current value of collectionMultipliers.
	 */
	Array get_collectionMultipliers() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERS
