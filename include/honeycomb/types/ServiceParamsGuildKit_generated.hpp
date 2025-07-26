#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSGUILDKIT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSGUILDKIT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ServiceParamsGuildKit.
 */
class ServiceParamsGuildKit : public Resource {
	GDCLASS(ServiceParamsGuildKit, Resource)

private:
	Variant kitId;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ServiceParamsGuildKit.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the kitId property.
	 * @param val New value for kitId.
	 */
	void set_kitId(const Variant &val);
	/**
	 * @brief Get the kitId property.
	 * @return Current value of kitId.
	 */
	Variant get_kitId() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSGUILDKIT
