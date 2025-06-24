#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLREWARDCONFIGPARAMSSTAKEWEIGHT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLREWARDCONFIGPARAMSSTAKEWEIGHT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type SplRewardConfigParamsStakeWeight.
 */
class SplRewardConfigParamsStakeWeight : public Resource {
	GDCLASS(SplRewardConfigParamsStakeWeight, Resource)

private:
	Array pools;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this SplRewardConfigParamsStakeWeight.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the pools property.
	 * @param val New value for pools.
	 */
	void set_pools(const Array &val);
	/**
	 * @brief Get the pools property.
	 * @return Current value of pools.
	 */
	Array get_pools() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLREWARDCONFIGPARAMSSTAKEWEIGHT
