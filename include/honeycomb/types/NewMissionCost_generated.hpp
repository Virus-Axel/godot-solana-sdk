#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONCOST
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONCOST

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type NewMissionCost.
 */
class NewMissionCost : public Resource {
	GDCLASS(NewMissionCost, Resource)

private:
	String address;
	int64_t amount;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this NewMissionCost.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const String &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	String get_address() const;

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
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONCOST
