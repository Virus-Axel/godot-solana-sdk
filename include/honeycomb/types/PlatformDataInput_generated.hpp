#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATAINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type PlatformDataInput.
 */
class PlatformDataInput : public Resource {
	GDCLASS(PlatformDataInput, Resource)

private:
	Variant custom;
	int64_t addXp;
	PackedInt32Array addAchievements;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this PlatformDataInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the custom property.
	 * @param val New value for custom.
	 */
	void set_custom(const Variant &val);
	/**
	 * @brief Get the custom property.
	 * @return Current value of custom.
	 */
	Variant get_custom() const;

	/**
	 * @brief Set the addXp property.
	 * @param val New value for addXp.
	 */
	void set_addXp(const int64_t &val);
	/**
	 * @brief Get the addXp property.
	 * @return Current value of addXp.
	 */
	int64_t get_addXp() const;

	/**
	 * @brief Set the addAchievements property.
	 * @param val New value for addAchievements.
	 */
	void set_addAchievements(const PackedInt32Array &val);
	/**
	 * @brief Get the addAchievements property.
	 * @return Current value of addAchievements.
	 */
	PackedInt32Array get_addAchievements() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATAINPUT
