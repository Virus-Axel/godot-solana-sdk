#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYEJECTED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYEJECTED

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type UsedByEjected.
 */
class UsedByEjected : public Resource {
	GDCLASS(UsedByEjected, Resource)

private:
	Variant mint;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this UsedByEjected.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the mint property.
	 * @param val New value for mint.
	 */
	void set_mint(const Variant &val);
	/**
	 * @brief Get the mint property.
	 * @return Current value of mint.
	 */
	Variant get_mint() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYEJECTED
