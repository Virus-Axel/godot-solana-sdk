#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESTAKINGPOOLMETADATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESTAKINGPOOLMETADATAINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateStakingPoolMetadataInput.
 */
class CreateStakingPoolMetadataInput : public Resource {
	GDCLASS(CreateStakingPoolMetadataInput, Resource)

private:
	String name;
	int64_t rewardsPerDuration;
	int64_t rewardsDuration;
	int64_t maxRewardsDuration;
	int64_t minStakeDuration;
	int64_t cooldownDuration;
	bool resetStakeDuration;
	int64_t startTime;
	String lockType;
	int64_t endTime;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateStakingPoolMetadataInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the name property.
	 * @param val New value for name.
	 */
	void set_name(const String &val);
	/**
	 * @brief Get the name property.
	 * @return Current value of name.
	 */
	String get_name() const;

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

	/**
	 * @brief Set the maxRewardsDuration property.
	 * @param val New value for maxRewardsDuration.
	 */
	void set_maxRewardsDuration(const int64_t &val);
	/**
	 * @brief Get the maxRewardsDuration property.
	 * @return Current value of maxRewardsDuration.
	 */
	int64_t get_maxRewardsDuration() const;

	/**
	 * @brief Set the minStakeDuration property.
	 * @param val New value for minStakeDuration.
	 */
	void set_minStakeDuration(const int64_t &val);
	/**
	 * @brief Get the minStakeDuration property.
	 * @return Current value of minStakeDuration.
	 */
	int64_t get_minStakeDuration() const;

	/**
	 * @brief Set the cooldownDuration property.
	 * @param val New value for cooldownDuration.
	 */
	void set_cooldownDuration(const int64_t &val);
	/**
	 * @brief Get the cooldownDuration property.
	 * @return Current value of cooldownDuration.
	 */
	int64_t get_cooldownDuration() const;

	/**
	 * @brief Set the resetStakeDuration property.
	 * @param val New value for resetStakeDuration.
	 */
	void set_resetStakeDuration(const bool &val);
	/**
	 * @brief Get the resetStakeDuration property.
	 * @return Current value of resetStakeDuration.
	 */
	bool get_resetStakeDuration() const;

	/**
	 * @brief Set the startTime property.
	 * @param val New value for startTime.
	 */
	void set_startTime(const int64_t &val);
	/**
	 * @brief Get the startTime property.
	 * @return Current value of startTime.
	 */
	int64_t get_startTime() const;

	/**
	 * @brief Set the lockType property.
	 * @param val New value for lockType.
	 */
	void set_lockType(const String &val);
	/**
	 * @brief Get the lockType property.
	 * @return Current value of lockType.
	 */
	String get_lockType() const;

	/**
	 * @brief Set the endTime property.
	 * @param val New value for endTime.
	 */
	void set_endTime(const int64_t &val);
	/**
	 * @brief Get the endTime property.
	 * @return Current value of endTime.
	 */
	int64_t get_endTime() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESTAKINGPOOLMETADATAINPUT
