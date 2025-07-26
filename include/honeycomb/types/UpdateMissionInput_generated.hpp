#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type UpdateMissionInput.
 */
class UpdateMissionInput : public Resource {
	GDCLASS(UpdateMissionInput, Resource)

private:
	Variant cost;
	int64_t minXp;
	int64_t duration;
	PackedInt32Array removeRewards;
	Array newRewards;
	Array updateRewards;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this UpdateMissionInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the cost property.
	 * @param val New value for cost.
	 */
	void set_cost(const Variant &val);
	/**
	 * @brief Get the cost property.
	 * @return Current value of cost.
	 */
	Variant get_cost() const;

	/**
	 * @brief Set the minXp property.
	 * @param val New value for minXp.
	 */
	void set_minXp(const int64_t &val);
	/**
	 * @brief Get the minXp property.
	 * @return Current value of minXp.
	 */
	int64_t get_minXp() const;

	/**
	 * @brief Set the duration property.
	 * @param val New value for duration.
	 */
	void set_duration(const int64_t &val);
	/**
	 * @brief Get the duration property.
	 * @return Current value of duration.
	 */
	int64_t get_duration() const;

	/**
	 * @brief Set the removeRewards property.
	 * @param val New value for removeRewards.
	 */
	void set_removeRewards(const PackedInt32Array &val);
	/**
	 * @brief Get the removeRewards property.
	 * @return Current value of removeRewards.
	 */
	PackedInt32Array get_removeRewards() const;

	/**
	 * @brief Set the newRewards property.
	 * @param val New value for newRewards.
	 */
	void set_newRewards(const Array &val);
	/**
	 * @brief Get the newRewards property.
	 * @return Current value of newRewards.
	 */
	Array get_newRewards() const;

	/**
	 * @brief Set the updateRewards property.
	 * @param val New value for updateRewards.
	 */
	void set_updateRewards(const Array &val);
	/**
	 * @brief Get the updateRewards property.
	 * @return Current value of updateRewards.
	 */
	Array get_updateRewards() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONINPUT
