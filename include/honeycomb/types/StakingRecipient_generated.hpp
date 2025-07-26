#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKINGRECIPIENT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKINGRECIPIENT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type StakingRecipient.
 */
class StakingRecipient : public Resource {
	GDCLASS(StakingRecipient, Resource)

private:
	PackedByteArray address;
	PackedByteArray tree_id;
	int64_t leaf_idx;
	Variant staker;
	int64_t stakedAmount;
	Variant rewardsClaimed;
	int32_t multiplier;
	int64_t stakeStartTime;
	int64_t lokedTill;
	Variant proof;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this StakingRecipient.
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
	 * @brief Set the tree_id property.
	 * @param val New value for tree_id.
	 */
	void set_tree_id(const PackedByteArray &val);
	/**
	 * @brief Get the tree_id property.
	 * @return Current value of tree_id.
	 */
	PackedByteArray get_tree_id() const;

	/**
	 * @brief Set the leaf_idx property.
	 * @param val New value for leaf_idx.
	 */
	void set_leaf_idx(const int64_t &val);
	/**
	 * @brief Get the leaf_idx property.
	 * @return Current value of leaf_idx.
	 */
	int64_t get_leaf_idx() const;

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
	 * @brief Set the stakedAmount property.
	 * @param val New value for stakedAmount.
	 */
	void set_stakedAmount(const int64_t &val);
	/**
	 * @brief Get the stakedAmount property.
	 * @return Current value of stakedAmount.
	 */
	int64_t get_stakedAmount() const;

	/**
	 * @brief Set the rewardsClaimed property.
	 * @param val New value for rewardsClaimed.
	 */
	void set_rewardsClaimed(const Variant &val);
	/**
	 * @brief Get the rewardsClaimed property.
	 * @return Current value of rewardsClaimed.
	 */
	Variant get_rewardsClaimed() const;

	/**
	 * @brief Set the multiplier property.
	 * @param val New value for multiplier.
	 */
	void set_multiplier(const int32_t &val);
	/**
	 * @brief Get the multiplier property.
	 * @return Current value of multiplier.
	 */
	int32_t get_multiplier() const;

	/**
	 * @brief Set the stakeStartTime property.
	 * @param val New value for stakeStartTime.
	 */
	void set_stakeStartTime(const int64_t &val);
	/**
	 * @brief Get the stakeStartTime property.
	 * @return Current value of stakeStartTime.
	 */
	int64_t get_stakeStartTime() const;

	/**
	 * @brief Set the lokedTill property.
	 * @param val New value for lokedTill.
	 */
	void set_lokedTill(const int64_t &val);
	/**
	 * @brief Get the lokedTill property.
	 * @return Current value of lokedTill.
	 */
	int64_t get_lokedTill() const;

	/**
	 * @brief Set the proof property.
	 * @param val New value for proof.
	 */
	void set_proof(const Variant &val);
	/**
	 * @brief Get the proof property.
	 * @return Current value of proof.
	 */
	Variant get_proof() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKINGRECIPIENT
