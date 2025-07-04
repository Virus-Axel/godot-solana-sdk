#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONHIVECONTROL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONHIVECONTROL

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ServiceDelegationHiveControl.
 */
class ServiceDelegationHiveControl : public Resource {
	GDCLASS(ServiceDelegationHiveControl, Resource)

private:
	String permission;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ServiceDelegationHiveControl.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the permission property.
	 * @param val New value for permission.
	 */
	void set_permission(const String &val);
	/**
	 * @brief Get the permission property.
	 * @return Current value of permission.
	 */
	String get_permission() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONHIVECONTROL
