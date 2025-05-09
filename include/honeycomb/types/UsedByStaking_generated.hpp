#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYSTAKING
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYSTAKING

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type UsedByStaking.
 */
class UsedByStaking : public Resource {
	GDCLASS(UsedByStaking, Resource)

private:
	Variant pool;
	Variant staker;
	int32_t stakedAt;
	int32_t claimedAt;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this UsedByStaking.
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
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYSTAKING
