#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREESETUPCONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREESETUPCONFIG

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type TreeSetupConfig.
 */
class TreeSetupConfig : public Resource {
	GDCLASS(TreeSetupConfig, Resource)

private:
	Variant basic;
	Variant advanced;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this TreeSetupConfig.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the basic property.
	 * @param val New value for basic.
	 */
	void set_basic(const Variant &val);
	/**
	 * @brief Get the basic property.
	 * @return Current value of basic.
	 */
	Variant get_basic() const;

	/**
	 * @brief Set the advanced property.
	 * @param val New value for advanced.
	 */
	void set_advanced(const Variant &val);
	/**
	 * @brief Get the advanced property.
	 * @return Current value of advanced.
	 */
	Variant get_advanced() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREESETUPCONFIG
