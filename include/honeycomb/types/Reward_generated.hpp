#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_REWARD
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_REWARD

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Reward.
 */
class Reward : public Resource {
	GDCLASS(Reward, Resource)

private:
	int64_t min;
	int64_t max;
	Dictionary rewardType;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Reward.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the min property.
	 * @param val New value for min.
	 */
	void set_min(const int64_t &val);
	/**
	 * @brief Get the min property.
	 * @return Current value of min.
	 */
	int64_t get_min() const;

	/**
	 * @brief Set the max property.
	 * @param val New value for max.
	 */
	void set_max(const int64_t &val);
	/**
	 * @brief Get the max property.
	 * @return Current value of max.
	 */
	int64_t get_max() const;

	/**
	 * @brief Set the rewardType property.
	 * @param val New value for rewardType.
	 */
	void set_rewardType(const Dictionary &val);
	/**
	 * @brief Get the rewardType property.
	 * @return Current value of rewardType.
	 */
	Dictionary get_rewardType() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_REWARD
