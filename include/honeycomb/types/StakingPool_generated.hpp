#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKINGPOOL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKINGPOOL

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type StakingPool.
 */
class StakingPool : public Resource {
	GDCLASS(StakingPool, Resource)

private:
	PackedByteArray address;
	int32_t bump;
	Variant project;
	Variant key;
	Variant resource;
	Variant lockType;
	String name;
	int64_t rewardsPerDuration;
	int64_t rewardsDuration;
	int64_t maxRewardsDuration;
	int64_t minStakeDuration;
	int64_t cooldownDuration;
	bool resetStakeDuration;
	bool allowedMints;
	int64_t totalStaked;
	int64_t startTime;
	int64_t endTime;
	Array characterModels;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this StakingPool.
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
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const Variant &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	Variant get_project() const;

	/**
	 * @brief Set the key property.
	 * @param val New value for key.
	 */
	void set_key(const Variant &val);
	/**
	 * @brief Get the key property.
	 * @return Current value of key.
	 */
	Variant get_key() const;

	/**
	 * @brief Set the resource property.
	 * @param val New value for resource.
	 */
	void set_resource(const Variant &val);
	/**
	 * @brief Get the resource property.
	 * @return Current value of resource.
	 */
	Variant get_resource() const;

	/**
	 * @brief Set the lockType property.
	 * @param val New value for lockType.
	 */
	void set_lockType(const Variant &val);
	/**
	 * @brief Get the lockType property.
	 * @return Current value of lockType.
	 */
	Variant get_lockType() const;

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
	 * @brief Set the allowedMints property.
	 * @param val New value for allowedMints.
	 */
	void set_allowedMints(const bool &val);
	/**
	 * @brief Get the allowedMints property.
	 * @return Current value of allowedMints.
	 */
	bool get_allowedMints() const;

	/**
	 * @brief Set the totalStaked property.
	 * @param val New value for totalStaked.
	 */
	void set_totalStaked(const int64_t &val);
	/**
	 * @brief Get the totalStaked property.
	 * @return Current value of totalStaked.
	 */
	int64_t get_totalStaked() const;

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
	 * @brief Set the endTime property.
	 * @param val New value for endTime.
	 */
	void set_endTime(const int64_t &val);
	/**
	 * @brief Get the endTime property.
	 * @return Current value of endTime.
	 */
	int64_t get_endTime() const;

	/**
	 * @brief Set the characterModels property.
	 * @param val New value for characterModels.
	 */
	void set_characterModels(const Array &val);
	/**
	 * @brief Get the characterModels property.
	 * @return Current value of characterModels.
	 */
	Array get_characterModels() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKINGPOOL
