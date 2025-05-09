#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLSTAKEWEIGHTCONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLSTAKEWEIGHTCONFIG

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type SplStakeWeightConfig.
 */
class SplStakeWeightConfig : public Resource {
	GDCLASS(SplStakeWeightConfig, Resource)

private:
	int32_t weight;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this SplStakeWeightConfig.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the weight property.
	 * @param val New value for weight.
	 */
	void set_weight(const int32_t &val);
	/**
	 * @brief Get the weight property.
	 * @return Current value of weight.
	 */
	int32_t get_weight() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLSTAKEWEIGHTCONFIG
