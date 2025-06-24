#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_REWARDPOOLCONFIGAPYINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_REWARDPOOLCONFIGAPYINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type RewardPoolConfigApyInput.
 */
class RewardPoolConfigApyInput : public Resource {
	GDCLASS(RewardPoolConfigApyInput, Resource)

private:
	int64_t rewardsPerDuration;
	int64_t rewardsDuration;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this RewardPoolConfigApyInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the rewardsPerDuration property.
	 * @param val New value for rewardsPerDuration.
	 */
	void set_rewardsPerDuration(const int64_t &val);
	/**
	 * @brief Get the rewardsPerDuration property.
	 * @return Current value of rewardsPerDuration.
	 */
	int64_t get_rewardsPerDuration() const;

	/**
	 * @brief Set the rewardsDuration property.
	 * @param val New value for rewardsDuration.
	 */
	void set_rewardsDuration(const int64_t &val);
	/**
	 * @brief Get the rewardsDuration property.
	 * @return Current value of rewardsDuration.
	 */
	int64_t get_rewardsDuration() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_REWARDPOOLCONFIGAPYINPUT
