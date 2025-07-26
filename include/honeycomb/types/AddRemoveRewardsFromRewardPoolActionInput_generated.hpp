#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADDREMOVEREWARDSFROMREWARDPOOLACTIONINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADDREMOVEREWARDSFROMREWARDPOOLACTIONINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type AddRemoveRewardsFromRewardPoolActionInput.
 */
class AddRemoveRewardsFromRewardPoolActionInput : public Resource {
	GDCLASS(AddRemoveRewardsFromRewardPoolActionInput, Resource)

private:
	int64_t add;
	int64_t remove;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this AddRemoveRewardsFromRewardPoolActionInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the add property.
	 * @param val New value for add.
	 */
	void set_add(const int64_t &val);
	/**
	 * @brief Get the add property.
	 * @return Current value of add.
	 */
	int64_t get_add() const;

	/**
	 * @brief Set the remove property.
	 * @param val New value for remove.
	 */
	void set_remove(const int64_t &val);
	/**
	 * @brief Get the remove property.
	 * @return Current value of remove.
	 */
	int64_t get_remove() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADDREMOVEREWARDSFROMREWARDPOOLACTIONINPUT
