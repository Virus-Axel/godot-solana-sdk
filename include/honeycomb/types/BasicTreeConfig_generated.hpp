#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BASICTREECONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BASICTREECONFIG

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type BasicTreeConfig.
 */
class BasicTreeConfig : public Resource {
	GDCLASS(BasicTreeConfig, Resource)

private:
	int32_t numAssets;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this BasicTreeConfig.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the numAssets property.
	 * @param val New value for numAssets.
	 */
	void set_numAssets(const int32_t &val);
	/**
	 * @brief Get the numAssets property.
	 * @return Current value of numAssets.
	 */
	int32_t get_numAssets() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BASICTREECONFIG
