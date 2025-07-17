#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKERCLAIMEDPARAMSNONE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKERCLAIMEDPARAMSNONE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type StakerClaimedParamsNone.
 */
class StakerClaimedParamsNone : public Resource {
	GDCLASS(StakerClaimedParamsNone, Resource)

private:
	bool none;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this StakerClaimedParamsNone.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the none property.
	 * @param val New value for none.
	 */
	void set_none(const bool &val);
	/**
	 * @brief Get the none property.
	 * @return Current value of none.
	 */
	bool get_none() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKERCLAIMEDPARAMSNONE
