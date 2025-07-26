#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDSFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDSFILTER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type EarnedRewardsFilter.
 */
class EarnedRewardsFilter : public Resource {
	GDCLASS(EarnedRewardsFilter, Resource)

private:
	int32_t delta;
	int32_t rewardIdx;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this EarnedRewardsFilter.
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
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDSFILTER
