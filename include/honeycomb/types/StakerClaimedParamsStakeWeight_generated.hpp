#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKERCLAIMEDPARAMSSTAKEWEIGHT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKERCLAIMEDPARAMSSTAKEWEIGHT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type StakerClaimedParamsStakeWeight.
 */
class StakerClaimedParamsStakeWeight : public Resource {
	GDCLASS(StakerClaimedParamsStakeWeight, Resource)

private:
	PackedInt32Array amounts;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this StakerClaimedParamsStakeWeight.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the amounts property.
	 * @param val New value for amounts.
	 */
	void set_amounts(const PackedInt32Array &val);
	/**
	 * @brief Get the amounts property.
	 * @return Current value of amounts.
	 */
	PackedInt32Array get_amounts() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKERCLAIMEDPARAMSSTAKEWEIGHT
