#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKERCLAIMEDPARAMSAPY
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKERCLAIMEDPARAMSAPY

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type StakerClaimedParamsApy.
 */
class StakerClaimedParamsApy : public Resource {
	GDCLASS(StakerClaimedParamsApy, Resource)

private:
	int64_t lastRewardTime;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this StakerClaimedParamsApy.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the lastRewardTime property.
	 * @param val New value for lastRewardTime.
	 */
	void set_lastRewardTime(const int64_t &val);
	/**
	 * @brief Get the lastRewardTime property.
	 * @return Current value of lastRewardTime.
	 */
	int64_t get_lastRewardTime() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKERCLAIMEDPARAMSAPY
