#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSEMBLER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSEMBLER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ServiceParamsAssembler.
 */
class ServiceParamsAssembler : public Resource {
	GDCLASS(ServiceParamsAssembler, Resource)

private:
	Variant assemblerId;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ServiceParamsAssembler.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the assemblerId property.
	 * @param val New value for assemblerId.
	 */
	void set_assemblerId(const Variant &val);
	/**
	 * @brief Get the assemblerId property.
	 * @return Current value of assemblerId.
	 */
	Variant get_assemblerId() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSEMBLER
