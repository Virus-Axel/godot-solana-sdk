#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSWRAPPEDFUNGIBLE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSWRAPPEDFUNGIBLE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ResourceKindParamsWrappedFungible.
 */
class ResourceKindParamsWrappedFungible : public Resource {
	GDCLASS(ResourceKindParamsWrappedFungible, Resource)

private:
	int32_t decimals;
	Variant custody;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ResourceKindParamsWrappedFungible.
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

	/**
	 * @brief Set the custody property.
	 * @param val New value for custody.
	 */
	void set_custody(const Variant &val);
	/**
	 * @brief Get the custody property.
	 * @return Current value of custody.
	 */
	Variant get_custody() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSWRAPPEDFUNGIBLE
