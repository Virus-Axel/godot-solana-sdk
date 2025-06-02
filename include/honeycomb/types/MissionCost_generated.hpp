#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONCOST
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONCOST

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MissionCost.
 */
class MissionCost : public Resource {
	GDCLASS(MissionCost, Resource)

private:
	int64_t amount;
	Variant resource_address;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MissionCost.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the amount property.
	 * @param val New value for amount.
	 */
	void set_amount(const int64_t &val);
	/**
	 * @brief Get the amount property.
	 * @return Current value of amount.
	 */
	int64_t get_amount() const;

	/**
	 * @brief Set the resource_address property.
	 * @param val New value for resource_address.
	 */
	void set_resource_address(const Variant &val);
	/**
	 * @brief Get the resource_address property.
	 * @return Current value of resource_address.
	 */
	Variant get_resource_address() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONCOST
