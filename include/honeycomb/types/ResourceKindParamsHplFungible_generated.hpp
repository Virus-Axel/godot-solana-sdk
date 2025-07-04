#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSHPLFUNGIBLE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSHPLFUNGIBLE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ResourceKindParamsHplFungible.
 */
class ResourceKindParamsHplFungible : public Resource {
	GDCLASS(ResourceKindParamsHplFungible, Resource)

private:
	int32_t decimals;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ResourceKindParamsHplFungible.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the decimals property.
	 * @param val New value for decimals.
	 */
	void set_decimals(const int32_t &val);
	/**
	 * @brief Get the decimals property.
	 * @return Current value of decimals.
	 */
	int32_t get_decimals() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSHPLFUNGIBLE
