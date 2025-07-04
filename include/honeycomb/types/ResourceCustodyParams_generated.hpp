#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCECUSTODYPARAMS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCECUSTODYPARAMS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ResourceCustodyParams.
 */
class ResourceCustodyParams : public Resource {
	GDCLASS(ResourceCustodyParams, Resource)

private:
	Variant burnerDestination;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ResourceCustodyParams.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the burnerDestination property.
	 * @param val New value for burnerDestination.
	 */
	void set_burnerDestination(const Variant &val);
	/**
	 * @brief Get the burnerDestination property.
	 * @return Current value of burnerDestination.
	 */
	Variant get_burnerDestination() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCECUSTODYPARAMS
