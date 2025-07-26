#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSMISSIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSMISSIONS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ServiceParamsMissions.
 */
class ServiceParamsMissions : public Resource {
	GDCLASS(ServiceParamsMissions, Resource)

private:
	Variant poolId;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ServiceParamsMissions.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the poolId property.
	 * @param val New value for poolId.
	 */
	void set_poolId(const Variant &val);
	/**
	 * @brief Get the poolId property.
	 * @return Current value of poolId.
	 */
	Variant get_poolId() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSMISSIONS
