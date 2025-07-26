#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEREWARDTYPEPARAMS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEREWARDTYPEPARAMS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ResourceRewardTypeParams.
 */
class ResourceRewardTypeParams : public Resource {
	GDCLASS(ResourceRewardTypeParams, Resource)

private:
	Variant address;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ResourceRewardTypeParams.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const Variant &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	Variant get_address() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEREWARDTYPEPARAMS
