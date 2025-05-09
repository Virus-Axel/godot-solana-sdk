#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERUSEDBYPARAMSFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERUSEDBYPARAMSFILTER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharacterUsedByParamsFilter.
 */
class CharacterUsedByParamsFilter : public Resource {
	GDCLASS(CharacterUsedByParamsFilter, Resource)

private:
	Variant pool;
	Variant staker;
	int32_t stakedAt;
	int32_t claimedAt;
	Variant id;
	Array rewards;
	int32_t endTime;
	bool rewardsCollected;
	Variant role;
	int32_t order;
	Variant mint;
	Variant user;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharacterUsedByParamsFilter.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the pool property.
	 * @param val New value for pool.
	 */
	void set_pool(const Variant &val);
	/**
	 * @brief Get the pool property.
	 * @return Current value of pool.
	 */
	Variant get_pool() const;

	/**
	 * @brief Set the staker property.
	 * @param val New value for staker.
	 */
	void set_staker(const Variant &val);
	/**
	 * @brief Get the staker property.
	 * @return Current value of staker.
	 */
	Variant get_staker() const;

	/**
	 * @brief Set the stakedAt property.
	 * @param val New value for stakedAt.
	 */
	void set_stakedAt(const int32_t &val);
	/**
	 * @brief Get the stakedAt property.
	 * @return Current value of stakedAt.
	 */
	int32_t get_stakedAt() const;

	/**
	 * @brief Set the claimedAt property.
	 * @param val New value for claimedAt.
	 */
	void set_claimedAt(const int32_t &val);
	/**
	 * @brief Get the claimedAt property.
	 * @return Current value of claimedAt.
	 */
	int32_t get_claimedAt() const;

	/**
	 * @brief Set the id property.
	 * @param val New value for id.
	 */
	void set_id(const Variant &val);
	/**
	 * @brief Get the id property.
	 * @return Current value of id.
	 */
	Variant get_id() const;

	/**
	 * @brief Set the rewards property.
	 * @param val New value for rewards.
	 */
	void set_rewards(const Array &val);
	/**
	 * @brief Get the rewards property.
	 * @return Current value of rewards.
	 */
	Array get_rewards() const;

	/**
	 * @brief Set the endTime property.
	 * @param val New value for endTime.
	 */
	void set_endTime(const int32_t &val);
	/**
	 * @brief Get the endTime property.
	 * @return Current value of endTime.
	 */
	int32_t get_endTime() const;

	/**
	 * @brief Set the rewardsCollected property.
	 * @param val New value for rewardsCollected.
	 */
	void set_rewardsCollected(const bool &val);
	/**
	 * @brief Get the rewardsCollected property.
	 * @return Current value of rewardsCollected.
	 */
	bool get_rewardsCollected() const;

	/**
	 * @brief Set the role property.
	 * @param val New value for role.
	 */
	void set_role(const Variant &val);
	/**
	 * @brief Get the role property.
	 * @return Current value of role.
	 */
	Variant get_role() const;

	/**
	 * @brief Set the order property.
	 * @param val New value for order.
	 */
	void set_order(const int32_t &val);
	/**
	 * @brief Get the order property.
	 * @return Current value of order.
	 */
	int32_t get_order() const;

	/**
	 * @brief Set the mint property.
	 * @param val New value for mint.
	 */
	void set_mint(const Variant &val);
	/**
	 * @brief Get the mint property.
	 * @return Current value of mint.
	 */
	Variant get_mint() const;

	/**
	 * @brief Set the user property.
	 * @param val New value for user.
	 */
	void set_user(const Variant &val);
	/**
	 * @brief Get the user property.
	 * @return Current value of user.
	 */
	Variant get_user() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERUSEDBYPARAMSFILTER
