#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type EarnedRewards.
 */
class EarnedRewards : public Resource {
	GDCLASS(EarnedRewards, Resource)

private:
	int32_t delta;
	int32_t rewardIdx;
	bool collected;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this EarnedRewards.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the delta property.
	 * @param val New value for delta.
	 */
	void set_delta(const int32_t &val);
	/**
	 * @brief Get the delta property.
	 * @return Current value of delta.
	 */
	int32_t get_delta() const;

	/**
	 * @brief Set the rewardIdx property.
	 * @param val New value for rewardIdx.
	 */
	void set_rewardIdx(const int32_t &val);
	/**
	 * @brief Get the rewardIdx property.
	 * @return Current value of rewardIdx.
	 */
	int32_t get_rewardIdx() const;

	/**
	 * @brief Set the collected property.
	 * @param val New value for collected.
	 */
	void set_collected(const bool &val);
	/**
	 * @brief Get the collected property.
	 * @return Current value of collected.
	 */
	bool get_collected() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDS
