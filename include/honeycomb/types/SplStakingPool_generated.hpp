#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLSTAKINGPOOL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLSTAKINGPOOL

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type SplStakingPool.
 */
class SplStakingPool : public Resource {
	GDCLASS(SplStakingPool, Resource)

private:
	PackedByteArray address;
	int32_t bump;
	int32_t nonce;
	Variant project;
	String name;
	Variant stake_token_mint;
	int64_t minStakeDurationSecs;
	int64_t maxStakeDurationSecs;
	int64_t startTime;
	int64_t endTime;
	int64_t totalStakedAmount;
	Array multipliers;
	Variant rewardConfig;
	Variant merkleTrees;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this SplStakingPool.
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
	 * @brief Set the nonce property.
	 * @param val New value for nonce.
	 */
	void set_nonce(const int32_t &val);
	/**
	 * @brief Get the nonce property.
	 * @return Current value of nonce.
	 */
	int32_t get_nonce() const;

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
	 * @brief Set the stake_token_mint property.
	 * @param val New value for stake_token_mint.
	 */
	void set_stake_token_mint(const Variant &val);
	/**
	 * @brief Get the stake_token_mint property.
	 * @return Current value of stake_token_mint.
	 */
	Variant get_stake_token_mint() const;

	/**
	 * @brief Set the minStakeDurationSecs property.
	 * @param val New value for minStakeDurationSecs.
	 */
	void set_minStakeDurationSecs(const int64_t &val);
	/**
	 * @brief Get the minStakeDurationSecs property.
	 * @return Current value of minStakeDurationSecs.
	 */
	int64_t get_minStakeDurationSecs() const;

	/**
	 * @brief Set the maxStakeDurationSecs property.
	 * @param val New value for maxStakeDurationSecs.
	 */
	void set_maxStakeDurationSecs(const int64_t &val);
	/**
	 * @brief Get the maxStakeDurationSecs property.
	 * @return Current value of maxStakeDurationSecs.
	 */
	int64_t get_maxStakeDurationSecs() const;

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
	 * @brief Set the totalStakedAmount property.
	 * @param val New value for totalStakedAmount.
	 */
	void set_totalStakedAmount(const int64_t &val);
	/**
	 * @brief Get the totalStakedAmount property.
	 * @return Current value of totalStakedAmount.
	 */
	int64_t get_totalStakedAmount() const;

	/**
	 * @brief Set the multipliers property.
	 * @param val New value for multipliers.
	 */
	void set_multipliers(const Array &val);
	/**
	 * @brief Get the multipliers property.
	 * @return Current value of multipliers.
	 */
	Array get_multipliers() const;

	/**
	 * @brief Set the rewardConfig property.
	 * @param val New value for rewardConfig.
	 */
	void set_rewardConfig(const Variant &val);
	/**
	 * @brief Get the rewardConfig property.
	 * @return Current value of rewardConfig.
	 */
	Variant get_rewardConfig() const;

	/**
	 * @brief Set the merkleTrees property.
	 * @param val New value for merkleTrees.
	 */
	void set_merkleTrees(const Variant &val);
	/**
	 * @brief Get the merkleTrees property.
	 * @return Current value of merkleTrees.
	 */
	Variant get_merkleTrees() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLSTAKINGPOOL
