#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLREWARDCONFIGPARAMSAPY
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLREWARDCONFIGPARAMSAPY

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type SplRewardConfigParamsApy.
 */
class SplRewardConfigParamsApy : public Resource {
	GDCLASS(SplRewardConfigParamsApy, Resource)

private:
	Variant rewardTokenMint;
	Variant rewardVault;
	int64_t rewardsPerDuration;
	int64_t rewardsDuration;
	int64_t totalRewardAmount;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this SplRewardConfigParamsApy.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the rewardTokenMint property.
	 * @param val New value for rewardTokenMint.
	 */
	void set_rewardTokenMint(const Variant &val);
	/**
	 * @brief Get the rewardTokenMint property.
	 * @return Current value of rewardTokenMint.
	 */
	Variant get_rewardTokenMint() const;

	/**
	 * @brief Set the rewardVault property.
	 * @param val New value for rewardVault.
	 */
	void set_rewardVault(const Variant &val);
	/**
	 * @brief Get the rewardVault property.
	 * @return Current value of rewardVault.
	 */
	Variant get_rewardVault() const;

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
	 * @brief Set the totalRewardAmount property.
	 * @param val New value for totalRewardAmount.
	 */
	void set_totalRewardAmount(const int64_t &val);
	/**
	 * @brief Get the totalRewardAmount property.
	 * @return Current value of totalRewardAmount.
	 */
	int64_t get_totalRewardAmount() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLREWARDCONFIGPARAMSAPY
